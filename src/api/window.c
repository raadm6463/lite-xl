#include <SDL.h>
#include "api.h"
#include "renwindow.h"

static void init_window_icon(SDL_Window* window) {
#if !defined(_WIN32) && !defined(__APPLE__)
  #include "../resources/icons/icon.inl"
  (void) icon_rgba_len; /* unused */
  SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(
    icon_rgba, 64, 64,
    32, 64 * 4,
    0x000000ff,
    0x0000ff00,
    0x00ff0000,
    0xff000000);
  SDL_SetWindowIcon(window, surf);
  SDL_FreeSurface(surf);
#endif
}

struct HitTestInfo {
  int title_height;
  int controls_width;
  int resize_border;
};
typedef struct HitTestInfo HitTestInfo;

static HitTestInfo window_hit_info[1] = {{0, 0, 0}};

#define RESIZE_FROM_TOP 0
#define RESIZE_FROM_RIGHT 0

static SDL_HitTestResult SDLCALL hit_test(SDL_Window *window, const SDL_Point *pt, void *data) {
  const HitTestInfo *hit_info = (HitTestInfo *) data;
  const int resize_border = hit_info->resize_border;
  const int controls_width = hit_info->controls_width;
  int w, h;

  SDL_GetWindowSize(window, &w, &h);

  if (pt->y < hit_info->title_height &&
    #if RESIZE_FROM_TOP
    pt->y > hit_info->resize_border &&
    #endif
    pt->x > resize_border && pt->x < w - controls_width) {
    return SDL_HITTEST_DRAGGABLE;
  }

  #define REPORT_RESIZE_HIT(name) { \
    return SDL_HITTEST_RESIZE_##name; \
  }

  if (pt->x < resize_border && pt->y < resize_border) {
    REPORT_RESIZE_HIT(TOPLEFT);
  #if RESIZE_FROM_TOP
  } else if (pt->x > resize_border && pt->x < w - controls_width && pt->y < resize_border) {
    REPORT_RESIZE_HIT(TOP);
  #endif
  } else if (pt->x > w - resize_border && pt->y < resize_border) {
    REPORT_RESIZE_HIT(TOPRIGHT);
  #if RESIZE_FROM_RIGHT
  } else if (pt->x > w - resize_border && pt->y > resize_border && pt->y < h - resize_border) {
    REPORT_RESIZE_HIT(RIGHT);
  #endif
  } else if (pt->x > w - resize_border && pt->y > h - resize_border) {
    REPORT_RESIZE_HIT(BOTTOMRIGHT);
  } else if (pt->x < w - resize_border && pt->x > resize_border && pt->y > h - resize_border) {
    REPORT_RESIZE_HIT(BOTTOM);
  } else if (pt->x < resize_border && pt->y > h - resize_border) {
    REPORT_RESIZE_HIT(BOTTOMLEFT);
  } else if (pt->x < resize_border && pt->y < h - resize_border && pt->y > resize_border) {
    REPORT_RESIZE_HIT(LEFT);
  }

  return SDL_HITTEST_NORMAL;
}

static int f_window_font_new(lua_State *L) {

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);

  SDL_Window* window = SDL_CreateWindow(
    "", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dm.w * 0.8, dm.h * 0.8,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN);
  init_window_icon(window);
  if (!window) {
    fprintf(stderr, "Error creating lite-xl window: %s", SDL_GetError());
    exit(1);
  }

  RenWindow* window_renderer = ren_init(window);
  RenWindow* userdata_renderer = lua_newuserdata(L, sizeof(RenWindow));
  *userdata_renderer = *window_renderer;
  free(window_renderer);

  luaL_setmetatable(L, API_TYPE_WINDOW);
  return 1;
}

static int f_window_font_gc(lua_State *L) {
  RenWindow* window_renderer = luaL_checkudata(L, 1, API_TYPE_WINDOW);
  ren_free(window_renderer);
  return 0;
}

static int f_window_set_title(lua_State *L) {
  RenWindow* window_renderer = luaL_checkudata(L, 1, API_TYPE_WINDOW);
  const char *title = luaL_checkstring(L, 2);
  SDL_SetWindowTitle(window_renderer->window, title);
  return 0;
}

static const char *window_opts[] = { "normal", "minimized", "maximized", "fullscreen", 0 };
enum { WIN_NORMAL, WIN_MINIMIZED, WIN_MAXIMIZED, WIN_FULLSCREEN };

static int f_window_set_mode(lua_State *L) {
  RenWindow* window_renderer = luaL_checkudata(L, 1, API_TYPE_WINDOW);
  int n = luaL_checkoption(L, 2, "normal", window_opts);
  SDL_SetWindowFullscreen(window_renderer->window,
    n == WIN_FULLSCREEN ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
  if (n == WIN_NORMAL) { SDL_RestoreWindow(window_renderer->window); }
  if (n == WIN_MAXIMIZED) { SDL_MaximizeWindow(window_renderer->window); }
  if (n == WIN_MINIMIZED) { SDL_MinimizeWindow(window_renderer->window); }
  return 0;
}

static int f_window_get_mode(lua_State *L) {
  RenWindow* window_renderer = luaL_checkudata(L, 1, API_TYPE_WINDOW);
  unsigned flags = SDL_GetWindowFlags(window_renderer->window);
  if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
    lua_pushstring(L, "fullscreen");
  } else if (flags & SDL_WINDOW_MINIMIZED) {
    lua_pushstring(L, "minimized");
  } else if (flags & SDL_WINDOW_MAXIMIZED) {
    lua_pushstring(L, "maximized");
  } else {
    lua_pushstring(L, "normal");
  }
  return 1;
}

static int f_window_set_bordered(lua_State *L) {
  RenWindow* window_renderer = luaL_checkudata(L, 1, API_TYPE_WINDOW);
  int bordered = lua_toboolean(L, 2);
  SDL_SetWindowBordered(window_renderer->window, bordered);
  return 0;
}

static int f_window_set_hit_test(lua_State *L) {
  RenWindow* window_renderer = luaL_checkudata(L, 1, API_TYPE_WINDOW);
  if (lua_gettop(L) == 1) {
    SDL_SetWindowHitTest(window_renderer->window, NULL, NULL);
    return 0;
  }
  window_hit_info->title_height = luaL_checknumber(L, 2);
  window_hit_info->controls_width = luaL_checknumber(L, 3);
  window_hit_info->resize_border = luaL_checknumber(L, 4);
  SDL_SetWindowHitTest(window_renderer->window, hit_test, window_hit_info);
  return 0; 
}

static int f_window_get_size(lua_State *L) {
  int x, y, w, h;
  RenWindow* window_renderer = luaL_checkudata(L, 1, API_TYPE_WINDOW);
  SDL_GetWindowSize(window_renderer->window, &w, &h);
  SDL_GetWindowPosition(window_renderer->window, &x, &y);
  lua_pushinteger(L, w);
  lua_pushinteger(L, h);
  lua_pushinteger(L, x);
  lua_pushinteger(L, y);
  return 4;
}

static int f_window_set_size(lua_State *L) {
  RenWindow* window_renderer = luaL_checkudata(L, 1, API_TYPE_WINDOW);
  double w = luaL_checknumber(L, 2);
  double h = luaL_checknumber(L, 3);
  double x = luaL_checknumber(L, 4);
  double y = luaL_checknumber(L, 5);
  SDL_SetWindowSize(window_renderer->window, w, h);
  SDL_SetWindowPosition(window_renderer->window, x, y);
  ren_resize_window(window_renderer);
  return 0;
}

static int f_window_has_focus(lua_State *L) {
  RenWindow* window_renderer = luaL_checkudata(L, 1, API_TYPE_WINDOW);
  unsigned flags = SDL_GetWindowFlags(window_renderer->window);
  lua_pushboolean(L, flags & SDL_WINDOW_INPUT_FOCUS);
  return 1;
}

static int f_window_focus(lua_State *L) {
  RenWindow* window_renderer = luaL_checkudata(L, 1, API_TYPE_WINDOW);
  /*
    SDL_RaiseWindow should be enough but on some window managers like the
    one used on Gnome the window needs to first have input focus in order
    to allow the window to be focused. Also on wayland the raise window event
    may not always be obeyed.
  */
  SDL_SetWindowInputFocus(window_renderer->window);
  SDL_RaiseWindow(window_renderer->window);
  return 0;
}

static int f_window_set_opacity(lua_State *L) {
  RenWindow* window_renderer = luaL_checkudata(L, 1, API_TYPE_WINDOW);
  double n = luaL_checknumber(L, 2);
  int r = SDL_SetWindowOpacity(window_renderer->window, n);
  lua_pushboolean(L, r > -1);
  return 1;
}

static const luaL_Reg lib[] = {
  { "new",                      f_window_font_new },
  { "__gc",                     f_window_font_gc },
  { "set_title",                f_window_set_title },
  { "set_mode",                 f_window_set_mode },
  { "get_mode",                 f_window_get_mode },
  { "set_bordered",             f_window_set_bordered },
  { "set_hit_test",             f_window_set_hit_test },
  { "get_size",                 f_window_get_size },
  { "set_size",                 f_window_set_size },
  { "has_focus",                f_window_has_focus },
  { "focus",                    f_window_focus },
  { "set_opacity",              f_window_set_opacity },
  //{ "get_active",               f_window_set_opacity },
  { NULL, NULL }
};

int luaopen_window(lua_State *L) {
  luaL_newmetatable(L, API_TYPE_WINDOW);
  luaL_newlib(L, lib);
  return 1;
}

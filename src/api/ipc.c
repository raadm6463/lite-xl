#include "api.h"
#include "../ipc.h"

static int f_sendmessage(lua_State* L)
{
  return 0;
}

static int f_receivemessage(lua_State* L)
{
  lua_newtable(L);

  return 1;
}

static const struct luaL_Reg ipc_metatable[] = {
  {"send_message", f_sendmessage},
  {"receive_message", f_receivemessage},
  {NULL, NULL}
};

int luaopen_ipc(lua_State *L) {
  luaL_newmetatable(L, API_TYPE_IPC);
  luaL_newlib(L, ipc_metatable);

  return 1;
}
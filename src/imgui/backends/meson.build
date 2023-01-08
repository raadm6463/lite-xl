# Impl
imgui_impl_sources = files(
    'imgui_impl_sdl.cpp',
    'imgui_impl_sdlrenderer.cpp',
)
imgui_impl_includes = include_directories('.')

imgui_impl = static_library(
    'imgui_impl',
    imgui_impl_sources,
    include_directories: imgui_impl_includes,
    dependencies: [imgui_dep, sdl_dep],
    cpp_args: ['-DIMGUI_IMPL_API=extern "C"']
)
imgui_impl_dep = declare_dependency(
    link_with: imgui_impl,
    include_directories: imgui_impl_includes,
    dependencies: [imgui_dep, sdl_dep],
)

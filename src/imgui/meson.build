# Core
imgui_sources = [
    'imgui.cpp',
    'imgui_demo.cpp',

    'imgui_draw.cpp',
    'imgui_tables.cpp',
    'imgui_widgets.cpp',
]
imgui_includes = include_directories('.')

imgui = static_library(
    'imgui',
    imgui_sources,
    include_directories: imgui_includes
)
imgui_dep = declare_dependency(
    link_with: imgui,
    include_directories: imgui_includes,
)

# dear_bindings
cimgui_sources = [
    'cimgui.cpp'
]
cimgui_includes = include_directories('.')

cimgui = static_library(
    'cimgui',
    cimgui_sources,
    include_directories: cimgui_includes,
    dependencies: imgui_dep,
    cpp_args: ['-DCIMGUI_API=extern "C"']
)
cimgui_dep = declare_dependency(
    link_with: cimgui,
    include_directories: cimgui_includes,
    dependencies: imgui_dep
)

subdir('backends')
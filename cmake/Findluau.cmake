#
# LUAU_FOUND
# LUAU_INCLUDE_DIRS
# LUAU_LIBRARIES

include(FetchContent)

# Luau lacks a language definition
enable_language(CXX)

FetchContent_Declare(
  luau
  GIT_REPOSITORY https://github.com/Roblox/luau
  GIT_TAG        97965c7c0a3e53ea92e7a51892b94acf12472268 # 0.576
)

set(LUAU_BUILD_CLI OFF CACHE INTERNAL "")
set(LUAU_BUILD_TESTS OFF CACHE INTERNAL "")
set(LUAU_EXTERN_C ON CACHE INTERNAL "")
FetchContent_MakeAvailable(luau)

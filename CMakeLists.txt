cmake_minimum_required(VERSION 3.20)
project(KOBOLD LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug)
endif()
option(KOBOLD_BUILD_GAME  "Build the game"  ON)
option(KOBOLD_BUILD_TESTS "Build the tests" OFF)
option(KOBOLD_SANITIZERS  "Enable ASan (+UBSan where supported)" ON)

# ---- Shared warnings/sanitizers (our targets only) ----
add_library(kobold_options INTERFACE)

if(MSVC)
  target_compile_options(kobold_options INTERFACE /W4 /WX)
  if(KOBOLD_SANITIZERS)
    target_compile_options(kobold_options INTERFACE /fsanitize=address)
    # MSVC ASan is incompatible with /RTC* and incremental linking
    foreach(flag CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG)
      string(REGEX REPLACE "/RTC[1csu]+" "" ${flag} "${${flag}}")
    endforeach()
    target_link_options(kobold_options INTERFACE /INCREMENTAL:NO)
  endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(kobold_options INTERFACE -Wall -Wextra -Werror)
  # MinGW GCC has no sanitizer runtime
  if(KOBOLD_SANITIZERS AND NOT (WIN32 AND CMAKE_C_COMPILER_ID STREQUAL "GNU"))
    target_compile_options(kobold_options INTERFACE
      -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(kobold_options INTERFACE
      -fsanitize=address,undefined)
  endif()
endif()

# ---- Game logic (everything except main.c) ----
add_library(game_core STATIC src/game.c)
target_link_libraries(game_core PRIVATE kobold_options)

# ---- Game executable ----
if(KOBOLD_BUILD_GAME)
  add_executable(app src/main.c)
  target_link_libraries(app PRIVATE game_core kobold_options)
endif()

# ---- Tests ----
if(KOBOLD_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

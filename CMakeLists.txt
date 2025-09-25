# === CMake Setup ===
cmake_minimum_required(VERSION 3.14)

# Project name, version and languages
project(log LANGUAGES C CXX)

# === BUILD OPTIONS ===
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
# Check if CMAKE_BUILD_TYPE is set; if not, default to Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug, Release)" FORCE)
endif()

# Automatically enable the generation of compile_commands.json in Debug mode.
# The compile_commands.json file is useful for tools like clangd or IDEs
# that rely on compile commands for features like autocompletion and diagnostics.
# In Release mode (or other build types), this feature is turned off by default
# to avoid unnecessary output, but it can still be manually enabled by users.
# For other build types (e.g., Release), it defaults to `OFF` unless explicitly enabled by the user.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Enable compile_commands.json generation" FORCE)
else()
  set(CMAKE_EXPORT_COMPILE_COMMANDS OFF CACHE BOOL "Disable compile_commands.json generation" FORCE)
endif()

# === Platform & Toolchain Configuration ===
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# --- liteseq Library ---
add_library(log
  ${SRC_DIR}/log.c
)

# C standard & basic props
set_target_properties(log PROPERTIES
  C_STANDARD 99
  CXX_STANDARD 11
)

# Compiler options
# Debug-specific flags

set(COMMON_WARNINGS
  -Wall
)

set(DEBUG_WARNINGS
  -Wextra
  -Wpedantic
  -Wshadow
  -Wunused
  -Wunknown-pragmas
  -Wunused-parameter
  -Wunused-variable
  -Wunused-function
  -Wunused-but-set-variable
  -Wmissing-field-initializers
  -Wunused-local-typedefs
  -Wuninitialized
  -Wmaybe-uninitialized
  -Wreturn-type
  # -Wpadded # Uncomment or gate by compiler if needed
)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-g)
  add_compile_options(${COMMON_WARNINGS})
  add_compile_options(${DEBUG_WARNINGS})
  add_compile_definitions(DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_options(${COMMON_WARNINGS})
  add_compile_options(-O3)
  add_compile_definitions(RELEASE)
endif()

if(APPLE)  # true for macOS/iOS simulators, etc.
  find_library(DISPATCH_LIB NAMES dispatch)
  if(DISPATCH_LIB)
    message(STATUS "Using Grand Central Dispatch (GCD) for threading on Apple platforms.")
    target_link_libraries(log PRIVATE "${DISPATCH_LIB}")
  else()
    message(FATAL_ERROR "Grand Central Dispatch (libdispatch) not found.")
  endif()
else()
  find_package(Threads REQUIRED)
  message(STATUS "Using pthreads for threading on non-Apple platforms.")
  target_link_libraries(log PRIVATE Threads::Threads)
endif()


add_compile_definitions(LOGC__USER_SETTINGS)

# Enable color output by default; can be disabled by setting LOG_USE_COLOR to OFF
option(LOG_USE_COLOR "Enable color output" ON)
if(LOG_USE_COLOR)
  target_compile_definitions(log PRIVATE LOG_USE_COLOR)
endif()

option(LOG_FULL_FILE_NAME "Log entire file path" OFF)
if(LOG_FULL_FILE_NAME)
  target_compile_definitions(log PRIVATE LOG_FULL_FILE_NAME)
endif()

option(LOG_FILE_INFO "Log file and line number" ON)
if(LOG_FILE_INFO)
  target_compile_definitions(log PRIVATE LOG_FILE_INFO)
endif()

# Choice between stdout and stderr
set(LOG_DEFAULT_STREAM "stderr" CACHE STRING "Default stream for log output (stdout/stderr)")

# Validate value
set_property(CACHE LOG_DEFAULT_STREAM PROPERTY STRINGS stdout stderr)

# Expose as a macro
target_compile_definitions(log PRIVATE LOG_DEFAULT_STREAM=${LOG_DEFAULT_STREAM})

# Include directories for the library
# expose the given directories to anything linking against loglib
target_include_directories(log
  PUBLIC
  ${INCLUDE_DIR}
)

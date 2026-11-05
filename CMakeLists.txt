# Top-level build for bug-gauntlet. Builds every levels/<name>/ as an executable
# target named <name>, plus the Phase 2 incidents app. Used by .vscode/tasks.json.
#   cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build
#   cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug -DGAUNTLET_SANITIZE=ON && cmake --build build-asan
cmake_minimum_required(VERSION 3.20)
project(bug_gauntlet CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()
# WHY: -O0 keeps every variable inspectable in lldb.
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fno-omit-frame-pointer")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(GAUNTLET_SANITIZE "Build with AddressSanitizer + UndefinedBehaviorSanitizer" OFF)
if(GAUNTLET_SANITIZE)
  add_compile_options(-fsanitize=address,undefined -fno-sanitize-recover=all)
  add_link_options(-fsanitize=address,undefined)
endif()

file(GLOB level_dirs LIST_DIRECTORIES true ${CMAKE_SOURCE_DIR}/levels/*)
foreach(dir ${level_dirs})
  if(IS_DIRECTORY ${dir})
    get_filename_component(level ${dir} NAME)
    file(GLOB level_sources CONFIGURE_DEPENDS ${dir}/*.cpp)
    if(level_sources)
      add_executable(${level} ${level_sources})
    endif()
  endif()
endforeach()

add_subdirectory(incidents/app)

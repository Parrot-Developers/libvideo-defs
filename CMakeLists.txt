cmake_minimum_required(VERSION 3.15)
project(libvideo-defs VERSION 1.0)

set(LIB_SOURCES
        src/vdefs.c
        src/vdefs_formats.c
        src/vdefs_json.c
        src/vdefs_params.c
)

add_library(video-defs ${LIB_SOURCES})
add_library(video-defs_compiler_flags INTERFACE)
target_compile_features(video-defs_compiler_flags INTERFACE cxx_std_11)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

target_include_directories(video-defs PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                            $<INSTALL_INTERFACE:include>
)

target_compile_definitions(video-defs PRIVATE "VDEF_API_EXPORTS")
target_compile_definitions(video-defs PRIVATE "_GNU_SOURCE")


find_package(json-c CONFIG REQUIRED)
target_link_libraries(video-defs PRIVATE json-c::json-c json-c::json-c-static)
target_link_libraries(video-defs PRIVATE ulog h264 h265 video-defs_compiler_flags)
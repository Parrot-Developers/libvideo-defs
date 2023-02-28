cmake_minimum_required(VERSION 3.15)
project(video-defs VERSION 1.0)

set(LIB_SOURCES
        src/vdefs.c
        src/vdefs_formats.c
        src/vdefs_json.c
        src/vdefs_params.c
)

add_library(${PROJECT_NAME} ${LIB_SOURCES})
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

target_include_directories(${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                            $<INSTALL_INTERFACE:include>
)

target_compile_definitions(${PROJECT_NAME} PRIVATE "VDEF_API_EXPORTS")
target_compile_definitions(${PROJECT_NAME} PRIVATE "_GNU_SOURCE")


find_package(json-c CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC 
                      json-c::json-c 
                      json-c::json-c-static
                      ulog 
                      h264 
                      h265 
                      )

install(TARGETS ${PROJECT_NAME}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}
)

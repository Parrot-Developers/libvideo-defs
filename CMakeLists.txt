cmake_minimum_required(VERSION 3.15)
project(video-defs VERSION 1.0)

# checks if set up rpath exists for install
if(COMMAND set_up_rpath)
    set_up_rpath()
else()
    message("Set up rpath not defined!")
endif()


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

target_link_libraries(${PROJECT_NAME} 
                        PRIVATE 
                      json-c::json-c 
                      json-c::json-c-static
                      ulog 

                        PUBLIC
                      h264 
                      h265 

                      )

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        )

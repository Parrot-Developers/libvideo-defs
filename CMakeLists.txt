cmake_minimum_required(VERSION 3.15)
project(video-defs VERSION 1.0)


set(LIB_SOURCES
        src/vdefs.c
        src/vdefs_formats.c
        src/vdefs_json.c
        src/vdefs_params.c
)

add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES})

# checks if set up rpath exists for install
if(COMMAND set_up_rpath)
    set_up_rpath()
else()
    message("Set up rpath not defined!")
endif()

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
set_target_properties(${PROJECT_NAME} PROPERTIES
                    POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
)

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
                        ulog
                    PUBLIC
                        h264 
                        h265 
                      )

# to get all header files correctly
set(${PROJECT_NAME}-headers
    ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/
)
install(
    DIRECTORY ${${PROJECT_NAME}-headers} DESTINATION include/${PROJECT_NAME}
)

if(WIN32)
    install(CODE "file(GLOB dll_files \"${CMAKE_CURRENT_BINARY_DIR}/*.dll\")")
    install(CODE "file(INSTALL DESTINATION \"\$\{CMAKE_INSTALL_PREFIX\}/bin\" TYPE SHARED_LIBRARY FILES \$\{dll_files\})")
else()
    install(CODE "set(DEPENDENCY_PATHS \"\$\<TARGET_FILE_DIR:json-c::json-c\>\")")
    install(CODE [[
        file(GET_RUNTIME_DEPENDENCIES
            LIBRARIES $<TARGET_FILE:video-defs>
            RESOLVED_DEPENDENCIES_VAR _r_deps
            UNRESOLVED_DEPENDENCIES_VAR _u_deps
            DIRECTORIES ${DEPENDENCY_PATHS}
            POST_EXCLUDE_REGEXES "^/lib" "^/usr/lib"
        )
        foreach(_file ${_r_deps})
            file(INSTALL
            DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
            TYPE SHARED_LIBRARY
            FOLLOW_SYMLINK_CHAIN
            FILES "${_file}"
            )
        endforeach()
        list(LENGTH _u_deps _u_length)
    ]])
endif()

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        )

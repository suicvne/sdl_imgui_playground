cmake_minimum_required(VERSION 3.10)

set(PROJECT_N ImguiTest)
set(PROJECT_N_LIB qgui)
project(${PROJECT_N} VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(DO_STATIC)
# STATIC
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(BUILD_SHARED_LIBS OFF)
endif()

# TODO: Replace this with a few different source group variables.
# EG: GAME_SOURCE -> src/*.cpp, src/etc/*.cpp 
# EG2: IMGUI_SOURCE -> src/imgui/*.cpp
file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp)

set(LIB_SRCS 
    ${PROJECT_SOURCE_DIR}/src/imgui/imgui_demo.cpp
    ${PROJECT_SOURCE_DIR}/src/imgui/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/src/imgui/imgui_tables.cpp 
    ${PROJECT_SOURCE_DIR}/src/imgui/imgui_widgets.cpp 
    ${PROJECT_SOURCE_DIR}/src/imgui/imgui.cpp
    ${PROJECT_SOURCE_DIR}/src/imgui/backends/imgui_impl_sdl.cpp
    ${PROJECT_SOURCE_DIR}/src/imgui/backends/imgui_impl_sdlrenderer.cpp
)
set(LIB_HEADERS
    ${PROJECT_SOURCE_DIR}/src/ui_backend.h
    ${PROJECT_SOURCE_DIR}/src/video_backend.h
    ${PROJECT_SOURCE_DIR}/src/common_types.h
    ${PROJECT_SOURCE_DIR}/src/imgui/imconfig.h
    ${PROJECT_SOURCE_DIR}/src/imgui/imgui_internal.h
    ${PROJECT_SOURCE_DIR}/src/imgui/imgui.h 
    ${PROJECT_SOURCE_DIR}/src/imgui/backends/imgui_impl_sdl.h
    ${PROJECT_SOURCE_DIR}/src/imgui/backends/imgui_impl_sdlrenderer.h
)

# a macro that gets all of the header containing directories. 
MACRO(header_directories return_list includes_base_folder extention )
    FILE(GLOB_RECURSE new_list ${includes_base_folder}/*.${extention})
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

# using said macro.
header_directories(INCLUDES ${PROJECT_SOURCE_DIR}/include h)

message("src files:")
foreach(file ${SRCS})
    message(STATUS ${file})
endforeach()

message("include directories:")
foreach(dir ${INCLUDES})
    message(STATUS ${dir})
endforeach()

# TODO: Build the sources list separately and make one add_executable call.
if(APPLE)
    add_executable(${PROJECT_N} src/macos.m ${SRCS} ${RESOURCES})
else()
    add_executable(${PROJECT_N} ${SRCS} ${RESOURCES})
    add_library(${PROJECT_N_LIB} ${LIB_SRCS})

endif()

# find_file(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
# find_library(SDL2_LIBRARY SDL2)

find_package(SDL2 REQUIRED)

message("\n\nSDL2 LIBRARIES: ${SDL2_LIBRARIES}")
message("SDL2 INCLUDE DIR: ${SDL2_INCLUDE_DIRS}\n")

if(DO_STATIC)
    target_link_libraries(${PROJECT_N} PUBLIC -static -static-libgcc -static-libstdc++ SDL2::SDL2-static)
else()
    target_link_libraries(${PROJECT_N} PUBLIC ${SDL2_LIBRARIES})
    if(NOT APPLE)
        target_link_libraries(${PROJECT_N_LIB} PUBLIC ${SDL2_LIBRARIES})
    endif()
    if(APPLE)
        target_link_libraries(${PROJECT_N} PUBLIC "-framework CoreFoundation" "-framework AppKit")
    endif()
endif()
target_include_directories(${PROJECT_N} PUBLIC include src/imgui src/imgui/backends ${SDL2_INCLUDE_DIRS})

if(NOT APPLE)
target_include_directories(${PROJECT_N_LIB} PUBLIC include src/imgui src/imgui/backends ${SDL2_INCLUDE_DIRS})
endif()
# target_include_directories(${PROJECT_N} PUBLIC )

# Copy Resources Directory
add_custom_command(TARGET ${PROJECT_N} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/Fonts/
            ${CMAKE_CURRENT_BINARY_DIR}/Fonts/
)

include(GNUInstallDirs)

if(NOT APPLE)
install(TARGETS ${PROJECT_N_LIB}
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qgui
)
endif()

install(FILES ${LIB_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qgui)
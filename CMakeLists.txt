cmake_minimum_required(VERSION 3.10)

set(PROJECT_N ImguiTest)
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
header_directories(INCLUDES ${PROJECT_SOURCE_DIR}/include/ hpp)

message("src files:")
foreach(file ${SRCS})
        message(STATUS ${file})
endforeach()

message("include directories:")
foreach(dir ${INCLUDES})
        message(STATUS ${dir})
endforeach()
add_executable(${PROJECT_N} src/macos.m ${SRCS})

# find_file(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
# find_library(SDL2_LIBRARY SDL2)

find_package(SDL2 REQUIRED)

message("\n\nSDL2 LIBRARIES: ${SDL2_LIBRARIES}")
message("SDL2 INCLUDE DIR: ${SDL2_INCLUDE_DIRS}\n")

if(DO_STATIC)
    target_link_libraries(${PROJECT_N} PUBLIC -static -static-libgcc -static-libstdc++ SDL2::SDL2-static)
else()
    target_link_libraries(${PROJECT_N} PUBLIC ${SDL2_LIBRARIES} "-framework CoreFoundation")
endif()
target_include_directories(${PROJECT_N} PUBLIC include src/imgui ${SDL2_INCLUDE_DIRS})
# target_include_directories(${PROJECT_N} PUBLIC )

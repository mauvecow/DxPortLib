# Very minimal cmake script.
#
# Needs support for debug/release builds and test progarms.

cmake_minimum_required(VERSION 2.6)

include(CheckLibraryExists)
include(FindPkgConfig)

project(DxPortLib)

pkg_search_module(SDL2 REQUIRED sdl2)
list(APPEND ADD_INCLUDES ${SDL2_INCLUDE_DIRS})
list(APPEND ADD_LIB_DIRS ${SDL2_LIBRARY_DIRS})
list(APPEND ADD_LIBS ${SDL2_LIBRARIES})

pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)
list(APPEND ADD_INCLUDES ${SDL2_TTF_INCLUDE_DIRS})
list(APPEND ADD_LIB_DIRS ${SDL2_TTF_LIBRARY_DIRS})
list(APPEND ADD_LIBS ${SDL2_TTF_LIBRARIES})

pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)
list(APPEND ADD_INCLUDES ${SDL2_IMAGE_INCLUDE_DIRS})
list(APPEND ADD_LIB_DIRS ${SDL2_IMAGE_LIBRARY_DIRS})
list(APPEND ADD_LIBS ${SDL2_IMAGE_LIBRARIES})

pkg_search_module(VORBISFILE REQUIRED vorbisfile)
list(APPEND ADD_INCLUDES ${VORBISFILE_INCLUDE_DIRS})
list(APPEND ADD_LIB_DIRS ${VORBISFILE_LIBRARY_DIRS})
list(APPEND ADD_LIBS ${VORBISFILE_LIBRARIES})

list(APPEND ADD_CFLAGS "-fvisibility=hidden")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${ADD_INCLUDES})

file(GLOB DXPORTLIB_SOURCES
	src/*.c
	src/*.cpp
)

set(CMAKE_C_FLAGS "-g -O2 -Wall ${ADD_CFLAGS}")

link_directories(${ADD_LIB_DIRS})
add_library(DxPortLib SHARED ${DXPORTLIB_SOURCES})
target_link_libraries(DxPortLib ${ADD_LIBS})


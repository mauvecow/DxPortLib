# Very minimal cmake script.
#
# Needs support for debug/release builds and test progarms.

cmake_minimum_required(VERSION 3.13)

include(CheckLibraryExists)
include(FindPkgConfig)

project(DxPortLib)

add_library(DxPortLib)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")

	add_subdirectory(html5)

else()

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

	set_target_properties(DxPortLib
		PROPERTIES 
			COMPILE_FLAGS	"-g -O2 -Wall ${ADD_CFLAGS}"
			LINK_FLAGS		"-g -O2 -Wall ${ADD_CFLAGS}"
	)

endif()

target_include_directories(DxPortLib
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${ADD_INCLUDES}
)

file(GLOB_RECURSE DXPORTLIB_SOURCES
	src/*.c
	src/*.cpp
)

target_sources(DxPortLib
	PRIVATE
		${DXPORTLIB_SOURCES}
)

target_link_directories(DxPortLib
	PUBLIC
		${ADD_LIB_DIRS}
)

target_link_libraries(DxPortLib ${ADD_LIBS})


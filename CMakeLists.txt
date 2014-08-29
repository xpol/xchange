cmake_minimum_required(VERSION 2.6) 
project(xchange) 

set(JSONCPP_WITH_TESTS OFF CACHE BOOL "Turn off JsonCpp tests")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(formats/jsoncpp) 
add_subdirectory(formats/msgpack) 

include_directories(
	src
	formats/msgpack/src
	${CMAKE_BINARY_DIR}/formats/msgpack/src
	formats/rapidjson/include
	formats/jsoncpp/include
)

link_directories(${CMAKE_BINARY_DIR}/lib)

list(APPEND SOURCES
	src/main.cpp
	src/msgpack/type/rapidjson/document.hpp
	src/msgpack/type/jsoncpp/value.hpp
)


add_executable(xchange ${SOURCES})

target_link_libraries(xchange msgpack-static jsoncpp_lib)

if (MSVC)
    set_property(TARGET xchange APPEND_STRING PROPERTY COMPILE_FLAGS "/wd4290")
    set_property(TARGET msgpack APPEND_STRING PROPERTY COMPILE_FLAGS "/wd4100 /wd4127 /wd4204 /wd4290")
    set_property(TARGET msgpack-static APPEND_STRING PROPERTY COMPILE_FLAGS "/wd4100 /wd4127 /wd4204 /wd4290")
endif (MSVC)

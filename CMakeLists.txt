cmake_minimum_required(VERSION 2.6) 
project(xchange) 

set(JSONCPP_WITH_TESTS OFF CACHE BOOL "Compile and run JsonCpp test executables")

add_subdirectory(formats/jsoncpp) 
add_subdirectory(formats/msgpack) 


include_directories(
	src
	formats/msgpack/src
	${CMAKE_BINARY_DIR}/formats/msgpack/src
	formats/rapidjson/include
	formats/jsoncpp/include
)

link_directories(${CMAKE_BINARY_DIR}/msgpack) 
link_directories(${CMAKE_BINARY_DIR}/jsoncpp_lib) 

list(APPEND SOURCES
	src/main.cpp
	src/msgpack/type/rapidjson/document.hpp
	src/msgpack/type/jsoncpp/value.hpp
)


add_executable(xchange ${SOURCES})

# Link the executable to the Hello library. 
target_link_libraries(xchange msgpack-static jsoncpp_lib)

if (MSVC)
    set_property(TARGET xchange APPEND_STRING PROPERTY COMPILE_FLAGS /wd4290)
endif (MSVC)

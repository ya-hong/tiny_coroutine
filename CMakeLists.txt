set(CMAKE_CXX_STANDARD 20)

add_library(tiny_coroutine INTERFACE)
target_include_directories(tiny_coroutine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB examples RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "example/*.cpp")

message(STATUS examples)
foreach(file IN LISTS examples)
    message(STATUS ${file})
	get_filename_component(example ${file} NAME_WE)
	add_executable(${example} ${file})
	target_link_libraries(${example} tiny_coroutine)
endforeach(file)

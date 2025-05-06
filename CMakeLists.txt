cmake_minimum_required(VERSION 3.15)

project(modern-string)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -fPIC")
endif()
if (CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -fPIC")
endif()


set(MY_LIB_NAME modern-string)
set(MY_LIB_TEST_NAME modern-string-test)

set(MY_SOURCE_FILES
	#about string
	ks_string.h
	ks_basic_mutable_string.h
	ks_basic_immutable_string.h
	ks_basic_xmutable_string_base.h
	ks_basic_xmutable_string_base.inl
	ks_basic_xmutable_string_base.cpp
	ks_basic_string_allocator.h
	#about string-view
	ks_string_view.h
	ks_basic_string_view.h
	ks_basic_string_view.inl
	ks_basic_string_view.cpp
	#about string-util
	ks_string_util.h
	ks_string_util.inl
	ks_string_util.cpp
	ks_string_util_convert.h
	ks_string_util_convert.cpp
	ks_string_util_parse.h
	ks_string_util_parse.cpp
	ks_type_traits.h
	#others
	base.h
	ks_basic_pointer_iterator.h
)

set(PUBLIC_HEADER_FILES
	#about string
	ks_string.h
	ks_basic_mutable_string.h
	ks_basic_immutable_string.h
	ks_basic_xmutable_string_base.h
	ks_basic_xmutable_string_base.inl
	ks_basic_string_allocator.h
	#about string-view
	ks_string_view.h
	ks_basic_string_view.h
	ks_basic_string_view.inl
	#about string-util
	ks_string_util.h
	ks_string_util.inl
	ks_string_util_convert.h
	ks_string_util_parse.h
	ks_type_traits.h
	#others
	base.h
	ks_basic_pointer_iterator.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MY_SOURCE_FILES} __test.cpp)


#static lib
add_library(${MY_LIB_NAME} STATIC ${MY_SOURCE_FILES})
target_compile_definitions(${MY_LIB_NAME} PRIVATE MODERN_STRING_EXPORTS)

#test exe
if (MODERN_STRING_TEST_ENABLED)
	add_executable(${MY_LIB_TEST_NAME} __test.cpp)
	target_link_libraries(${MY_LIB_TEST_NAME} PRIVATE ${MY_LIB_NAME})
endif()


#install
install(TARGETS ${MY_LIB_NAME})
install(FILES ${PUBLIC_HEADER_FILES} DESTINATION include/modern-string)
install(FILES README.md DESTINATION include/modern-string)
install(FILES LICENSE DESTINATION include/modern-string)

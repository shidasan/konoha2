cmake_minimum_required(VERSION 2.6)
set(CMAKE_BUILD_TYPE ${KONOHA_BUILD_TYPE})

project(llvm)
set(PACKAGE_SOURCE_CODE llvm.cpp)
set(PACKAGE_SCRIPT_CODE llvm_glue.k)
find_program(LLVM_CONFIG NAMES llvm-config DOC "llvm-config")
if(LLVM_CONFIG)
execute_process(
		COMMAND ${LLVM_CONFIG} --version
		OUTPUT_VARIABLE LLVM_VERSION
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)

execute_process(
		COMMAND ${LLVM_CONFIG} --cppflags
		OUTPUT_VARIABLE LLVM_CFLAGS
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
execute_process(
		COMMAND ${LLVM_CONFIG} --cxxflags
		OUTPUT_VARIABLE LLVM_CXXFLAGS
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
execute_process(
		COMMAND ${LLVM_CONFIG} --ldflags
		OUTPUT_VARIABLE LLVM_LDFLAGS
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
execute_process(
	COMMAND ${LLVM_CONFIG} --libs core jit native linker ipo engine interpreter
	OUTPUT_VARIABLE LLVM_JIT_LIBS
	OUTPUT_STRIP_TRAILING_WHITESPACE
	)
execute_process(
		COMMAND ${LLVM_CONFIG} --libdir
		OUTPUT_VARIABLE LLVM_LIBDIR
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)

set(CMAKE_CXX_FLAGS "${LLVM_CXXFLAGS} -Wno-variadic-macros ${CMAKE_CXX_FLAGS} -UNDEBUG")
string(REPLACE " " ";" v ${LLVM_JIT_LIBS})
link_directories(${LINK_DIRECTORIES} ${LLVM_LIBDIR})
set(ExtraLibs ${ExtraLibs} ${v})

if("${LLVM_VERSION}" MATCHES "2.9")
add_definitions(-DUSE_LLVM_2_9=1)
endif("${LLVM_VERSION}" MATCHES "2.9")

if(${LLVM_VERSION} VERSION_GREATER "3.0")
set(PKG_USE_LLVM_3_1 1)
add_definitions(-DUSE_LLVM_3_1=1)
endif(${LLVM_VERSION} VERSION_GREATER "3.0")

set(PACKAGE_EXTRA_LIBRARY ${ExtraLibs})
add_konoha_package(konoha.llvm)
endif(LLVM_CONFIG)

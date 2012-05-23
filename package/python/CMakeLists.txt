cmake_minimum_required(VERSION 2.6)
set(PACKAGE_SOURCE_CODE python_glue.c)
set(PACKAGE_SCRIPT_CODE python_glue.k)

find_program(PYTHON_CONFIG NAMES python-config)
execute_process(
		COMMAND ${PYTHON_CONFIG} --include
		OUTPUT_VARIABLE PYCNF_INCLUDE_RESULT
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)

string(REGEX MATCH "/[^ ]+/python[2-3].[0-9]" PYTHON_INCLUDE_PATH ${PYCNF_INCLUDE_RESULT})
string(REGEX REPLACE "/[^ ]+/(python[2-3].[0-9])" "\\1" python_ver "${PYTHON_INCLUDE_PATH}")
message("replaced PYTHON_INCLUDE_PATH = '" ${PYTHON_INCLUDE_PATH} "'")

find_library(PACKAGE_EXTRA_LIBRARY NAMES ${python_ver})
if(PACKAGE_EXTRA_LIBRARY)
	include_directories(${PYTHON_INCLUDE_PATH})
	set(PACKAGE_ENV "PYTHONPATH=$ENV{PYTHONPATH}:${CMAKE_CURRENT_SOURCE_DIR}/test/")
	add_konoha_package(python)
endif(PACKAGE_EXTRA_LIBRARY)

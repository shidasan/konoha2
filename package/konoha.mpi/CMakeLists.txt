cmake_minimum_required(VERSION 2.6)
set(PACKAGE_SOURCE_CODE
    mpi_glue.c
)
set(PACKAGE_SCRIPT_CODE mpi_glue.k)
add_konoha_package(konoha.mpi)


# Make my own CMake file
cmake_minimum_required(VERSION 3.9 FATAL_ERROR) # Set up minimum requirement of CMake version
project(RTPC_drift)  # Definew project name
if (NOT TARGET Garfield::Garfield)  # find garfield++
	find_package(Garfield REQUIRED)
endif()

add_library(InfoRead SHARED InfoRead.cc) # add my own library
# ---Build executable------------------------------------------------------------
add_executable(RTPC_drift RTPC_drift.cc) # execuable file name and the source code
target_link_libraries(RTPC_drift Garfield::Garfield InfoRead)  # Set up lib link




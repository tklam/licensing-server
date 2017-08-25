cmake_minimum_required(VERSION 2.6)

#project info
project(sqLITER)

#set version
set(sqLITER_VERSION_MAJOR 1)
set(sqLITER_VERSION_MINOR 0)

#set environment variables
message("CMAKE_CURRENT_SOURCE_DIR = " ${CMAKE_CURRENT_SOURCE_DIR})
message("CMAKE_CURRENT_BINARY_DIR = " ${CMAKE_CURRENT_BINARY_DIR})
message("CMAKE_INSTALL_PREFIX = " ${CMAKE_INSTALL_PREFIX})

#headers
include_directories (./src)

set(LIB_SOURCE 
    src/sqlite3.c
    src/sqliter.cpp
)
add_library(libsqLITER
    ${LIB_SOURCE}
)
target_link_libraries(libsqLITER dl)

# change lib_target properties
SET_TARGET_PROPERTIES( libsqLITER PROPERTIES
# create *nix style library versions + symbolic links
    VERSION ${${PROJECT_NAME}_VERSION}
    SOVERSION ${${PROJECT_NAME}_SOVERSION}
    # allow creating static and shared libs without conflicts
    CLEAN_DIRECT_OUTPUT 1
    # avoid conflicts between library and binary target names
    OUTPUT_NAME sqLITER)

cmake_minimum_required(VERSION 2.6)

#project info
project(Licensing)

#set version
set(Licensing_VERSION_MAJOR 1)
set(Licensing_VERSION_MINOR 0)

set(CMAKE_INSTALL_PREFIX ".")

#set environment variables
message("CMAKE_CURRENT_SOURCE_DIR = " ${CMAKE_CURRENT_SOURCE_DIR})
message("CMAKE_CURRENT_BINARY_DIR = " ${CMAKE_CURRENT_BINARY_DIR})
message("CMAKE_INSTALL_PREFIX = " ${CMAKE_INSTALL_PREFIX})

add_subdirectory(./lib/cppzmq)
add_subdirectory(./lib/sqLITER)


#headers
include_directories (. ./lib/cppzmq ./lib ./lib/sqLITER/src)

set(CMAKE_CXX_FLAGS -std=c++14)
set(global_optimization_flag -O0 -g -Wall)

#library directory
add_definitions(${global_optimization_flag})

#config of OrderRingServer
ADD_DEFINITIONS(
    -DLICENSING_SERVER_ADDRESS="tcp://127.0.0.1:5559"
    -DCLIENT_ID="TESTING_CLIENT"
)

set(LIB_SOURCE 
    client.cpp
    database.cpp
    authorization.cpp
    crypto.cpp
)
add_library(libLicensing
    ${LIB_SOURCE}
)
target_link_libraries(libLicensing pthread zmq libsqLITER cryptopp)

# change lib_target properties
SET_TARGET_PROPERTIES( libLicensing PROPERTIES
# create *nix style library versions + symbolic links
    VERSION ${${PROJECT_NAME}_VERSION}
    SOVERSION ${${PROJECT_NAME}_SOVERSION}
    # allow creating static and shared libs without conflicts
    CLEAN_DIRECT_OUTPUT 1
    # avoid conflicts between library and binary target names
    OUTPUT_NAME Licensing)


# server
add_executable(licensingServer
    server.cpp
)
target_link_libraries(licensingServer pthread zmq libLicensing )

# sample client application
add_executable(sampleClientApp
    sampleClientApp.cpp
)
target_link_libraries(sampleClientApp zmq libLicensing )

# sample crypto application
add_executable(sampleCrypto
    sampleCrypto.cpp
)
target_link_libraries(sampleCrypto pthread zmq libLicensing)

FILE(GLOB scripts
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/*.sql"
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/*.sh"
)
INSTALL(FILES ${scripts} DESTINATION bin 
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    COMPONENT scripts)

INSTALL(TARGETS licensingServer sampleClientApp sampleCrypto
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

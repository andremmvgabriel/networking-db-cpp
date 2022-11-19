cmake_minimum_required(VERSION 3.10)

# Set the project name (and version?)
project(NetworkingDB)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)

# Changes the binary, static lib, and libraries default paths of the CMake build, respectively
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)

# Creates a preprocessing macro for DEBUG BUILD TYPE
if ( CMAKE_BUILD_TYPE MATCHES "Debug" )
    add_definitions( -DDEBUG )
else()
    add_definitions( -DNDEBUG )
endif()

# Dependencies packages
find_package(Crow)

# TODO(gabe) - Maybe do not have this lib in CMake 
include(FetchContent)
FetchContent_Declare(cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 871ed52d350214a034f6ef8a3b8f51c5ce1bd400)
    # The commit hash for 1.9.0. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

# Dependencies
INCLUDE_DIRECTORIES ( include )

set( src_networking_db_lib
    src/gabe/networkingDB/NetworkDatabase.cpp
    src/gabe/networkingDB/Server.cpp
    src/gabe/networkingDB/Client.cpp
)
add_library( networking_db STATIC ${src_networking_db_lib} )
target_link_libraries( networking_db Crow::Crow cpr::cpr fmt sqlite3 )

add_subdirectory(tests)
add_subdirectory(examples)

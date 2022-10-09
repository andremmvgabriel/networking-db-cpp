cmake_minimum_required(VERSION 3.10)

# Set the project name (and version?)
project(NetworkingDB)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)

# Changes the binary, static lib, and libraries default paths of the CMake build, respectively
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)

# Dependencies
INCLUDE_DIRECTORIES ( include )

set( src_networking_db_lib
#     # src/gabe/networking/networkingDB.cpp

#     # src/gabe/networking/core/NetworkManager.cpp

#     # src/gabe/networking/utils/Communication.cpp
#     # src/gabe/networking/utils/NetworkDatabase.cpp

#     src/gabe/networkingdB/Client.
    src/gabe/networkingDB/NetworkDatabase.cpp
    src/gabe/networkingDB/Server.cpp
)
add_library( networking_db STATIC ${src_networking_db_lib} )

find_package( oatpp REQUIRED )

add_executable( Main main.cpp )
target_link_libraries( Main fmt sqlite3 )

add_executable( Clients main_clients.cpp )
target_link_libraries( Clients fmt sqlite3 )

find_package(Crow)

add_executable( Server main_server.cpp )
target_link_libraries( Server networking_db fmt sqlite3 Crow::Crow )

include(FetchContent)
FetchContent_Declare(cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 871ed52d350214a034f6ef8a3b8f51c5ce1bd400)
    # The commit hash for 1.9.0. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

add_executable( Requests main_requests.cpp )
target_link_libraries( Requests PRIVATE cpr::cpr )



add_executable( Database main_database.cpp )
target_link_libraries( Database networking_db fmt sqlite3 )

add_executable( Database2 database_architecture.cpp )
target_link_libraries( Database2 fmt sqlite3 )

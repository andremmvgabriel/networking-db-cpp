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

# set( src_networking_db_lib
#     # src/gabe/networking/networkingDB.cpp

#     # src/gabe/networking/core/NetworkManager.cpp

#     # src/gabe/networking/utils/Communication.cpp
#     # src/gabe/networking/utils/NetworkDatabase.cpp

#     src/gabe/networkingdB/Client.
# )
# add_library( networking_db STATIC ${src_networking_lib} )


add_executable( Main main.cpp )
target_link_libraries( Main fmt sqlite3 )

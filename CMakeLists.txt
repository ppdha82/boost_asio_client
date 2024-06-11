project (asio_client)

cmake_minimum_required(VERSION 3.15)

set (asio_SOURCES src/main.cpp src/client.cpp)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -std=c++11")

# add_library (asio STATIC ${asio_HEAD} ${asio_SOURCES})

# Library properties.
#"${CMAKE_BINARY_DIR}/lib"
# set (asio_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${platform}/lib")
# set_target_properties (asio_test PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${asio_OUTPUT_DIRECTORY})

set (Boost_INCLUDE_DIR "c:/boost_1_56_0")
include_directories (include . "${Boost_INCLUDE_DIR}")
add_definitions (-DBOOST_ALL_NO_LIB)
set (Boost_USE_STATIC_LIBS ON)

set (LIBRARY_DIRS "c:/boost_1_56_0/stage/lib")
set (Boost_PACKAGE_OPTION thread system regex)

find_package(Boost REQUIRED COMPONENTS ${Boost_PACKAGE_OPTION})

add_executable(asio_client ${asio_SOURCES})

TARGET_LINK_LIBRARIES(asio_client "${Boost_SYSTEM_LIBRARY}" ws2_32)
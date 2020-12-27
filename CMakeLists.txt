cmake_minimum_required(VERSION 3.12)

project(ftp VERSION 0.1.0
            DESCRIPTION "Simple FTP client library"
            LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED yes)

find_package(Boost 1.72 REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

set(STATIC_LIBRARY_TARGET ftp_static)
set(SHARED_LIBRARY_TARGET ftp_shared)
set(LIBRARY_PUBLIC_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/ftp/codes.hpp
                           ${CMAKE_CURRENT_LIST_DIR}/include/ftp/ftp.hpp)
set(LIBRARY_PRIVATE_HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/commands.hpp
                            ${CMAKE_CURRENT_LIST_DIR}/src/util.hpp
                            ${CMAKE_CURRENT_LIST_DIR}/src/logger.hpp)
set(LIBRARY_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/ftp.cpp
                    ${CMAKE_CURRENT_LIST_DIR}/src/logger.cpp)


add_library(${STATIC_LIBRARY_TARGET} STATIC ${LIBRARY_PUBLIC_HEADERS} ${LIBRARY_PRIVATE_HEADERS} ${LIBRARY_SOURCES})
target_include_directories(${STATIC_LIBRARY_TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
                                                    PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${STATIC_LIBRARY_TARGET} PRIVATE Boost::system Threads::Threads OpenSSL::SSL)

add_library(ftp::ftp_static ALIAS ${STATIC_LIBRARY_TARGET})

add_library(${SHARED_LIBRARY_TARGET} SHARED ${LIBRARY_PUBLIC_HEADERS} ${LIBRARY_PRIVATE_HEADERS} ${LIBRARY_SOURCES})
target_include_directories(${SHARED_LIBRARY_TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
                                                    PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${SHARED_LIBRARY_TARGET} PRIVATE Boost::system Threads::Threads OpenSSL::SSL)

add_library(ftp::ftp_shared ALIAS ${SHARED_LIBRARY_TARGET})

option(FTP_ENABLE_TESTS "Built the FTP client library tests" ON)

if(FTP_ENABLE_TESTS)
    find_package(Catch2 REQUIRED)
    enable_testing()

    add_library(ftp_test_main STATIC ${CMAKE_CURRENT_LIST_DIR}/tests/test_main.cpp)
    target_link_libraries(ftp_test_main PUBLIC Catch2::Catch2)

    add_executable(ftp_test_executor ${CMAKE_CURRENT_LIST_DIR}/tests/client_test.cpp)
    target_link_libraries(ftp_test_executor PRIVATE ftp_test_main ftp::ftp_static)

    include(CTest)
    include(Catch)
    catch_discover_tests(ftp_test_executor)
endif()

message(WARNING "The author of this library is currently looking for a job - contact at rosengeorgiev93 at gmail dot com")

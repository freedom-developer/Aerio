cmake_minimum_required(VERSION 3.8)

project(AerioTest LANGUAGES CXX)

add_library(aerio INTERFACE)
target_include_directories(aerio INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/..
)
target_compile_features(aerio INTERFACE cxx_std_17)

add_executable(aerio_test main.cpp)
target_link_libraries(aerio_test PRIVATE aerio)

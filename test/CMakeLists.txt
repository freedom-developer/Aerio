cmake_minimum_required(VERSION 3.8)

project(AerioTest LANGUAGES CXX)

# Export the compile commands so clangd can use the same include paths and
# language standard as the CMake build.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(aerio INTERFACE)
target_include_directories(aerio INTERFACE
    # Allow includes relative to the public header root, e.g. "ip/tcp.hpp".
    ${CMAKE_CURRENT_LIST_DIR}/../aerio
    # Keep the repository root as well for the existing "aerio/..." form.
    ${CMAKE_CURRENT_LIST_DIR}/..
)
target_compile_features(aerio INTERFACE cxx_std_17)

add_executable(aerio_test main.cpp)
target_link_libraries(aerio_test PRIVATE aerio)

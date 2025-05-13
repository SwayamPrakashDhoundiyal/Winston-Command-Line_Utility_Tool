cmake_minimum_required(VERSION 3.10.0)
project(winston VERSION 0.1.0 LANGUAGES C CXX)

add_executable(winston main.cpp function.cpp)

target_include_directories(winston PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/headers")
target_link_libraries(winston PRIVATE bthprops bluetoothapis setupapi pdh )

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

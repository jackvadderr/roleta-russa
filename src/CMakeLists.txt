cmake_minimum_required(VERSION 3.24)
project(rolleta_russa)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)


# Find OpenSSL package
find_package(OpenSSL REQUIRED)

# Include OpenSSL headers
include_directories(${OPENSSL_INCLUDE_DIR})

# Add your project files
add_executable(rolleta_russa main.cpp)

# Link OpenSSL libraries to your executable
target_link_libraries(rolleta_russa ${OPENSSL_LIBRARIES})

cmake_minimum_required(VERSION 3.16)

# Project information
project(2DPhysicsEngine
    VERSION 1.0.0
    DESCRIPTION "A 2D Physics Engine built with C++ and SDL2"
    LANGUAGES CXX C
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set C standard for SDL extensions
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG -O0 -Wall -Wextra")
set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O3 -Wall -Wextra")

# Platform-specific settings
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
endif()

# Include directories
include_directories(include)
include_directories(vendor/SDL2/include)

# Source files
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.c"
    "src/*.h"
)

# Remove any test files from main build
list(FILTER SOURCES EXCLUDE REGEX ".*test.*")
list(FILTER SOURCES EXCLUDE REGEX ".*Test.*")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link directories
link_directories(lib)

# Find and link SDL2
if(WIN32)
    # Windows: Use bundled SDL2
    target_link_libraries(${PROJECT_NAME}
        mingw32
        SDL2main
        SDL2
        SDL2_image
    )
else()
    # Linux/macOS: Find system SDL2
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m)
    target_compile_options(${PROJECT_NAME} PRIVATE ${SDL2_CFLAGS_OTHER} ${SDL2_IMAGE_CFLAGS_OTHER})
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Copy assets to build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Install assets
install(DIRECTORY assets/
    DESTINATION share/${PROJECT_NAME}/assets
)

# Package configuration
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "2D Physics Engine Team")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
else()
    set(CPACK_GENERATOR "TGZ;DEB")
endif()

include(CPack)

# Enable testing
enable_testing()

# Add subdirectories for tests if they exist
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "${PROJECT_NAME} ${PROJECT_VERSION} Configuration Summary")
message(STATUS "=====================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")

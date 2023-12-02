cmake_minimum_required(VERSION 3.10)
project(aserverlibrary VERSION 0.1.1)

# Options
option(DEBUG "Enable debugging" OFF)
option(ADDRESS_SANITIZER "Enable Address Sanitizer" OFF)

set(CMAKE_INSTALL_INCLUDEDIR include)
set(CMAKE_INSTALL_BINDIR bin)
set(CMAKE_INSTALL_DOCDIR share/doc/aserverlibrary)
set(CMAKE_C_STANDARD 99)

# Set C standard
set(CMAKE_C_STANDARD 99)

# Find the required libraries
find_package(amemorylibrary REQUIRED)
find_package(themacrolibrary REQUIRED)
find_package(ajsonlibrary REQUIRED)
find_package(httpparserlibrary REQUIRED)

# Find the libuv package
find_package(libuv REQUIRED)

# Include libuv headers
include_directories(${LIBUV_INCLUDE_DIRS})

# Compiler options
if(ADDRESS_SANITIZER)
    add_compile_options(-fsanitize=address)
endif()

# Source files
set(SOURCE_FILES src/aserver.c)

# Debug library
add_library(aserverlibrary_debug STATIC ${SOURCE_FILES})
target_include_directories(aserverlibrary_debug PUBLIC ${themacrolibrary_INCLUDE_DIRS})
target_include_directories(aserverlibrary_debug PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_definitions(aserverlibrary_debug PUBLIC -D_AML_DEBUG_)
target_compile_options(aserverlibrary_debug PRIVATE -g -O0)
set_target_properties(aserverlibrary_debug PROPERTIES OUTPUT_NAME "aserverlibrary_debug")
target_compile_options(aserverlibrary_debug PRIVATE -g -O0)


# Release library
add_library(aserverlibrary_static STATIC ${SOURCE_FILES})
target_include_directories(aserverlibrary_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(aserverlibrary_static PUBLIC ${themacrolibrary_INCLUDE_DIRS})
target_compile_options(aserverlibrary_static PRIVATE -O3)
set_target_properties(aserverlibrary_static PROPERTIES OUTPUT_NAME "aserverlibrary_static")
target_compile_options(aserverlibrary_static PRIVATE -O3)

# Shared library
add_library(aserverlibrary SHARED ${SOURCE_FILES})
target_include_directories(aserverlibrary PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(aserverlibrary PUBLIC ${themacrolibrary_INCLUDE_DIRS})
target_compile_options(aserverlibrary PRIVATE -O3)
set_target_properties(aserverlibrary PROPERTIES OUTPUT_NAME "aserverlibrary")
target_compile_options(aserverlibrary PRIVATE -O3)

# Link libraries
target_link_libraries(aserverlibrary_debug PUBLIC amemorylibrary_debug ajsonlibrary_debug httpparserlibrary_debug uv_a)
target_link_libraries(aserverlibrary_static PUBLIC amemorylibrary_static ajsonlibrary_static httpparserlibrary_static uv_a)
target_link_libraries(aserverlibrary PUBLIC amemorylibrary ajsonlibrary httpparserlibrary uv)

# Installation of the library
install(TARGETS aserverlibrary aserverlibrary_static aserverlibrary_debug
        EXPORT aserverlibraryTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION include)

# Export the targets
install(EXPORT aserverlibraryTargets
        FILE aserverlibraryTargets.cmake
        DESTINATION lib/cmake/aserverlibrary)

# Create and install the config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "aserverlibraryConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_file(aserverlibraryConfig.cmake.in aserverlibraryConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/aserverlibraryConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/aserverlibraryConfigVersion.cmake"
        DESTINATION lib/cmake/aserverlibrary)

# Installation of other files
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/a-server-library DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES README.md AUTHORS NEWS.md CHANGELOG.md LICENSE NOTICE DESTINATION ${CMAKE_INSTALL_DOCDIR})

# Uninstall command
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/bin/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

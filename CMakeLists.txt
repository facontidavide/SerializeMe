cmake_minimum_required(VERSION 3.7)

project(SerializeMe VERSION 0.2 LANGUAGES CXX)


add_library(SerializeMe INTERFACE)
target_include_directories(SerializeMe INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(SerializeMe INTERFACE cxx_std_17)

################################################################
include(GNUInstallDirs)

install(TARGETS SerializeMe
    EXPORT SerializeMeTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include
)

install(
    EXPORT SerializeMeTargets
    FILE SerializeMeTargets.cmake
    DESTINATION  ${CMAKE_INSTALL_LIBDIR}/cmake/SerializeMe
    NAMESPACE SerializeMe::
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "SerializeMeConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/SerializeMeConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/SerializeMeConfig.cmake"
    INSTALL_DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/SerializeMe
)

install(
    FILES
        ${PROJECT_BINARY_DIR}/SerializeMeConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/SerializeMeConfigVersion.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/SerializeMeConfig
)
################################################################

add_executable(example  example.cpp)
target_link_libraries(example SerializeMe)


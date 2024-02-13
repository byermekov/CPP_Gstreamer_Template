cmake_minimum_required(VERSION 3.0.0)
project(gstreamer_template VERSION 0.1.0)


include(CTest)
enable_testing()


# include headers & cpp

# gstreamer_wrapper
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/gstreamer_wrapper)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/gstreamer_wrapper DIRSRCS)



# src
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)
aux_source_directory(${PROJECT_SOURCE_DIR}/src DIRSRCS)


add_executable(${PROJECT_NAME} ${DIRSRCS})


# Thrid party from vcpkg
message(STATUS "Using fmt installed by vcpkg")
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

message(STATUS "Using jsoncpp installed by vcpkg")
find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)

message(STATUS "Using spdlog installed by vcpkg")
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog spdlog::spdlog_header_only)



# GStreamer installed in system
message(STATUS "Using gstreamer installed in system")
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::GSTREAMER)


# opencv
message(STATUS "Using OpenCV installed in system")
find_package(OpenCV REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})
# if ("${VCPKG_TARGET_TRIPLET}" STREQUAL "arm64-linux")
#     message(STATUS "Using OpenCV installed in system")
#     find_package(OpenCV REQUIRED)
#     target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})
# else()
#     message(STATUS "Using OpenCV installed by vcpkg")
#     set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share/opencv4")
#     find_package(OpenCV REQUIRED)
#     target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})
# endif()





# OpenMP
FIND_PACKAGE(OpenMP REQUIRED)
if(OPENMP_FOUND)
    message(STATUS "Using OpenMP")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

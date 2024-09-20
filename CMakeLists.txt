cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRD ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set (CMAKE_CXX_FLAG "-Wall") 
set(CMAKE_DEBUG_POSTFIX "_d")
# 设置预处理器宏
add_definitions(-D_WIN32_WINNT=0x0601)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Set default build type as Debug")
  set(CMAKE_BUILD_TYPE
        Debug
      CACHE STRING "Build Type" FORCE)
endif()

# import 3rd party deps configured by vcpkg
set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/3rd/vcpkg")

# set cmake tool chain
set(CMAKE_TOOLCHAIN_FILE
    ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
    CACHE STRING "Vcpkg toolchain file")
    set(VCPKG_VERBOSE
    ON
    CACHE BOOL "Vcpkg VCPKG_VERBOSE")

    # set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
    # set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
    # set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

    
set(OUTPUT_MID_PATH $<$<CONFIG:Debug>:/debug>)
set(OUTPUT_PATH ${CMAKE_BINARY_DIR}/${OUTPUT_MID_PATH})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_PATH}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_PATH}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_PATH}/bin)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

link_directories(${OUTPUT_MID_PATH}/lib )

# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already (but later on when
# installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)


set(VCPKG_INCLUDE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
include_directories(${VCPKG_INCLUDE})



project(asio_example)
find_package(Boost REQUIRED COMPONENTS asio)



file(GLOB sources CONFIGURE_DEPENDS "src/*.cpp")
foreach(source IN LISTS sources)
    get_filename_component(name ${source} NAME_WE)
    add_executable(${name} ${source})
    # 如果需要对特定目标文件设置不同的编译选项
    # target_compile_options(${name} PRIVATE /Wp,return-type)
    target_link_libraries(${name} PRIVATE Boost::asio)
    set_target_properties(${name} PROPERTIES DEBUG_POSTFIX _d)
    target_include_directories(${name} PRIVATE ./include)
    # target_include_directories(${name} PRIVATE /usr/include/python3.12)
    # find_package(pybind11 REQUIRED)
    # target_link_libraries(${name} PRIVATE pybind11)
endforeach()


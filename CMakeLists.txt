project(googlepinyin)
cmake_minimum_required(VERSION 2.6)

option(ENABLE_STATIC "Build static library" True)

# 添加模块gflag、glog、GTest
find_package(gflags REQUIRED)
include_directories (${gflags_INCLUDE_DIR})

find_package(glog REQUIRED)
include_directories(${GLOG_INCLUDE_DIRS}) 

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

# 加上下面这行以屏蔽Warning: MACOSX_RPATH is not specified
SET(CMAKE_MACOSX_RPATH 1)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

if(NOT DEFINED LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib)
endif()

set(prefix      ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_PREFIX})
set(bindir ${prefix}/bin)
set(libdir ${LIB_INSTALL_DIR})
set(datadir ${CMAKE_INSTALL_PREFIX}/share)
set(localedir ${CMAKE_INSTALL_PREFIX}/share/locale)
set(includedir  ${CMAKE_INSTALL_PREFIX}/include/googlepinyin)
set(CMAKE_C_FLAGS "-Wall ${CMAKE_C_FLAGS}")

#set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined ${CMAKE_SHARED_LINKER_FLAG}")  # 在mac OS下会报错，用下面这行替换
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-undefined,error ${CMAKE_SHARED_LINKER_FLAG}")


add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(include)
add_subdirectory(data)

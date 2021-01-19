cmake_minimum_required(VERSION 3.17)
project(snippets C)
set(CMAKE_C_STANDARD 99)
set(SOURCE_FILES main.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})


find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB_PKG glib-2.0)
pkg_check_modules(GIO_PKG gio-2.0)

if (GLIB_PKG_FOUND)
    message(Found glib-2.0)
endif()

include_directories(${GIO_PKG_INCLUDE_DIRS})
include_directories(${GLIB_PKG_INCLUDE_DIRS})


target_link_libraries(${PROJECT_NAME} ${GIO_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${GLIB_LIBRARIES})
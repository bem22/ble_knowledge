cmake_minimum_required(VERSION 3.7)
project(snippets C)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)
include(FindPkgConfig)
pkg_check_modules(GLIB glib-2.0 REQUIRED)
include_directories(${GLIB_INCLUDE_DIRS})

pkg_check_modules(GIO gio-2.0 REQUIRED)
include_directories(${GIO_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${GLIB_LDFLAGS})
target_link_libraries(${PROJECT_NAME} ${GIO_LDFLAGS})
cmake_minimum_required(VERSION 3.7)
project(snippets C)

set(CMAKE_C_STANDARD 99)

# PTHREAD sets
set(CMAKE_USE_PTHREADS_INIT true)
set(CMAKE_USE_PTHREADS_INIT ON)
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)
SET(CMAKE_C_FLAGS "-pthread -lrt ${CMAKE_C_FLAGS}")


set(SOURCE_FILES panoramalabs-daemon.c panoramalabs-agent.c panoramalabs-agent.h panoramalabs-service.c panoramalabs-service.h)

include(FindPkgConfig)

pkg_check_modules(GIO-UNIX gio-unix-2.0 REQUIRED)
include_directories(${GIO-UNIX_INCLUDE_DIRS})

pkg_check_modules(GLIB glib-2.0 REQUIRED)
include_directories(${GLIB_INCLUDE_DIRS})

pkg_check_modules(GIO gio-2.0 REQUIRED)
include_directories(${GIO_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${GIO-UNIX_LDFLAGS})
target_link_libraries(${PROJECT_NAME} ${GLIB_LDFLAGS})
target_link_libraries(${PROJECT_NAME} ${GIO_LDFLAGS})
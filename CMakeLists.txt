cmake_minimum_required(VERSION 3.16)

project(evsieve-wrapper
		VERSION 1.0.0
		LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

find_package(Qt6 REQUIRED Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

pkg_check_modules(LIBEVDEV REQUIRED libevdev)

add_executable(${PROJECT_NAME})

add_subdirectory("src")

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBEVDEV_LIBRARIES})

target_include_directories(${PROJECT_NAME} PRIVATE ${LIBEVDEV_INCLUDE_DIRS})
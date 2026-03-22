target_sources(${PROJECT_NAME} PRIVATE "main.cpp")

add_subdirectory("core")
add_subdirectory("gui")
add_subdirectory("dev")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
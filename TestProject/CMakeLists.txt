file(GLOB TEST_PROJECT_SOURCES ./*.cpp)
file(GLOB TEST_PROJECT_HEADERS ./*.hpp)

add_executable(TestProject ${TEST_PROJECT_SOURCES} ${TEST_PROJECT_HEADERS})
target_link_libraries(TestProject PUBLIC CGE)

target_include_directories(TestProject PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/CGE"
    )

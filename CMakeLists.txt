cmake_minimum_required(VERSION 3.15)
project(FileBroadcaster CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# Version string baked into the binary; the release workflow overrides this
# from the git tag (e.g. -DFILEBROADCASTER_VERSION=v1.0.0).
set(FILEBROADCASTER_VERSION "1.0.0" CACHE STRING "Version string for --version output")

# --- main executable ---------------------------------------------------------

add_executable(FileBroadcaster
    src/Main.cpp
    src/Sender.cpp
    src/Receiver.cpp
    src/Config.cpp
)

target_include_directories(FileBroadcaster PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/cxxopts/include
)

target_compile_definitions(FileBroadcaster PRIVATE
    FILEBROADCASTER_VERSION="${FILEBROADCASTER_VERSION}"
)

if(MSVC)
    target_compile_options(FileBroadcaster PRIVATE /W4 /permissive-)
else()
    target_compile_options(FileBroadcaster PRIVATE -Wall -Wextra)
endif()

if(WIN32)
    target_link_libraries(FileBroadcaster PRIVATE ws2_32)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(FileBroadcaster PRIVATE Threads::Threads)
endif()

# --- tests -------------------------------------------------------------------

option(BUILD_TESTING "Build the unit-test target" ON)

if(BUILD_TESTING)
    enable_testing()
    find_package(GTest QUIET)
    if(GTest_FOUND)
        add_executable(GTests tests/Tests.cpp)
        target_link_libraries(GTests PRIVATE GTest::gtest)
        if(NOT WIN32)
            find_package(Threads REQUIRED)
            target_link_libraries(GTests PRIVATE Threads::Threads)
        endif()
        add_test(NAME unit COMMAND GTests)
    else()
        message(STATUS "GTest not found — skipping GTests target")
    endif()

    # E2E loopback test (Linux/macOS only — Winsock SO_REUSEADDR semantics
    # break two-process loopback, see tests/e2e.sh comment).
    if(NOT WIN32)
        add_test(
            NAME e2e
            COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/e2e.sh
        )
        set_tests_properties(e2e PROPERTIES
            ENVIRONMENT "BINARY=$<TARGET_FILE:FileBroadcaster>"
        )
    endif()
endif()

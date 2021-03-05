cmake_minimum_required(VERSION 3.4.1)

file(GLOB source_files
        "./src/*.cpp"
        )

include_directories(include)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows" )
    include_directories(include.MVSDK.win)
else()
    include_directories(include.MVSDK)
endif()

add_library(libCamera STATIC ${source_files})
target_include_directories(libCamera PRIVATE ../libLED/include)
target_link_libraries(libCamera)

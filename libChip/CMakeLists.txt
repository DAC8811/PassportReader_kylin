cmake_minimum_required(VERSION 3.4.1)

file(GLOB source_files
        "./src/*.cpp"
        )

include_directories(include)
add_library(libChip STATIC ${source_files})
target_include_directories(libChip PRIVATE ../libLED/include)
target_link_libraries(libChip glog)

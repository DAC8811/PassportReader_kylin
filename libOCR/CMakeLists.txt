cmake_minimum_required(VERSION 3.4.1)

file(GLOB source_files
        "./src/*.cpp"
        )

include_directories(include)
add_library(libOCR STATIC ${source_files})
target_link_libraries(libOCR)

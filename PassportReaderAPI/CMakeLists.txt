project(PassportReaderAPI)
cmake_minimum_required(VERSION 3.4.1)

file(GLOB source_files
        "./src/*.cpp"
        )

add_library(getData SHARED ${source_files})
#add_executable(getData ${source_files})

target_include_directories(getData BEFORE PRIVATE include)

#target_include_directories(getData PRIVATE ../libCamera/include)
#if (CMAKE_SYSTEM_NAME STREQUAL "Windows" )
#    target_include_directories(PassportReaderAPI PRIVATE ../libCamera/include.MVSDK.win)
#else()
#    target_include_directories(PassportReaderAPI PRIVATE ../libCamera/include.MVSDK)
#endif()

target_include_directories(getData PRIVATE ../libChip/include)
target_include_directories(getData PRIVATE ../libLED/include)
target_include_directories(getData PRIVATE ../libImageProcess/include)
target_include_directories(getData PRIVATE ../libCheck/include)
if (NOT USE_OLD_OCR)
    target_include_directories(getData PRIVATE ../libOCR/include)
else()
    target_include_directories(PassportReaderAPI PRIVATE ../libOCR.old/include)
endif()
#target_include_directories(PassportReaderAPI PRIVATE ../libCheck/include)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows" )
    #windows
    target_link_libraries(PassportReaderAPI libCamera libChip libLED libOCR ${OpenCV_LIBS} usb-1.0 openjp2 crypto crypt32 glog MVCAMSDK_X64)
elseif (ANDROID_TOOLCHAIN)
    #android
    target_link_libraries(getData libImageProcess libChip libLED libOCR libCheck opencv usb-1.0 openjp2 crypto)
else()
    # linux
    target_link_libraries(getData libImageProcess libChip libLED libOCR libCheck ${OpenCV_LIBS} usb-1.0 openjp2 glog ${OPENSSL_CRYPTO_LIBRARY})
    if (BUILD_WITH_IDR220S)
        message (" building with idr220s ")
        add_compile_definitions(WITH_IDR220S)
        target_link_libraries(PassportReaderAPI idr220s_reader)
    elseif (BUILD_WITH_EST100)
        message (" building with est100 ")
        add_compile_definitions(WITH_EST100)
        target_link_libraries(PassportReaderAPI wlt est100)
    else()
        message (" not using any sfz decode lib ")
    endif()
endif()

install(TARGETS getData
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static)

install(FILES ${PROJECT_SOURCE_DIR}/include/getData.h DESTINATION include)

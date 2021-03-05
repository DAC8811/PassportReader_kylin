cmake_minimum_required(VERSION 3.4.1)
project (PassportReaderC)

set(CMAKE_CXX_STANDARD 14)

set(api_header_files
        "${CMAKE_CURRENT_SOURCE_DIR}/PassportReaderAPI/include/"
        )

message("================================================================")
message("Building with ${CMAKE_SYSTEM_NAME} and ${CMAKE_SYSTEM_PROCESSOR}")

# 根据不同操作系统，选择不同的编译选项
if (ANDROID_TOOLCHAIN)
    #add_compile_definitions(__ANDROID__)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__ANDROID__")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__ANDROID__")
    file(GLOB 3rdparty_header_files
            "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/include/"
            )
    include_directories(${3rdparty_header_files} ${api_header_files})# 向工程添加多个指定头文件的搜索路径,因为main中使用的是相对路径
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -ftree-vectorize -fexceptions -fPIC -fdata-sections -ffunction-sections -Wl,--gc-sections -fvisibility=hidden")#-flto
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffast-math -ftree-vectorize  -fPIC -fdata-sections -ffunction-sections -Wl,--gc-sections -fvisibility=hidden")#-flto
    find_package(OpenCV 4 REQUIRED)
    find_package(OpenSSL REQUIRED)
    find_package(OpenJPEG REQUIRED)
    #find_path(LIBUSB_INCLUDE_DIR
    #    NAMES libusb.h
    #    PATH_SUFFIXES "include" "libusb" "libusb-1.0")
    add_library(usb-1.0 SHARED IMPORTED)
    set_target_properties(usb-1.0 PROPERTIES
    	IMPORTED_LOCATION "${CMAKE_FIND_ROOT_PATH}/lib/libusb-1.0.so")
	add_library(glog SHARED IMPORTED)
    set_target_properties(glog PROPERTIES
    	IMPORTED_LOCATION "${CMAKE_FIND_ROOT_PATH}/lib/libglog.so.0.0.0")
    include_directories(${OpenCV_INCLUDE_DIRS} ${LIBUSB_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${GLOG_INCLUDE_DIR} ${api_header_files} ${OPENJPEG_INCLUDE_DIRS}) 


    if (CMAKE_SYSTEM_NAME STREQUAL "Windows" )
        add_compile_definitions(__WINDOWS__)
    else()
        if (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch" )
            add_compile_definitions(__LINUX_AARCH__)
            message (" building on aarch, disable things to make it faster ")
            set(USE_OLD_OCR TRUE)
            set(BUILD_WITH_IDR220S TRUE)
        elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64" )
            add_compile_definitions(__LINUX_AARCH64__)
            # message (" building on x86-64, use est100 to decode sfz image ")
            # set(BUILD_WITH_EST100 TRUE)
        elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
            add_compile_definitions(__LINUX_X86_64__)
            # message (" building on x86-64, use est100 to decode sfz image ")
            # set(BUILD_WITH_EST100 TRUE)
        endif()
    endif()
endif ()

message("CXXFLAGS ${CMAKE_CXX_FLAGS}")
message("CFLAGS ${CMAKE_C_FLAGS}")

add_subdirectory(libLED)#向当前工程添加存放源文件的子目录，并可以指定中间二进制和目标二进制的存放位置
#add_subdirectory(libCamera)
add_subdirectory(libChip)
add_subdirectory(libImageProcess)
add_subdirectory(libCheck)
if (USE_OLD_OCR)
    message (" building with old ocr lib ")
    add_compile_definitions(USE_OLD_OCR)
    add_subdirectory(libOCR.old)
else()
    message (" building with new CNN ocr lib ")
    add_subdirectory(libOCR)
endif()
#add_subdirectory(libCheck)
add_subdirectory(PassportReaderAPI)
if (NOT ANDROID_TOOLCHAIN)
    #add_subdirectory(PassportReaderDEMO) #测试工程，不需要参与编译
endif ()

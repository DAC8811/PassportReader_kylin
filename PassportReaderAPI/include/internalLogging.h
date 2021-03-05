//
// Created by Comic Chang on 2018/8/24.
//

#ifndef PASSPORTREADER_INTERNAL_H
#define PASSPORTREADER_INTERNAL_H

#ifdef __ANDROID__
  #include <android/log.h>
  #ifndef LOG_TAG
    #define LOG_TAG "PassportReaderCPP"
  #endif
  #ifndef LOGD
  #ifdef LOG_DEBUG
    #define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG ,__VA_ARGS__) // 定义LOGD类型
    #define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG ,__VA_ARGS__) // 定义LOGD类型
  #else
    #define LOGV(...) asm("nop")
    #define LOGD(...) asm("nop")
    #define LOGI(...) asm("nop")
    #define LOGW(...) asm("nop")
    #define LOGE(...) asm("nop")
    #define LOGF(...) asm("nop")
  #endif
//  #define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG ,__VA_ARGS__) // 定义LOGI类型
//  #define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG ,__VA_ARGS__) // 定义LOGW类型
//  #define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG ,__VA_ARGS__) // 定义LOGE类型
//  #define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,LOG_TAG ,__VA_ARGS__) // 定义LOGF类型
  #endif
#else
  #define GLOG_NO_ABBREVIATED_SEVERITIES
  #include <glog/logging.h>
  #include <stdio.h>
  using std::endl;
  //#define DEBUG
  extern char LOGBUFFER[300];

  #ifdef LOG_DEBUG
    #define LOGV(...) snprintf(LOGBUFFER, 300, __VA_ARGS__) && LOG(INFO) << LOGBUFFER << endl
    #define LOGD(...) snprintf(LOGBUFFER, 300, __VA_ARGS__) && LOG(INFO) << LOGBUFFER << endl
  #else
    #define LOGV(...) asm("nop")
    #define LOGD(...) asm("nop")
  #endif
  #define LOGI(...) snprintf(LOGBUFFER, 300, __VA_ARGS__) && LOG(INFO) << LOGBUFFER << endl
  #define LOGW(...) snprintf(LOGBUFFER, 300, __VA_ARGS__) && LOG(WARNING) << LOGBUFFER << endl
  #define LOGE(...) snprintf(LOGBUFFER, 300, __VA_ARGS__) && LOG(ERROR) << LOGBUFFER << endl
  #define LOGF(...) snprintf(LOGBUFFER, 300, __VA_ARGS__) && LOG(FATAL) << LOGBUFFER << endl
#endif

#endif //PASSPORTREADER_INTERNAL_H

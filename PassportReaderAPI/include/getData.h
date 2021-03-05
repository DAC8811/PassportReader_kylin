//
// Created by 张迪 on 2020/3/21.
//

#ifndef PASSPORTREADER_GETDATA_H
#define PASSPORTREADER_GETDATA_H
#include <string>

using namespace std;

#ifdef __ANDROID__
#define EXPORT_FUN __attribute__((visibility("default")))
#else
#define EXPORT_FUN __attribute__((visibility("default")))
#endif

/******************************************************/
// 函数名   : initProgram
// 功能描述 : 设备SDK初始化，在调用任何SDK其他接口前，必须先调用该接口进行初始化。该函数在整个进程运行期间只需要调用一次。
// 参数     : WorkingFolder 工作目录，字符串
// 返回值   : 成功时，返回0
//            否则参考日志输出
/******************************************************/
extern "C" EXPORT_FUN
int initProgram(const char* workingFolder);
/******************************************************/
// 函数名   : getData
// 功能描述 :采集数据（Ocr、读卡)
// 参数     :irPath 红外图像完整路径
// 返回值   :0 护照读取成功；-1 失败
//
/******************************************************/
extern "C" EXPORT_FUN
int getData(const char* irPath);
/******************************************************/
// 函数名   :closeProgram
// 功能描述 :关闭设备
// 参数     :无
//
// 返回值   :0 成功； -1 失败
//
/******************************************************/
extern "C" EXPORT_FUN
int closeProgram();
/******************************************************/
// 函数名   :getJsonNew
// 功能描述 :获取证件各项信息
// 参数     :无
// 返回值   :Json字符串
//
/******************************************************/
extern "C" EXPORT_FUN
const char* getJsonNew();
extern "C" EXPORT_FUN
string getEChipDG1();
extern "C" EXPORT_FUN
string getEChipDG11();
extern "C" EXPORT_FUN
const char* getSFZImage();
extern "C" EXPORT_FUN
/******************************************************/
// 函数名   :getAuthenticity
// 功能描述 :获取护照的鉴伪结果
// 参数     :whitePath 护照的白光图像 路径
//           irPath  护照的红外图像路径
//           uvPath  护照的紫外图像路径
// 返回值   :1  鉴伪结果为真     0 鉴伪结果为假
//
/******************************************************/
int getAuthenticity(const char* whitePath, const char* irPath, const char* uvPath);
#endif //PASSPORTREADER_GETDATA_H

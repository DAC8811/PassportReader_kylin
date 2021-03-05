#pragma once
#include <string>
//#include <Cstring>
//#include <windows.h>
#include <opencv2/core/core.hpp> 
//#include "atlstr.h"
//#include <io.h>
#include <string>

//////////////////////////////////////////////////////////////////
/*-------------------文件处理类操作的头文件---------------------*/
//////////////////////////////////////////////////////////////////

/*char型数字转int型数字的模块*/
int str2int(std::string strNum);
/*获取dll绝对路径的递归模块*/
//HMODULE GetSelfModuleHandle();
/*计算文件夹内数据个数的模块*/
int FileVisit(std::string filepath, int layer);

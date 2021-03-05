#pragma once

#include <iostream> 
#include <cstring>
#include <string.h>
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>  

using namespace cv;
using namespace std;

//////////////////////////////////////////////////////////////////
/*-------------------检测紫外图像的头文件-------------------*/
//////////////////////////////////////////////////////////////////

/*创建二维数组*/
int ** CreateArray(int dim1, int dim2);
/*注销二维数组*/
void DeleteArray(int ** Array, int dim1);
/*直方图均衡*/
Mat ContrastStretch_New(Mat src);
/*紫外图像中的图案与白纸判别模块*/
int UV_Search(Mat& src);
/*紫外图案二值化*/
Mat UV_PROSCESS(Mat& src, int value);
/*紫外图案检查*/
int UV_Check(Mat& src);


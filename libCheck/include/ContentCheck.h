#pragma once


#include <iostream> 
#include <cstring>
#include <string>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>  
#include "Country.h"
#include "ImageCode.h"
#include "FileProcess.h"
//#include "tchar.h"
#include "UVCheck.h"


using namespace cv;
using namespace std;
//////////////////////////////////////////////////////////////////
/*-----------------------主程序的头文件-------------------------*/
//////////////////////////////////////////////////////////////////
//负责完成各项检测操作
extern "C"
{ 

	class  ContentCheck
	{
	public:
		//////////////////////////////////////////////////////////////////
		/*-------------------------顶层模块-----------------------------*/
		//////////////////////////////////////////////////////////////////
		/*旧版本接口*/
		//int SourceSeperate(char* ImageLoc, char* nation, char* type, int mode);
		/*新版本接口*/
		int SourceSeperate_New(char* WIImageLoc, char* NIRImageLoc, char* UVImageLoc, char* nation, char* CodePath,int(&RCode)[14]);
		/*新版本接口*/
		int UVImageCheck(char* UVImageLoc);
	private:

		//////////////////////////////////////////////////////////////////
		/*------------------------第二级模块----------------------------*/
		//////////////////////////////////////////////////////////////////

		/*-----------------------紫外模块部分---------------------------*/
		/*检测紫外图像中机读码区域*/
		int Mode1_Data(Mat& src, int nation, int type, char* ImageLoc);
		/*检测紫外图像中花纹并鉴定真伪*/
		int Mode2_Data(Mat& src, int nation, int type, char* ImageLoc);
		/*检测紫外图像中证件码*/
		int Mode3_Data(Mat& src, int nation, int type, char* ImageLoc);
		/*检测紫外图像中人眼识别信息*/
		int Mode4_Data(Mat& src, int nation, int type, char* ImageLoc);
		/*检测紫外图像中人脸照片*/
		int Mode5_Data(Mat& src, int nation, int type, char* ImageLoc);
		/*检测紫外图像中人脸花纹背景*/
		int Mode6_Data(Mat& src, int nation, int type, char* ImageLoc);
		/*检测紫外图像中证件标识*/
		int Mode7_Data(Mat& src, int nation, int type, char* ImageLoc);
		/*检测紫外图像中机器读码区的荧光纤维*/
		int Mode8_Data(Mat& src, int nation, int type, char* ImageLoc);//青色
		int Mode9_Data(Mat& src, int nation, int type, char* ImageLoc);//亮蓝色
		int Mode10_Data(Mat& src, int nation, int type, char* ImageLoc);//绿色
		int Mode11_Data(Mat& src, int nation, int type, char* ImageLoc);//红色
		/*检测紫外图像中照片区的荧光纤维*/
		int Mode12_Data(Mat& src, int nation, int type, char* ImageLoc);//绿色
		int Mode13_Data(Mat& src, int nation, int type, char* ImageLoc);//亮蓝色

		/*------------------------红外模块部分--------------------------*/
		/*检测红外图像中机读码的部分编码*/
		int Mode14_Data(Mat& src, int nation, int type, char* ImageLoc);
		/*检测红外图像中证件码*/
		int Mode15_Data(Mat& src, int nation, int type, char* ImageLoc);
		/*检测红外图像中个人信息*/
		int Mode16_Data(Mat& src, int nation, int type, char* ImageLoc);
		/*检测红外图像中个人照片*/
		int Mode17_Data(Mat& src, int nation, int type, char* ImageLoc);
		/*检测红外图像中证件标识*/
		int Mode18_Data(Mat& src, int nation, int type, char* ImageLoc);
		/*检测机读码的奇偶校验位*/
		int Mode19_Data(char* MRZ);

		/*------------------------合并模块部分--------------------------*/
		/*个人照片检测模块：模块5与6的合并模块*/
		int Mode_Mix_5_6(Mat& src, int nation, int type, char* ImageLoc);
		/*机器读码区域荧光纤维检测模块：模块8、9与10的合并模块*/
		int Mode_Mix_8_9_10_11(Mat& src, int nation, int type, char* ImageLoc);
		/*个人照片区域荧光纤维检测模块：模块11、12与13的合并模块*/
		int Mode_Mix_12_13(Mat& src, int nation, int type, char* ImageLoc);

		//////////////////////////////////////////////////////////////////
		/*------------------------第三级模块----------------------------*/
		//////////////////////////////////////////////////////////////////

		/*------------机读码检测相关模块（包含紫外和红外）--------------*/
		/*紫外图像中机读码检测模块*/
		int MRZ_UV(const Mat& src, int select);
		/*红外图像中机读码部分编码检测模块*/
		int Authenticator_NIR(const Mat& src);
		/*机读码校验位检测模块*/
		int MRZParityCheck1(int* MRZ, int start, int end);
		int MRZParityCheck2(int* MRZ);
		/*机读码二值化模块*/
		int MRZ_UV_PROSCESS(const Mat& src, Mat& dst, int select);
		int MRZ2_UV_PROSCESS(const Mat& src, Mat& dst, int select);//已失效
		/*紫外图像中机读码块分割总模块*/
		bool UV_ImageSegmentation(const Mat& src, Mat& dst1, Mat& dst2);
		/*紫外图像中机读码块垂直分割模块*/
		bool VerticalSegmentattion(const Mat& src, int* MatPro, Mat& dst, int param1, int param2);
		/*紫外图像中机读码块水平分割模块*/
		bool HorizonSegmentattion(const Mat& src, int* MatPro, Mat& dst1, Mat& dst2);
		
		/*----------------------花纹检测相关模块------------------------*/
		/*紫外图像中的花纹检测总模块*/
		int Pattern_UV(const Mat& src, int VALID_MAX, int VALID_MIN, int value);
		/*紫外图像中的花纹匹配模块*/
		int PatternMtach_UV(Mat& src, int nation, char const* ImageLoc);
		/*紫外图像中的花纹与荧光纤维判别模块*/
		int Pattern_Search_UV(const Mat& src);
		/*图像匹配模块*/
		double ImageMatch(Mat src1, Mat src2);
		/*图像锐化模块*/
		int IFDO(Mat src, Mat& dst);
		/*紫外图像中荧光纤维检测模块*/
		int Thread_UV(Mat& src, int select);
		/*荧光纤维二值化模块*/
		int Thread_UV_Process(Mat& src, Mat& dst, int mode);
		/*紫外花纹二值化模块*/
		Mat Pattern_UV_PROSCESS(const Mat& src, int value);
		Mat Pattern_UV_PROCESS2(const Mat& src, int param1, int value, int mode);

		/*不同国家特殊紫外花纹检测模块*/
		int Circle_Detect_USA(Mat & src);
		int Pattern_Check_CHN_E(Mat& src, int value);
		int Pattern_Check_CAN(Mat& src);
		int Pattern_Check_IDN(Mat& src);
		int Pattern_Check_PAN(Mat& src);
		int Pattern_Check_NPL(Mat& src);
		int Pattern_Check_HND(Mat& src);
		int Pattern_Check_MDA(Mat& src);
		int Pattern_Check_MUS(Mat& src);
		int Pattern_Check_BLZ(Mat& src);
		int Pattern_Check_AUS(Mat& src);
		int Pattern_Check_ARG(Mat& src);
		int Pattern_Check_BEL(Mat& src);
		int Pattern_Check_IRL(Mat& src);
		int Pattern_Check_GRC(Mat& src);
		int Pattern_Check_MAR(Mat& src);
		int Pattern_Check_LSO(Mat& src);
		int Pattern_Check_LIE(Mat& src);

		/*---------------------其他紫外检测相关模块----------------------*/
		/*紫外证件码检测模块*/
		int PassNum_UV(const Mat& src, int select1, int select2);
		/*紫外人脸检测模块*/
		int Photo_Shape_UV(const Mat& src, int VALID_MAX, int VALID_MIN);
		/*紫外人脸照片背景花纹检测模块*/
		int Photo_BG_UV(const Mat& src, int VALID_MAX, int VALID_MIN);
		Mat PHOTO_UV_PROSCESS(const Mat& src);//无效
		int Photo_Color_UV(const Mat& src, int mode);//无效
		/*紫外人脸照片背景荧光纤维检测模块*/
		int Pattern_Thread_UV(const Mat& src, int select1, int select2, int select3);
		/*紫外人眼识别信息检测模块*/
		int DATA_UV(const Mat& src);
		/*紫外证件标识（大型证件码）检测模块*/
		int Pattern_Code_UV(const Mat& src);

		/*---------------------其他红外检测相关模块----------------------*/
		/*红外人眼识别信息检测模块*/
		int DATA_NIR(const Mat& src);
		/*红外证件码检测模块*/
		int PassNum_NIR(const Mat& src);
		/*红外姓氏检测模块*/
		int FamilyName_NIR(const Mat& src);//无效
		/*红外人脸照片检测模块*/
		int Photo_NIR(const Mat& src);
		/*红外证件标识检测模块*/
		int Pattern_Code_NIR(const Mat& src);

		/*--------------------------底层模块-----------------------------*/
		/*计数模块*/
		int PointCount(const Mat& src);
		/*判断数据有效性模块*/
		bool CheckValid(const Mat& src, int ValidMax, int VAlidMin);


		/*红外机读码个人码区分割总模块*/
		bool AT_NIR_ImageSegmentation(const Mat& src, Mat& dst);
		/*红外机读码个人码区水平分割模块*/
		bool AT_HorizonSegmentattion(const Mat& src, int* MatPro, Mat& dst, int param1, int param2);
		bool AT_HorizonSegmentattion2(const Mat& src, int* MatPro, Mat& dst, int param1, int param2);
		/*红外机读码个人码区垂直分割模块*/
		bool AT_VerticalSegmentattion(const Mat& src, int* MatPro, Mat& dst);
		

		/*紫外证件码区分割总模块*/
		bool PN_UV_ImageSegmentation(const Mat& src, Mat& dst);
		/*紫外证件码区水平分割模块*/
		bool PN_HorizonSegmentattion(const Mat& src, int* MatPro, Mat& dst);
		

		/*红外证件码区分割总模块*/
		bool PN_NIR_ImageSegmentation(const Mat& src, Mat& dst);
		/*红外证件码区水平分割模块*/
		bool PN_NIR_HorizonSegmentattion(const Mat& src, int* MatPro, Mat& dst);
		

		/*红外姓氏区分割总模块*/
		bool FN_NIR_ImageSegmentation(const Mat& src, Mat& dst);
		/*红外姓氏区垂直分割模块*/
		bool FN_VerticalSegmentattion(const Mat& src, int* MatPro, Mat& dst);		


		/*人眼识别信息区分割总模块*/
		bool DATA_ImageSegmentation(const Mat& src, Mat& dst, int param1, int param2);


		/*水平投影模块*/
		void HorizonProjection(const Mat& src, int* dst);
		/*垂直投影模块*/
		void VerticalProjection(const Mat& src, int* dst);

		/*图像灰度拉伸模块*/
		Mat ContrastStretch(Mat src);
		/*红外人眼识别信息区的二值化模块*/
		Mat DATA_NIR_PROSCESS(const Mat& src);
		/*红外人脸轮廓的二值化模块*/
		Mat PHOTO_NIR_PROSCESS(const Mat& src);

		/*特定颜色通道下的二值化模块*/
		int RGB_Red_Process(Mat& src, Mat& dst);//红色通道
		int RGB_Green_Process(Mat& src, Mat& dst);//绿色通道
		int RGB_Blue_Process(Mat& src, Mat& dst);//蓝色通道
		int RGB_Black_Process(Mat& src, Mat& dst);//黑色通道
		

		/*-------------------------特殊附加模块---------------------------*/
		/*红外机读码区定位模块*/
		int Location_NIR(const Mat src, int& DstX, int& DstY);
		/*紫外机读码区定位模块*/
		int Location_UV(const Mat src, int& DstX, int& DstY, int select);


		/*机读码信息提取模块*/
		int GetInfo(char* MRZ, char* nation, char* type);
		int DateDetect(int* num, char* date);//无效
		/*国家码转码模块*/
		int NationDetect(char* nation);
		/*护照类型转码模块*/
		int PassportType(char* type);
		
		
		/*图像校正模块*/
		int Image_Modification(Mat& WIImg, Mat& UVImg, Mat& NIRImg);
		/*图像映射模块*/
		void ImageTransform(Mat const& src, Mat& dst, int dx, int dy);
		/*旋转角度计算模块*/
		int GetTheta(Mat src, double & theta, int & dev);
		/*图像旋转模块*/
		Mat ImageRotate(Mat src, double theta);
		/*红外机读码二值化模块*/
		Mat MRZ_NIR_PROSCESS(const Mat& src);
		
		/*图像错误信息保存模块*/
		int SaveError(char* ImageLoc, char* ImageName, Mat src, int PointX, int PointY, int width, int height);

		//int PatternMtach_UV(const Mat& src1, const Mat& src2);
		//int Pattern_Check_SAU(Mat& src);
		//int ImageSeg(const Mat& src, Mat& dst, int SegNum);
		//int HanmingDistance(string &str1, string &str2);
		//string pHashValue(Mat &src);
		//int Mode18_Data(Mat& src, int nation, int type);
		//int Mode_Select(int mode, Mat& src, int param1, int param2, int param3, int param4);

		//int CHN_E_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3);
		//int CHN_G_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3, int& param4);
		//int CHN_W_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3);
		//int CHN_P_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3);
		//int AIT_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3, int& param4);
		//int NLD_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3);
		//int USA_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3);
		//int AUS_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3);
		//int JPN_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3);
		//double ssim(Mat &i1, Mat & i2);

		bool CHN_E(cv::Mat wi, cv::Mat uv, cv::Mat ir, char* mrz, int nationCode, int typeCode, int(&rCode)[14], char* CodePath);

		int PassportCodeAuthentication(cv::Mat wi, cv::Mat uv, cv::Mat ir, int mode);
		int MicroFaceAuthentication(cv::Mat wi, cv::Mat ir);

		bool PassportCodeArea(cv::Mat& wi, cv::Mat& uv, cv::Mat& ir);
		void MicroFaceArea(cv::Mat& wi, cv::Mat& ir);

		Mat CHAR_WI_PROSCESS(const cv::Mat& src, int scale, int value, int mode = 1);
		Mat CHAR_UV_REMOVE_COLOR(const cv::Mat& src, int scale, int value, int mode = 3);
		bool HorizonProjection_Vector(const cv::Mat& src, std::vector<int>& dst);
		bool HorizonSegmentation_Vector(std::vector<int>& hp, int& topEdge, int& bottomEdge);
		bool VerticalProjection_Vector(const cv::Mat& src, std::vector<int>& hp);
		bool VerticalSegmentation_Vector(std::vector<int>& vp, int& rightEdge, int& leftEdge);
		bool CharAreaFind(const cv::Mat& wi, int& topEdge, int& bottomEdge, int& rightEdge, int& leftEdge);
		bool CharAreaRsize(const cv::Mat& src, cv::Mat& wi, cv::Mat& uv, cv::Mat& ir);

		Mat CHAR_UV_PROSCESS_HSV(const Mat& src, int mode);
	
	};
}
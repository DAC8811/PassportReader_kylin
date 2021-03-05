#pragma once

#include <string>
#include <opencv2/highgui/highgui.hpp>

//////////////////////////////////////////////////////////////////
/*-------------------读取和写入文件的头文件---------------------*/
//////////////////////////////////////////////////////////////////

class imageCoder {
public:

	imageCoder();
	~imageCoder();

	/*图像编码模块*/
	int img2code(cv::Mat img);	//The input img should be binary. 
	/*解码模块*/
	int code2img();
	/*文件读取的模块*/
	int exportCode(std::string filePath);
	/*文件写入的模块*/
	int importCode(std::string filePath);

	//private:
	int raw2rl();	//Raw code 2 run length code.
	int rl2raw();

	//private:
	cv::Mat m_rawImg;
	std::string m_rawCode;
	std::string m_rlCode;
	std::string m_head;  //The head include the information for height and width of the image.
};

#include <fstream>
#include <sstream>
#include <iostream>

#include "ImageCode.h"
#include "FileProcess.h"

using namespace std;
using namespace cv;


imageCoder::imageCoder():
	m_rawImg(Mat()),m_head(""),m_rawCode(""),m_rlCode("")
{
}


imageCoder::~imageCoder()
{
	m_rawImg.release();
	m_head.clear();
	m_rawCode.clear();
	m_rlCode.clear();
}


/*图像编码模块*/
int imageCoder::img2code(cv::Mat img)
{
	string height = to_string(img.rows);
	string width = to_string(img.cols);
	m_head = height + " " + width;

	m_rawCode = "";
	for (int i = 0; i < img.rows; ++i)
	{
		for (int j = 0; j < img.cols; ++j)
		{
			if (img.at<uchar>(i, j) > 0)
				m_rawCode.push_back('1');
			else
				m_rawCode.push_back('0');
		}
	}
	raw2rl();
	return 0;
}

/*解码模块*/
int imageCoder::code2img()
{
	string heightStr(""), widthStr("");
	stringstream ss;
	ss << m_head;
	ss >> heightStr;
	ss >> widthStr;
	int height = str2int(heightStr);
	int width = str2int(widthStr);
	Mat img(height, width, CV_8UC1);
	MatIterator_<uchar> it = img.begin<uchar>();
	//for (int i = 0; i < m_rawCode.size(); ++i, ++it)
	for (int i = 0; i < (height*width); ++i, ++it)
		*it = m_rawCode[i] == '0' ? 0 : 255;



	m_rawImg = img.clone();
	return 0;
}

/*文件读取的模块*/
int imageCoder::exportCode(std::string filePath)
{

	ofstream out(filePath);
	out << m_head << endl;
	out << m_rlCode;
	out.close();
	return 0;
}

/*文件写入的模块*/
int imageCoder::importCode(std::string filePath)
{
	ifstream in(filePath);
	if (!in.good())
		return -1;
	m_head = "";
	m_rlCode = "";
	getline(in, m_head);
	while (!in.eof())
		m_rlCode.push_back(in.get());
	rl2raw();
	in.close();
	return 0;
}

int imageCoder::raw2rl()
{
	m_rlCode = "";
	char digit;
	for (int i = 0; i < m_rawCode.size();)
	{
		digit = m_rawCode[i];
		int number = 0;
		while (m_rawCode[i] == digit)
		{
			i++;
			number++;
		}
		string numStr = to_string(number);
		m_rlCode.push_back('#');
		m_rlCode.push_back(digit);
		m_rlCode += numStr;
	}
	return 0;
}

int imageCoder::rl2raw()
{
	m_rawCode = "";
	char digit;
	for (int i = 0; i < m_rlCode.size();)
	{
		i++;
		digit = m_rlCode[i];
		i++;
		int subStrHead = i;
		while (i < m_rlCode.size() && m_rlCode[i] != '#')
			i++;
		string numStr = m_rlCode.substr(subStrHead, i - subStrHead);
		int num = str2int(numStr);
		for (num; num > 0; num--)
			m_rawCode.push_back(digit);
	}
	return 0;
}


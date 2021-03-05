#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

static const char charTable[37] = {
	'0', '1', '2', '3', '4', '5', '6',
	'7', '8', '9', 'A', 'B', 'C', 'D',
	'E', 'F', 'G', 'H', 'I', 'J', 'K',
	'L', 'M', 'N', 'O', 'P', 'Q', 'R',
	'S', 'T', 'U', 'V', 'W', 'X', 'Y',
	'Z', '<'};

cv::Mat findMrz(const cv::Mat & img, int type);
int findMRZ(cv::Mat image,std::vector<cv::Mat>& result);
class MRZ
{
public:
	MRZ();
	~MRZ();
    int initNet(std::string modelPath);

public:
	//0 普通 1卡牌单行 2卡牌三行
	std::string OCR(const cv::Mat &img, int type);
    int getMrzNums(const cv::Mat &img);
#ifdef _DEBUG
public:
#else
private:
#endif // _DEBUG

	int segmentChars();
	void recognize();
	int checkMrzStr();
	std::string mrzStr();

	void alpha2num(char &c);
	void num2alpha(char &c);

	cv::Mat m_mrzImg;
	std::vector<std::vector<cv::Mat>> m_charImgMat;
	std::string m_mrzStr;
	int m_type;
	cv::dnn::Net m_net;

    int mrzNum = 0;//机读码行数
    int cardType = -1;//证件类型
    std::vector<cv::Mat> lineImgs;
};
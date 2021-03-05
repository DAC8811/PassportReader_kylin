//#include "CHN.h"
#include "ContentCheck.h"

const static int ValidCharMax = 4500;	//字符区域的有效值的最大值
const static int ValidCharMin = 1500;	//字符区域的有效值的最小值
const static int CharHeightMax = 45;	//字符区域的宽度的最大值
const static int CharHeightMin = 20;	//字符区域的宽度的最小值
const static int CharWidthMax = 300;	//字符区域的高度的最大值
const static int CharWidthMin = 200;	//字符区域的高度的最小值

const static int CharHeightOffsite = 5;	//字符区域的高度的重定义
const static int CharWidthOffsite = 5;	//字符区域的宽度的重定义

/*针对CHN国家的E型护照的完整检测模块*/
bool ContentCheck::CHN_E(cv::Mat wi, cv::Mat uv, cv::Mat ir, char* mrz, int nationCode, int typeCode, int(&rCode)[14], char* CodePath)
{
	//MicroFaceAuthentication(wi, ir);

	//反馈各个检测模块的结果
	rCode[1] = Mode2_Data(uv, nationCode, typeCode, CodePath);

	rCode[8] = Mode14_Data(ir, nationCode, typeCode, CodePath);

	rCode[9] = PassportCodeAuthentication(wi, uv, ir, 1);

	//rCode[9] = rCode[2];

	rCode[10] = Mode16_Data(ir, nationCode, typeCode, CodePath);

	rCode[13] = Mode19_Data(mrz);

	//for (auto m : rCode)
	//{
		//std::cout << m << " ";	//打印检测结果
	//}
	////std::cout << std::endl;

	return true;	//检测完成
}

/*证件号码的认证模块*/
int ContentCheck::PassportCodeAuthentication(cv::Mat wi, cv::Mat uv, cv::Mat ir, int mode)
{
	if (!PassportCodeArea(wi, uv, ir))	//分割区域
		return 2;

	//cv::imshow("第一次分割的白光图像", wi);
	//cv::imshow("第一次分割的紫外图像", uv);
	//cv::imshow("第一次分割的红外图像", ir);

	Mat resultWI, resultUV, resultIR;

	//resultUV = CHAR_UV_REMOVE_COLOR(uv, 2, -70, 2);	//紫外图像二值化
	//resultIR = CHAR_WI_PROSCESS(ir, 2, 70, 3);	//红外图像二值化
	resultWI = CHAR_WI_PROSCESS(wi, 2, 70, 3);	//白光图像二值化

	//cv::imshow("第一次二值化图像", resultWI);

	if (!CharAreaRsize(resultWI, wi, uv, ir)) //字符区重分割
		return 2;
	//std::cout <//< std::endl;
	//std::cout <//< std::endl;

	//cv::imshow("第二次分割的白光图像", wi);
	//cv::imshow("第二次分割的紫外图像", uv);
	//cv::imshow("第二次分割的红外图像", ir);

	resultWI = CHAR_WI_PROSCESS(wi, 2, 30, 3);	//白光图像二值化
	//resultUV = CHAR_UV_REMOVE_COLOR(uv, 2, -70, 2);	//紫外图像二值化
	resultIR = CHAR_WI_PROSCESS(ir, 2, 30, 3);	//红外图像二值化

	//cv::imshow("二值化的白光图像", resultWI);
	//cv::imshow("二值化的紫外图像", resultUV);
	//cv::imshow("二值化的红外图像", resultIR);

	//Mat resultChar = resultUV ^ resultIR;	//红外、紫外图像异或操作

	//Mat openWind = getStructuringElement(MORPH_RECT, Size(3, 3));
	//morphologyEx(resultChar, resultChar, MORPH_OPEN, openWind);
	//闭操作
	//Mat closeWind = getStructuringElement(MORPH_RECT, Size(3, 3));
	//morphologyEx(resultChar, resultChar, MORPH_CLOSE, closeWind);
	//cv::imshow("resultUV_OR_IR", resultChar);
	//resultChar = resultChar & resultWI;	//与操作，得到完整的，噪声较少的字符图像
	//cv::imshow("紫外、红外组合图像", resultChar);

	int numWI = cv::countNonZero(resultWI);	//白光图像有效点数

	switch (mode)	//检测图像有效值是否符合规范
	{
	case 1:
	{
		//std::cout << "resultWI" << std::endl;
		if (!CheckValid(resultWI, ValidCharMax, ValidCharMin))	//检测白光二值化图像
			return 2;
		//std::cout << "resultUV" << std::endl;
		//if(!CheckValid(resultUV, numWI * 3 / 5, numWI * 1 / 5)) //检测红外二值化图像
		//	return 2;
		//std::cout << "resultIR" << std::endl;
		if (!CheckValid(resultIR, numWI * 4 / 5, numWI * 1 / 5)) //检测红外二值化图像
			return 2;
		//std::cout << "resultChar" << std::endl;
		//if (!CheckValid(resultChar, ValidCharMax * 3 / 2, ValidCharMin))	//检测字符二值化图像
		//	return 2;
		break;
	}
	default:
		break;
	}

	//std::cout << "图像匹配结果:" << std::endl;
	//if (ImageMatch(resultWI, resultChar) < 0.50)		//字符图像与白光图像匹配
		//return 2;	//相似度不匹配

	return 1;	//相似度匹配
}

/*微型人脸的认证模块*/
int ContentCheck::MicroFaceAuthentication(cv::Mat wi, cv::Mat ir)
{
	MicroFaceArea(wi, ir);	//分割区域

	//Mat resultWI = CHAR_WI_PROSCESS(wi, 2, -50, 3);	//白光图像二值化
	//Mat resultIR = CHAR_WI_PROSCESS(ir, 2, 70, 2);	//红外图像二值化

	Mat resultWI ;
	cvtColor(wi, resultWI, CV_RGB2GRAY);
	//cv::imwrite("D:\\project\\中国护照\\分割图像.jpg", resultWI);

	//cv::imshow("resultWI", resultWI);
	//cv::imshow("resultIR", resultIR);

	return 1;	//执行成功
}

/*证件号码区域分割模块*/
bool ContentCheck::PassportCodeArea(cv::Mat& wi, cv::Mat& uv, cv::Mat& ir)
{
	int pointX = 0, pointY = 0, width = 0, height = 0;
	if (Location_NIR(ir, pointX, pointY) != 1)	//获取参考点坐标参数
		return false;

	//设置坐标、长宽等参数
	pointX = ((pointX + 770) > 1250) ? 950 : (pointX + 770);
	pointY = ((pointY - 730) < 0) ? 0 : (pointY - 730);
	width = ((1250 - pointX) > 370) ? 370 : (1250 - pointX);
	height = ((900 - pointY) > 160) ? 160 : (900 - pointY);

	//在白光、紫外、红外图像中分割出相应的区域
	Mat segWI(wi, Rect(pointX, pointY, width, height));
	Mat segUV(uv, Rect(pointX, pointY, width, height));
	Mat segIR(ir, Rect(pointX, pointY, width, height));

	//在显示相应的区域
	//cv::imshow("SegWI", segWI);
	//cv::imshow("SegUV", segUV);
	//cv::imshow("SegIR", segIR);

	//返回分割后的区域
	wi = segWI.clone();
	uv = segUV.clone();
	ir = segIR.clone();
	return true;
}

/*微型人脸区域分割模块*/
void ContentCheck::MicroFaceArea(cv::Mat& wi, cv::Mat& ir)
{
	int pointX = 0, pointY = 0, width = 0, height = 0;
	int check = Location_NIR(ir, pointX, pointY);	//获取参考点坐标参数

	//设置坐标、长宽等参数
	pointX = ((pointX + 950) > 1250) ? 950 : (pointX + 950);
	pointY = ((pointY - 580) < 0) ? 0 : (pointY - 580);
	width = ((1250 - pointX) > 260) ? 260 : (1250 - pointX);
	height = ((900 - pointY) > 280) ? 280 : (900 - pointY);

	//在白光、紫外、红外图像中分割出相应的区域
	Mat segWI(wi, Rect(pointX, pointY, width, height));
	Mat segIR(ir, Rect(pointX, pointY, width, height));

	//在显示相应的区域
	//cv::imshow("SegWI", segWI);
	//cv::imshow("SegIR", segIR);

	wi = segWI.clone();
	ir = segIR.clone();
}

/*白光图像字符区域检测模块*/
Mat ContentCheck::CHAR_WI_PROSCESS(const cv::Mat& src, int scale, int value, int mode)
{
	Mat srcGray;
	switch (mode)
	{
	case 0:
	case 1:
	case 2:
	{
		vector<Mat> RGBSplit;

		split(src, RGBSplit); //通道分离

		//imshow("R", RGBSplit[2]);
		srcGray = RGBSplit[mode].clone();

		RGBSplit.clear();
		RGBSplit.shrink_to_fit();
		break;
	}
	case 3:
		cvtColor(src, srcGray, CV_RGB2GRAY);	//灰度化
		break;
	default:
		cvtColor(src, srcGray, CV_RGB2GRAY);	//灰度化
		break;
	}

	Mat midImage;
	medianBlur(srcGray, midImage, 3);	//中值滤波
	//imshow("midImage", midImage);

	midImage = ContrastStretch(midImage);	//直方图均衡	
	//imshow("分离的通道图像", midImage);

	int pointMin = (midImage.rows > midImage.cols) ? midImage.cols : midImage.rows;	//设置窗口大小

	//int blockSize = 2 * (pointMin / 8) + 1;
	//int constValue = 30;
	int blockSize = 2 * (pointMin / scale) + 1;	//设置窗口大小
	int constValue = value;	//固定值
	Mat resultImage;
	adaptiveThreshold(midImage, resultImage, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);	//二值化

	return resultImage;		//返回二值化图像
}

/*字符区域特定颜色去除的模块*/
Mat ContentCheck::CHAR_UV_REMOVE_COLOR(const cv::Mat& src, int scale, int value, int mode)
{
	Mat red = Pattern_UV_PROCESS2(src, scale, value, 2);	//红色通道下的图像

	Mat green = Pattern_UV_PROCESS2(src, scale , value / 4, 1);	//绿色通道下的图像

	Mat blue = CHAR_WI_PROSCESS(src , scale, - value / 10, 0);	//蓝色通道下的图像


	Mat singleResult = (cv::countNonZero(green) > (cv::countNonZero(red))) ? red : (red - ((cv::countNonZero(blue) > (5 * cv::countNonZero(green))) ? green : (green - blue)));	//单颜色的图像

	//std::cout << "红光图像有效值:" << cv::countNonZero(red) << std::endl;
	//std::cout << "蓝光图像有效值:" << cv::countNonZero(blue) << std::endl;
	//std::cout << "绿光图像有效值:" << cv::countNonZero(green) << std::endl;
	//cv::imshow("红色通道的二值化图像", red);
	//cv::imshow("绿色通道的二值化图像", green);
	//cv::imshow("蓝色通道的二值化图像", blue);
	//cv::imshow("通道组合图像", singleResult);

	//开操作
	Mat openWind = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(singleResult, singleResult, MORPH_OPEN, openWind);
	//闭操作
	Mat closeWind = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(singleResult, singleResult, MORPH_CLOSE, closeWind);

	return singleResult;
}


/*vector版的水平投影模块*/
bool ContentCheck::HorizonProjection_Vector(const cv::Mat& src, std::vector<int>& hp)
{
	//std::cout << "进入水平投影模块" << std::endl;

	if (src.empty() || hp.size() != src.rows || src.channels() != 1) //检查传入数据是否合格
		return false;

	const uchar* p;

	//std::cout << "开始水平投影" << std::endl;
	for (int i = 0; i < src.rows; i++)
	{
		hp[i] = 0;	//重置
		p = src.ptr<uchar>(i);	
		for(int j = 0; j < src.cols; j++)
		{
			if(p[j] != 0)
				++hp[i];	//有效点计数

		}
	}
	//std::cout << "结束水平投影" << std::endl;

	return true;
}

/*vector版的水平分割模块*/
bool ContentCheck::HorizonSegmentation_Vector(std::vector<int>& hp, int& topEdge, int& bottomEdge)
{
	//std::cout << "进入水平分割模块" << std::endl;

	if (hp.size() == 0 ) //检查传入数据是否合格
		return false;

	//std::cout << "开始水平分割" << std::endl;
	int aver = static_cast<int>(std::accumulate(hp.begin(), hp.end(), 0.0) * 2 /( 3 * hp.size()));	//计算均值
	//std::cout << "水平投影的平均值:" << aver << std::endl;

	topEdge = 0, bottomEdge = 0;
	for (auto start = hp.rbegin(); true;)
	{
		auto n = find_if(start, hp.rend(), [&aver](int& i) {return (i > aver); });	//查找字符区下边界
		//std::cout << "字符区下边界:" << hp.rend() - n << std::endl;
		if (n == hp.rend())
			break;
		auto m = find_if(n, hp.rend(), [&aver](int& i) {return (i <= aver); });	//查找字符区上边界
		//std::cout << "字符区上边界:" << hp.rend() - m << std::endl;
		if (m == hp.rend())
			break;

		//std::cout << "字符区高度:" << m - n << std::endl;
		if (m - n > CharHeightMin && m - n < CharHeightMax)	//检测字符区边界是否合格
		{
			topEdge = hp.rend() - m;	//字符区上边界
			bottomEdge = hp.rend() - n; //字符区下边界
			break;
		}
		start = m;
	}
	//std::cout << "结束水平分割" << std::endl;
	if (!topEdge && !bottomEdge)
	{
		return false;
	}
	return true;
}

/*vector版的垂直投影模块*/
bool ContentCheck::VerticalProjection_Vector(const cv::Mat& src, std::vector<int>& dst)
{
	//std::cout << "进入垂直投影模块" << std::endl;
	if (src.empty() || dst.size() != src.cols || src.channels() != 1) //检查传入数据是否合格
		return false;

	const uchar* p;

	//std::cout << "开始垂直投影" << std::endl;
	for (int i = 0; i < src.cols; i++)
	{
		dst[i] = 0;	//重置
		for (int j = 0; j < src.rows; j++)
		{
			p = src.ptr<uchar>(j);
			if (p[i] != 0)
				++dst[i];	//有效点计数
		}
	}
	//std::cout << "结束垂直投影" << std::endl;
	return true;
}

/*vector版的垂直分割模块*/
bool ContentCheck::VerticalSegmentation_Vector(std::vector<int>& vp, int& leftEdge, int& rightEdge)
{
	//std::cout << "进入垂直分割模块" << std::endl;

	if (vp.size() == 0) //检查传入数据是否合格
		return false;

	//std::cout << "开始垂直分割" << std::endl;
	int aver = static_cast<int>(std::accumulate(vp.begin(), vp.end(), 0.0) / vp.size());	//计算均值
	//std::cout << "垂直投影的平均值:" << aver << std::endl;

	leftEdge = 0, rightEdge = 0;

	auto n = find_if(vp.begin(), vp.end(), [&aver](int& i) {return (i > aver); });	//查找字符区下边界
	//std::cout << "字符区下边界:" << n - vp.begin() << std::endl;
	if (n == vp.end())
		return false;
	auto m = find_if(vp.rbegin(), vp.rend(), [&aver](int& i) {return (i > aver); });	//查找字符区上边界
	//std::cout << "字符区上边界:" << vp.rend() - m << std::endl;
	if (m == vp.rend())
		return false;

	//std::cout << "结束垂直分割" << std::endl;

	//std::cout << "字符区宽度:" << (vp.rend() - m) - (n - vp.begin()) << std::endl;
	if (((vp.rend() - m) - (n - vp.begin())) > CharWidthMin && ((vp.rend() - m) - (n - vp.begin())) < CharWidthMax)	//检测字符区边界是否合格
	{
		leftEdge = n - vp.begin();	//字符区上边界
		rightEdge = vp.rend() - m; //字符区下边界
		return true;
	}
	return false;
}

/*字符区域查找模块*/
bool ContentCheck::CharAreaFind(const cv::Mat& src, int& topEdge, int& bottomEdge, int& leftEdge, int& rightEdge)
{
	//std::cout << "进入字符区域查找模块" << std::endl;
	std::vector<int> hp(src.rows, 0);	//水平投影的数据集

	if (!HorizonProjection_Vector(src, hp))	//水平投影
		return false;

	if (!HorizonSegmentation_Vector(hp, topEdge, bottomEdge))	//水平分割
		return false;

	cv::Mat segSrc(src, Rect(0, topEdge, src.cols, bottomEdge - topEdge));
	//imshow("水平分割后的图像", segWI);

	std::vector<int> vp(segSrc.cols, 0);
	if (!VerticalProjection_Vector(segSrc, vp))	//垂直投影
		return false;

	if (!VerticalSegmentation_Vector(vp, leftEdge, rightEdge))
		return false;

	//cv::Mat resultWI(segWI, Rect(leftEdge, 0, rightEdge - leftEdge, segWI.rows));
	//imshow("垂直分割后的图像", resultWI);
	//std::cout << "结束字符区域查找模块" << std::endl;

	return true;
}

/*字符区重定位模块*/
bool ContentCheck::CharAreaRsize(const cv::Mat& src, cv::Mat& wi, cv::Mat& uv, cv::Mat& ir)
{
	if (src.empty() || wi.empty() || uv.empty() || ir.empty())	//检查数据是否为空
		return false;

	//std::cout << "进入字符区域重定义模块" << std::endl;
	int area[4];	//字符区域边界
	if (!CharAreaFind(src, area[0], area[1], area[2], area[3])) //查找字符边界区域
		return false;

	area[0] = (area[0] - CharHeightOffsite > 0) ? (area[0] - CharHeightOffsite) : 0; //上边界修正
	area[1] = (area[1] + CharHeightOffsite < src.rows - 1) ? (area[1] + CharHeightOffsite) : (src.rows - 1); //下边界修正
	area[2] = (area[2] - CharWidthOffsite > 0) ? (area[2] - CharWidthOffsite) : 0; //左边界修正
	area[3] = (area[3] + CharWidthOffsite < src.cols - 1) ? (area[3] + CharWidthOffsite) : (src.cols - 1); //右边界修正

	cv::Mat resultWI(wi, Rect(area[2], area[0], area[3] - area[2], area[1] - area[0]));
	//cv::imshow("白光结果图像", resultWI);

	cv::Mat resultUV(uv, Rect(area[2], area[0], area[3] - area[2], area[1] - area[0]));
	//cv::imshow("紫外结果图像", resultUV);

	cv::Mat resultIR(ir, Rect(area[2], area[0], area[3] - area[2], area[1] - area[0]));
	//cv::imshow("红外结果图像", resultIR);

	wi = resultWI.clone();
	uv = resultUV.clone();
	ir = resultIR.clone();

	//std::cout << "结束字符区域重定义模块" << std::endl;
	return true;
}

Mat ContentCheck::CHAR_UV_PROSCESS_HSV(const Mat& src, int mode)
{
	int HL, HH, SL, SH, VL, VH;

	switch (mode)
	{
	case 1://red
	{
		HL = 130;
		HH = 180;
		SL = 80;
		SH = 255;
		VL = 50;
		VH = 255;
		break;
	}
	case 2://red
	{
		HL = 0;
		HH = 20;
		SL = 130;
		SH = 255;
		VL = 60;
		VH = 255;
		break;
	}
	default:return src; break;
	}

	Mat imageHSV;
	vector<Mat> hsv;
	cvtColor(src, hsv, COLOR_BGR2HSV);


	split(imageHSV, hsv);
	//equalizeHist(HSVSplit[2], HSVSplit[2]);
	//equalizeHist(HSVSplit[1], HSVSplit[1]);
	merge(hsv, imageHSV);


	Mat result;

	inRange(imageHSV, Scalar(HL, SL, VL), Scalar(HH, SH, VH), result);
	//imshow("ImageHSV3 Image", ResultImage);

	Mat element = getStructuringElement(MORPH_RECT, Size(2, 2));
	//Mat element = getStructuringElement(MORPH_RECT, Size(4, 4));
	morphologyEx(result, result, MORPH_OPEN, element);
	//Mat element1 = getStructuringElement(MORPH_RECT, Size(7, 7));
	Mat element1 = getStructuringElement(MORPH_RECT, Size(2, 2));
	morphologyEx(result, result, MORPH_CLOSE, element1);
	//imshow("ResultImage Image", ResultImage);

	hsv.clear();
	hsv.shrink_to_fit();

	return result;
}
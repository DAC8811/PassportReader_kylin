#include "ContentCheck.h"



#define DATA_NIR_VALID_MIN 1500
#define DATA_NIR_VALID_MAX 5000


#define PN_HIGH_MIN 11
#define PN_HIGH_MAX 100
#define PN_WIDTH_MIN 50
#define PN_WIDTH_MAX 500
#define PN_UV_VALID_MAX 8000
#define PN_UV_VALID_MIN 500

#define Photo_NIR_VALID_MAX 80000
#define Photo_NIR_VALID_MIN 8000

#define CHN_VALID_MAX 20000
#define CHN_VALID_MIN 3000

#define PN_VALID_MIN 500
#define PN_VALID_MAX 2000

#define FN_HIGH_MIN 30
#define FN_HIGH_MAX 150
#define FN_WIDTH_MIN 18
#define FN_WIDTH_MAX 300
#define FN_NIR_VALID_MAX 800
#define FN_NIR_VALID_MIN 60
#define FN_VALID_BLANK 7

#define DATA_HIGH_MIN 60
#define DATA_WIDTH_MIN 90

#define NLD_VALID_MAX 5000
#define NLD_VALID_MIN 9000




int ContentCheck::SaveError(char* ImageLoc, char* ImageName, Mat src, int PointX, int PointY, int width, int height)
{
	int state = 0;
	char * Loc1 = NULL;
	char SaveLoc[150];

	int Length1 = 0;
	int Length2 = 0;

	int Dev = 0;

	if (ImageName == NULL)
		return 0;

	if (ImageLoc == NULL)
		return 0;


	Loc1 = strrchr(ImageLoc, '\\');

	if (Loc1 == NULL)
	{
		Loc1 = strrchr(ImageLoc, '/');
		state = 1;
		if (Loc1 == NULL)
		{
			state = 2;
		}
	}

	Length1 = strlen(ImageLoc);
	Length2 = strlen(Loc1);

	if (state != 2)
	{
		Dev = Length1 - Length2 + 1;

		//cout << Dev << endl;
		strncpy(SaveLoc, ImageLoc, Dev);
		//cout << SaveLoc << endl;

		strcat(SaveLoc, ImageName);
	}
	else
	{
		strcpy(SaveLoc, ImageName);
	}


	//cout << SaveLoc << endl;

	rectangle(src, Rect(PointX, PointY, width, height), Scalar(0, 0, 255), 3, 1, 0);
	Mat src1;
	resize(src, src1, Size(1126, 786), 0, 0, INTER_LINEAR);
	//imwrite(SaveLoc, src1);


	return 1;
}



int ContentCheck::PointCount(const Mat& src)
{
	Mat seg = src.clone();
	int count = 0;
	float * point;

	for (int i = 0; i < seg.rows; i++)
	{
		point = seg.ptr<float>(i);
		for (int j = 0; j < seg.cols; j++)
		{
			if (point[j] != 0)
			{
				//cout << point[j] << endl;
				count++;
			}	
		}
			
	}
	//cout << "count" << endl;
	//cout << count << endl;
	seg.release();
	return count;
}

bool  ContentCheck::CheckValid(const Mat& src, int ValidMax, int VAlidMin)
{
	int count = 0;

	count = countNonZero(src);
	//cout << "白像素点数:" << endl;
	//cout << count << endl;
	if ((count > VAlidMin)&(count < ValidMax))
		return true;
	else
		return false;
}




void ContentCheck::HorizonProjection(const Mat& src, int* dst)
{
	CV_Assert(src.depth() != sizeof(uchar));

	int i, j;
	const uchar* p;

	for (i = 0; i < src.rows; i++) 
	{
		dst[i] =0;
		p = src.ptr<uchar>(i);
		for (j = 0; j < src.cols; j++) 
		{
			if (p[j] != 0)
				dst[i] ++;

		}
	}
}

void ContentCheck::VerticalProjection(const Mat& src, int* dst)
{
	CV_Assert(src.depth() != sizeof(uchar));

	int i, j;
	const uchar* p;

	for (i = 0; i < src.cols; i++)
	{
		dst[i] = 0;
		for (j = 0; j < src.rows; j++)
		{
			p = src.ptr<uchar>(j);
			if (p[i] != 0)
				dst[i] ++;
		}
		//cout << dst[i] << endl;
	}
}


bool ContentCheck::PN_HorizonSegmentattion(const Mat& src,int* MatPro, Mat& dst)
{
	int* ImageSum = MatPro;
	Mat SrcImage = src.clone();
	int average = 0;
	int RowNum = src.rows;

	for (int i = 0; i < RowNum; i++)
		average = average + ImageSum[i];

	average = average / RowNum;

	average = average /2 ;

	int MatRow[2] = { 0, 0 };

	int Count = 0;

	for (int i = 0; i < RowNum - PN_HIGH_MIN - 1; i++)
	{
		if (ImageSum[i] < average)
		{
			if (!(ImageSum[i + 1] < average))
			{
				for (int j = i + 1, RowDev = 0; (j < i + PN_HIGH_MAX)&(j < RowNum - 1); j++)
				{
					if (ImageSum[j - 1] > average)
					{
						if (!(ImageSum[j] > average))
						{
							RowDev = j - i;

							if (RowDev > PN_HIGH_MIN)
							{
								MatRow[0] = i;
								MatRow[1] = j;
								Count++;
							}

							break;
						}
					}
				}
			}
		}
		if (Count == 1)
			break;
	}


	if (Count != 1)
	{
		SrcImage.release();
		return false;
	}


	int RowDev = MatRow[1] - MatRow[0];
	//cout << RowDev << endl;
	Mat SrcROI1(SrcImage, Rect(0, MatRow[0], SrcImage.cols, RowDev));

	dst = SrcROI1.clone();

	SrcROI1.release();
	SrcImage.release();
	return true;
}


bool ContentCheck::PN_UV_ImageSegmentation(const Mat& src, Mat& dst)
{
	
	Mat HPSeg;
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		HPSeg.release();
		SrcImage.release();
		return false;
	}
	int* HP = new int[SrcImage.rows];
	int* VP = new int[SrcImage.cols];
	HorizonProjection(SrcImage, HP);

	bool en = PN_HorizonSegmentattion(SrcImage, HP, HPSeg);

	if (!en)
	{
		HPSeg.release();
		delete[] HP;
		delete[] VP;
		SrcImage.release();
		return false;
	}
	//imshow("HPSeg", HPSeg);

	VerticalProjection(HPSeg, VP);
	en = VerticalSegmentattion(HPSeg, VP, dst, PN_WIDTH_MAX, PN_WIDTH_MIN);
	//cout << "en" << endl;
	//cout << en << endl;

	HPSeg.release();
	delete[] HP;
	delete[] VP;
	SrcImage.release();
	if (en)
		return true;
	else
		return false;
}

bool ContentCheck::PN_NIR_HorizonSegmentattion(const Mat& src, int* MatPro, Mat& dst)
{
	int* ImageSum = MatPro;
	Mat SrcImage = src.clone();
	int average = 0;
	int RowNum = src.rows;

	for (int i = 0; i < RowNum; i++)
		average = average + ImageSum[i];

	average = average / RowNum;

	int MatRow[2] = { 0, 0 };

	int Count = 0;

	for (int i = RowNum - 1; i > PN_HIGH_MIN; i--)
	{
		if (ImageSum[i] < average)
		{
			if (!(ImageSum[i - 1] < average))
			{
				for (int j = i - 1, RowDev = 0; !((j < i - PN_HIGH_MAX) | (j < 0)); j--)
				{
					if (ImageSum[j - 1] < average)
					{
						if (!(ImageSum[j] < average))
						{
							RowDev = i - j;

							if (RowDev > PN_HIGH_MIN)
							{
								MatRow[0] = i;
								MatRow[1] = j;
								Count++;
							}

							break;
						}
					}
				}
			}
		}
		if (Count == 1)
			break;
	}


	if (Count != 1)
	{
		SrcImage.release();
		return false;
	}


	int RowDev = MatRow[0] - MatRow[1];
	//cout << RowDev << endl;

	Mat SrcROI1(SrcImage, Rect(0, MatRow[1], SrcImage.cols, RowDev));

	dst = SrcROI1.clone();

	SrcROI1.release();
	SrcImage.release();
	return true;
}



bool ContentCheck::PN_NIR_ImageSegmentation(const Mat& src, Mat& dst)
{
	Mat HPSeg;
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		HPSeg.release();
		SrcImage.release();
		return false;
	}

	int* HP = new int[SrcImage.rows];
	int* VP = new int[SrcImage.cols];
	HorizonProjection(SrcImage, HP);

	bool en = PN_NIR_HorizonSegmentattion(SrcImage, HP, HPSeg);
	//imshow("HPSeg", HPSeg);
	if (!en)
	{
		HPSeg.release();
		delete[] HP;
		delete[] VP;
		SrcImage.release();
		return false;
	}


	VerticalProjection(HPSeg, VP);
	en = VerticalSegmentattion(HPSeg, VP, dst, PN_WIDTH_MAX, PN_WIDTH_MIN);


	HPSeg.release();
	delete[] HP;
	delete[] VP;
	if (en)
		return true;
	else
		return false;
}

bool ContentCheck::FN_VerticalSegmentattion(const Mat& src, int* MatPro, Mat& dst)
{
	int* ImageSum = MatPro;
	Mat SrcImage = src.clone();
	int average = 0;

	int ColNum = src.cols;

	for (int i = 0; i < ColNum; i++)
		average = average + ImageSum[i];

	average = average / ColNum;
	average = average /4;

	int MatCol[2] = { 0,0 };

	int count = 0;


	for (int i = 0; i < ColNum - 2 ; i++)
	{
		if (ImageSum[i] < average)
		{
			if (!(ImageSum[i + 1] < average))
			{
				for (int j = i + 1, ColDev = 0; (j < i + FN_WIDTH_MAX + 1)&(j < ColNum - 2); j++)
				{
					if (ImageSum[j] > average)
					{
						if (!(ImageSum[j + 1] > average))
						{
							ColDev = j - i;
							if (ColDev > FN_WIDTH_MIN)
							{
								count = 0;
								for (int m = j + 1; (m < j + FN_VALID_BLANK + 1)&(m < ColNum - 1); m++)
								{
									if (ImageSum[m] < average)
									//if (sum[m] == 0)
									{
										count++;
									}
								}

								if (count == FN_VALID_BLANK)
								{
									MatCol[0] = i;
									MatCol[1] = j;

									break;
								}
							}
						}
					}

					if (count == FN_VALID_BLANK)
					{
						break;
					}
				}
			}
		}
		if (count == FN_VALID_BLANK)
		{
			break;
		}
	}
	//cout << count << endl;
	if (count != FN_VALID_BLANK)
	{
		SrcImage.release();
		return false;
	}

	int ColDev = MatCol[1] - MatCol[0];
	//cout << "233" <<  endl;
	//cout << ColDev << endl;


	Mat SrcROI1(SrcImage, Rect(MatCol[0], 0, ColDev, SrcImage.rows));
	//imshow("SrcROI1", SrcROI1);
	dst = SrcROI1.clone();

	SrcROI1.release();
	SrcImage.release();

	return 1;
}

bool ContentCheck::FN_NIR_ImageSegmentation(const Mat& src, Mat& dst)
{
	Mat HPSeg;
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		HPSeg.release();
		SrcImage.release();
		return false;
	}

	int* HP = new int[SrcImage.rows];
	int* VP = new int[SrcImage.cols];
	HorizonProjection(SrcImage, HP);

	bool en = PN_NIR_HorizonSegmentattion(SrcImage, HP, HPSeg);
	
	if (!en)
	{
		HPSeg.release();
		delete[] HP;
		delete[] VP;
		SrcImage.release();
		return false;
	}
	//imshow("HPSeg", HPSeg);

	VerticalProjection(HPSeg, VP);
	en = FN_VerticalSegmentattion(HPSeg, VP, dst);


	HPSeg.release();
	delete[] HP;
	delete[] VP;
	SrcImage.release();
	if (en)
		return true;
	else
		return false;
}




bool ContentCheck::DATA_ImageSegmentation(const Mat& src, Mat& dst, int param1, int param2)
{
	Mat HPSeg;
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		HPSeg.release();
		SrcImage.release();
		return false;
	}

	int* HP = new int[SrcImage.rows];
	int* VP = new int[SrcImage.cols];
	HorizonProjection(SrcImage, HP);
	bool en = AT_HorizonSegmentattion(SrcImage, HP, HPSeg, SrcImage.rows, param1);

	if (!en)
	{
		HPSeg.release();
		delete[] HP;
		delete[]VP;
		SrcImage.release();
		return false;
	}

	//imshow("HPSeg", HPSeg);
	VerticalProjection(HPSeg, VP);
	
	en = VerticalSegmentattion(HPSeg, VP, dst, HPSeg.cols, param2);

	HPSeg.release();
	delete[] HP;
	delete[]VP;
	SrcImage.release();
	if (en)
		return true;
	else
		return false;
}

/*void AutoMedianFilter(Mat src, Mat dst, uint window) {
uint d = window / 2;
for (uint px = d; px < (src.rows - d); ++px) {
for (uint py = d; py < (src.cols - d); ++py) {
std::vector<uchar> v;
for (uint sx = (px - d); sx < (px + d); ++sx) {
for (uint sy = (py - d); sy < (py + d); ++sy) {
v.push_back(src.at<uchar>(sx, sy));
}
}

for (uint i = 0; i < (v.size() - 1); ++i) {
for (uint j = (i + 1); j < v.size(); ++j) {
if (v.at(i) > v.at(j)) { uchar temp; temp = v.at(i); v.at(i) = v.at(j); v.at(j) = temp; }
}
}

if ((src.at<uchar>(px, py) == v.at((v.size() - 1))) || (src.at<uchar>(px, py) == v.at(0))) {
dst.at<uchar>(px, py) = v.at((v.size() / 2));
}
}
}
}*/




Mat ContentCheck::ContrastStretch(Mat src)
{
	Mat SrcImage = src.clone();
	int SrcRow = SrcImage.rows;
	int SrcCol = SrcImage.cols;

	if (SrcImage.isContinuous())
	{
		SrcCol = SrcCol*SrcRow;
		SrcRow = 1;
	}

	uchar * MatData;
	int PointMax = 0, PointMin = 255;
	double c = 0;

	for (int j = 0; j < SrcRow; j++)
	{
		MatData = SrcImage.ptr<uchar>(j);
		for (int i = 0; i < SrcCol; i++)
		{
			if (MatData[i] > PointMax)
				PointMax = MatData[i];
			if (MatData[i] < PointMin)
				PointMin = MatData[i];
		}
	}

	for (int j = 0; j < SrcRow; j++)
	{
		MatData = SrcImage.ptr<uchar>(j);
		for (int i = 0; i < SrcCol; i++)
		{
			MatData[i] = (MatData[i] - PointMin) * 255 / (PointMax - PointMin);
		}
	}

	return SrcImage;
}


Mat ContentCheck::DATA_NIR_PROSCESS(const Mat& src)
{
	Mat SrcGray;
	//imshow("DATA_NIR_PROSCESS", src);
	cvtColor(src, SrcGray, CV_RGB2GRAY);

	//imshow("122", SrcGray);
	Mat ResultImage1 = ContrastStretch(SrcGray);

	Mat ResultImage2 = ResultImage1.clone();

	medianBlur(ResultImage1, ResultImage2, 3);
	//imshow("233", ResultImage2);

	int PointMin = (ResultImage2.rows > ResultImage2.cols) ? ResultImage2.cols : ResultImage2.rows;

	//int BlockSize = 2 * (PointMin / 8) + 1;
	//int ConstValue = 30;
	int BlockSize = 2 * (PointMin / 8) + 1;
	int ConstValue = 30;
	Mat ResultImage3;
	adaptiveThreshold(ResultImage2, ResultImage3, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, BlockSize, ConstValue);
	
	SrcGray.release();
	ResultImage1.release();
	ResultImage2.release();

	return ResultImage3;
}


Mat ContentCheck::PHOTO_NIR_PROSCESS(const Mat& src)
{
	Mat SrcGray;
	cvtColor(src, SrcGray, CV_RGB2GRAY);

	//imshow("122", SrcGray);
	Mat ResultImage1 = ContrastStretch(SrcGray);

	Mat ResultImage2 = ResultImage1.clone();

	medianBlur(ResultImage1, ResultImage2, 3);
	//imshow("233", ResultImage2);

	int PointMin = (ResultImage2.rows > ResultImage2.cols) ? ResultImage2.cols : ResultImage2.rows;

	//int BlockSize = 2 * (PointMin / 8) + 1;
	//int ConstValue = 30;
	int BlockSize = 2 * (PointMin / 4) + 1;
	int ConstValue = 40;
	Mat ResultImage3;
	adaptiveThreshold(ResultImage2, ResultImage3, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, BlockSize, ConstValue);

	SrcGray.release();
	ResultImage1.release();
	ResultImage2.release();

	return ResultImage3;
}

int ContentCheck::MRZ_UV_PROSCESS(const Mat& src, Mat& dst, int select)
{
	int HL, HH, SL, SH, VL, VH;

	switch (select)
	{
	case 1://photo_black
	{
		HL = 0;
		HH = 180;
		SL = 0;
		SH = 255;
		VL = 0;
		VH = 147;
		break;
	}
	case 2://word_black
	{
		HL = 0;
		HH = 180;
		SL = 0;
		SH = 255;
		VL = 0;
		VH = 20;
		break;
	}
	case 3://word_black
	{
		HL = 0;
		HH = 180;
		SL = 0;
		SH = 255;
		VL = 0;
		VH = 15;
		break;
	}
	case 4://word_black
	{
		HL = 0;
		HH = 180;
		SL = 180;
		SH = 255;
		VL = 0;
		VH = 50;
		break;
	}

	case 5://MRZ_word_white
	{
		HL = 0;
		HH = 180;
		SL = 0;
		SH = 60;
		VL = 180;
		VH = 255;
		break;
	}

	default:return 0; break;
	}

	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ImageHSV;
	vector<Mat> HSVSplit;
	cvtColor(src, ImageHSV, COLOR_BGR2HSV);


	split(ImageHSV, HSVSplit);
	equalizeHist(HSVSplit[2], HSVSplit[2]);
	equalizeHist(HSVSplit[1], HSVSplit[1]);
	merge(HSVSplit, ImageHSV);


	Mat ResultImage;

	inRange(ImageHSV, Scalar(HL, SL, VL), Scalar(HH, SH, VH), ResultImage);
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	//Mat element = getStructuringElement(MORPH_RECT, Size(4, 4));
	//morphologyEx(ResultImage, ResultImage, MORPH_OPEN, element);
	//Mat element1 = getStructuringElement(MORPH_RECT, Size(7, 7));
	Mat element1 = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(ResultImage, ResultImage, MORPH_CLOSE, element1);
	//imshow("ImageHSV3 Image", ResultImage);
	//Mat ResultImage2 = ResultImage.clone();
	//medianBlur(ResultImage, ResultImage, 3);

	dst = ResultImage.clone();

	HSVSplit.clear();
	HSVSplit.shrink_to_fit();
	ImageHSV.release();
	ResultImage.release();
	return 1;
}



/*Mat ContentCheck::Pattern_UV_PROSCESS(const Mat& src, int value)
{
	Mat SrcGray;
	cvtColor(src, SrcGray, CV_RGB2GRAY);
	imshow("SrcGray", SrcGray);

	Mat ResultImage1;

	ResultImage1 = ContrastStretch(SrcGray);
	Mat ResultImage2;

	
	Canny(ResultImage1, ResultImage2, 300, 250);
	imshow("ResultImage2", ResultImage2);

	vector<std::vector<Point>> contours;

	findContours(ResultImage2, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	cv::drawContours(SrcGray, contours, -1, cv::Scalar::all(255));

	cv::imshow("Contours", SrcGray);

	SrcGray.release();
	ResultImage1.release();
	ResultImage2.release();

	return ResultImage2;
}*/


Mat ContentCheck::Pattern_UV_PROSCESS(const Mat& src, int value)
{
	Mat SrcGray;
	cvtColor(src, SrcGray, CV_RGB2GRAY);

	
	Mat ResultImage1 ;
	medianBlur(SrcGray, ResultImage1, 3);
	Mat ResultImage2 = ContrastStretch(ResultImage1);
	

	//imshow("ResultImage2", ResultImage2);
	int PointMin = (ResultImage2.rows > ResultImage2.cols) ? ResultImage2.cols : ResultImage2.rows;

	int BlockSize = 2 * (PointMin / 2) + 1;
	int ConstValue = value;
	Mat ResultImage3;
	adaptiveThreshold(ResultImage2, ResultImage3, 250, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, BlockSize, ConstValue);
	Mat element1 = getStructuringElement(MORPH_RECT, Size(7, 7));
	morphologyEx(ResultImage3, ResultImage3, MORPH_CLOSE, element1);
	
	SrcGray.release();
	ResultImage1.release();
	ResultImage2.release();
	return ResultImage3;
	
}


/*Mat ContentCheck::Pattern_UV_PROSCESS(const Mat& src, int value)
{
	Mat SrcGray;
	cvtColor(src, SrcGray, CV_RGB2GRAY);


	Mat ResultImage1;
	medianBlur(SrcGray, ResultImage1, 5);
	Mat ResultImage2 = ContrastStretch(ResultImage1);


	//imshow("ResultImage2", ResultImage2);
	int PointMin = (ResultImage2.rows > ResultImage2.cols) ? ResultImage2.cols : ResultImage2.rows;

	int BlockSize = 2 * (PointMin / 2) + 1;
	int ConstValue = value;
	Mat ResultImage3;
	adaptiveThreshold(ResultImage2, ResultImage3, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, BlockSize, ConstValue);
	//Mat element1 = getStructuringElement(MORPH_RECT, Size(7, 7));
	//morphologyEx(ResultImage3, ResultImage3, MORPH_CLOSE, element1);

	SrcGray.release();
	ResultImage1.release();
	ResultImage2.release();
	return ResultImage3;

}*/






int ContentCheck::DATA_UV(const Mat& src)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ResultImage;
	int check = MRZ_UV_PROSCESS(SrcImage, ResultImage, 2);

	if ((!ResultImage.data) | (check != 1))
	{
		SrcImage.release();
		ResultImage.release();
		return 3;
	}


	Mat seg;
	bool en = DATA_ImageSegmentation(ResultImage, seg, DATA_HIGH_MIN, DATA_WIDTH_MIN);

	if (en != 1)
	{
		seg.release();
		SrcImage.release();
		ResultImage.release();
		return 3;
	}
	//imshow("seg", seg);
	int MaxNum = seg.rows * seg.cols / 2;
	int MinNum = seg.rows * seg.cols / 45;
	//cout << "参数" << endl;

	//cout << seg.rows << endl;
	//cout << seg.cols << endl;
	//cout << MaxNum << endl;
	//cout << MinNum << endl;

	en = CheckValid(ResultImage, MaxNum, MinNum);

	seg.release();
	ResultImage.release();
	SrcImage.release();

	if (en)
		return 1;
	else
		return 2;
}

int ContentCheck::DATA_NIR(const Mat& src)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ResultImage = DATA_NIR_PROSCESS(SrcImage);
	//imshow("ResultImage", ResultImage);
	if (!ResultImage.data)
	{
		SrcImage.release();
		ResultImage.release();
		return 3;
	}

	Mat seg;
	bool en = DATA_ImageSegmentation(ResultImage, seg, DATA_HIGH_MIN, DATA_WIDTH_MIN);

	if (en != 1)
	{
		seg.release();
		SrcImage.release();
		ResultImage.release();
		return 3;
	}
	//imshow("seg", seg);
	int MaxNum = seg.rows * seg.cols * 2 /3;
	int MinNum = seg.rows * seg.cols / 25;
	//cout << "参数" << endl;
	//cout << seg.rows << endl;
	//cout << seg.cols << endl;
	//cout << seg.cols * seg.rows << endl;
	//cout << MaxNum << endl;
	//cout << MinNum << endl;
	en = CheckValid(ResultImage, MaxNum, MinNum);

	seg.release();
	ResultImage.release();
	SrcImage.release();

	if (en)
		return 1;
	else
		return 2;
}




/*double ContentCheck::ssim(Mat &i1, Mat & i2) {
	const double C1 = 6.5025, C2 = 58.5225;
	int d = CV_32F;
	Mat I1, I2;
	i1.convertTo(I1, d);
	i2.convertTo(I2, d);
	Mat I1_2 = I1.mul(I1);
	Mat I2_2 = I2.mul(I2);
	Mat I1_I2 = I1.mul(I2);
	Mat mu1, mu2;
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);
	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);
	Mat sigma1_2, sigam2_2, sigam12;
	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;

	GaussianBlur(I2_2, sigam2_2, Size(11, 11), 1.5);
	sigam2_2 -= mu2_2;

	GaussianBlur(I1_I2, sigam12, Size(11, 11), 1.5);
	sigam12 -= mu1_mu2;
	Mat t1, t2, t3;
	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigam12 + C2;
	t3 = t1.mul(t2);

	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigam2_2 + C2;
	t1 = t1.mul(t2);

	Mat ssim_map;
	divide(t3, t1, ssim_map);
	Scalar mssim = mean(ssim_map);

	double ssim = (mssim.val[0] + mssim.val[1] + mssim.val[2]) / 3;
	return ssim;
}*/

int ContentCheck::Pattern_Code_UV(const Mat& src)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ResultImage = Pattern_UV_PROSCESS(SrcImage, -30);
	//imshow("ResultImage", ResultImage);

	if (!ResultImage.data)
	{
		SrcImage.release();
		ResultImage.release();
		return 3;
	}

	Mat seg;
	bool en = DATA_ImageSegmentation(ResultImage, seg, PN_HIGH_MIN, PN_WIDTH_MIN);


	if (en != 1)
	{
		seg.release();
		SrcImage.release();
		ResultImage.release();
		return 3;
	}

	//imshow("seg", seg);
	int MaxNum = seg.rows * seg.cols ;
	int MinNum = seg.rows * seg.cols / 20;
	//cout << "参数" << endl;

	//cout << seg.rows << endl;
	//cout << seg.cols << endl;
	//cout << MaxNum << endl;
	//cout << MinNum << endl;

	en = CheckValid(seg, MaxNum, MinNum);

	seg.release();
	SrcImage.release();
	ResultImage.release();
	if (en)
		return 1;
	else
		return 2;

}



int ContentCheck::Pattern_Code_NIR(const Mat& src)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ResultImage = DATA_NIR_PROSCESS(SrcImage);
	//imshow("ResultImage", ResultImage);

	if (!ResultImage.data)
	{
		SrcImage.release();
		ResultImage.release();
		return 3;
	}

	Mat seg;
	bool en = DATA_ImageSegmentation(ResultImage, seg, PN_HIGH_MIN, PN_WIDTH_MIN/2);


	if (en != 1)
	{
		seg.release();
		SrcImage.release();
		ResultImage.release();
		return 3;
	}

	//imshow("seg", seg);
	int MaxNum = seg.rows * seg.cols ;
	int MinNum = seg.rows * seg.cols / 20;
	//cout << "参数" << endl;

	//cout << seg.rows << endl;
	//cout << seg.cols << endl;
	//cout << MaxNum << endl;
	//cout << MinNum << endl;

	en = CheckValid(seg, MaxNum, MinNum);

	seg.release();
	SrcImage.release();
	ResultImage.release();
	if (en)
		return 1;
	else
		return 2;

}



int ContentCheck::PassNum_UV(const Mat& src, int select1, int select2)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ResultImage1;
	Mat ResultImage2;
	//Thread_UV_Process(SrcImage, ResultImage1, select1);
	//Thread_UV_Process(SrcImage, ResultImage2, select2);
	//imshow("ResultImage1", ResultImage1);
	//imshow("ResultImage2", ResultImage2);
	//Mat SumImage = ResultImage1 + ResultImage2;
	Mat SumImage;
	RGB_Red_Process(SrcImage, SumImage);

	if (!SumImage.data)
	{
		//ResultImage1.release();
		//ResultImage2.release();
		SumImage.release();
		SrcImage.release();
		return 3;
	}
	//imshow("SumImage", SumImage);
	Mat ResultImage3;



	bool en = PN_UV_ImageSegmentation(SumImage, ResultImage3);
	if (!en)
	{
		//ResultImage1.release();
		//ResultImage2.release();
		ResultImage3.release();
		SumImage.release();
		SrcImage.release();
		return 3;
	}

	//imshow("ResultImage3", ResultImage3);
	//cout << ResultImage3.rows << endl;
	//cout << ResultImage3.cols << endl;
	en = CheckValid(ResultImage3, PN_UV_VALID_MAX, PN_UV_VALID_MIN);

	//ResultImage1.release();
	//ResultImage2.release();
	ResultImage3.release();
	SumImage.release();
	SrcImage.release();
	if (en)
		return 1;
	else
		return 2;

}


int ContentCheck::RGB_Black_Process(Mat& src, Mat& dst)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	vector<Mat> RGBSplit;

	split(SrcImage, RGBSplit);

	Scalar RGBVal = mean(SrcImage);

	//imshow("Black", RGBSplit[2]);
	Mat SingleImage;
	Mat ResultImage1;
	Mat ResultImage2;
	int PointMin = 0;
	int BlockSize = 0;
	int ConstValue = 20;
	Mat ResultImage3[3];

	for (int mode = 0; mode < 3; mode++)
	{
		SingleImage = RGBSplit[mode].clone();

		//ResultImage2 = ContrastStretch(SingleImage);

		double Val = RGBVal.val[mode];

		//Val = Val * 9 / 13;

		//Val = Val * 8 / 13;
		Val = Val * 12 / 13;

		threshold(SingleImage, ResultImage3[mode], Val, 255, THRESH_BINARY_INV);
		
		int ValidNum = countNonZero(ResultImage3[mode]);
		//Mat ResultImage =  Pattern_UV_PROSCESS(SingleImage, 10);

		
		/*medianBlur(SingleImage, ResultImage1, 5);
		ResultImage2 = ContrastStretch(ResultImage1);
		//ResultImage2 = ResultImage1.clone();


		//imshow("BlackResultImage2", ResultImage2);
		PointMin = (ResultImage2.rows > ResultImage2.cols) ? ResultImage2.cols : ResultImage2.rows;

		BlockSize = 2 * (PointMin / 2) + 1;
		
		
		adaptiveThreshold(ResultImage2, ResultImage3[mode], 250, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, BlockSize, ConstValue);

		//threshold(ResultImage2, ResultImage3[mode], 0, 255, THRESH_OTSU);*/


		
	}
	//imshow("ResultImage3[0]", ResultImage3[0]);
	//imshow("ResultImage3[1]", ResultImage3[1]);
	//imshow("ResultImage3[2]", ResultImage3[2]);
	
	
	ResultImage1 = ResultImage3[0]& ResultImage3[1] & ResultImage3[2];
	//imshow("ResultImage1", ResultImage1);


	Mat element = getStructuringElement(MORPH_RECT, Size(2, 2));
	//Mat element = getStructuringElement(MORPH_RECT, Size(4, 4));
	morphologyEx(ResultImage1, ResultImage1, MORPH_OPEN, element);
	//Mat element1 = getStructuringElement(MORPH_RECT, Size(7, 7));
	Mat element1 = getStructuringElement(MORPH_RECT, Size(2, 2));
	morphologyEx(ResultImage1, ResultImage1, MORPH_CLOSE, element1);

	dst = ResultImage1.clone();
	//imshow("dst", dst);

	RGBSplit.clear();
	RGBSplit.shrink_to_fit();

	return 1;
}

int ContentCheck::RGB_Red_Process(Mat& src, Mat& dst)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	vector<Mat> RGBSplit;

	split(SrcImage, RGBSplit);

	//imshow("R", RGBSplit[2]);
	Mat SingleImage = RGBSplit[2].clone();
	//Mat ResultImage =  Pattern_UV_PROSCESS(SingleImage, 10);

	Mat ResultImage1;
	medianBlur(SingleImage, ResultImage1, 3);
	Mat ResultImage2 = ContrastStretch(ResultImage1);


	//imshow("ResultImage2", ResultImage2);
	int PointMin = (ResultImage2.rows > ResultImage2.cols) ? ResultImage2.cols : ResultImage2.rows;

	int BlockSize = 2 * (PointMin / 2) + 1;
	int ConstValue = -90;
	//int ConstValue = -100;
	Mat ResultImage3;
	adaptiveThreshold(ResultImage2, ResultImage3, 250, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, BlockSize, ConstValue);
	Mat element1 = getStructuringElement(MORPH_RECT, Size(7, 7));
	morphologyEx(ResultImage3, ResultImage3, MORPH_CLOSE, element1);
	//imshow("ResultImage3", ResultImage3);

	dst = ResultImage3.clone(); 

	RGBSplit.clear();
	RGBSplit.shrink_to_fit();

	return 1;
}

int ContentCheck::RGB_Blue_Process(Mat& src, Mat& dst)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	vector<Mat> RGBSplit;

	split(SrcImage, RGBSplit);

	//imshow("B", RGBSplit[0]);
	Mat SingleImage = RGBSplit[0].clone();
	//Mat ResultImage =  Pattern_UV_PROSCESS(SingleImage, 10);

	Mat ResultImage1;
	medianBlur(SingleImage, ResultImage1, 3);
	Mat ResultImage2 = ContrastStretch(ResultImage1);


	//imshow("ResultImage2", ResultImage2);
	int PointMin = (ResultImage2.rows > ResultImage2.cols) ? ResultImage2.cols : ResultImage2.rows;

	int BlockSize = 2 * (PointMin / 2) + 1;
	int ConstValue = -80;
	Mat ResultImage3;
	adaptiveThreshold(ResultImage2, ResultImage3, 250, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, BlockSize, ConstValue);
	Mat element1 = getStructuringElement(MORPH_RECT, Size(7, 7));
	morphologyEx(ResultImage3, ResultImage3, MORPH_CLOSE, element1);
	//imshow("ResultImage3", ResultImage3);

	dst = ResultImage3.clone();

	RGBSplit.clear();
	RGBSplit.shrink_to_fit();

	return 1;
}

int ContentCheck::RGB_Green_Process(Mat& src, Mat& dst)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}
	Scalar RGBVal = mean(SrcImage);

	double GreenVal = RGBVal.val[1];

	//cout << "GreenVal" << endl;
	//cout << GreenVal << endl;
	vector<Mat> RGBSplit;

	split(SrcImage, RGBSplit);

	//imshow("G", RGBSplit[1]);
	Mat SingleImage = RGBSplit[1].clone();
	//Mat ResultImage =  Pattern_UV_PROSCESS(SingleImage, 10);

	Mat ResultImage1;
	medianBlur(SingleImage, ResultImage1, 3);
	Mat ResultImage2 = ContrastStretch(ResultImage1);


	//imshow("ResultImage2", ResultImage2);
	int PointMin = (ResultImage2.rows > ResultImage2.cols) ? ResultImage2.cols : ResultImage2.rows;

	int BlockSize = 2 * (PointMin / 2) + 1;
	int ConstValue = -60;
	Mat ResultImage3;
	adaptiveThreshold(ResultImage2, ResultImage3, 250, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, BlockSize, ConstValue);
	Mat element1 = getStructuringElement(MORPH_RECT, Size(7, 7));
	morphologyEx(ResultImage3, ResultImage3, MORPH_CLOSE, element1);
	//imshow("ResultImage3", ResultImage3);

	dst = ResultImage3.clone();

	RGBSplit.clear();
	RGBSplit.shrink_to_fit();

	return 1;
}

int ContentCheck::Thread_UV_Process(Mat& src, Mat& dst, int mode)
{

	int HL, HH, SL, SH, VL, VH;

	switch (mode)
	{
	case 1: //green
	{
		HL = 35;
		HH = 110;
		SL = 70;
		SH = 255;
		VL = 70;
		VH = 255;
		//HL = 55;
		//HH = 97;
		//SL = 80;
		//SH = 255;
		//VL = 80;
		//VH = 255;
		break;
	}
	case 2://red
	{
		HL = 130;
		HH = 180;
		SL = 80;
		SH = 255;
		VL = 50;
		VH = 255;
		//HL = 140;
		//HH = 180;
		//SL = 80;
		//SH = 255;
		//VL = 40;
		//VH = 255;
		break;
	}
	case 3://red
	{
		HL = 0;
		HH = 20;
		SL = 130;
		SH = 255;
		VL = 60;
		VH = 255;
		break;
	}
	case 4://Bright_Blue
	{
		HL = 90;
		HH = 140;
		SL = 140;
		SH = 255;
		VL = 110;
		VH = 255;
		break;
	}
	case 5://cyan
	{
		HL = 35;
		HH = 130;
		SL = 40;
		SH = 255;
		VL = 120;
		VH = 255;
		break;
	}

	case 6://photo_shape
	{
		HL = 0;
		HH = 180;
		SL = 0;
		SH = 255;
		VL = 0;
		VH = 30;
		break;
	}

	case 7://photo_shape
	{
		HL = 0;
		HH = 180;
		SL = 100;
		SH = 255;
		VL = 100;
		VH = 255;
		break;
	}

	case 8://photo_shape
	{
		HL = 90;
		HH = 180;
		SL = 10;
		SH = 255;
		VL = 220;
		VH = 255;
		break;
	}
	default:return 0; break;
	}

	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ImageHSV;
	vector<Mat> HSVSplit;
	cvtColor(src, ImageHSV, COLOR_BGR2HSV);


	split(ImageHSV, HSVSplit);
	//equalizeHist(HSVSplit[2], HSVSplit[2]);
	//equalizeHist(HSVSplit[1], HSVSplit[1]);
	merge(HSVSplit, ImageHSV);


	Mat ResultImage;

	inRange(ImageHSV, Scalar(HL, SL, VL), Scalar(HH, SH, VH), ResultImage);
	//imshow("ImageHSV3 Image", ResultImage);

	Mat element = getStructuringElement(MORPH_RECT, Size(2, 2));
	//Mat element = getStructuringElement(MORPH_RECT, Size(4, 4));
	morphologyEx(ResultImage, ResultImage, MORPH_OPEN, element);
	//Mat element1 = getStructuringElement(MORPH_RECT, Size(7, 7));
	Mat element1 = getStructuringElement(MORPH_RECT, Size(6, 6));
	morphologyEx(ResultImage, ResultImage, MORPH_CLOSE, element1);
	//imshow("ResultImage Image", ResultImage);


	dst = ResultImage.clone();

	HSVSplit.clear();
	HSVSplit.shrink_to_fit();
	ImageHSV.release();
	ResultImage.release();
	return 1;
}





Mat ContentCheck::PHOTO_UV_PROSCESS(const Mat& src)
{
	Mat SrcGray;
	cvtColor(src, SrcGray, CV_RGB2GRAY);


	Mat ResultImage1 = ContrastStretch(SrcGray);
	//imshow("ResultImage1", ResultImage1);
	Mat ResultImage2 = ResultImage1.clone();
	medianBlur(ResultImage1, ResultImage2, 3);
	//imshow("ResultImage2", ResultImage2);

	int PointMin = (ResultImage2.rows > ResultImage2.cols) ? ResultImage2.cols : ResultImage2.rows;

	int BlockSize = 2 * (PointMin / 4) + 1;
	int ConstValue = 10;
	Mat ResultImage3;
	adaptiveThreshold(ResultImage2, ResultImage3, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, BlockSize, ConstValue);

	SrcGray.release();
	ResultImage1.release();
	ResultImage2.release();

	return ResultImage3;
}

int ContentCheck::Photo_Shape_UV(const Mat& src, int VALID_MAX, int VALID_MIN)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}


	Mat ResultImage;
	int check = Thread_UV_Process(SrcImage, ResultImage, 6);
	if (check != 1)
	{
		SrcImage.release();
		ResultImage.release();
		return 3;
	}
	//imshow("seg1", ResultImage);


	bool en = CheckValid(ResultImage, VALID_MAX, VALID_MIN);


	SrcImage.release();
	ResultImage.release();

	if (en)
		return 1;
	else
		return 2;

}


int ContentCheck::Photo_BG_UV(const Mat& src, int VALID_MAX, int VALID_MIN)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ResultImage;
	int check = 1;
	//check = Thread_UV_Process(SrcImage, ResultImage, 7);
	ResultImage = Pattern_UV_PROSCESS(SrcImage, -20);

	if ((!ResultImage.data) | (check != 1))
	{
		SrcImage.release();
		ResultImage.release();
		return 3;
	}

	//imshow("seg1", ResultImage);

	Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(ResultImage, ResultImage, MORPH_OPEN, element1);
	Mat element2 = getStructuringElement(MORPH_RECT, Size(7, 7));
	morphologyEx(ResultImage, ResultImage, MORPH_CLOSE, element2);
	//imshow("seg2", ResultImage);


	bool en = CheckValid(ResultImage, VALID_MAX, VALID_MIN);


	SrcImage.release();
	ResultImage.release();

	if (en)
		return 1;
	else
		return 2;
}



int ContentCheck::Photo_NIR(const Mat& src)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ResultImage = PHOTO_NIR_PROSCESS(SrcImage);
	//imshow("ResultImage", ResultImage);

	if (!ResultImage.data)
	{
		ResultImage.release();
		SrcImage.release();
		return 3;
	}
	bool en = CheckValid(ResultImage, Photo_NIR_VALID_MAX, Photo_NIR_VALID_MIN);

	ResultImage.release();
	SrcImage.release();

	if (en)
		return 1;
	else
		return 2;
}

int ContentCheck::Photo_Color_UV(const Mat& src, int mode)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ResultImage;
	int check = Thread_UV_Process(SrcImage, ResultImage, mode);
	//imshow("23333", ResultImage);
	if ((!ResultImage.data)|(check != 1))
	{
		ResultImage.release();
		SrcImage.release();
		return 3;
	}
	bool en = CheckValid(ResultImage, Photo_NIR_VALID_MAX, Photo_NIR_VALID_MIN);

	ResultImage.release();
	SrcImage.release();

	if (en)
		return 1;
	else
		return 2;
}


int ContentCheck::PassNum_NIR(const Mat& src)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ResultImage = DATA_NIR_PROSCESS(SrcImage);
	//MRZ_UV_PROSCESS(SrcImage, ResultImage, 2);
	if (!ResultImage.data)
	{
		SrcImage.release();
		ResultImage.release();
		return 3;
	}
	//imshow("ResultImage", ResultImage);

	Mat seg;
	bool en = PN_NIR_ImageSegmentation(ResultImage, seg);


	if (!en)
	{
		ResultImage.release();
		seg.release();
		return 3;
	}

	//imshow("ResultImageseg", seg);

	en = CheckValid(seg, PN_UV_VALID_MAX, PN_UV_VALID_MIN);

	ResultImage.release();
	seg.release();
	if (en)
		return 1;
	else
		return 2;
}

int ContentCheck::FamilyName_NIR(const Mat& src)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ResultImage;
	int check = RGB_Black_Process(SrcImage, ResultImage);

	if (check != 1)
	{
		ResultImage.release();
		SrcImage.release();
		return 3;
	}
	//imshow("ResultImage", ResultImage);

	Mat seg;
	bool en = FN_NIR_ImageSegmentation(ResultImage, seg);


	if (!en)
	{
		ResultImage.release();
		SrcImage.release();
		seg.release();
		return 3;
	}

	//imshow("seg", seg);

	en = CheckValid(seg, FN_NIR_VALID_MAX, FN_NIR_VALID_MIN);

	ResultImage.release();
	SrcImage.release();
	seg.release();
	if (en)
		return 1;
	else
		return 2;
}








/*

int ContentCheck::Mode_Select(int mode, Mat& src, int param1, int param2, int param3, int param4)
{
Mat SrcImage = src.clone();

if (!SrcImage.data)
{
SrcImage.release();
return 0;
}

int state = 0;

switch (mode)
{
case 1: state = MRZ_UV(SrcImage, 0); break;
case 2:
{
if (param4 == 0)
state = PatternMtach_UV(SrcImage, SrcImage);
else if(param4 == 1)
state = Pattern_Thread_UV(SrcImage, param1, param2, param3);
else if (param4 == 2)
state = Thread_UV(SrcImage, param1);
else if (param4 == 3)
state = Pattern_UV(SrcImage, param1, param2, param3);
break;
}
case 3: state = PassNum_UV(SrcImage, param1, param2); break;
case 4: state = DATA_UV(SrcImage); break;
case 5: state = Photo_Shape_UV(SrcImage, 7000, 2000); break;
case 6: state = Photo_BG_UV(SrcImage, param1, param2); break;
case 7: state = Pattern_Code_UV(SrcImage); break;
case 8: state = Thread_UV(SrcImage, 5); break;
case 9: state = Thread_UV(SrcImage, 4); break;
case 10: state = Thread_UV(SrcImage, 1); break;
case 11: state = Thread_UV(SrcImage, 2); break;
case 12: state = Thread_UV(SrcImage, 2); break;
case 13: state = Thread_UV(SrcImage, 4); break;

case 14: state = Authenticator_NIR(SrcImage); break;
case 15: state = PassNum_NIR(SrcImage); break;
case 16: state = DATA_NIR(SrcImage); break;
case 17: state = Photo_NIR(SrcImage); break;
case 18: state = FamilyName_NIR(SrcImage); break;
case 19: state = Pattern_Code_NIR(SrcImage); break;

default: return 5;
}

return state;


}

int ContentCheck::PatternMtach_UV(const Mat& src1, const Mat& src2)
{
Mat SrcImage1 = src1.clone();
if (!SrcImage1.data)
{
SrcImage1.release();
return 0;
}

Mat SrcImage2 = src2.clone();
if (!SrcImage2.data)
{
SrcImage2.release();
return 0;
}

Mat ResultImage1 = Pattern_UV_PROSCESS(SrcImage1, -40);
Mat ResultImage2 = Pattern_UV_PROSCESS(SrcImage2, -40);
Mat SrcGray1, SrcGray2;
cvtColor(SrcImage1, SrcGray1, CV_RGB2GRAY);
cvtColor(SrcImage2, SrcGray2, CV_RGB2GRAY);
Mat ResultImage1;
medianBlur(SrcGray1, ResultImage1, 5);
Mat ResultImage2;
medianBlur(SrcGray2, ResultImage2, 5);

//imshow("ResultImage1", ResultImage1);
//imshow("ResultImage2", ResultImage2);

Mat seg;

int MatCol = 0;
int MatRow = 0;
int sum = 0;

int ConstSize = 4;

for (int i = 0; i < ConstSize; i++)
{
	for (int j = 0; j < ConstSize; j++)
	{
		MatCol = ResultImage1.cols / ConstSize;
		MatRow = ResultImage1.rows / ConstSize;

		seg = ResultImage1(Rect(MatCol*i, MatCol*j, MatCol, MatRow));
		string str1 = pHashValue(seg);

		MatCol = ResultImage2.cols / ConstSize;
		MatRow = ResultImage2.rows / ConstSize;

		seg = ResultImage2(Rect(MatCol*i, MatCol*j, MatCol, MatRow));
		string str2 = pHashValue(seg);

		int num = HanmingDistance(str1, str2);
		////cout << "感知哈希" << endl;
		//cout << num << endl;

		sum = sum + num;
	}
}
sum = sum / ConstSize / ConstSize;
//cout << "均值" << endl;
//cout << sum << endl;

/*Mat seg, seg1, seg2;
int MatCol = 0;
int MatRow = 0;
int count;
double sum = 0;
int ConstSize = 4;

for (int i = 0; i < ConstSize; i++)
{
for (int j = 0; j < ConstSize; j++)
{
count = i + j;
if (count % 2 == 0)
{
MatCol = ResultImage1.cols / ConstSize;
MatRow = ResultImage1.rows / ConstSize;

seg1 = ResultImage1(Rect(MatCol*i, MatCol*j, MatCol, MatRow));

MatCol = ResultImage2.cols / ConstSize;
MatRow = ResultImage2.rows / ConstSize;

seg2 = ResultImage2(Rect(MatCol*i, MatCol*j, MatCol, MatRow));

double num = ssim(seg1, seg2);
cout << "感知哈希" << endl;
cout << num << endl;

sum = sum + num;


}

}
}
sum = sum / 5;
cout << "均值" << endl;
cout << sum << endl;

//cout << ssim(ResultImage1, ResultImage2) << endl;
Mat seg1;
Canny(ResultImage2, seg1, 300, 200);

imshow("seg1", seg1);
PCA pca1(seg1, Mat(), CV_PCA_DATA_AS_ROW, 120);
cout << "pca1" << endl;



//cout << pca1.eigenvectors.size() << endl;
//PCA pca2(ResultImage2, Mat(), CV_PCA_DATA_AS_ROW, 120);
//cout << "pca2" << endl;
//cout << pca2.eigenvalues.size() << endl;
//cout << pca2.eigenvectors.size() << endl;

//seg.release();
SrcImage1.release();
SrcImage2.release();
ResultImage1.release();
ResultImage2.release();
if (1)
return 1;
else
return 2;

}

string ContentCheck::pHashValue(Mat &src)
{
Mat img, dst;
string rst(64, '\0');
double dIdex[64];
double mean = 0.0;
int k = 0;
if (src.channels() == 3)
{
cvtColor(src, src, CV_BGR2GRAY);
img = Mat_<double>(src);
}
else
{
img = Mat_<double>(src);
}

//第一步，缩放尺寸
resize(img, img, Size(8, 8));

//第二步，离散余弦变换，DCT系数求取
dct(img, dst);

//第三步，求取DCT系数均值（左上角8*8区块的DCT系数）
for (int i = 0; i < 8; ++i) {
	for (int j = 0; j < 8; ++j)
	{
		dIdex[k] = dst.at<double>(i, j);
		mean += dst.at<double>(i, j) / 64;
		++k;
	}
}

//第四步，计算哈希值。
for (int i = 0; i<64; ++i)
{
	if (dIdex[i] >= mean)
	{
		rst[i] = '1';
	}
	else
	{
		rst[i] = '0';
	}
}
return rst;
}

int ContentCheck::HanmingDistance(string &str1, string &str2)
{
	if ((str1.size() != 64) || (str2.size() != 64))
		return -1;
	int difference = 0;
	for (int i = 0; i<64; i++)
	{
		if (str1[i] != str2[i])
			difference++;
	}
	return difference;
}

//中国E型护照图像截取
int ContentCheck::CHN_E_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3)
{
	int  width = 0, height = 0;

	switch (mode)
	{
	case 1:  //MRZ_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 1170;
		height = 130;
		//PointX = 20;
		//PointY = 680;
		//width = 1170;
		//height = 190;
		break;
	}



	case 2: //Pattern_UV
	{
		PointX = PointX + 450;
		PointY = PointY - 570;
		width = 550;
		height = 400;
		//PointX = 700;
		//PointY = 200;
		//width = 450;
		//height = 450;
		break;
	}



	case 3:  //PassortNum_UV
	{
		PointX = PointX + 750;
		PointY = PointY - 620;
		width = 350;
		height = 70;
		//PointX = 800;
		//PointY = 50;
		//width = 390;
		//height = 200;
		break;
	}

	case 14: //Authenticator_NIR
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 210;
		height = 130;
		//PointX = 20;
		//PointY = 650;
		//width = 210;
		//height = 200;
		break;
	}

	case 15:  //PassortNum_UV
	{
		PointX = PointX + 750;
		PointY = PointY - 620;
		width = 350;
		height = 100;
		//PointX = 800;
		//PointY = 50;
		//width = 390;
		//height = 200;
		break;
	}

	case 16:  //CHN_E_DATA_NIR
	{
		PointX = PointX + 330;
		PointY = PointY - 400;
		width = 350;
		height = 320;
		//PointX = 380;
		//PointY = 300;
		//width = 200;
		//height = 350;
		break;
	}

	default:return 5; break;

	}

	Mat SrcImage = src.clone();

	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	//cout << PointX << endl;
	//cout << PointY << endl;

	if ((PointX < 0) | (PointX > SrcImage.cols - 1))
		return 6;

	if ((PointY < 0) | (PointY > SrcImage.rows - 1))
		return 6;

	if ((PointX + width < 0) | (PointX + width > SrcImage.cols - 1))
		return 6;

	if ((PointY + height < 0) | (PointY + height > SrcImage.rows - 1))
		return 6;

	Mat Seg(SrcImage, Rect(PointX, PointY, width, height));

	if (!Seg.data)
	{
		Seg.release();
		SrcImage.release();
		return 0;
	}
	//imshow("Seg", Seg);

	int state = 1;

	switch (mode)
	{
	case 1: break;
	case 2:
	{
		param1 = 18000;
		param2 = 6000;
		param3 = -50;
		break;
	}
	case 3:
	{
		param1 = 2;
		param2 = 3;
		break;
	}
	case 14: break;
	case 15: break;
	case 16:
	{
		param1 = 7000;
		param2 = 2000;
		break;
	}
	default:state = 5; break;
	}

	switch (mode)
	{
	case 1:state = MRZ_UV(Seg); break;
	case 2:state = Authenticator_NIR(Seg); break;
	case 3:state = Pattern_UV(Seg, 13000, 4000, -60); break;
	case 4:state = DATA_NIR(Seg, 4000, 2000); break;
	case 5:state = PassNum_UV(Seg, 2, 3); break;
	case 6:state = PassNum_NIR(Seg); break;
	default:return 0; break;
	}

	dst = Seg.clone();

	SrcImage.release();
	Seg.release();
	return state;
}

//中国G型护照图像截取
int ContentCheck::CHN_G_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3, int& param4)
{
	int width = 0, height = 0;

	switch (mode)
	{
	case 1:  //MRZ_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 1170;
		height = 130;
		//PointX = 20;
		//PointY = 680;
		//width = 1170;
		//height = 190;
		break;
	}

	case 2: //Pattern_UV
	{
		PointX = PointX + 650;
		PointY = PointY - 570;
		width = 450;
		height = 450;
		//PointX = 700;
		//PointY = 200;
		//width = 450;
		//height = 450;
		break;
	}

	case 3:  //PassortNum_UV
	{
		PointX = PointX + 770;
		PointY = PointY - 680;
		width = 350;
		height = 100;
		//PointX = 800;
		//PointY = 50;
		//width = 390;
		//height = 200;
		break;
	}

	case 4:  //DATA_UV
	{
		PointX = PointX + 300;
		PointY = PointY - 340;
		width = 750;
		height = 170;
		//PointX = 550;
		//PointY = 350;
		//width = 330;
		//height = 250;
		break;
	}

	case 8:  //Thread_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 150;
		width = 1170;
		height = 300;
		//PointX = 20;
		//PointY = 600;
		//width = 1170;
		//height = 250;
		break;
	}

	case 9:  //Thread_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 150;
		width = 1170;
		height = 300;
		//PointX = 20;
		//PointY = 600;
		//width = 1170;
		//height = 250;
		break;
	}

	case 10:  //Thread_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 150;
		width = 1170;
		height = 300;
		//PointX = 20;
		//PointY = 600;
		//width = 1170;
		//height = 250;
		break;
	}

	case 11:  //Thread_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 150;
		width = 1170;
		height = 300;
		//PointX = 20;
		//PointY = 600;
		//width = 1170;
		//height = 250;
		break;
	}

	case 12:  //Photo_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 600;
		width = 350;
		height = 550;
		//PointX = 40;
		//PointY = 130;
		//width = 350;
		//height = 530;
		break;
	}


	case 13:  //Photo_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 600;
		width = 350;
		height = 550;
		//PointX = 40;
		//PointY = 130;
		//width = 350;
		//height = 530;
		break;
	}

	case 14: //Authenticator_NIR
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 210;
		height = 130;
		//PointX = 20;
		//PointY = 650;
		//width = 210;
		//height = 200;
		break;
	}



	case 16:  //CHN_E_DATA_NIR
	{
		PointX = PointX + 300;
		PointY = PointY - 340;
		width = 750;
		height = 170;
		//PointX = 380;
		//PointY = 300;
		//width = 300;
		//height = 350;
		break;
	}


	case 17:  //Photo_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 600;
		width = 350;
		height = 550;
		//PointX = 40;
		//PointY = 130;
		//width = 350;
		//height = 530;
		break;
	}



	default:return 5; break;

	}


	//cout << PointX << endl;
	//cout << PointY << endl;


	Mat SrcImage = src.clone();

	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}


	if ((PointX < 0) | (PointX > SrcImage.cols - 1))
		return 6;

	if ((PointY < 0) | (PointY > SrcImage.rows - 1))
		return 6;

	if ((PointX + width < 0) | (PointX + width > SrcImage.cols - 1))
		return 6;

	if ((PointY + height < 0) | (PointY + height > SrcImage.rows - 1))
		return 6;


	Mat Seg(SrcImage, Rect(PointX, PointY, width, height));

	if (!Seg.data)
	{
		Seg.release();
		SrcImage.release();
		return 0;
	}
	//imshow("Seg", Seg);
	int state = 1;

	switch (mode)
	{
	case 1: break;
	case 2:
	{
		param1 = 1;
		param2 = 2;
		param3 = 4;
		param4 = 1;
		break;
	}
	case 3:
	{
		param1 = 2;
		param2 = 3;
		break;
	}
	case 4:
	{
		param1 = 7000;
		param2 = 2000;
		break;
	}
	case 8:
	{
		param1 = 5;
		break;
	}
	case 9:
	{
		param1 = 4;
		break;
	}
	case 10:
	{
		param1 = 1;
		break;
	}
	case 11:
	{
		param1 = 2;
		break;
	}
	case 12:
	{
		param1 = 2;
		break;
	}
	case 13:
	{
		param1 = 4;
		break;
	}
	case 14: break;
	case 15: break;
	case 16:
	{
		param1 = 7000;
		param2 = 2000;
		break;
	}
	case 17: break;
	default:state = 5; break;
	}

	switch (mode)
	{
	case 1:state = MRZ_UV(Seg); break;
	case 2:state = Authenticator_NIR(Seg); break;
	case 3:state = Pattern_Thread_UV(Seg, 1, 2, 4); break;
	case 4:state = DATA_NIR(Seg, 4000, 2000); break;
	case 5:state = PassNum_UV(Seg, 2, 3); break;
	case 6:state = Thread_UV(Seg, 5); break;
	case 7:state = Thread_UV(Seg, 4); break;
	case 8:state = Thread_UV(Seg, 1); break;
	case 9:state = Thread_UV(Seg, 2); break;
	case 10:state = Thread_UV(Seg, 2); break;
	case 11:state = Thread_UV(Seg, 4); break;
	case 12:state = DATA_UV(Seg, 4000, 3000); break;
	case 13:state = Photo_NIR(Seg); break;

	default:return 0; break;
	}
	dst = Seg.clone();
	SrcImage.release();
	Seg.release();
	return state;
}


//中国W型护照图像截取
int ContentCheck::CHN_W_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3)
{
	int width = 0, height = 0;

	switch (mode)
	{
	case 1:  //MRZ_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 1170;
		height = 130;
		//PointX = 20;
		//PointY = 680;
		//width = 1170;
		//height = 190;
		break;
	}

	case 2: //Pattern_UV
	{
		PointX = PointX + 650;
		PointY = PointY - 570;
		width = 450;
		height = 450;
		//PointX = 700;
		//PointY = 200;
		//width = 450;
		//height = 450;
		break;
	}

	case 3:  //PassortNum_UV
	{
		PointX = PointX + 770;
		PointY = PointY - 680;
		width = 400;
		height = 100;
		//PointX = 800;
		//PointY = 50;
		//width = 400;
		//height = 200;
		break;
	}

	case 5:  //Photo_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 550;
		width = 330;
		height = 500;
		//PointX = 50;
		//PointY = 170;
		//width = 330;
		//height = 500;
		break;
	}

	case 6:  //Photo_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 550;
		width = 330;
		height = 500;
		//PointX = 50;
		//PointY = 200;
		//width = 330;
		//height = 500;
		break;
	}

	case 14: //Authenticator_NIR
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 210;
		height = 130;
		//PointX = 20;
		//PointY = 650;
		//width = 210;
		//height = 200;
		break;
	}



	case 16:  //CHN_E_DATA_NIR
	{
		PointX = PointX + 750;
		PointY = PointY - 400;
		width = 360;
		height = 250;
		//PointX = 800;
		//PointY = 300;
		//width = 360;
		//height = 250;
		break;
	}




	case 18:  //Family Name
	{
		PointX = PointX + 330;
		PointY = PointY - 520;
		width = 400;
		height = 140;
		//PointX = 380;
		//PointY = 200;
		//width = 400;
		//height = 140;
		break;
	}

	default:return 5; break;

	}




	Mat SrcImage = src.clone();

	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	if ((PointX < 0) | (PointX > SrcImage.cols - 1))
		return 6;

	if ((PointY < 0) | (PointY > SrcImage.rows - 1))
		return 6;

	if ((PointX + width < 0) | (PointX + width > SrcImage.cols - 1))
		return 6;

	if ((PointY + height < 0) | (PointY + height > SrcImage.rows - 1))
		return 6;


	Mat Seg(SrcImage, Rect(PointX, PointY, width, height));

	if (!Seg.data)
	{
		Seg.release();
		SrcImage.release();
		return 0;
	}
	//imshow("Seg", Seg);

	int state = 0;

	switch (mode)
	{
	case 1: break;
	case 2:
	{
		param1 = 18000;
		param2 = 6000;
		param3 = -50;
		break;
	}
	case 3:
	{
		param1 = 2;
		param2 = 3;
		break;
	}
	case 5: break;
	case 6:
	{
		param1 = 7000;
		param2 = 2000;
		break;
	}
	case 14: break;
	case 16:
	{
		param1 = 7000;
		param2 = 1000;
		break;
	}
	case 18: break;
	default:state = 5; break;
	}

	switch (mode)
	{
	case 1:state = MRZ_UV(Seg); break;
	case 2:state = Authenticator_NIR(Seg); break;
	case 3:state = Pattern_UV(Seg, 15000, 10000, -60); break;
	case 4:state = DATA_NIR(Seg, 3700, 2300); break;
	case 5:state = PassNum_UV(Seg, 2, 3); break;
	case 6:state = Photo_Shape_UV(Seg, 4500, 3000); break;
	case 7:state = Photo_BG_UV(Seg, 8000, 4000); break;
	case 8:state = FamilyName_NIR(Seg); break;
	default:return 0; break;
	}
	dst = Seg.clone();
	SrcImage.release();
	Seg.release();
	return state;
}


int ContentCheck::CHN_P_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3)
{
	int width = 0, height = 0;

	switch (mode)
	{
	case 1:  //MRZ_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 1170;
		height = 130;
		//PointX = 20;
		//PointY = 680;
		//width = 1170;
		//height = 190;
		break;
	}

	case 2: //Pattern_UV
	{
		PointX = PointX + 650;
		PointY = PointY - 570;
		width = 450;
		height = 450;
		//PointX = 700;
		//PointY = 200;
		//width = 450;
		//height = 450;
		break;
	}

	case 5:  //Photo_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 570;
		width = 330;
		height = 500;
		//PointX = 40;
		//PointY = 170;
		//width = 330;
		//height = 500;
		break;
	}



	case 14: //Authenticator_NIR
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 210;
		height = 130;
		//PointX = 20;
		//PointY = 650;
		//width = 210;
		//height = 200;
		break;
	}

	case 16:  //CHN_P_DATA_NIR
	{
		PointX = PointX + 330;
		PointY = PointY - 400;
		width = 350;
		height = 320;
		//PointX = 380;
		//PointY = 270;
		//width = 300;
		//height = 380;
		break;
	}

	case 17:  //Photo_NIR
	{
		PointX = PointX - 20;
		PointY = PointY - 570;
		width = 330;
		height = 500;
		//PointX = 40;
		//PointY = 170;
		//width = 330;
		//height = 500;
		break;
	}



	case 18:  //Family Name
	{
		PointX = PointX + 330;
		PointY = PointY - 520;
		width = 400;
		height = 120;
		//PointX = 380;
		//PointY = 200;
		//width = 400;
		//height = 120;
		break;
	}

	default:return 5; break;

	}

	Mat SrcImage = src.clone();

	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}


	if ((PointX < 0) | (PointX > SrcImage.cols - 1))
		return 6;

	if ((PointY < 0) | (PointY > SrcImage.rows - 1))
		return 6;

	if ((PointX + width < 0) | (PointX + width > SrcImage.cols - 1))
		return 6;

	if ((PointY + height < 0) | (PointY + height > SrcImage.rows - 1))
		return 6;

	Mat Seg(SrcImage, Rect(PointX, PointY, width, height));

	if (!Seg.data)
	{
		Seg.release();
		SrcImage.release();
		return 0;
	}
	//imshow("Seg", Seg);

	int state = 0;


	switch (mode)
	{
	case 1: break;
	case 2:
	{
		param1 = 4000;
		param2 = 600;
		param3 = -50;
		break;
	}
	case 5: break;
	case 14: break;
	case 16:
	{
		param1 = 9000;
		param2 = 2000;
		break;
	}
	case 18: break;
	default:state = 5; break;
	}

	switch (mode)
	{
	case 1:state = MRZ_UV(Seg); break;
	case 2:state = Authenticator_NIR(Seg); break;
	case 3:state = Pattern_UV(Seg, 2500, 700, -60); break;
	case 4:state = DATA_NIR(Seg, 6200, 4000); break;
	case 5:state = Photo_Shape_UV(Seg, 4500, 3000); break;
	case 6:state = Photo_NIR(Seg); break;
	case 7:state = FamilyName_NIR(Seg); break;
	default:return 0; break;
	}
	dst = Seg.clone();
	SrcImage.release();
	Seg.release();
	return state;
}


int ContentCheck::AIT_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3, int& param4)
{
	int width = 0, height = 0;

	switch (mode)
	{
	case 1:  //MRZ_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 1170;
		height = 130;
		//PointX = 20;
		//PointY = 680;
		//width = 1170;
		//height = 190;
		break;
	}

	case 2: //Pattern_UV
	{
		PointX = PointX + 650;
		PointY = PointY - 570;
		width = 450;
		height = 450;
		//PointX = 700;
		//PointY = 200;
		//width = 450;
		//height = 450;
		break;
	}

	case 14: //Authenticator_NIR
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 210;
		height = 130;
		//PointX = 20;
		//PointY = 650;
		//width = 210;
		//height = 200;
		break;
	}


	case 15:  //PassortNum_UV
	{
		PointX = PointX + 750;
		PointY = PointY - 580;
		width = 390;
		height = 200;
		//PointX = 800;
		//PointY = 50;
		//width = 390;
		//height = 200;
		break;
	}

	case 16:  //CHN_E_DATA_NIR
	{
		PointX = PointX + 490;
		PointY = PointY - 500;
		width = 200;
		height = 400;
		//PointX = 540;
		//PointY = 250;
		//width = 200;
		//height = 400;
		break;
	}


	default:return 5; break;

	}

	Mat SrcImage = src.clone();

	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}


	if ((PointX < 0) | (PointX > SrcImage.cols - 1))
		return 6;

	if ((PointY < 0) | (PointY > SrcImage.rows - 1))
		return 6;

	if ((PointX + width < 0) | (PointX + width > SrcImage.cols - 1))
		return 6;

	if ((PointY + height < 0) | (PointY + height > SrcImage.rows - 1))
		return 6;


	Mat Seg(SrcImage, Rect(PointX, PointY, width, height));

	if (!Seg.data)
	{
		Seg.release();
		SrcImage.release();
		return 0;
	}
	//imshow("Seg", Seg);

	int state = 0;



	switch (mode)
	{
	case 1: break;
	case 2:
	{
		param1 = 1;
		param4 = 2;
		break;
	}
	case 14: break;
	case 15: break;
	case 16:
	{
		param1 = 9000;
		param2 = 800;
		break;
	}
	default:state = 5; break;
	}
	switch (mode)
	{
	case 1:state = MRZ_UV(Seg); break;
	case 2:state = Authenticator_NIR(Seg); break;
	case 3:state = Thread_UV(Seg, 1); break;
	case 4:state = DATA_NIR(Seg, 2800, 1400); break;
	case 5:state = PassNum_NIR(Seg); break;
	default:return 0; break;
	}
	dst = Seg.clone();
	SrcImage.release();
	Seg.release();
	return state;
}


int ContentCheck::NLD_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3)
{
	int  width = 0, height = 0;

	switch (mode)
	{
	case 1:  //MRZ_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 1170;
		height = 130;
		//PointX = 20;
		//PointY = 680;
		//width = 1170;
		//height = 190;
		break;
	}

	case 2: //Pattern_UV
	{
		PointX = PointX + 650;
		PointY = PointY - 570;
		width = 450;
		height = 450;
		//PointX = 700;
		//PointY = 200;
		//width = 450;
		//height = 450;
		break;
	}

	case 14: //Authenticator_NIR
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 210;
		height = 130;
		//PointX = 20;
		//PointY = 650;
		//width = 210;
		//height = 200;
		break;
	}



	default:return 5; break;

	}

	Mat SrcImage = src.clone();

	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	if ((PointX < 0) | (PointX > SrcImage.cols - 1))
		return 6;

	if ((PointY < 0) | (PointY > SrcImage.rows - 1))
		return 6;

	if ((PointX + width < 0) | (PointX + width > SrcImage.cols - 1))
		return 6;

	if ((PointY + height < 0) | (PointY + height > SrcImage.rows - 1))
		return 6;


	Mat Seg(SrcImage, Rect(PointX, PointY, width, height));

	if (!Seg.data)
	{
		Seg.release();
		SrcImage.release();
		return 0;
	}
	//imshow("Seg", Seg);

	int state = 0;


	switch (mode)
	{
	case 1: break;
	case 2:
	{
		param1 = 10000;
		param2 = 2000;
		break;
	}
	case 14: break;
	default:state = 5; break;
	}
	switch (mode)
	{
	case 1:state = MRZ_UV(Seg); break;
	case 2:state = Authenticator_NIR(Seg); break;
	case 3:state = Pattern_UV(Seg, 10000, 7500, -60); break;
	default:return 0; break;
	}
	dst = Seg.clone();
	SrcImage.release();
	Seg.release();
	return state;
}


int ContentCheck::USA_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3)
{
	int width = 0, height = 0;

	switch (mode)
	{
	case 1:  //MRZ_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 1170;
		height = 130;
		//PointX = 20;
		//PointY = 680;
		//width = 1170;
		//height = 190;
		break;
	}

	case 2: //Pattern_UV
	{
		PointX = PointX + 370;
		PointY = PointY - 570;
		width = 450;
		height = 450;
		//PointX = 420;
		//PointY = 200;
		//width = 450;
		//height = 450;
		break;
	}

	case 5:  //Photo_NIR
	{
		PointX = PointX;
		PointY = PointY - 570;
		width = 370;
		height = 500;
		//PointX = 40;
		//PointY = 170;
		//width = 370;
		//height = 500;
		break;
	}

	case 14: //Authenticator_NIR
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 210;
		height = 130;
		//PointX = 20;
		//PointY = 690;
		//width = 210;
		//height = 180;
		break;
	}

	case 19: //Authenticator_NIR
	{
		PointX = PointX + 900;
		PointY = PointY - 150;
		width = 250;
		height = 150;
		//PointX = 20;
		//PointY = 690;
		//width = 210;
		//height = 180;
		break;
	}



	default:return 5; break;

	}

	Mat SrcImage = src.clone();

	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}


	if ((PointX < 0) | (PointX > SrcImage.cols - 1))
		return 6;

	if ((PointY < 0) | (PointY > SrcImage.rows - 1))
		return 6;

	if ((PointX + width < 0) | (PointX + width > SrcImage.cols - 1))
		return 6;

	if ((PointY + height < 0) | (PointY + height > SrcImage.rows - 1))
		return 6;


	Mat Seg(SrcImage, Rect(PointX, PointY, width, height));

	if (!Seg.data)
	{
		Seg.release();
		SrcImage.release();
		return 0;
	}
	//imshow("Seg", Seg);

	int state = 0;


	switch (mode)
	{
	case 1: break;
	case 2:
	{
		param1 = 10000;
		param2 = 2000;
		break;
	}
	case 5: break;
	case 14: break;
	case 19: break;

	default:state = 5; break;
	}
	switch (mode)
	{
	case 1:state = MRZ_UV(Seg); break;
	case 2:state = Authenticator_NIR(Seg); break;
	case 3:state = Pattern_UV(Seg, 24000, 17000, -20); break;
	case 4:state = Photo_Shape_UV(Seg, 6000, 3500); break;
	default:return 0; break;
	}
	dst = Seg.clone();
	SrcImage.release();
	Seg.release();
	return state;
}


int ContentCheck::AUS_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3)
{
	int width = 0, height = 0;

	switch (mode)
	{
	case 1:  //MRZ_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 1170;
		height = 130;
		//PointX = 20;
		//PointY = 660;
		//width = 1170;
		//height = 200;
		break;
	}

	case 2: //Pattern_UV
	{
		PointX = PointX + 370;
		PointY = PointY - 570;
		width = 450;
		height = 450;
		//PointX = 700;
		//PointY = 200;
		//width = 450;
		//height = 450;
		break;
	}

	case 7: //Pattern_UV
	{
		PointX = PointX - 30;
		PointY = PointY - 720;
		width = 400;
		height = 200;
		//PointX = 20;
		//PointY = 90;
		//width = 400;
		//height = 150;
		break;
	}

	case 14: //Authenticator_NIR
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 210;
		height = 130;
		//PointX = 20;
		//PointY = 690;
		//width = 210;
		//height = 180;
		break;
	}



	case 16:  //DATA_NIR
	{
		PointX = PointX + 330;
		PointY = PointY - 440;
		width = 300;
		height = 350;
		//PointX = 380;
		//PointY = 300;
		//width = 300;
		//height = 350;
		break;
	}


	default:return 5; break;

	}

	Mat SrcImage = src.clone();

	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}


	if ((PointX < 0) | (PointX > SrcImage.cols - 1))
		return 6;

	if ((PointY < 0) | (PointY > SrcImage.rows - 1))
		return 6;

	if ((PointX + width < 0) | (PointX + width > SrcImage.cols - 1))
		return 6;

	if ((PointY + height < 0) | (PointY + height > SrcImage.rows - 1))
		return 6;

	Mat Seg(SrcImage, Rect(PointX, PointY, width, height));

	if (!Seg.data)
	{
		Seg.release();
		SrcImage.release();
		return 0;
	}
	//imshow("Seg", Seg);

	int state = 0;
	switch (mode)
	{
	case 1: break;
	case 2:
	{
		param1 = 13000;
		param2 = 5000;
		param3 = -60;
		break;
	}
	case 7:
	{
		param1 = 4000;
		param2 = 1000;
		param3 = -60;
		break;
	}
	case 14: break;
	case 16:
	{
		param1 = 7000;
		param2 = 2000;
		break;
	}
	default:state = 5; break;
	}
	switch (mode)
	{
	case 1:state = MRZ_UV(Seg); break;
	case 2:state = Authenticator_NIR(Seg); break;
	case 3:state = Pattern_UV(Seg, 13000, 7000, -60); break;
	case 4:state = DATA_NIR(Seg, 6000, 3500); break;
	case 5:state = Pattern_UV(Seg, 3000, 1000, -60); break;

	default:return 0; break;
	}
	dst = Seg.clone();
	SrcImage.release();
	Seg.release();
	return state;
}

int ContentCheck::JPN_DATA(int mode, Mat& src, Mat& dst, int PointX, int PointY, int& param1, int& param2, int& param3)
{
	int width = 0, height = 0;

	switch (mode)
	{
	case 1:  //MRZ_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 1170;
		height = 130;
		//PointX = 20;
		//PointY = 680;
		//width = 1170;
		//height = 190;
		break;
	}

	case 2: //Pattern_UV
	{
		PointX = PointX + 370;
		PointY = PointY - 570;
		width = 450;
		height = 450;
		//PointX = 700;
		//PointY = 200;
		//width = 450;
		//height = 450;
		break;
	}

	case 5:  //Photo_UV
	{
		PointX = PointX - 20;
		PointY = PointY - 540;
		width = 350;
		height = 500;
		//PointX = 50;
		//PointY = 200;
		//width = 350;
		//height = 500;
		break;
	}

	case 14: //Authenticator_NIR
	{
		PointX = PointX - 20;
		PointY = PointY - 20;
		width = 210;
		height = 130;
		//PointX = 20;
		//PointY = 650;
		//width = 210;
		//height = 240;
		break;
	}


	case 16:  //CHN_E_DATA_NIR
	{
		PointX = PointX + 280;
		PointY = PointY - 480;
		width = 300;
		height = 350;
		//PointX = 380;
		//PointY = 300;
		//width = 300;
		//height = 350;
		break;
	}





	default:return 5; break;

	}

	Mat SrcImage = src.clone();

	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	if ((PointX < 0) | (PointX > SrcImage.cols - 1))
		return 6;

	if ((PointY < 0) | (PointY > SrcImage.rows - 1))
		return 6;

	if ((PointX + width < 0) | (PointX + width > SrcImage.cols - 1))
		return 6;

	if ((PointY + height < 0) | (PointY + height > SrcImage.rows - 1))
		return 6;


	Mat Seg(SrcImage, Rect(PointX, PointY, width, height));

	if (!Seg.data)
	{
		Seg.release();
		SrcImage.release();
		return 0;
	}
	//imshow("Seg", Seg);

	int state = 0;
	switch (mode)
	{
	case 1: break;
	case 2:
	{
		param1 = 15000;
		param2 = 6000;
		param3 = -60;
		break;
	}
	case 5: break;
	case 14: break;
	case 16:
	{
		param1 = 10000;
		param2 = 3000;
		break;
	}
	default:state = 5; break;
	}
	switch (mode)
	{
	case 1:state = MRZ_UV(Seg); break;
	case 2:state = Authenticator_NIR(Seg); break;
	case 3:state = Pattern_UV(Seg, 10000, 8000, -60); break;
	case 4:state = DATA_NIR(Seg, 8000, 6000); break;
	case 5:state = Photo_Shape_UV(Seg, 4000, 2000); break;
	default:return 0; break;
	}
	dst = Seg.clone();
	SrcImage.release();
	Seg.release();
	return state;
}*/


//int ContentCheck::ImageSeg(const Mat& src, Mat& dst, int SegNum)
//{
//	Mat SrcImage = src.clone();
//
//	if (!SrcImage.data)
//		return 0;
//
//	int PointX = 0, PointY = 0, width = 0, height = 0;
//
//	switch (SegNum)
//	{
//	case 1:  //MRZ_UV
//	{
//		PointX = 20;
//		PointY = SrcImage.rows - 190;
//		width = SrcImage.cols - 80;
//		height = 160;
//		break;
//	}
//
//	case 2: //Authenticator_NIR
//	{
//		PointX = 20;
//		PointY = 650;
//		width = 210;
//		height = 200;
//		break;
//	}
//
//	case 3:  //CHN_E_DATA_NIR
//	{
//		PointX = 380;
//		PointY = 300;
//		width = 200;
//		height = 350;
//		break;
//	}
//
//
//	case 4: //Pattern_UV
//	{
//		PointX = 700;
//		PointY = 200;
//		width = 450;
//		height = 450;
//		break;
//	}
//	case 5: //Photo_UV
//	{
//		PointX = 50;
//		PointY = 130;
//		width = 370;
//		height = 530;
//		break;
//	}
//	case 6:  //PassortNum_UV
//	{
//		PointX = 800;
//		PointY = 50;
//		width = 390;
//		height = 200;
//		break;
//	}
//	case 7:  //Thread_UV
//	{
//		PointX = 20;
//		PointY = SrcImage.rows - 300;
//		width = SrcImage.cols - 80;
//		height = 250;
//		break;
//	}
//	case 8:  //Family Name
//	{
//		PointX = 380;
//		PointY = 200;
//		width = 400;
//		height = 120;
//		break;
//	}
//	case 9:  //CHN_E_DATA_NIR
//	{
//		PointX = 800;
//		PointY = 320;
//		width = 320;
//		height = 220;
//		break;
//	}
//	default:return 0; break;
//
//	}
//
//
//	Mat SrcROI(SrcImage, Rect(PointX, PointY, width, height));
//
//	dst = SrcROI.clone();
//
//	return 1;
//}
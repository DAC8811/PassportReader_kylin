#include "ContentCheck.h"

#define MRZ_HIGH_MIN 15
#define MRZ_HIGH_MAX 60
#define MRZ_WIDTH_MIN 700
#define MRZ_WIDTH_MAX 1250
#define MRZ_UV_VALID_MIN 2000
#define MRZ_UV_VALID_MAX 40000



int ContentCheck::MRZ_UV(const Mat& src, int select)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}


	Mat ResultImage;
	int check = 1;
	medianBlur(SrcImage, SrcImage, 5);

	Mat imageLog(SrcImage.size(), CV_32FC3);
	for (int i = 0; i < SrcImage.rows; i++)
	{
		for (int j = 0; j < SrcImage.cols; j++)
		{
			imageLog.at<Vec3f>(i, j)[0] = log(1 + SrcImage.at<Vec3b>(i, j)[0]);
			imageLog.at<Vec3f>(i, j)[1] = log(1 + SrcImage.at<Vec3b>(i, j)[1]);
			imageLog.at<Vec3f>(i, j)[2] = log(1 + SrcImage.at<Vec3b>(i, j)[2]);
		}
	}

	normalize(imageLog, imageLog, 0, 255, CV_MINMAX);

	convertScaleAbs(imageLog, imageLog);
	Mat SrcGray;
	cvtColor(imageLog, SrcGray, CV_BGR2GRAY);



	//imshow("SrcGray", SrcGray);

	int PointMin = (SrcGray.rows > SrcGray.cols) ? SrcGray.cols : SrcGray.rows;
	int BlockSize = 2 * (PointMin / 8) + 1;
	//int ConstValue = 30;
	int ConstValue = 50;

	adaptiveThreshold(SrcGray, ResultImage, 250, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, BlockSize, ConstValue);


	/*if (select == 0)
	check = RGB_Black_Process(imageLog, ResultImage);
	//check = MRZ_UV_PROSCESS(SrcImage, ResultImage, 4);
	else if (select == 1)
	check = MRZ_UV_PROSCESS(imageLog, ResultImage, 5);
	else if (select == 2)
	check = MRZ2_UV_PROSCESS(imageLog, ResultImage, 20);*/


	if ((!ResultImage.data) | (check != 1))
	{
		SrcImage.release();
		ResultImage.release();
		return 3;
	}


	//imshow("MRZ_UV_seg", ResultImage);

	Mat seg1;
	Mat seg2;

	bool DoubleSeg = UV_ImageSegmentation(ResultImage, seg1, seg2);

	if (!DoubleSeg)
	{
		seg1.release();
		seg2.release();
		SrcImage.release();
		ResultImage.release();
		return 3;
	}

	//imshow("seg1", seg1);
	//cout << seg1.rows << endl;
	//cout << seg1.cols << endl;
	//imshow("seg2", seg2);
	//cout << seg2.rows << endl;
	//cout << seg2.cols << endl;
	int en;
	bool en1 = true, en2 = true;

	//cout << "ValidNum" << endl;
	en1 = CheckValid(seg1, MRZ_UV_VALID_MAX, MRZ_UV_VALID_MIN);
	en2 = CheckValid(seg2, MRZ_UV_VALID_MAX, MRZ_UV_VALID_MIN);

	if (en1 & en2)
		en = 1;
	else
		en = 2;

	seg1.release();
	seg2.release();
	SrcImage.release();
	ResultImage.release();
	return en;
}


int ContentCheck::MRZ2_UV_PROSCESS(const Mat& src, Mat& dst, int select)
{
	Mat SrcGray;
	cvtColor(src, SrcGray, CV_RGB2GRAY);


	Mat ResultImage1;
	medianBlur(SrcGray, ResultImage1, 5);
	Mat ResultImage2 = ContrastStretch(ResultImage1);


	//imshow("ResultImage2", ResultImage2);
	int PointMin = (ResultImage2.rows > ResultImage2.cols) ? ResultImage2.cols : ResultImage2.rows;

	int BlockSize = 2 * (PointMin / 2) + 1;
	int ConstValue = select;
	Mat ResultImage3;
	adaptiveThreshold(ResultImage2, ResultImage3, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, BlockSize, ConstValue);
	//Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
	//morphologyEx(ResultImage3, ResultImage3, MORPH_OPEN, element1);
	//Mat element2 = getStructuringElement(MORPH_RECT, Size(4, 4));
	//morphologyEx(ResultImage3, ResultImage3, MORPH_OPEN, element2);


	dst = ResultImage3.clone();

	SrcGray.release();
	ResultImage1.release();
	ResultImage2.release();
	return 1;

}

bool ContentCheck::UV_ImageSegmentation(const Mat& src, Mat& dst1, Mat& dst2)
{
	Mat HPSeg1, HPSeg2;
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		HPSeg1.release();
		HPSeg2.release();
		return false;
	}

	int* HP = new int[SrcImage.rows];
	int* VP1 = new int[SrcImage.cols];
	int* VP2 = new int[SrcImage.cols];
	HorizonProjection(SrcImage, HP);

	bool en = HorizonSegmentattion(SrcImage, HP, HPSeg1, HPSeg2);
	//cout << en << endl;
	if (!en)
	{
		HPSeg1.release();
		HPSeg2.release();
		delete[] HP;
		delete[] VP1;
		delete[] VP2;
		return false;
	}
	//imshow("HPSeg1", HPSeg1);
	//imshow("HPSeg2", HPSeg2);
	VerticalProjection(HPSeg1, VP1);

	en = VerticalSegmentattion(HPSeg1, VP1, dst1, MRZ_WIDTH_MAX, MRZ_WIDTH_MIN);

	if (!en)
	{
		HPSeg1.release();
		HPSeg2.release();
		delete[] HP;
		delete[] VP1;
		delete[] VP2;
		return false;
	}

	VerticalProjection(HPSeg2, VP2);
	en = VerticalSegmentattion(HPSeg2, VP2, dst2, MRZ_WIDTH_MAX, MRZ_WIDTH_MIN);


	HPSeg1.release();
	HPSeg2.release();
	delete[] HP;
	delete[] VP1;
	delete[] VP2;
	if (!en)
		return false;
	else
		return true;
}

bool ContentCheck::VerticalSegmentattion(const Mat& src, int* MatPro, Mat& dst, int param1, int param2)
{
	int * ImageSum = MatPro;
	Mat SrcImage = src.clone();
	int average = 0;
	int ColNum = src.cols;

	for (int i = 0; i < ColNum; i++)
		average = average + ImageSum[i];

	average = average / ColNum;

	average = average;

	int matcol[2];

	for (int i = 0; i < ColNum; i++)
	{
		if (ImageSum[i] < average)
		{
			if (!(ImageSum[i + 1] < average))
			{

				matcol[0] = i;

				break;
			}
		}

	}



	for (int i = ColNum - 1; (i > 0) || (i = 0); i--)
	{
		if (ImageSum[i] < average)
		{
			if (!(ImageSum[i - 1] < average))
			{

				matcol[1] = i;
				break;
			}
		}
	}


	int ColDev = matcol[1] - matcol[0];
	//cout << "ColDev" << endl;
	//cout << ColDev << endl;

	if (!((ColDev > param2)&(ColDev < param1)))
	{
		SrcImage.release();
		return false;
	}
	Mat SrcROI1(SrcImage, Rect(matcol[0], 0, ColDev, SrcImage.rows));

	dst = SrcROI1.clone();

	SrcROI1.release();
	SrcImage.release();

	return 1;
}

bool ContentCheck::HorizonSegmentattion(const Mat& src, int* MatPro, Mat& dst1, Mat& dst2)
{
	int* ImageSum = MatPro;
	Mat SrcImage = src.clone();
	int average = 0;
	int RowNum = src.rows;

	for (int i = 0; i < RowNum; i++)
		average = average + ImageSum[i];

	average = average / RowNum;

	average = average / 2;

	int MatRow[2][2] = { 0, 0, 0, 0 };

	int Count = 0;

	for (int i = RowNum - 1; i >MRZ_HIGH_MIN - 1; i--)
	{
		//cout << ImageSum[i] << endl;
		if (ImageSum[i] < average)
		{
			if (!(ImageSum[i - 1] < average))
			{
				for (int j = i - 1, RowDev = 0; (j > i - MRZ_HIGH_MAX)&(j > 1); j--)
				{
					if (ImageSum[j] > average)
					{
						if (!(ImageSum[j - 1] > average))
						{
							RowDev = i - j;

							if (RowDev > MRZ_HIGH_MIN)
							{
								MatRow[Count][0] = i;
								MatRow[Count][1] = j;
								Count++;

							}
							break;

						}
					}
				}
			}
		}
		if (Count == 2)
			break;
	}


	if (Count != 2)
	{
		SrcImage.release();
		return false;
	}

	int RowDev = MatRow[0][0] - MatRow[0][1];
	//cout << "RowDev1" << endl;
	//cout << RowDev << endl;
	Mat SrcROI1(SrcImage, Rect(0, MatRow[0][1], SrcImage.cols, RowDev));

	dst1 = SrcROI1.clone();

	RowDev = MatRow[1][0] - MatRow[1][1];
	//cout << "RowDev2" << endl;
	//cout << RowDev << endl;

	Mat SrcROI2(SrcImage, Rect(0, MatRow[1][1], SrcImage.cols, RowDev));

	dst2 = SrcROI2.clone();

	/*int DownCount = 0, UpCount = 0;

	sum = ImageSum.ptr<float>(0);

	for (int i = 0 ; i < ImageSum.rows - 1; i++)
	{
	if (sum[i] < average)
	{
	if (!(sum[i + 1] < average))
	{

	MatRow[0][DownCount] = i;

	DownCount++;
	}
	}

	if (DownCount == 2)
	break;
	}



	for (int i = 0; i < ImageSum.rows - 1; i++)
	{
	if (sum[i] > average)
	{
	if (!(sum[i + 1] > average))
	{

	MatRow[1][UpCount] = i;

	UpCount++;
	}
	}

	if (UpCount == 2)
	break;
	}

	if ((DownCount != 2) | (UpCount != 2))
	return false;

	if (!((MatRow[1][0] > MatRow[0][0])&(MatRow[1][1] > MatRow[0][1])))
	return false;

	int RowDev = MatRow[1][0] - MatRow[0][0];
	cout << RowDev << endl;
	if (!((RowDev > MRZ_HIGH_MIN )&(RowDev < MRZ_HIGH_MAX)))
	return false;

	Mat SrcROI1(SrcImage, Rect(0, MatRow[0][0], SrcImage.cols, RowDev));

	dst1 = SrcROI1.clone();
	RowDev = MatRow[1][1] - MatRow[0][1];
	cout << RowDev << endl;
	if (!((RowDev > MRZ_HIGH_MIN)&(RowDev < MRZ_HIGH_MAX)))
	return false;

	Mat SrcROI2(SrcImage, Rect(0, MatRow[0][1], SrcImage.cols, RowDev));

	dst2 = SrcROI2.clone();*/




	SrcROI1.release();
	SrcROI2.release();
	SrcImage.release();
	return true;
}
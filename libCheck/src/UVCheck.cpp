#include "ContentCheck.h"

int ** CreateArray(int dim1, int dim2)
{
	int ** Array = new int *[dim1];
	for (int i = 0; i < dim1; ++i)
	{
		Array[i] = new int[dim2];
	}
	return Array;
}

void DeleteArray(int ** Array, int dim1)
{
	for (int i = 0; i < dim1; ++i)
	{

		delete[] Array[i];
	}
	delete[] Array;
}

Mat  ContrastStretch_New(Mat src)
{
	Mat SrcImage = src.clone();
	int SrcRow = SrcImage.rows;
	int SrcCol = SrcImage.cols;

	if (SrcImage.isContinuous())
	{
		SrcCol = SrcCol * SrcRow;
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

int  UV_Search(Mat& src)
{
	Mat SrcImage = src.clone();

	if (!SrcImage.data)
		return 0;
	//imshow("Pattern_Search_UVSrcImage", SrcImage);
	int BlockSize = 5;
	//int BlockSize = 5;

	int ColNum = SrcImage.cols / BlockSize;
	int RowNum = SrcImage.rows / BlockSize;

	int ** MatBlock = CreateArray(RowNum, ColNum);

	int count = 0;
	int ValidNum = 0;
	Mat Seg;
	Mat get;
	int Num = 0;
	int PointX = 0;
	int PointY = 0;
	count = countNonZero(SrcImage);
	//cout << "ÓÐÐ§ÏñËØµã" << endl;
	//cout << count << endl;
	if (count > 10000)
		return 1;

	for (int i = 0; i < RowNum; i++)
	{
		PointY = i * BlockSize;
		for (int j = 0; j < ColNum; j++)
		{
			PointX = j * BlockSize;

			Seg = SrcImage(Rect(PointX, PointY, BlockSize, BlockSize));
			get = Seg.clone();

			ValidNum = countNonZero(get);

			if (ValidNum > (BlockSize * BlockSize * 1 / 2))
			{
				Num++;
				MatBlock[i][j] = 1;
			}
			else
			{
				MatBlock[i][j] = 0;
			}
		}
	}

	//cout << "Num:" << Num << endl;
	int Valid = count / (BlockSize * BlockSize);
	//cout << "Valid:" << Valid << endl;
	//cout << "Valid2:" << (Valid * 1 / 2) << endl;

	DeleteArray(MatBlock, RowNum);
	//if (Num > (Rad * 1 / 2))
	if (Num > (Valid * 1 / 2))
		return 1;
	else
		return 0;
}


Mat  UV_PROSCESS(Mat& src, int value)
{
	Mat SrcGray;
	cvtColor(src, SrcGray, CV_RGB2GRAY);
	//SrcGray = SrcGray - 10;

	Mat ResultImage1;
	//medianBlur(SrcGray, ResultImage1, 3);
	bilateralFilter(SrcGray, ResultImage1, 10, 10, 36);
	//ResultImage1 = ResultImage1 - 50;
	//Mat ResultImage2 = ContrastStretch_New(ResultImage1);
	Mat ResultImage2 = ResultImage1.clone();


	//imshow("ResultImage2", ResultImage2);
	int PointMin = (ResultImage2.rows > ResultImage2.cols) ? ResultImage2.cols : ResultImage2.rows;

	int BlockSize = 2 * (PointMin / 8) + 1;
	int ConstValue = value;
	Mat ResultImage3;
	adaptiveThreshold(ResultImage2, ResultImage3, 250, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, BlockSize, ConstValue);
	Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(ResultImage3, ResultImage3, MORPH_CLOSE, element1);

	SrcGray.release();
	ResultImage1.release();
	ResultImage2.release();
	return ResultImage3;

}


int  UV_Check(Mat& src)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ResultImage = UV_PROSCESS(SrcImage, -20);
	//Mat ResultImage = UV_PROSCESS(SrcImage, 40);

	//imshow("Pattern", ResultImage);
	int state = 0;
	Mat Seg;
	for (int i = 2; i > -1; i--)
	{
		Seg = ResultImage(Rect(ResultImage.cols * i / 3, 0, ResultImage.cols / 3, ResultImage.rows));
		state = UV_Search(Seg);
		if (state == 1)
			break;
	}


	if (state == 1)
		return 1;
	else
		return 2;

}
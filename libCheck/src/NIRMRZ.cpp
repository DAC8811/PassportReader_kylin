#include "ContentCheck.h"

#define AT_HIGH_MIN 60
#define AT_HIGH_MAX 220
#define AT_WIDTH_MIN 70
#define AT_WIDTH_MAX 180
#define AT_NIR_VALID_MAX 6000
#define AT_NIR_VALID_MIN 700

#define MRZ_HIGH_MIN 15
#define MRZ_HIGH_MAX 60
#define MRZ_WIDTH_MIN 700
#define MRZ_WIDTH_MAX 1250
#define MRZ_UV_VALID_MIN 2000
#define MRZ_UV_VALID_MAX 40000

int ContentCheck::Authenticator_NIR(const Mat& src)
{
	Mat SrcImage = src.clone();
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat ResultImage = DATA_NIR_PROSCESS(SrcImage);
	//imshow("ResultImageAT", ResultImage);
	if (!ResultImage.data)
	{
		SrcImage.release();
		ResultImage.release();
		return 3;
	}

	Mat seg;
	bool en = AT_NIR_ImageSegmentation(ResultImage, seg);

	if (!en)
	{

		ResultImage.release();
		SrcImage.release();
		seg.release();
		return 3;
	}
	//imshow("seg_AT", seg);
	en = CheckValid(seg, AT_NIR_VALID_MAX, AT_NIR_VALID_MIN);

	ResultImage.release();
	SrcImage.release();
	seg.release();

	if (en)
		return 1;
	else
		return 2;
}

bool ContentCheck::AT_HorizonSegmentattion(const Mat& src, int* MatPro, Mat& dst, int param1, int param2)
{
	int * ImageSum = MatPro;
	Mat SrcImage = src.clone();
	int average = 0;


	int RowNum = src.rows;

	for (int i = 0; i < RowNum; i++)
		average = average + ImageSum[i];

	average = average / RowNum;

	int MatRow[2] = { 0, 0 };

	int Count = 0;

	for (int i = 0; i < RowNum; i++)
	{
		if (ImageSum[i] < average)
		{
			if (!(ImageSum[i + 1] < average))
			{

				MatRow[Count] = i;

				Count++;
			}
		}

		if (Count == 1)
			break;
	}



	for (int i = RowNum - 1; i > 0; i--)
	{
		if (ImageSum[i] < average)
		{
			if (!(ImageSum[i - 1] < average))
			{

				MatRow[Count] = i;

				Count++;
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

	if (MatRow[1] < MatRow[0])
	{
		SrcImage.release();
		return false;
	}

	int RowDev = MatRow[1] - MatRow[0];
	//cout << "RowDev" << endl;
	//cout << RowDev << endl;

	if (!((RowDev > param2)&(RowDev < param1)))
	{
		SrcImage.release();
		return false;
	}


	Mat SrcROI1(SrcImage, Rect(0, MatRow[0], SrcImage.cols, RowDev));

	dst = SrcROI1.clone();


	SrcROI1.release();
	SrcImage.release();
	return true;
}


bool ContentCheck::AT_HorizonSegmentattion2(const Mat& src, int* MatPro, Mat& dst, int param1, int param2)
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
		//cout << ImageSum[i]
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

	int RowDev = MatRow[0][0] - MatRow[1][1];
	//cout << "RowDev1" << endl;
	//cout << RowDev << endl;
	Mat SrcROI1(SrcImage, Rect(0, MatRow[1][1], SrcImage.cols, RowDev));

	dst = SrcROI1.clone();

	SrcROI1.release();
	SrcImage.release();
	return true;
}

bool ContentCheck::AT_VerticalSegmentattion(const Mat& src, int* MatPro, Mat& dst)
{
	int* ImageSum = MatPro;
	Mat SrcImage = src.clone();
	int average = 0;

	int ColNum = src.cols;

	for (int i = 0; i < ColNum; i++)
		average = average + ImageSum[i];

	average = average / ColNum;
	average = average * 2 / 3;

	int MatCol[2] = { 0,0 };

	int count = 0;

	for (int i = 0; i < ColNum - 1; i++)
	{
		if (ImageSum[i] < average)
		{
			if (!(ImageSum[i + 1] < average))
			{
				if (count == 1)
					MatCol[0] = i;

				if (count == 6)
					MatCol[1] = i;

				count++;

				if (count == 7)
					break;

			}
		}

	}

	if (count != 7)
	{
		SrcImage.release();
		return false;
	}

	int ColDev = MatCol[1] - MatCol[0];

	//cout << ColDev << endl;
	if (!((ColDev > AT_WIDTH_MIN)&(ColDev < AT_WIDTH_MAX)))
	{
		SrcImage.release();
		return false;
	}


	Mat SrcROI1(SrcImage, Rect(MatCol[0], 0, ColDev, SrcImage.rows));
	//imshow("SrcROI1", SrcROI1);
	dst = SrcROI1.clone();

	SrcROI1.release();
	SrcImage.release();

	return 1;
}

bool ContentCheck::AT_NIR_ImageSegmentation(const Mat& src, Mat& dst)
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

	bool en = AT_HorizonSegmentattion2(SrcImage, HP, HPSeg, AT_HIGH_MAX, AT_HIGH_MIN);

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

	en = AT_VerticalSegmentattion(HPSeg, VP, dst);

	HPSeg.release();
	delete[] HP;
	delete[] VP;
	SrcImage.release();
	if (en)
		return true;
	else
		return false;
}
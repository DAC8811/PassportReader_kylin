#include "ContentCheck.h"

/*图像校正模块*/
int ContentCheck::Image_Modification(Mat& WIImg, Mat& UVImg, Mat& NIRImg)
{
	double theta = 0;
	int dev = 0;
	/*获取倾斜角*/
	int check = GetTheta(NIRImg, theta, dev);

	//std::cout << "倾斜角：" << dev << std::endl;

	if (check != 1)
		return 2;
	Mat SrcImage;
	/*--------------------------红外部分-----------------------------*/
	if (dev != 0)
	{
		dev = (dev + 50) / 2;
		/*图像映射校正*/
		ImageTransform(NIRImg, SrcImage, 0, dev);
	}
	else
	{
		SrcImage = NIRImg.clone();
	}
	/*图像尺寸校正*/
	Mat ResultImage = ImageRotate(SrcImage, theta);
	//imshow("src", NIRImg);
	//imshow("ResultImage2", ResultImage);
	//cout << "theta:" << theta << endl;
	//imwrite("ResultImage.jpg", ResultImage);
	int theta1 = theta * 3.1415926 / 180.0;
	theta1 = abs(theta1);
	//double param1 = 24.00 / (18.00 * sin(theta1) + 25.00 * cos(theta1));
	double param1 = 24.00 / (18.00 * sin(theta1) + 25.00 * cos(theta1));
	int ColLength = int((double)ResultImage.cols / param1);
	int RowLength = int((double)ResultImage.rows / param1);
	//dev = int((double)dev / param1);

	//cout << "param: " << param1 << endl;
	//cout << "RowLength: " << RowLength << endl;
	//cout << "ColLength: " << ColLength << endl;
	//imwrite("NewImage.jpg", ResultImage);
	Mat NewImage;
	resize(ResultImage, NewImage, Size(ColLength, RowLength), 0, 0, INTER_LINEAR);

	//imwrite("NewImage1.jpg", ResultImage);
	Mat DstImage = NewImage(Rect(NewImage.cols / 2 - 625, NewImage.rows / 2 - 450, 1250, 900));
	//imwrite("DstImage1.jpg", DstImage);

	NIRImg = DstImage.clone();
	//cout << "NIR  FINISH" << endl;

	SrcImage.release();

	/*--------------------------紫外部分-----------------------------*/
	if (dev != 0)
	{
		/*图像映射校正*/
		ImageTransform(UVImg, SrcImage, 0, dev);
	}
	else
	{
		SrcImage = UVImg.clone();
	}
	/*图像尺寸校正*/
	ResultImage = ImageRotate(SrcImage, theta);
	ColLength = int((double)ResultImage.cols / param1);
	RowLength = int((double)ResultImage.rows / param1);

	resize(ResultImage, NewImage, Size(ColLength, RowLength), 0, 0, INTER_LINEAR);
	//imwrite("NewImage2.jpg", ResultImage);
	DstImage = NewImage(Rect(NewImage.cols / 2 - 625, NewImage.rows / 2 - 450, 1250, 900));
	//imwrite("DstImage2.jpg", DstImage);

	UVImg = DstImage.clone();

	SrcImage.release();
	/*--------------------------白光部分-----------------------------*/
	if (dev != 0)
	{
		/*图像映射校正*/
		ImageTransform(WIImg, SrcImage, 0, dev);
	}
	else
	{
		SrcImage = WIImg.clone();
	}
	/*图像尺寸校正*/
	ResultImage = ImageRotate(SrcImage, theta);
	ColLength = int((double)ResultImage.cols / param1);
	RowLength = int((double)ResultImage.rows / param1);

	resize(ResultImage, NewImage, Size(ColLength, RowLength), 0, 0, INTER_LINEAR);
	//imwrite("NewImage2.jpg", ResultImage);
	DstImage = NewImage(Rect(NewImage.cols / 2 - 625, NewImage.rows / 2 - 450, 1250, 900));
	//imwrite("DstImage2.jpg", DstImage);

	WIImg = DstImage.clone();

	SrcImage.release();

	return 1;
}

void ContentCheck::ImageTransform(Mat const& src, Mat& dst, int dx, int dy)
{
	CV_Assert(src.depth() == CV_8U);

	const int rows = src.rows;
	const int cols = src.cols;

	dst.create(rows, cols, src.type());

	Vec3b *p;
	for (int i = 0; i < rows; i++)
	{
		p = dst.ptr<Vec3b>(i);
		for (int j = 0; j < cols; j++)
		{
			//平移后坐标映射到原图像
			int x = j - dx;
			int y = i - dy;

			//保证映射后的坐标在原图像范围内
			if (x >= 0 && y >= 0 && x < cols && y < rows)
				p[j] = src.ptr<Vec3b>(y)[x];
		}
	}
}

int  ContentCheck::GetTheta(Mat src, double & theta, int & dev)
{
	Mat SrcImg = src.clone();
	//SrcImg = src(Rect(src.cols * 1 / 5, src.rows * 3 / 4 , src.cols * 3 / 5, src.rows - src.rows * 4 / 5));
	SrcImg = src(Rect(250, 550, 750, 300));
	//imshow("SrcImg", SrcImg);

	Mat ResultImage = MRZ_NIR_PROSCESS(SrcImg);
	//imshow("ResultImage1", ResultImage);
	Mat Seg = ResultImage(Rect(0, 120, 750, 180));

	int* HP = new int[Seg.rows];
	HorizonProjection(Seg, HP);
	//cout << "ResultImage.rows: " << ResultImage.rows << endl;
	//imshow("ResultImage1", ResultImage);
	//imwrite("ResultImage1.jpg", ResultImage);


	int check = 0;
	int num = 0;
	int ValidNum = src.cols * 1 / 5;

	for (int n = Seg.rows - 1; n > 0; n--)
	{
		if (HP[n] < ValidNum)
		{
			num = n;
			check = 1;
			break;
		}

	}

	dev = 179 - num;
	//cout << "num1: " << num << endl;

	if (check != 1)
	{
		delete[] HP;
		return 0;
	}
	check = 0;


	int aver = 0;
	for (int n = 0; n < num; n++)
	{
		aver = aver + HP[n];
	}

	aver = (aver / Seg.rows) * 3 / 2;

	for (int n = num; n > 5; n--)
	{
		if ((HP[n] < aver) && (!(HP[n - 1] < aver)))
		{
			int count = 0;
			for (int m = n - 1; (m > -1) && (m > n - 80); m--)
				if ((HP[m] > aver) && (!(HP[m - 1] > aver)))
				{
					if (m < n - 10)
					{
						check = 1;
					}
					break;
				}
			if (check == 1)
			{
				num = n;
				//check = 1;
				break;
			}
		}
	}

	//cout << "num2: " << num << endl;
	if (check != 1)
	{
		delete[] HP;
		return 0;
	}


	check = 0;

	/*for (int n = num + 15; n < ResultImage.rows - 1; n++)
	{
	if ((HP[n - 1] < 1) && (!(HP[n ] < 1)))
	{
	num = n;
	check = 1;
	break;
	}

	}
	if (check != 1)
	num = ResultImage.rows - 1;*/
	num = (num + 30 > Seg.rows - 1) ? (Seg.rows - 1) : (num + 30);
	//cout << "num3: " << num << endl;
	//Mat NewImage = ResultImage(Rect(0, 0, ResultImage.cols, num));
	//SrcImg = src(Rect(src.cols * 1 / 5, (src.rows * 3 / 4 - src.rows / 5 + num - 2 ), src.cols * 3 / 5, src.rows / 5));
	Seg = ResultImage(Rect(0, 120 - 180 + num - 2, 750, 180));
	//Mat NewImage = DATA_NIR_PROSCESS(SrcImg);
	//Mat NewImage = DATA_NIR_PROSCESS(SrcImg);
	/*Mat SrcGray;
	cvtColor(SrcImg, SrcGray, CV_RGB2GRAY);
	Mat NewImage;
	medianBlur(SrcGray, SrcGray, 3);

	threshold(SrcGray, NewImage, 60, 255, THRESH_BINARY_INV|THRESH_OTSU);*/

	//imshow("Seg", Seg);
	Canny(Seg, Seg, 30, 300);
	//imshow("CannySeg", NewImage);
	//imwrite("CannySeg.jpg", NewImage);

	vector<pair<int, int>> pointsVec;
	for (int i = 0; i < Seg.rows; ++i)
		for (int j = 0; j < Seg.cols; ++j)
			if (Seg.at<uchar>(i, j) > 0)
				pointsVec.push_back(pair<int, int>(i, j));

	Mat points(pointsVec.size(), 2, CV_64FC1);
	for (int n = 0; n < pointsVec.size(); n++)
	{
		points.at<double>(n, 0) = (double)pointsVec[n].first;
		points.at<double>(n, 1) = (double)pointsVec[n].second;
	}


	PCA pca(points, Mat(), CV_PCA_DATA_AS_ROW);
	theta = atan(pca.eigenvectors.at<double>(0, 0) / pca.eigenvectors.at<double>(0, 1));


	theta = 180.0*theta / 3.1415926;
	delete[] HP;
	return 1;
}


Mat  ContentCheck::ImageRotate(Mat src, double theta)
{
	Mat originImg = src.clone();
	Mat ResultImage = MRZ_NIR_PROSCESS(src);

	Point2f center(ResultImage.cols / 2, ResultImage.rows / 2);
	Mat rot = getRotationMatrix2D(center, theta, 1);
	Rect bbox = RotatedRect(center, src.size(), theta).boundingRect();
	rot.at<double>(0, 2) += bbox.width / 2.0 - center.x;
	rot.at<double>(1, 2) += bbox.height / 2.0 - center.y;
	int originHeight = originImg.rows;
	Mat rotatedImg;
	cv::warpAffine(originImg, rotatedImg, rot, bbox.size());
	int cutMargin = (rotatedImg.rows - originHeight) / 2;
	rotatedImg = rotatedImg(Rect(0, cutMargin, rotatedImg.cols, rotatedImg.rows - 2 * cutMargin - 1));
	return rotatedImg;
}


Mat ContentCheck::MRZ_NIR_PROSCESS(const Mat& src)
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
	//int BlockSize = 2 * (PointMin / 8) + 1;
	//int ConstValue = 30;
	int BlockSize = 2 * (PointMin / 2) + 1;
	int ConstValue = 30;
	Mat ResultImage3;
	adaptiveThreshold(ResultImage2, ResultImage3, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, BlockSize, ConstValue);

	SrcGray.release();
	ResultImage1.release();
	ResultImage2.release();

	return ResultImage3;
}

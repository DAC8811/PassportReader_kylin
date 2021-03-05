#include "ContentCheck.h"
#include "PatternData.h"

int ContentCheck::Circle_Detect_USA(Mat & src)
{
	

	Mat SrcImg;
	resize(src, SrcImg, Size(src.cols / 2, src.rows / 2), 0, 0, INTER_LINEAR);
	//SrcImg = src.clone();
	int(*ImgData)[7] = USA_DATA;
	//SrcImg = src.clone();
	Mat Seg = SrcImg(Rect(300, 80, 310, 300));

	Mat LogImg(Seg.size(), CV_32FC3);
	for (int i = 0; i < Seg.rows; i++)
	{
		for (int j = 0; j < Seg.cols; j++)
		{
			LogImg.at<Vec3f>(i, j)[0] = log(1 + Seg.at<Vec3b>(i, j)[0]);
			LogImg.at<Vec3f>(i, j)[1] = log(1 + Seg.at<Vec3b>(i, j)[1]);
			LogImg.at<Vec3f>(i, j)[2] = log(1 + Seg.at<Vec3b>(i, j)[2]);
		}
	}

	normalize(LogImg, LogImg, 0, 255, CV_MINMAX);

	convertScaleAbs(LogImg, LogImg);

	Mat NewImg = Pattern_UV_PROCESS2(LogImg, 32, 10, 3);
	//Mat NewImg = Pattern_UV_PROCESS2(LogImg, 32, ImgData[0][0], 3);
	//Mat NewImg = Pattern_UV_PROCESS2(LogImg, 32, 0);
	Mat element1(3, 3, CV_8U, cv::Scalar(1));
	morphologyEx(NewImg, NewImg, MORPH_OPEN, element1);

	//Mat element2(3, 3, CV_8U, cv::Scalar(1));
	//morphologyEx(NewImg, NewImg, MORPH_CLOSE, element2);

	//imshow("src5", NewImg);
	//imshow("imageLog", LogImg);

	vector<Vec3f> circles;

	/// Apply the Hough Transform to find the circles
	HoughCircles(NewImg, circles, CV_HOUGH_GRADIENT, 1, NewImg.rows / 8, 80, 70, 0, 0);

	int check = 0;
	/// Draw the circles detected
	for (size_t i = 0; i < circles.size(); i++)
	{
		//Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);

		//circle(SrcImg, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		//circle(SrcImg, center, radius, Scalar(0, 0, 255), 3, 8, 0);
		//cout << "radius: " << radius << endl;

		if ((radius < 100) && (radius > 60))
		{
			check = 1;
			break;
		}
	}

	//namedWindow("Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE);
	//imshow("Hough Circle Transform Demo", SrcImg);
	if (check == 1)
		return 1;
	else
		return 2;
}
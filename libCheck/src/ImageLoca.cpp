#include "ContentCheck.h"

#define MRZ_HIGH_MIN 15			//MRZ有效高度下限
#define MRZ_HIGH_MAX 60			//MRZ有效高度上限
#define MRZ_WIDTH_MIN 700		//MRZ有效宽度下限
#define MRZ_WIDTH_MAX 1250		//MRZ有效宽度上限
#define MRZ_UV_VALID_MIN 2000	//MRZ有效面积下限
#define MRZ_UV_VALID_MAX 40000	//MRZ有效面积上限


/*红外机读码区定位模块*/
int ContentCheck::Location_NIR(const Mat src, int& DstX, int& DstY)
{
	if (!src.data)
	{
		return 0;
	}
	//std::cout << "进入参考点定位模块" << std::endl;
	/*图像大区域粗分割*/
	int PointY = ((src.rows - 300) > 0 ) ? (src.rows - 300) : 0;

	Mat SrcImage = src(Rect(0, PointY, 500, 300));
	//imshow("参考点区域", SrcImage);
	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	/*图像二值化*/
	Mat SrcGray;
	//SrcGray = DATA_NIR_PROSCESS(SrcImage);
	SrcGray = CHAR_WI_PROSCESS(SrcImage, 2, 30, 3);
	//cvtColor(SrcImage, SrcGray, CV_RGB2GRAY);
	//imshow("二值化的参考点区域", SrcGray);

	/*图像区域细分割*/
	int RowNum = SrcGray.rows;
	int ColNum = SrcGray.cols;
	int* HP = new int[SrcImage.rows];
	int* VP = new int[SrcImage.cols];
	Mat seg;

	/*水平投影*/
	HorizonProjection(SrcGray, HP);

	
	int MatAver = 0;

	for (int i = 0; i < RowNum; i++)
		MatAver = MatAver + HP[i];

	MatAver = MatAver / RowNum;

	//cout << "水平投影的均值" <<MatAver <<endl;
	MatAver = MatAver ;//图像的均值

	int count = 0;
	int Scale[2][2] = { 0, 0, 0, 0 };//存储分割的坐标

	//水平分割与垂直分割（先水平后垂直）
	for (int i = SrcGray.rows - 1, rows = 0, VPAver = 0; i > MRZ_HIGH_MAX; --i)
	{
		if ((HP[i] < MatAver)&(HP[i - 1] >= MatAver))
		{
			count = 0;
			//std::cout << "第一段字符区域下边界：" << i << endl;
			for (int j = i - 1; (j > 0)&(j > i - MRZ_HIGH_MAX * 3); --j)
			{
				//std::cout << "第一段字符区域下边界：" << HP[j - 1] << std::endl;
				if ((HP[j] > MatAver)&(HP[j - 1] <= MatAver))
				{
					++count;
					rows = i - j;
					//std::cout << "第一段字符区域的间隔：" << rows << std::endl;

					if (count == 1)
					{
						if (!((rows < MRZ_HIGH_MAX)&(rows > MRZ_HIGH_MIN)))
						{
							//std::cout << "类型1重置"<< std::endl;
							count = 0;
							break;
						}
					}
					else if (count == 2)
					{
						if (rows < MRZ_HIGH_MAX)
						{
							//std::cout << "类型2重置" << std::endl;
							count = 0;
							break;
						}
					}

					if (count == 2)
					{

						seg = SrcGray(Rect(0, j, ColNum, rows));

						if (!seg.data)
							break;
						//imshow("seg", seg);

						/*垂直投影*/
						VerticalProjection(seg, VP);

						/*垂直分割*/
						for (int i = 0; i < ColNum; ++i)
							VPAver = VPAver + VP[i];

						VPAver = VPAver / ColNum;
						VPAver = VPAver;
						//std::cout << "VPAver" << endl;
						//std::cout << VPAver << endl;


						for (int m = 0; m < ColNum; ++m)
						{
							//cout << VP[m + 1] << endl;
							if ((VP[m] < VPAver)&(!(VP[m + 1] < VPAver)))
							{
								Scale[1][0] = m;
								int num = 0;

								for (int z = m + 2; (z < m + 9)&(z < ColNum); ++z)
									if (!(VP[m + 1] < VPAver))
										num++;
								if (num > 4)
								{
									++count;
									break;
								}
								else
								{
									Scale[1][0] = 0;
								}
							}
						}

						if (Scale[1][0] == 0)
							count = 0;
						else
						{
							Scale[0][0] = j;
							Scale[0][1] = i;
							break;
						}
					}

				}
			}

			if (count == 3)
				break;
		}
	}

	//std::cout << "坐标参数" << endl;
	//std::cout << Scale[0][0] << endl;
	//std::cout << Scale[0][1] << endl;
	//std::cout << Scale[1][0] << endl;
	//std::cout << Scale[1][1] << endl;

	//seg = SrcGray(Rect(Scale[1][0], Scale[0][0], ColNum - Scale[1][0], Scale[0][1] - Scale[0][0]));
	//imshow("Locseg", seg);

	SrcImage.release();
	SrcGray.release();
	seg.release();
	delete[] HP;
	delete[] VP;

	//std::cout << "结束参考点定位模块" << std::endl;

	if (count == 3)
	{
		DstY = Scale[0][0] + src.rows - 300;//MRZ左上角顶点的x轴坐标
		DstX = Scale[1][0]; //MRZ左上角顶点的y轴坐标

		//std::cout << "坐标参数" << endl;
		//std::cout << "DstY:" << DstY << endl;
		//std::cout << "DstX:" << DstX << endl;

		return 1;
	}
	else
		return 6;

}

/*int ContentCheck::Location_UV(const Mat src, int& DstX, int& DstY,int select)
{
Mat SrcImage = src.clone();

if (!SrcImage.data)
{
SrcImage.release();
return 0;
}

Mat SrcGray;
int check = 0;

if(select == 0)
check = MRZ_UV_PROSCESS(SrcImage, SrcGray, 4);
else if(select == 1)
check = MRZ_UV_PROSCESS(SrcImage, SrcGray, 5);
else if (select == 2)
check = MRZ2_UV_PROSCESS(SrcImage, SrcGray, 20);

if (check != 1)
{
SrcImage.release();
SrcGray.release();
return 6;
}
//cvtColor(SrcImage, SrcGray, CV_RGB2GRAY);
Mat showimag(SrcGray,Rect(0, SrcGray.rows/2-1, SrcGray.cols, SrcGray.rows / 2 - 1));
//imshow("SrcGray", showimag);
int RowNum = SrcGray.rows;
int ColNum = 1200;
int* HP = new int[RowNum];
int* VP = new int[ColNum];

Mat seg;


HorizonProjection(SrcGray, HP);


int MatAver = 0;

for (int i = 0; i < RowNum; i++)
{
//cout << HP[i] << endl;
MatAver = MatAver + HP[i];
}

MatAver = MatAver / RowNum;

cout << "MatAver" << endl;
cout << MatAver << endl;
//cout << "233" << endl;
MatAver = MatAver;

int VPAver = 0;
int rows = 0;
int count = 0;

int Scale[2][2] = { 0, 0, 0, 0 };

for (int i = SrcGray.rows - 1; i > SrcGray.rows / 2; i--)
{
if ((HP[i] < MatAver)&(HP[i - 1] > MatAver))
{
count = 0;
for (int j = i - 1; (j > SrcGray.rows / 2)&(j > i - MRZ_HIGH_MAX * 3); j--)
{
if ((HP[j] > MatAver)&(HP[j - 1] < MatAver))
{
count++;
rows = i - j;
cout << "rows" << endl;
cout << rows << endl;
if (count == 1)
{
if (!((rows < MRZ_HIGH_MAX)&(rows > MRZ_HIGH_MIN)))
break;
}
else if (count == 2)
{
if (rows < MRZ_HIGH_MAX)
{
count = 0;
break;
}
}

if (count == 2)
{

seg = SrcGray(Rect(20, j, ColNum, rows));

if (!seg.data)
break;
//imshow("seg", seg);

VerticalProjection(seg, VP);

for (int i = 0; i < ColNum; i++)
VPAver = VPAver + VP[i];

VPAver = VPAver / ColNum;
cout << VPAver << endl;


for (int m = 0; m < seg.cols / 6; m++)
{
if ((VP[m] < VPAver)&(VP[m + 1] > VPAver))
{
Scale[1][0] = m;
break;
}
}

for (int m = seg.cols - 1; m > seg.cols * 5 / 6; m--)
{
if ((VP[m] < VPAver)&(VP[m - 1] > VPAver))
{
Scale[1][1] = m;
break;
}

}

if (Scale[1][1] - Scale[1][0] < 900)
count = 0;
else
{
Scale[0][0] = j;
Scale[0][1] = i;
break;
}
}

}
}

if (count == 2)
break;
}
}
cout << "参数" << endl;
cout << Scale[0][0] << endl;
cout << Scale[0][1] << endl;
cout << Scale[1][0] << endl;
cout << Scale[1][1] << endl;


//seg = SrcGray(Rect(Scale[1][0] + 20, Scale[0][0], Scale[1][1] - Scale[1][0], Scale[0][1] - Scale[0][0]));
//imshow("seg", seg);

SrcImage.release();
SrcGray.release();
seg.release();
delete[] HP;
delete[] VP;

if (count == 2)
{
DstY = Scale[0][0];
DstX = Scale[1][0] + 20;



return 1;
}
else
return 6;

}*/

/*紫外机读码区定位模块*/
int ContentCheck::Location_UV(const Mat src, int& DstX, int& DstY, int select)
{
	if (!src.data)
	{
		return 0;
	}
	/*图像大区域粗分割*/
	int PointY = ((src.rows - 250) > 0) ? (src.rows - 250) : 0;
	Mat SrcImage = src(Rect(0, PointY, 300, 250));

	if (!SrcImage.data)
	{
		SrcImage.release();
		return 0;
	}

	Mat SrcGray, ResultImage;
	/*int check = 0;
	//RGB_Black_Process(SrcImage, SrcGray);
	if (select == 0)
	check = RGB_Black_Process(SrcImage, SrcGray);
	//check = MRZ_UV_PROSCESS(SrcImage, SrcGray, 4);
	else if (select == 1)
	check = MRZ_UV_PROSCESS(SrcImage, SrcGray, 4);
	else if (select == 2)
	check = MRZ_UV_PROSCESS(SrcImage, SrcGray, 5);
	else if (select == 3)
	check = MRZ2_UV_PROSCESS(SrcImage, SrcGray, 30);
	*/


	int check = 1;
	Mat image01;
	/*图像滤波*/
	//bilateralFilter(SrcImage, image01, 30, 10, 16);
	medianBlur(SrcImage, image01, 3);
	//////////////////////////////////

	//bilateralFilter(image02, image01, 24, 6, 6);
	//GaussianBlur(image02, image01, Size(3, 3), 0, 0);

	/*图像对数变换*/
	Mat imageLog(image01.size(), CV_32FC3);
	for (int i = 0; i < image01.rows; i++)
	{
		for (int j = 0; j < image01.cols; j++)
		{
			imageLog.at<Vec3f>(i, j)[0] = log(1 + image01.at<Vec3b>(i, j)[0]);
			imageLog.at<Vec3f>(i, j)[1] = log(1 + image01.at<Vec3b>(i, j)[1]);
			imageLog.at<Vec3f>(i, j)[2] = log(1 + image01.at<Vec3b>(i, j)[2]);
		}
	}

	normalize(imageLog, imageLog, 0, 255, CV_MINMAX);

	convertScaleAbs(imageLog, imageLog);
	//imshow("imageLog", imageLog);
	//check = RGB_Black_Process(imageLog, SrcGray);

	cvtColor(imageLog, ResultImage, CV_BGR2GRAY);



	//imshow("ResultImage", ResultImage);
	/*图像自适应二值化*/
	int PointMin = (ResultImage.rows > ResultImage.cols) ? ResultImage.cols : ResultImage.rows;
	int BlockSize = 2 * (PointMin / 8) + 1;
	//int ConstValue = 40;
	int ConstValue = 60;

	adaptiveThreshold(ResultImage, SrcGray, 250, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, BlockSize, ConstValue);

	//imshow("SrcGray", SrcGray);
	/*统计有效点数*/
	int ValidNum = countNonZero(SrcGray);

	//cout << "ValidNum参数" << endl;
	//cout << ValidNum << endl;
	if ((check != 1) | (ValidNum < 700) | (ValidNum > 80000))
	{
		SrcImage.release();
		SrcGray.release();
		return 6;
	}

	int RowNum = SrcGray.rows;
	int ColNum = SrcGray.cols;
	int* HP = new int[RowNum];
	int* VP = new int[ColNum];

	Mat seg;

	/*水平投影*/
	HorizonProjection(SrcGray, HP);


	int MatAver = 0;

	for (int i = 0; i < RowNum; i++)
	{
		//cout << HP[i] << endl;
		MatAver = MatAver + HP[i];
	}

	MatAver = MatAver / RowNum;

	//cout << "MatAver" << endl;
	//cout << MatAver << endl;
	//cout << "233" << endl;
	//MatAver = MatAver / 3 ;
	MatAver = MatAver;//图像的均值

	int VPAver = 0;
	int rows = 0;
	int count = 0;

	int Scale[2][2] = { 0, 0, 0, 0 };//存储分割的坐标
	/*水平分割与垂直分割（先水平后垂直）*/
	for (int i = SrcGray.rows - 1; i > SrcGray.rows / 4; i--)
	{
		//cout << HP[i - 1] << endl;
		if ((HP[i] < MatAver)&(!(HP[i - 1] < MatAver)))
		{
			count = 0;
			for (int j = i - 1; (j > 0)&(j > i - MRZ_HIGH_MAX * 3); j--)
			{
				//cout << HP[j - 1] << endl;
				if ((HP[j] > MatAver)&(!(HP[j - 1] > MatAver)))
				{
					count++;
					rows = i - j;
					//cout << "rows" << endl;
					//cout << rows << endl;

					if (count == 1)
					{
						if (!((rows < MRZ_HIGH_MAX)&(rows > MRZ_HIGH_MIN)))
							break;
					}
					else if (count == 2)
					{
						if (rows < MRZ_HIGH_MAX)
						{
							count = 0;
							break;
						}
					}

					if (count == 2)
					{

						seg = SrcGray(Rect(0, j, ColNum, rows));

						if (!seg.data)
							break;
						//imshow("seg", seg);
						/*垂直投影*/
						VerticalProjection(seg, VP);
						/*垂直分割*/
						for (int i = 0; i < ColNum; i++)
							VPAver = VPAver + VP[i];

						VPAver = VPAver / ColNum;
						VPAver = VPAver;
						///cout << "VPAver" << endl;
						//cout << VPAver << endl;


						for (int m = 0; m < ColNum; m++)
						{
							//cout << VP[m + 1] << endl;
							if ((VP[m] < VPAver)&(!(VP[m + 1] < VPAver)))
							{
								Scale[1][0] = m;
								//count++;
								//break;
								int num = 0;

								for (int z = m + 2; (z < m + 9)&(z < ColNum); z++)
									if (!(VP[z] < VPAver))
										num++;
								if (num > 3)
								{
									count++;
									break;
								}
								else
								{
									Scale[1][0] = 0;
								}
							}
						}

						if (Scale[1][0] == 0)
							count = 0;
						else
						{
							Scale[0][0] = j;
							Scale[0][1] = i;
							break;
						}
					}

				}
			}

			if (count == 3)
				break;
		}
	}

	//cout << "坐标参数" << endl;
	//cout << Scale[0][0] << endl;
	//cout << Scale[0][1] << endl;
	//cout << Scale[1][0] << endl;
	//cout << Scale[1][1] << endl;


	//seg = SrcGray(Rect(Scale[1][0], Scale[0][0], ColNum - Scale[1][0], Scale[0][1] - Scale[0][0]));
	//imshow("Location_seg", seg);

	SrcImage.release();
	SrcGray.release();
	seg.release();
	delete[] HP;
	delete[] VP;

	if (count == 3)
	{
		DstY = Scale[0][0] + src.rows - 250;//MRZ左上角顶点的x轴坐标
		DstX = Scale[1][0];//MRZ左上角顶点的y轴坐标

		//cout << "DstX" << endl;
		//cout << DstX << endl;
		//cout << "DstY" << endl;
		//cout << DstY << endl;


		return 1;
	}
	else
		return 6;

}

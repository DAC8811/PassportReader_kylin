#include "ContentCheck.h"
#include "PatternData.h"


int ContentCheck::PatternMtach_UV(Mat& src, int nation, char const* ImageLoc)
{
	if (!src.data)
		return 0;

	int type = 0;
	int (*ImgData)[7] = NULL;
	switch (nation)
	{
	case 1:
	{
		type = 1;
		ImgData = AFG_DATA;
		break;
	}
	case 2:
	{
		type = 1;
		ImgData = ALB_DATA;
		break;
	}
	case 5:
	{
		type = 1;
		ImgData = AFG_DATA;
		break;
	}
	case 6:
	{
		type = 2;
		ImgData = ARM_DATA;
		break;
	}
	case 7:
	{
		type = 3;
		ImgData = AUS_DATA;
		break;
	}
	case 8:
	{
		type = 1;
		ImgData = AUT_DATA;
		break;
	}
	case 9:
	{
		type = 1;
		ImgData = AZE_DATA;
		break;
	}
	case 10:
	{
		type = 1;
		ImgData = ARE_DATA;
		break;
	}
	case 12:
	{
		type = 2;
		ImgData = BHS_DATA;
		break;
	}
	case 13:
	{
		type = 1;
		ImgData = BHR_DATA;
		break;
	}
	case 14:
	{
		type = 1;
		ImgData = BGD_DATA;
		break;
	}
	case 15:
	{
		type = 1;
		ImgData = BRB_DATA;
		break;
	}
	case 16:
	{
		type = 1;
		ImgData = BLR_DATA;
		break;
	}
	case 18:
	{
		type = 1;
		ImgData = BEN_DATA;
		break;
	}

	case 19:
	{
		type = 1;
		ImgData = BTN_DATA;
		break;
	}
	case 21:
	{
		type = 1;
		ImgData = BWA_DATA;
		break;
	}
	case 22:
	{
		type = 2;
		ImgData = BRA_DATA;
		break;
	}
	case 23:
	{
		type = 1;
		ImgData = BRN_DATA;
		break;
	}
	case 24:
	{
		type = 1;
		ImgData = BGR_DATA;
		break;
	}
	case 25:
	{
		type = 2;
		ImgData = BFA_DATA;
		break;
	}
	case 26:
	{
		type = 2;
		ImgData = BDI_DATA;
		break;
	}
	case 27:
	{
		type = 1;
		ImgData = CMR_DATA;
		break;
	}
	case 28:
	{
		type = 1;
		ImgData = CAN_DATA;
		break;
	}
	case 29:
	{
		type = 2;
		ImgData = CPV_DATA;
		break;
	}
	case 31:
	{
		type = 2;
		ImgData = CHL_DATA;
		break;
	}
	case 32:
	{
		type = 7;
		ImgData = CHN_DATA;
		break;
	}
	case 33:
	{
		type = 1;
		ImgData = CIV_DATA;
		break;
	}
	case 34:
	{
		type = 1;
		ImgData = COL_DATA;
		break;
	}
	case 36:
	{
		type = 1;
		ImgData = COD_DATA;
		break;
	}
	case 37:
	{
		type = 1;
		ImgData = CUB_DATA;
		break;
	}
	case 38:
	{
		type = 2;
		ImgData = CYP_DATA;
		break;
	}
	case 39:
	{
		type = 1;
		ImgData = CZE_DATA;
		break;
	}
	case 40:
	{
		type = 1;
		ImgData = CHE_DATA;
		break;
	}
	case 42:
	{
		type = 1;
		ImgData = DNK_DATA;
		break;
	}
	case 43:
	{
		type = 1;
		ImgData = DJI_DATA;
		break;
	}
	case 44:
	{
		type = 2;
		ImgData = DOM_DATA;
		break;
	}
	case 45:
	{
		type = 2;
		ImgData = DEU_DATA;
		break;
	}
	case 46:
	{
		type = 2;
		ImgData = EST_DATA;
		break;
	}
	case 48:
	{
		type = 1;
		ImgData = ECU_DATA;
		break;
	}
	case 49:
	{
		type = 1;
		ImgData = EGY_DATA;
		break;
	}
	case 50:
	{
		type = 2;
		ImgData = ESP_DATA;
		break;
	}
	case 51:
	{
		type = 2;
		ImgData = FJI_DATA;
		break;
	}
	case 52:
	{
		type = 2;
		ImgData = FIN_DATA;
		break;
	}
	case 53:
	{
		type = 2;
		ImgData = FRA_DATA;
		break;
	}
	case 54:
	{
		type = 2;
		ImgData = GAB_DATA;
		break;
	}
	//case 54:
	//{
	//	type = 1;
	//	ImgData = GAB_DATA;
	//	break;
	//}
	case 55:
	{
		type = 1;
		ImgData = GNQ_DATA;
		break;
	}
	case 56:
	{
		type = 2;
		ImgData = GMB_DATA;
		break;
	}
	case 57:
	{
		type = 1;
		ImgData = GEO_DATA;
		break;
	}
	case 58:
	{
		type = 2;
		ImgData = GHA_DATA;
		break;
	}
	case 60:
	{
		type = 1;
		ImgData = GRD_DATA;
		break;
	}
	case 61:
	{
		type = 1;
		ImgData = GTM_DATA;
		break;
	}
	case 63:
	{
		type = 2;
		ImgData = GUY_DATA;
		break;
	}
	case 64:
	{
		type = 3;
		ImgData = GBR_DATA;
		break;
	}
	case 65:
	{
		type = 1;
		ImgData = HTI_DATA;
		break;
	}
	case 66:
	{
		type = 2;
		ImgData = HND_DATA;
		break;
	}
	case 67:
	{
		type = 1;
		ImgData = HUN_DATA;
		break;
	}
	case 68:
	{
		type = 1;
		ImgData = HRV_DATA;
		break;
	}
	case 69:
	{
		type = 2;
		ImgData = ISL_DATA;
		break;
	}
	case 70:
	{
		type = 1;
		ImgData = IND_DATA;
		break;
	}
	case 71:
	{
		type = 1;
		ImgData = IDN_DATA;
		break;
	}
	case 72:
	{
		type = 1;
		ImgData = IRN_DATA;
		break;
	}
	case 73:
	{
		type = 1;
		ImgData = IRQ_DATA;
		break;
	}

	case 74:
	{
		type = 3;
		ImgData = ISR_DATA;
		break;
	}
	case 75:
	{
		type = 4;
		ImgData = ITA_DATA;
		break;
	}
	case 76:
	{
		type = 2;
		ImgData = JPN_DATA;
		break;
	}
	case 77:
	{
		type = 2;
		ImgData = JOR_DATA;
		break;
	}
	case 78:
	{
		type = 2;
		ImgData = KAZ_DATA;
		break;
	}
	case 79:
	{
		type = 1;
		ImgData = KEN_DATA;
		break;
	}
	case 80:
	{
		type = 1;
		ImgData = KOR_DATA;
		break;
	}
	case 81:
	{
		type = 1;
		ImgData = KGZ_DATA;
		break;
	}
	case 82:
	{
		type = 2;
		ImgData = KWT_DATA;
		break;
	}
	case 83:
	{
		type = 2;
		ImgData = KHM_DATA;
		break;
	}
	case 84:
	{
		type = 1;
		ImgData = KNA_DATA;
		break;
	}
	case 85:
	{
		type = 1;
		ImgData = LAO_DATA;
		break;
	}
	case 86:
	{
		type = 2;
		ImgData = LVA_DATA;
		break;
	}
	case 87:
	{
		type = 1;
		ImgData = LBN_DATA;
		break;
	}
	case 88:
	{
		type = 1;
		ImgData = LSO_DATA;
		break;
	}

	case 89:
	{
		type = 2;
		ImgData = LBR_DATA;
		break;
	}
	case 90:
	{
		type = 1;
		ImgData = LBY_DATA;
		break;
	}
	case 91:
	{
		type = 1;
		ImgData = LIE_DATA;
		break;
	}
	case 92:
	{
		type = 1;
		ImgData = LTU_DATA;
		break;
	}
	case 93:
	{
		type = 2;
		ImgData = LUX_DATA;
		break;
	}
	case 94:
	{
		type = 2;
		ImgData = LKA_DATA;
		break;
	}
	case 95:
	{
		type = 2;
		ImgData = LCA_DATA;
		break;
	}
	case 96:
	{
		type = 1;
		ImgData = MMR_DATA;
		break;
	}
	case 97:
	{
		type = 2;
		ImgData = MDG_DATA;
		break;
	}
	case 99:
	{
		type = 2;
		ImgData = MYS_DATA;
		break;
	}
	case 100:
	{
		type = 2;
		ImgData = MDV_DATA;
		break;
	}

	case 101:
	{
		type = 2;
		ImgData = MLI_DATA;
		break;
	}
	case 102:
	{
		type = 1;
		ImgData = MRT_DATA;
		break;
	}
	case 104:
	{
		type = 3;
		ImgData = MEX_DATA;
		break;
	}
	case 105:
	{
		type = 2;
		ImgData = MDA_DATA;
		break;
	}
	case 106:
	{
		type = 1;
		ImgData = MCO_DATA;
		break;
	}
	case 107:
	{
		type = 2;
		ImgData = MNG_DATA;
		break;
	}
	case 109:
	{
		type = 2;
		ImgData = MOZ_DATA;
		break;
	}
	case 110:
	{
		type = 1;
		ImgData = NAM_DATA;
		break;
	}
	case 111:
	{
		type = 1;
		ImgData = NRU_DATA;
		break;
	}
	case 113:
	{
		type = 2;
		ImgData = NLD_DATA;
		break;
	}
	case 114:
	{
		type = 2;
		ImgData = NZL_DATA;
		break;
	}
	case 115:
	{
		type = 1;
		ImgData = NIC_DATA;
		break;
	}

	case 117:
	{
		type = 1;
		ImgData = NOR_DATA;
		break;
	}
	case 118:
	{
		type = 3;
		ImgData = OMN_DATA;
		break;
	}
	case 119:
	{
		type = 1;
		ImgData = PAK_DATA;
		break;
	}
	case 120:
	{
		type = 1;
		ImgData = PLW_DATA;
		break;
	}
	case 122:
	{
		type = 1;
		ImgData = PAN_DATA;
		break;
	}
	case 124:
	{
		type = 2;
		ImgData = PRY_DATA;
		break;
	}
	case 125:
	{
		type = 2;
		ImgData = PER_DATA;
		break;
	}
	case 126:
	{
		type = 3;
		ImgData = PHL_DATA;
		break;
	}
	case 127:
	{
		type = 1;
		ImgData = POL_DATA;
		break;
	}
	case 128:
	{
		type = 2;
		ImgData = PRT_DATA;
		break;
	}
	case 129:
	{
		type = 1;
		ImgData = PRK_DATA;
		break;
	}
	case 130:
	{
		type = 1;
		ImgData = QAT_DATA;
		break;
	}
	case 131:
	{
		type = 1;
		ImgData = ROU_DATA;
		break;
	}
	case 133:
	{
		type = 1;
		ImgData = RWA_DATA;
		break;
	}
	case 134:
	{
		type = 2;
		ImgData = SMR_DATA;
		break;
	}
	case 137:
	{
		type = 1;
		ImgData = SEN_DATA;
		break;
	}
	case 138:
	{
		type = 1;
		ImgData = SRB_DATA;
		break;
	}
	case 139:
	{
		type = 1;
		ImgData = SYC_DATA;
		break;
	}
	case 140:
	{
		type = 1;
		ImgData = SGP_DATA;
		break;
	}
	case 141:
	{
		type = 1;
		ImgData = SVK_DATA;
		break;
	}
	case 142:
	{
		type = 2;
		ImgData = SVN_DATA;
		break;
	}
	case 143:
	{
		type = 1;
		ImgData = SOM_DATA;
		break;
	}
	case 144:
	{
		type = 1;
		ImgData = SSD_DATA;
		break;
	}
	case 145:
	{
		type = 1;
		ImgData = SDN_DATA;
		break;
	}
	case 146:
	{
		type = 1;
		ImgData = SUR_DATA;
		break;
	}
	case 148:
	{
		type = 1;
		ImgData = SYR_DATA;
		break;
	}
	case 149:
	{
		type = 2;
		ImgData = TWN_DATA;
		break;
	}
	case 150:
	{
		type = 1;
		ImgData = TJK_DATA;
		break;
	}
	case 152:
	{
		type = 3;
		ImgData = THA_DATA;
		break;
	}
	case 153:
	{
		type = 1;
		ImgData = TGO_DATA;
		break;
	}
	case 154:
	{
		type = 1;
		ImgData = TON_DATA;
		break;
	}
	case 155:
	{
		type = 2;
		ImgData = TTO_DATA;
		break;
	}
	case 157:
	{
		type = 2;
		ImgData = TUR_DATA;
		break;
	}
	case 158:
	{
		type = 1;
		ImgData = UGA_DATA;
		break;
	}
	case 160:
	{
		type = 1;
		ImgData = USA_DATA;
		break;
	}
	case 161:
	{
		type = 3;
		ImgData = URY_DATA;
		break;
	}
	case 162:
	{
		type = 1;
		ImgData = UZB_DATA;
		break;
	}
	case 163:
	{
		type = 1;
		ImgData = VUT_DATA;
		break;
	}
	case 165:
	{
		type = 1;
		ImgData = VEN_DATA;
		break;
	}
	case 166:
	{
		type = 2;
		ImgData = VNM_DATA;
		break;
	}
	case 167:
	{
		type = 2;
		ImgData = VCT_DATA;
		break;
	}
	case 168:
	{
		type = 1;
		ImgData = WSM_DATA;
		break;
	}
	case 169:
	{
		type = 3;
		ImgData = YEM_DATA;
		break;
	}
	case 170:
	{
		type = 1;
		ImgData = ZMB_DATA;
		break;
	}
	case 172:
	{
		type = 1;
		ImgData = ZAF_DATA;
		break;
	}
	case 173:
	{
		type = 2;
		ImgData = MKD_DATA;
		break;
	}
	case 177:
	{
		type = 1;
		ImgData = COG_DATA;
		break;
	}
	case 181:
	{
		type = 1;
		ImgData = NGA_DATA;
		break;
	}
	case 182:
	{
		type = 1;
		ImgData = MNE_DATA;
		break;
	}
	case 183:
	{
		type = 1;
		ImgData = SWE_DATA;
		break;
	}
	case 184:
	{
		type = 1;
		ImgData = ETH_DATA;
		break;
	}
	case 185:
	{
		type = 2;
		ImgData = CRI_DATA;
		break;
	}
	case 186:
	{
		type = 2;
		ImgData = SLV_DATA;
		break;
	}
	case 187:
	{
		type = 1;
		ImgData = ZWE_DATA;
		break;
	}
	case 188:
	{
		type = 2;
		ImgData = JAM_DATA;
		break;
	}

	case 189:
	{
		type = 1;
		ImgData = RKS_DATA;
		break;
	}
	case 190:
	{
		type = 1;
		ImgData = DMA_DATA;
		break;
	}
	case 191:
	{
		type = 1;
		ImgData = IRL_DATA;
		break;
	}
	case 260:
	{
		type = 1;
		ImgData = TUV_DATA;
		break;
	}

	case 261:
	{
		type = 1;
		ImgData = DMA_DATA;
		break;
	}

	case 262:
	{
		type = 1;
		ImgData = SLB_DATA;
		break;
	}

	case 263:
	{
		type = 3;
		ImgData = GNB_DATA;
		break;
	}

	case 264:
	{
		type = 1;
		ImgData = SLE_DATA;
		break;
	}
	case 265:
	{
		type = 1;
		ImgData = TCA_DATA;
		break;
	}
	case 266:
	{
		type = 1;
		ImgData = CYM_DATA;
		break;
	}
	case 267:
	{
		type = 1;
		ImgData = ATG_DATA;
		break;
	}
	case 268:
	{
		type = 2;
		ImgData = BIH_DATA;
		break;
	}
	case 269:
	{
		type = 1;
		ImgData = TKM_DATA;
		break;
	}
	default:
		return 5;
	}

	Mat ResultImage[10];
	Mat NewImg[10];
	Mat SrcImg;

	for (int m = 0; m < type; m++)
	{
		SrcImg = src(Rect(ImgData[m][1], ImgData[m][2], ImgData[m][3], ImgData[m][4]));

		ResultImage[m]= Pattern_UV_PROCESS2(SrcImg, ImgData[m][6],ImgData[m][0], ImgData[m][5]);

		resize(ResultImage[m], NewImg[m], Size(ResultImage[m].cols / 2, ResultImage[m].rows / 2), 0, 0, INTER_LINEAR);
	}
	//imshow("NewImg[3]", NewImg[3]);

//	TCHAR curDllPath[MAX_PATH] = { 0 };
//	GetModuleFileName(GetSelfModuleHandle(), curDllPath, MAX_PATH);
//	(_tcsrchr(curDllPath, _T('\\')))[1] = 0; //该版本为windows,linux中停止条件是'/'
//	CString filepath = curDllPath;
//	filepath = filepath + "PatternCode/";
//	//wcout << filepath.GetBuffer() << endl;
//
//	string fp = CT2A(filepath.GetBuffer());
//	fp = fp + to_string(nation);
//	//cout << fp << endl;
//	int count = FileVisit(fp, 1);
	//cout << count << endl;
	string fp = ImageLoc + string("/") + to_string(nation);
	int count = FileVisit(fp, 1);

	double check = 0;

	if ((count < 1)|(count > type))
	{
		return 5;
	}
	else
	{
		fp = fp + "/";

		imageCoder A;
		for (int m = 1; m < (count + 1); m++)
		{
			//cout << "分界线0" << endl;
			string fp2 = fp;
			fp2 = fp2 + to_string(m) + ".txt";
			//fp2 = fp2 + "3.txt\0";
			//cout << fp2 << endl;
			if(A.importCode(fp2) != 0)
				continue;
			A.code2img();
			//cout << "分界线1" << endl;
			//cout << "NewImg.cols" << NewImg.cols << "  " << "NewImg.rows" << NewImg.rows  <<endl;
			check = ImageMatch(A.m_rawImg, NewImg[m-1]);
			//imshow("A.m_rawImg", A.m_rawImg);
			//imshow("NewImg[m-1]", NewImg[m - 1]);
			//std::cout << "图像相似度检查结果:" << check << std::endl;
			if (check > 0.45)
				break;
			//cout << "分界线2" << endl;
			//cout << endl;

		}
	}


	if (check > 0.45)
		return 1;
	else
		return 2;
}

Mat ContentCheck::Pattern_UV_PROCESS2(const Mat& src, int param1, int value, int mode)
{
	Mat SrcGray;
	switch(mode)
	{
	case 0:
	case 1:
	case 2:
	{
		vector<Mat> RGBSplit;

		split(src, RGBSplit);

		//imshow("R", RGBSplit[2]);
		SrcGray = RGBSplit[mode].clone();

		RGBSplit.clear();
		RGBSplit.shrink_to_fit();
		break;
	}
	case 3:
		cvtColor(src, SrcGray, CV_RGB2GRAY);
		break;
	default:
		cvtColor(src, SrcGray, CV_RGB2GRAY);
		break;
	}
	
	//imshow("SrcGray", SrcGray);
	//medianBlur(SrcGray, SrcGray, 5);
	medianBlur(SrcGray, SrcGray, 3);
	//GaussianBlur(SrcGray, SrcGray, Size(7, 7), 2, 2);
	//blur(SrcGray, SrcGray, Size(3, 3));
	//Mat ResultImage2 = ContrastStretch(SrcGray);
	//Mat ResultImage2 = SrcGray.clone();

	Mat ResultImage1 = ContrastStretch(SrcGray);
	Mat ResultImage2;
	//Mat ResultImage2 = SrcGray.clone();
	IFDO(ResultImage1, ResultImage2);

	//imshow("ResultImage2", ResultImage2);
	int PointMin = (ResultImage2.rows > ResultImage2.cols) ? ResultImage2.cols : ResultImage2.rows;
	
	int BlockSize = 2 * (PointMin/ param1) + 1;
	//int BlockSize = 2 * (PointMin / 32) + 1;
	//int BlockSize = 2 * (PointMin / 2) + 1;
	int ConstValue = value;
	Mat ResultImage3;
	adaptiveThreshold(ResultImage2, ResultImage3, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, BlockSize, ConstValue);
	//imshow("ResultImage3", ResultImage3);

	return ResultImage3;
}


double ContentCheck::ImageMatch(Mat src1, Mat src2)
{

	if ((!src1.data) | (!src2.data))
		return 0;

	Mat tempImg = src1.clone();
	Mat srcImg = src2.clone();
	//cout << "Size of template: " << tempImg.size() << endl;

	int width = srcImg.cols - tempImg.cols + 1;
	int height = srcImg.rows - tempImg.rows + 1;
	Mat resultImg(Size(width, height), CV_32FC1);

	matchTemplate(srcImg, tempImg, resultImg, TM_CCOEFF_NORMED);
	//imshow("result", resultImg);

	double minv = 0.0, maxv = 0.0;
	double* minp = &minv;
	double* maxp = &maxv;

	minMaxIdx(resultImg, minp, maxp);

	//cout << "Mat minv = " << minv << endl;
	//cout << "Mat maxv = " << maxv << endl;
	//3.正则化(归一化到0-1)  
	normalize(resultImg, resultImg, 0, 1, NORM_MINMAX, -1);
	//4.找出resultImg中的最大值及其位置  
	double minValue = 0;
	double maxValue = 0;
	Point minPosition;
	Point maxPosition;

	minMaxLoc(resultImg, &minValue, &maxValue, &minPosition, &maxPosition);
	//cout << "minValue: " << minValue << endl;
	//cout << "maxValue: " << maxValue << endl;
	//cout << "minPosition: " << minPosition << endl;
	//cout << "maxPosition: " << maxPosition << endl;
	//5.根据resultImg中的最大值位置在源图上画出矩形  
	//rectangle(srcImg, maxPosition, Point(maxPosition.x + tempImg.cols, maxPosition.y + tempImg.rows), Scalar(0, 255, 0), 1, 8);
	//imshow("srcImg", srcImg);
	//imshow("template", tempImg);
	tempImg.release();
	srcImg.release();
	resultImg.release();

	//std::cout << "图像相似度:" << maxv << std::endl;
	return maxv;
}


int ContentCheck::IFDO(Mat src, Mat& dst)
{
	//Mat GrayImg;
	//cvtColor(src, GrayImg, CV_BGR2GRAY);

	Mat FDO = (Mat_<double>(5, 5) << -0.125 / 3, 0, -0.125 / 3, 0, -0.125 / 3,
		0, -0.5 / 3, -0.5 / 3, -0.5 / 3, 0,
		-0.125 / 3, -0.5 / 3, 8.0 / 3, -0.5 / 3, -0.125 / 3,
		0, -0.5 / 3, -0.5 / 3, -0.5 / 3, 0,
		-0.125 / 3, 0, -0.125 / 3, 0, -0.125 / 3);

	/*Mat FDO = (Mat_<double>(3, 3) << 0, -0.5 / 6, 0,
	-0.5 / 6, 8.0/6, -0.5 / 6,
	0, -0.5 / 6, 0);*/
	/*Mat FDO = (Mat_<double>(3, 3) << -1, -2, -1,
	0, 0, 0,
	1, 2, 1);*/

	/*Mat FDO = (Mat_<double>(3, 3) <<  0, -1, 0,
	-1, 5, -1,
	0, -1, 0);*/

	Mat result;
	filter2D(src, result, src.depth(), FDO);
	//imshow("result", result);
	//result = result + src ;


	dst = result.clone();

	return 1;
}

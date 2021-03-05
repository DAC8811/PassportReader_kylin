#include "ContentCheck.h"

int ContentCheck::Mode1_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;
	
	int check = 0;

	switch (nation)
	{
	case 32:
	{
		if(type == 1)
			return 5;
		break;
	}
	case 51:return 5; break; 
	case 46:return 5; break;
	}

	check = Location_UV(src, PointX, PointY, 0);
	if (check == 1)
	{
		select = 0;
		PointX = ((PointX - 20) > 0) ? (PointX - 20) : 0;
		PointY = ((PointY - 80) > 0) ? (PointY - 80) : 0;
		width = src.cols - PointX -10;
		height = src.rows - PointY -20;

		if ((!(width > 0)) | (!(height > 0)))
			select = 1;

		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		PointX = 10;
		PointY = src.rows - 300;
		width = 1200;
		height = 300;
	}

	//cout << "select:" << select << endl;
	Mat Seg(src, Rect(PointX, PointY, width, height));
	//imshow("Seg", Seg);
	check = MRZ_UV(Seg, 0);
	
	/*if ((check != 1) & (check != 5))
	{
		SaveError(ImageLoc, "Mode1.bmp", src, PointX, PointY, width, height);
	}*/
	
	Seg.release();
	return check;

}

int ContentCheck::Mode2_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int	select = 0;

	switch (nation)
	{
	case 3:
	{
		if (type == 11)
			state = 1;

		if (type == 17)
			state = 1;
		break;
	}

	case 4:
	{
		return 5;
		break;
	}

	//case 5:
	//{
	//	return 5;
	//	break;
	//}

	case 6:
	{
		if (type == 6)
			state = 1;
		break;
	}

	//case 13:
	//{
	//	return 5;
	//	break;
	//}
	case 20:
	{
		if (type == 6)
			state = 1;
		break;
	}

	//case 22:
	//{
	//	//return 5;
	//	break;
	//}

	case 23:
	{
		if (type == 5)
			return 5;
		break;
	}
	//case 27:
	//{
	//	return 5;
	//	break;
	//}

	//case 29:
	//{
	//	if (type == 15)
	//		return 5;
	//	break;
	//}

	/*case 32:
	{
		if (type == 10)
			state = 1;
		if (type == 6)
			return 5;
		break;
	}*/

	case 35:
	{
		state = 1;
		break;
	}
	case 41:
	{
		return 5;
		break;
	}
	case 54:
	{
		if (type == 6)
			state = 1;
		break;
	}
	case 57:
	{
		if (type == 6)
			state = 1;
		break;
	}
	case 62:
	{
		if (type == 6)
			state = 1;
		if (type == 14)
			state = 1;
		if (type == 15)
			state = 1;
		break;
	}
	//case 70:
	//{
	//	return 5;
	//	break;
	//}
	//case 71:
	//{
	//	return 5;
	//	break;
	//}
	case 72:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 101:
	{
		if (type == 6)
			state = 1;
		break;
	}

	//case 115:
	//{
	//	return 5;
	//	break;
	//}

	case 121:
	{
		if (type == 6)
			return 5;
		break;
	}

	case 123:
	{
		if (type == 6)
			return 5;
		break;
	}


	//case 124:
	//{
	//	if (type == 6)
	//		state = 1;

	//	if (type == 4)
	//		return 5;
	//	break;
	//}
	

	case 132:
	{
		return 5;
		break;
	}

	//case 133:
	//{
	//	return 5;
	//	break;
	//}
	case 143:
	{
		return 5;
		break;
	}
	case 150:
	{
		if (type == 6)
			return 5;
		break;
	}
	//case 158:
	//{
	//	return 5;
	//	break;
	//}
	case 159:
	{
		if (type == 15)
			return 5;
		break;
	}

	//case 160:
	//{
	//	return 5;
	//	break;
	//}

	case 175:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 176:
	{
		return 5;
		break;
	}

	case 180:
	{
		if (type == 15)
			state = 1;
		break;
	}

	case 181:
	{
		if (type == 15)
			state = 1;

		if (type == 21)
			state = 1;
		break;
	}
	case 264:
	{
		if (type == 15)
			return 5;
		break;
	}
	default: state = 0; break;
	}

	int check = 0;

	check = Location_UV(src, PointX, PointY, 0);

	if (check == 1)
	{

		if (state == 0) //Pattern
		{
			PointX = PointX;
			PointY = ((PointY - 600) > 0) ? (PointY - 600) : 0;
			width = 1120;
			height = 600;
		}
		else //Thread
		{
			/*PointX = PointX + 650;
			PointY = PointY - 570;
			width = 550;
			height = 400;*/
			PointX = PointX;
			PointY = ((PointY - 550) > 0) ? (PointY - 550) : 0 ;
			width = 1120;
			height = 600;
		}

		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		PointX = 20;
		PointY = 100;
		width = 1200;
		height = 650;	
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));
	//imshow("Pattern_Seg", Seg);
	if (state == 0)
	{
		//check = Pattern_UV(Seg, 23000, 2000, -30);
		//check = Pattern_UV(Seg, 23000, 2000, -30);
		check = Pattern_UV(Seg, 23000, 2000, -50);

		/*if (check != 1)
		{
			check = Pattern_Thread_UV(Seg, 1, 2, 4);
		}*/
	}
	else if (state == 1)
	{
		//check = Pattern_Thread_UV(Seg, 1, 2, 4);

		//if (check != 1)
		//{
			//check = Pattern_UV(Seg, 23000, 2000, -30);
			check = Pattern_UV(Seg, 23000, 2000, -50);
		//}
	}
		
	//check = Pattern_UV(Seg, 23000, 2000, -50);
	//check = Pattern_Thread_UV(Seg, 1, 2, 4);
	//check = DATA_UV(Seg);


	/*if ((check != 1) & (check != 5))
	{
		SaveError(ImageLoc, "Mode2.bmp", src, PointX, PointY, width, height);
	}*/
	//cout << "check: " << check << endl;

	if ((check != 1) & (check != 3)& (check != 5))
		return check;
	if (check == 3)
	{
		//cout << "PatternMtach_UV_START" << endl;
		check = PatternMtach_UV(src, nation,ImageLoc);
		//cout << "PatternMtach_UV_END" << endl;

		if (check != 1)
		{
			switch (nation)
			{
			case 5:
				check = Pattern_Check_ARG(src);
				break;
			case 7:
				check = Pattern_Check_AUS(src);
				break;
			case 17:
				check = Pattern_Check_BLZ(src);
				break;
			case 28:
				check = Pattern_Check_CAN(src);
				break;
			case 59:
				check = Pattern_Check_GRC(src);
				break;
			case 71:
				check = Pattern_Check_IDN(src);
				break;
			case 88:
				check = Pattern_Check_LSO(src);
				break;
			case 91:
				check = Pattern_Check_LIE(src);
				break;
			case 105:
				check = Pattern_Check_MDA(src);
				break;
			case 103:
				check = Pattern_Check_MUS(src);
				break;
			case 108:
				check = Pattern_Check_MAR(src);
				break;
			case 112:
				check = Pattern_Check_NPL(src);
				break;
			case 122:
				check = Pattern_Check_PAN(src);
				break;
			case 160:
				check = Circle_Detect_USA(src);
				break;
			case 174:
				check = Pattern_Check_BEL(src);
				break;
			case 191:
				check = Pattern_Check_IRL(src);
				break;

			}
		}
	}
	else
		return check;

	if ((check != 1)& (check != 5))
		return check;
	else
		return 1;

}


int ContentCheck::Mode3_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	switch (nation)
	{
	case 32:
	{
		if (type == 9)
			state = 1;

		if (type == 10)
			state = 1;
		break;
	}

	case 85:
	{
		if (type == 15)
			state = 1;
		break;
	}
	default: return 5; break;
	}

	if (state == 0)
		return 5;

	//int check = Location_UV(src, PointX, PointY, 0);

	int check = 0;

	check = Location_UV(src, PointX, PointY, 0);

	if (check == 1)
	{
		select = 0;

		switch (state)
		{
		case 1:
		{
			PointX = ((PointX + 750) < 830) ? (PointX + 750) : 830;
			PointY = ((PointY - 740) > 0) ? (PointY - 740) : 0;
			width = 420;
			height = 270;
			break;
		}

		case 2:
		{
			PointX = ((PointX + 750) < 830) ? (PointX + 750) : 830;
			PointY = ((PointY - 730) > 0) ? (PointY - 730) : 0;
			width = 420;
			height = 270;
			break;
		}

		}
		

		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		PointX = 800;
		PointY = 0;
		width = 430;
		height = 420;
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));
	//imshow("Seg", Seg);
	check = PassNum_UV(Seg, 2, 3);

	/*if ((check != 1) & (check != 5))
	{
		SaveError(ImageLoc, "Mode3.bmp", src, PointX, PointY, width, height);
	}*/

	Seg.release();
	return check;

}



int ContentCheck::Mode4_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	switch (nation)
	{
	case 1:
	{
		if (type == 4)
			state = 1;
		break;
	}
	case 6:
	{
		if (type == 6)
			state = 1;
		break;
	}
	case 32:
	{
		if (type == 9)
			state = 2;
		if (type == 12)
			state = 1;
		break;
	}
	default: return 5; break;
	}

	if (state == 0)
		return 5;

	//int check = Location_UV(src, PointX, PointY, 0);

	int check = 0;

	check = Location_UV(src, PointX, PointY, 0);

	if (check == 1)
	{


		switch (state)
		{
		case 1:
		{
			PointX = ((PointX + 300) < 850) ? (PointX + 300) : 850;
			PointY = ((PointY - 450) > 0) ? (PointY - 450) : 0;
			width = 400;
			height = 400;
			break;
		}
		case 2:
		{
			PointX = ((PointX + 300) < 500) ? (PointX + 300) : 500;
			PointY = ((PointY - 450) > 0) ? (PointY - 450) : 0;
			width = 750;
			height = 300;
			break;
		}
		default:return 0; break;
		}

		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		switch (state)
		{
		case 1:
		{
			PointX = 350;
			PointY = 300;
			width = 500;
			height = 400;
			break;
		}
		case 2:
		{
			PointX = 350;
			PointY = 400;
			width = 750;
			height = 300;
			break;
		}
		}
		
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));
	//imshow("Seg", Seg);

	check = DATA_UV(Seg);

	/*if ((check != 1) & (check != 5))
	{
		SaveError(ImageLoc, "Mode4.bmp", src, PointX, PointY, width, height);
	}*/

	Seg.release();
	return check;

}

int ContentCheck::Mode5_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	switch (nation)
	{
	case 1:
	{
		if (type == 4)
			state = 1;
		break;
	}
	case 3:
	{
		if (type == 11)
			state = 1;
		break;
	}
	case 5:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 20:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 32:
	{
		if (type == 6)
			state = 1;
		if (type == 9)
			state = 1;
		if (type == 10)
			state = 1;
		break;
	}

	case 36:
	{
		if (type == 4)
			state = 1;
		break;
	}


	case 52:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 59:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 75:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 104:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 114:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 128:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 140:
	{
		if (type == 2)
			state = 1;
		break;
	}

	case 174:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 133:
	{
		if (type == 6)
			state = 1;
		break;
	}
	case 160:
	{
		if (type == 6)
			state = 1;
		break;
	}


	case 182:
	{
		if (type == 14)
			state = 1;
		break;
	}
	case 109:
	{
		if (type == 14)
			state = 1;
		break;
	}

	case 88:
	{
		if (type == 6)
			state = 1;
		break;
	}
	
	default: return 5; break;
	}

	if (state == 0)
		return 5;

	//int check = Location_UV(src, PointX, PointY, 0);
	int check = 0;

	check = Location_UV(src, PointX, PointY, 0);

	if (check == 1)
	{

		PointX = ((PointX - 30) > 0) ? (PointX - 30) : 0;
		PointY = ((PointY - 570) > 0) ? (PointY - 570) : 0;
		width = 400;
		height = 540;

		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		PointX = 10;
		PointY = 170;
		width = 400;
		height = 550;
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));
	//imshow("Seg", Seg);

	//check = Photo_Shape_UV(Seg, 150000, 12000);
	check = Photo_Shape_UV(Seg, (width*height), 12000);


	/*if ((check != 1) & (check != 5))
	{
		SaveError(ImageLoc, "Mode5.bmp", src, PointX, PointY, width, height);
	}*/

	Seg.release();
	return check;

}


int ContentCheck::Mode6_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	switch (nation)
	{

	case 9:
	{
		if (type == 15)
			state = 1;
		break;
	}

	case 25:
	{
		if (type == 6)
			state = 1;
		break;
	}
	case 28:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 80:
	{
		if (type == 15)
			state = 1;
		break;
	}

	case 32:
	{
		if (type == 8)
			state = 1;
		if (type == 1)
			state = 1;
		break;
	}

	case 35:
	{
		if (type == 4)
			state = 1;
		break;
	}


	case 64:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 70:
	{
		if (type == 6)
			state = 1;
		break;
	}


	case 75:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 104:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 109:
	{
		if (type == 14)
			state = 1;
		break;
	}

	case 117:
	{
		if (type == 15)
			state = 1;
		break;
	}

	case 138:
	{
		if (type == 2)
			state = 1;
		break;
	}

	case 140:
	{
		if (type == 2)
			state = 1;
		break;
	}

	case 146:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 149:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 172:
	{
		if (type == 16)
			state = 1;
		if (type == 2)
			state = 1;
		break;
	}

	case 182:
	{
		if (type == 14)
			state = 1;
		break;
	}

	default: return 5; break;
	}

	if (state == 0)
		return 5;

	//int check = Location_UV(src, PointX, PointY, 0);
	int check = 0;

	check = Location_UV(src, PointX, PointY, 0);

	if (check == 1)
	{

		PointX = ((PointX - 30) > 0) ? (PointX - 30) : 0;
		PointY = ((PointY - 570) > 0) ? (PointY - 570) : 0;
		width = 400;
		height = 540;

		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		PointX = 10;
		PointY = 170;
		width = 400;
		height = 550;
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));
	//imshow("Seg", Seg);

	check = Photo_BG_UV(Seg, 150000, 7000);


	//if ((check != 1) & (check != 5))
	//{
	//	SaveError(ImageLoc, "Mode6.bmp", src, PointX, PointY, width, height);
	//}

	Seg.release();
	return check;

}


int ContentCheck::Mode7_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	switch (nation)
	{


	case 7:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 14:
	{
		if (type == 6)
			state = 3;

		if (type == 15)
			state = 3;
		break;
	}

	case 74:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 80:
	{
		if (type == 13)
			state = 1;
		if (type == 15)
			state = 1;
		break;
	}

	case 106:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 138:
	{
		if (type == 6)
			state = 2;
		break;
	}


	default: return 5; break;
	}

	if (state == 0)
		return 5;

	//int check = Location_UV(src, PointX, PointY, 0);
	int check = 0;

	check = Location_UV(src, PointX, PointY, 0);

	if (check == 1)
	{

		switch (state)
		{
		case 1:
		{
			PointX = ((PointX - 30) > 0) ? (PointX - 30) : 0;
			PointY = ((PointY - 720) > 0) ? (PointY - 720) : 0;
			width = 450;
			height = 300;
			break;
		}
		case 2:
		{
			PointX = ((PointX + 700) < 850) ? (PointX + 700) : 850;
			PointY = ((PointY - 600) > 0) ? (PointY - 600) : 0;
			width = 400;
			height = 300;
			break;
		}
		case 3:
		{
			PointX = ((PointX + 700) < 850) ? (PointX + 700) : 850;
			PointY = ((PointY - 600) > 0) ? (PointY - 600) : 0;
			width = 400;
			height = 200;
			break;
		}
		}


		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		switch (state)
		{
		case 1:
		{
			PointX = 20;
			PointY = 20;
			width = 450;
			height = 300;
			break;
		}
		case 2:
		{
			PointX = 750;
			PointY = 140;
			width = 400;
			height = 300;
			break;
		}

		case 3:
		{
			return 5;
			break;
		}
		}
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));
	//imshow("Seg", Seg);
	
	check = Pattern_Code_UV(Seg);


	//if ((check != 1) & (check != 5))
	//{
	//	SaveError(ImageLoc, "Mode7.bmp", src, PointX, PointY, width, height);
	//}

	Seg.release();
	return check;

}


int ContentCheck::Mode8_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	switch (nation)
	{

	case 6:
	{
		if (type == 6)
			state = 1;
		break;
	}

	//case 32:
	//{
	//	if (type == 10)
	//		state = 1;
	//	break;
	//}

	case 35:
	{
		state = 1;
		break;
	}

	case 44:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 47:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 48:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 57:
	{
		if (type == 6)
			state = 1;
		break;
	}
	case 62:
	{
		if (type == 15)
			state = 1;
		if (type == 14)
			state = 1;
		break;
	}
	case 72:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 108:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 124:
	{
		if (type == 6)
			state = 1;
		break;
	}
	case 128:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 180:
	{
		if (type == 15)
			state = 1;
		break;
	}
	default: return 5; break;
	}

	if (state == 0)
		return 5;

	//int check = Location_UV(src, PointX, PointY, 0);
	int check = 0;

	check = Location_UV(src, PointX, PointY, 0);

	if (check == 1)
	{

		PointX = ((PointX - 20) > 0) ? (PointX - 20) : 0;
		PointY = ((PointY - 120) > 0) ? (PointY - 120) : 0;
		width = 1170;
		height = 220;


		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		PointX = 20;
		PointY = 650;
		width = 1210;
		height = 220;
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));
	//imshow("Seg", Seg);

	check = Thread_UV(Seg, 2);
	if (check == 1)
		return 1;

	check = Thread_UV(Seg, 3);
	if (check == 1)
		return 1;

	/*if ((check != 1) & (check != 5))
	{
		SaveError(ImageLoc, "Mode8.bmp", src, PointX, PointY, width, height);
	}*/

	Seg.release();
	return 2;

}


int ContentCheck::Mode9_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	switch (nation)
	{

	case 6:
	{
		if (type == 6)
			state = 1;
		break;
	}

	//case 32:
	//{
	//	if (type == 10)
	//		state = 1;
	//	break;
	//}
	case 35:
	{
		state = 1;
		break;
	}
	case 44:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 47:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 48:
	{
		if (type == 4)
			state = 1;
		break;
	}
	case 57:
	{
		if (type == 6)
			state = 1;
		break;
	}
	case 62:
	{
		if (type == 15)
			state = 1;
		if (type == 14)
			state = 1;
		break;
	}
	case 72:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 124:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 126:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 128:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 180:
	{
		if (type == 15)
			state = 1;
		break;
	}
	default: return 5; break;
	}

	if (state == 0)
		return 5;

	//int check = Location_UV(src, PointX, PointY, 0);
	int check = 0;

	check = Location_UV(src, PointX, PointY, 0);

	if (check == 1)
	{

		PointX = ((PointX - 20) > 0) ? (PointX - 20) : 0;
		PointY = ((PointY - 120) > 0) ? (PointY - 120) : 0;
		width = 1170;
		height = 220;


		if ((PointX < 0) | (PointX > src.cols - 1))
			return 6;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		PointX = 20;
		PointY = 650;
		width = 1210;
		height = 220;
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));

	check = Thread_UV(Seg, 3);


	//if ((check != 1) & (check != 5))
	//{
	//	SaveError(ImageLoc, "Mode9.bmp", src, PointX, PointY, width, height);
	//}

	Seg.release();
	return check;

}

int ContentCheck::Mode10_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	switch (nation)
	{

	case 6:
	{
		if (type == 6)
			state = 1;
		break;
	}

	//case 32:
	//{
	//	if (type == 10)
	//		state = 1;
	//	break;
	//}

	case 35:
	{
		state = 1;
		break;
	}
	case 57:
	{
		if (type == 6)
			state = 1;
		break;
	}
	case 62:
	{
		if (type == 15)
			state = 1;
		if (type == 14)
			state = 1;
		break;
	}
	case 72:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 108:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 124:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 126:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 127:
	{
		if (type == 6)
			state = 1;
		break;
	}
	case 128:
	{
		if (type == 6)
			state = 1;
		break;
	}


	case 180:
	{
		if (type == 15)
			state = 1;
		break;
	}

	case 181:
	{
		if (type == 15)
			state = 1;
		break;
	}
	default: return 5; break;
	}

	if (state == 0)
		return 5;

	//int check = Location_UV(src, PointX, PointY, 0);
	int check = 0;

	check = Location_UV(src, PointX, PointY, 0);

	if (check == 1)
	{

		PointX = ((PointX - 20) > 0) ? (PointX - 20) : 0;
		PointY = ((PointY - 120) > 0) ? (PointY - 120) : 0;
		width = 1170;
		height = 220;

		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		PointX = 20;
		PointY = 650;
		width = 1210;
		height = 220;
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));

	check = Thread_UV(Seg, 2);


	/*if ((check != 1) & (check != 5))
	{
		SaveError(ImageLoc, "Mode10.bmp", src, PointX, PointY, width, height);
	}*/

	Seg.release();
	return check;

}

int ContentCheck::Mode11_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	switch (nation)
	{


	case 6:
	{
		if (type == 6)
			state = 1;
		break;
	}

	//case 32:
	//{
	//	if (type == 10)
	//		state = 1;
	//	break;
	//}

	/*case 72://干扰
	{
		if (type == 6)
			state = 1;
		break;
	}*/

	case 91:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 108:
	{
		if (type == 6)
			state = 1;
		break;
	}

	/*case 128://干扰
	{
		if (type == 6)
			state = 1;
		break;
	}*/
	default: return 5; break;
	}

	if (state == 0)
		return 5;

	//int check = Location_UV(src, PointX, PointY, 0);
	int check = 0;

	check = Location_UV(src, PointX, PointY, 0);

	if (check == 1)
	{

		PointX = ((PointX - 20) > 0) ? (PointX - 20) : 0;
		PointY = ((PointY - 120) > 0) ? (PointY - 120) : 0;
		width = 1170;
		height = 250;



		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		PointX = 20;
		PointY = 650;
		width = 1210;
		height = 220;
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));

	check = Thread_UV(Seg, 1);

	/*if ((check != 1) & (check != 5))
	{
		SaveError(ImageLoc, "Mode11.bmp", src, PointX, PointY, width, height);
	}*/

	Seg.release();
	return check;

}


int ContentCheck::Mode12_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	switch (nation)
	{

	case 3:
	{
		if (type == 17)
			state = 1;
		break;
	}
	
	case 10:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 126:
	{
		if (type == 6)
			state = 1;
		break;
	}

	//case 32:
	//{
	//	if (type == 10)
	//		state = 1;
	//	break;
	//}

	case 44:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 123:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 124:
	{
		if (type == 6)
			state = 1;
		break;
	}
	case 180:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 181:
	{
		if (type == 15)
			state = 1;
		break;
	}

	default: return 5; break;
	}

	if (state == 0)
		return 5;

	//int check = Location_UV(src, PointX, PointY, 0);
	int check = 0;

	check = Location_UV(src, PointX, PointY, 0);

	if (check == 1)
	{

		PointX = ((PointX - 20) > 0) ? (PointX - 20) : 0;
		PointY = ((PointY - 600) > 0) ? (PointY - 600) : 0;
		width = 400;
		height = 550;


		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		PointX = 20;
		PointY = 130;
		width = 400;
		height = 550;
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));
	//imshow("Seg", Seg);

	check = Thread_UV(Seg, 2);

	/*if ((check != 1) & (check != 5))
	{
		SaveError(ImageLoc, "Mode12.bmp", src, PointX, PointY, width, height);
	}*/

	Seg.release();
	return check;

}


int ContentCheck::Mode13_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	switch (nation)
	{
	case 10:
	{
		if (type == 6)
			state = 1;
		break;
	}

	//case 32:
	//{
	//	if (type == 10)
	//		state = 1;
	//	break;
	//}

	case 44:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 124:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 126:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 174:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 180:
	{
		if (type == 6)
			state = 1;
		break;
	}

	default: return 5; break;
	}

	if (state == 0)
		return 5;

	//int check = Location_UV(src, PointX, PointY, 0);
	int check = 0;

	check = Location_UV(src, PointX, PointY, 0);

	if (check == 1)
	{

		PointX = ((PointX - 20) > 0) ? (PointX - 20) : 0;
		PointY = ((PointY - 600) > 0) ? (PointY - 600) : 0;
		width = 400;
		height = 550;


		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		PointX = 20;
		PointY = 130;
		width = 400;
		height = 550;
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));

	check = Thread_UV(Seg, 3);

	/*if ((check != 1) & (check != 5))
	{
		SaveError(ImageLoc, "Mode13.bmp", src, PointX, PointY, width, height);
	}*/

	Seg.release();
	return check;

}


int ContentCheck::Mode14_Data(Mat& src, int nation, int type, char* ImageLoc)
{

	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	int check = Location_NIR(src, PointX, PointY);

	if (check == 1)
	{

		PointX = ((PointX - 30) > 0) ? (PointX - 30) : 0;
		PointY = ((PointY - 60) > 0) ? (PointY - 60) : 0;
		width = 300;
		height = src.rows - PointY - 2;

		if ((!(width > 0)) | (!(height > 0)))
			select = 1;

		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		PointX = 10;
		PointY = src.rows-300;
		width = 600;
		height = 300;
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));
	//imshow("Seg1", Seg);

	check = Authenticator_NIR(Seg);

	/*if ((check != 1) & (check != 5))
	{
		SaveError(ImageLoc, "Mode14.bmp", src, PointX, PointY, width, height);
	}*/

	Seg.release();
	return check;

}


int ContentCheck::Mode15_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	switch (nation)
	{

	case 3:
	{
		if (type == 11)
			state = 1;
		break;
	}

	case 6:
	{
		if (type == 6)
			state = 1;
		break;
	}
	case 7:
	{
		if (type == 6)
			state = 1;
		break;
	}
	case 8:
	{
		if (type == 15)
			state = 1;
		break;
	}
	case 9:
	{
		if (type == 15)
			state = 2;
		break;
	}
	case 31:
	{
		if (type == 15)
			state = 1;
		if (type == 6)
			state = 1;
		break;
	}

	case 32:
	{
		if (type == 7)
			state = 1;
		break;
	}
	case 40:
	{
		if (type == 4)
			state = 1;
		if (type == 5)
			state = 1;
		if (type == 13)
			state = 1;
		break;
	}

	case 42:
	{
		if (type == 6)
			state = 3;
		break;
	}

	case 52:
	{
		if (type == 6)
			state = 3;
		break;
	}

	case 53:
	{
		if (type == 6)
			state = 3;
		break;
	}

	case 64:
	{
		if (type == 6)
			state = 2;
		break;
	}

	case 76:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 100:
	{
		if (type == 4)
			state = 1;
		if (type == 15)
			state = 1;
		break;
	}

	case 114:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 117:
	{
		if (type == 24)
			state = 2;
		break;
	}

	case 126:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 131:
	{
		if (type == 14)
			state = 2;
		if (type == 15)
			state = 2;
		if (type == 16)
			state = 2;
		break;
	}

	case 132:
	{
		if (type == 6)
			state = 3;
		break;
	}

	case 140:
	{
		if (type == 2)
			state = 4;
		break;
	}

	case 163:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 165:
	{
		if (type == 15)
			state = 1;
		break;
	}

	case 174:
	{
		if (type == 6)
			state = 3;
		break;
	}

	case 183:
	{
		if (type == 6)
			state = 1;
		break;
	}

	default:state = 0; break;
	}
	if (state == 0)
		return 5;

	int check = Location_NIR(src, PointX, PointY);

	if (check == 1)
	{
		switch (state)
		{
		case 1:
		{
			PointX = ((PointX + 600) < 700) ? (PointX + 600) : 700;
			PointY = ((PointY - 680) > 0) ? (PointY - 680) : 0;
			width = 550;
			height = 200;
			break;
		}
		case 2:
		{
			PointX = ((PointX + 700) < 950) ? (PointX + 700) : 950;
			PointY = ((PointY - 680) > 0) ? (PointY - 680) : 0;
			width = 300;
			height = 200;
			break;
		}
		case 3:
		{
			PointX = ((PointX + 700) < 800) ? (PointX + 700) : 800;
			PointY = ((PointY - 680) > 0) ? (PointY - 680) : 0;
			width = 450;
			height = 100;
			break;
		}

		case 4:
		{
			PointX = ((PointX + 500) < 800) ? (PointX + 500) : 800;
			PointY = ((PointY - 600) > 0) ? (PointY - 680) : 0;
			width = 450;
			height = 100;
			break;
		}
		default: return 0; break;
		}

		/*cout << "参数" << endl;
		cout << "PointX:" << PointX << endl;
		cout << "PointY:" << PointY << endl;
		cout << "width:" << width << endl;
		cout << "height:" << height << endl;*/

		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		switch (state)
		{
		case 1:
		{
			PointX = 550;
			PointY = 30;
			width = 600;
			height = 200;
			break;
		}
		case 2:
		{
			PointX = 750;
			PointY = 30;
			width = 400;
			height = 200;
			break;
		}
		case 3:
		{
			PointX = 750;
			PointY = 30;
			width = 400;
			height = 200;
			break;
		}
		case 4:
		{
			PointX = 550;
			PointY = 130;
			width = 400;
			height = 200;
			break;
		}

		default: return 0; break;
		}
	}
	//cout << "Select" << select << endl;
	Mat Seg(src, Rect(PointX, PointY, width, height));
	//imshow("Seg", Seg);

	check = PassNum_NIR(Seg);

	/*if ((check != 1) & (check != 5))
	{
		SaveError(ImageLoc, "Mode15.bmp", src, PointX, PointY, width, height);
	}*/

	Seg.release();
	return check;

}

int ContentCheck::Mode16_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	
	switch (nation)
	{
	case 8:
	{
		if (type == 4)
			state = 1;
		break;
	}
	case 32:
	{
		if (type == 1)
			state = 1;
		if (type == 8)
			state = 1;
		break;
	}

	default: state = 0; break;
	}

	int check = Location_NIR(src, PointX, PointY);

	if (check == 1)
	{	
		

		switch (state)
		{
		case 0:
		{
			PointX = ((PointX + 330) < 900) ? (PointX + 330) : 900;
			PointY = ((PointY - 400) > 0) ? (PointY - 400) : 0;
			width = 350;
			height = 320;
			break;
		}

		case 1:
		{
			PointX = ((PointX + 700) < 840) ? (PointX + 700) : 840;
			PointY = ((PointY - 400) > 0) ? (PointY - 400) : 0;
			width = 410;
			height = 250;
			break;
		}
		}


		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		switch (state)
		{
		case 0:
		{
			PointX = 350;
			PointY = 300;
			width = 400;
			height = 400;
			break;
		}

		case 1:
		{
			PointX = 700;
			PointY = 300;
			width = 500;
			height = 300;
			break;
		}
		}
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));
	//imshow("Seg", Seg);

	check = DATA_NIR(Seg);


	/*if ((check != 1) & (check != 5))
	{
		SaveError(ImageLoc, "Mode16.bmp", src, PointX, PointY, width, height);
	}*/

	Seg.release();
	return check;

}

int ContentCheck::Mode17_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	switch (nation)
	{
	case 1:
	{
		if (type == 4)
			state = 1;
		break;
	}
	case 3:
	{
		if (type == 11)
			state = 1;
		break;
	}
	case 5:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 8:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 14:
	{
		if (type == 6)
			state = 1;
		if (type == 15)
			state = 1;
		break;
	}

	case 20:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 23:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 42:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 46:
	{
		if (type == 6)
			state = 1;
		break;
	}


	case 52:
	{
		if (type == 6)
			state = 1;
		if (type == 15)
			state = 1;
		break;
	}

	case 84:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 114:
	{
		if (type == 6)
			state = 1;
		break;
	}
	case 131:
	{
		if (type == 14)
			state = 1;
		if (type == 15)
			state = 1;
		break;
	}

	case 133:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 140:
	{
		if (type == 2)
			state = 1;
		break;
	}

	case 174:
	{
		if (type == 6)
			state = 1;
		break;
	}

	default: return 5; break;
	}

	if (state == 0)
		return 0;

	int check = Location_NIR(src, PointX, PointY);

	if (check == 1)
	{

		switch (state)
		{
		case 1:
		{
			PointX = ((PointX - 20) > 0) ? (PointX - 20) : 0;
			PointY = ((PointY - 600) > 0) ? (PointY - 600) : 0;
			width = 400;
			height = 550;
			break;
		}

		default:return 0; break;
		}



		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		switch (state)
		{
		case 1:
		{
			PointX = 20;
			PointY = 130;
			width = 400;
			height = 550;
			break;
		}

		default:return 0; break;
		}
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));

	check = Photo_NIR(Seg);

	/*if ((check != 1) & (check != 5))
	{
		SaveError(ImageLoc, "Mode17.bmp", src, PointX, PointY, width, height);
	}*/

	Seg.release();
	return check;

}


/*int ContentCheck::Mode18_Data(Mat& src, int nation, int type)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;

	switch (nation)
	{
	case 32:
	{
		if (type == 9)
			state = 1;
		break;
	}
	default: state = 0; break;
	}

	if (state == 0)
		return 5;

	int check = Location_NIR(src, PointX, PointY);

	if (check != 1)
		return 6;
	

	PointX = PointX + 330;
	PointY = PointY - 520;
	width = 400;
	height = 140;


	if ((PointX < 0) | (PointX > src.cols - 1))
		return 6;

	if ((PointY < 0) | (PointY > src.rows - 1))
		return 6;

	if ((PointX + width < 0) | (PointX + width > src.cols - 1))
		return 6;

	if ((PointY + height < 0) | (PointY + height > src.rows - 1))
		return 6;

	Mat Seg(src, Rect(PointX, PointY, width, height));

	check = FamilyName_NIR(Seg);


	Seg.release();
	return check;

}*/


int ContentCheck::Mode18_Data(Mat& src, int nation, int type, char* ImageLoc)
{
	int state = 0;
	int PointX = 0, PointY = 0, width = 0, height = 0;
	int select = 0;

	switch (nation)
	{

	case 4:
	{
		if (type == 5)
			state = 2;
		break;
	}

	case 45:
	{
		if (type == 6)
			state = 1;
		break;
	}

	case 68:
	{
		if (type == 14)
			state = 4;
		break;
	}

	case 148:
	{
		if (type == 5)
			state = 3;
		break;
	}


	default: return 5; break;
	}

	if (state == 0)
		return 5;

	int check = Location_NIR(src, PointX, PointY);

	if (check == 1)
	{

		switch (state)
		{
		case 1:
		{
			PointX = ((PointX + 800) < 900) ? (PointX + 800) : 900;
			PointY = ((PointY - 250) > 0) ? (PointY - 250) : 0;
			width = 350;
			height = 300;
			break;
		}
		case 2:
		{
			PointX = ((PointX + 920) < 990) ? (PointX + 920) : 990;
			PointY = ((PointY - 750) > 0) ? (PointY - 750) : 0;
			width = 260;
			height = 300;
			break;
		}

		case 3:
		{
			PointX = ((PointX + 150) < 950) ? (PointX + 150) : 950;
			PointY = ((PointY - 750) > 0) ? (PointY - 750) : 0;
			width = 300;
			height = 200;
			break;
		}
		case 4:
		{
			PointX = ((PointX + 950) < 1050) ? (PointX + 950) : 1050;
			PointY = ((PointY - 350) > 0) ? (PointY - 350) : 0;
			width = 200;
			height = 200;
			break;
		}
		default:return 0; break;
		}



		if ((PointX < 0) | (PointX > src.cols - 1))
			select = 1;

		if ((PointY < 0) | (PointY > src.rows - 1))
			select = 1;

		if ((PointX + width < 0) | (PointX + width > src.cols - 1))
			select = 1;

		if ((PointY + height < 0) | (PointY + height > src.rows - 1))
			select = 1;
	}
	else
	{
		select = 1;
	}

	if (select == 1)
	{
		switch (state)
		{
		case 1:
		{
			PointX = 850;
			PointY = 450;
			width = 350;
			height = 300;
			break;
		}
		case 2:
		{
			PointX = 950;
			PointY = 10;
			width = 300;
			height = 300;
			break;
		}

		case 3:
		{
			PointX = 200;
			PointY = 20;
			width = 300;
			height = 300;
			break;
		}
		case 4:
		{
			PointX = 1020;
			PointY = 400;
			width = 200;
			height = 200;
			break;
		}
		default:return 0; break;
		}
	}

	Mat Seg(src, Rect(PointX, PointY, width, height));
	//imshow("Seg", Seg);
	check = Pattern_Code_NIR(Seg);

	/*if ((check != 1) & (check != 5))
	{
		SaveError(ImageLoc, "Mode18.bmp", src, PointX, PointY, width, height);
	}*/


	Seg.release();
	return check;

}

int ContentCheck::Mode19_Data(char* MRZ)
{
	int MRZ_ASC[89];
	if (strlen(MRZ) < 89)
		return 0;
	//char to ASCII
	for (int m = 0; m < 89; m++)
	{
		MRZ_ASC[m] = MRZ[m];
	}

	//number
	for (int m = 0; m < 89; m++)
	{
		if ((MRZ_ASC[m] > 47)&(MRZ_ASC[m] < 58))
			MRZ_ASC[m] = MRZ_ASC[m] - 48;
		else if ((MRZ_ASC[m] > 64)&(MRZ_ASC[m] < 91))
			MRZ_ASC[m] = MRZ_ASC[m] - 55;
		else if ((MRZ_ASC[m] > 96)&(MRZ_ASC[m] < 123))
			MRZ_ASC[m] = MRZ_ASC[m] - 87;
		else
			MRZ_ASC[m] = 0;

		//cout << MRZ_ASC[m] << "-";
	}
	//cout << endl;
	int check = 0;
	check = MRZParityCheck1(MRZ_ASC, 45, 53);
	if (check != 1)
		return 2;
	check = MRZParityCheck1(MRZ_ASC, 58, 63);
	if (check != 1)
		return 2;
	check = MRZParityCheck1(MRZ_ASC, 66, 71);
	if (check != 1)
		return 2;
	check = MRZParityCheck1(MRZ_ASC, 73, 86);
	if (check != 1)
		return 2;
	check = MRZParityCheck2(MRZ_ASC);
	if (check != 1)
		return 2;

	return 1;
}

int ContentCheck::Mode_Mix_5_6(Mat& src, int nation, int type, char* ImageLoc)
{
	int check[2] = {0, 0};
	int satte = 0;
	check[0] = Mode5_Data(src, nation, type, ImageLoc);
	check[1] = Mode6_Data(src, nation, type, ImageLoc);

	if ((check[0] != 1)&(check[0] != 5))
		return check[0];

	if ((check[1] != 1)&(check[1] != 5))
		return check[1];

	if ((check[0] == 1)|(check[1] == 1))
		return 1;

	return 5;
}

int ContentCheck::Mode_Mix_8_9_10_11(Mat& src, int nation, int type, char* ImageLoc)
{
	int check[4] = { 0, 0, 0, 0};
	int satte = 0;
	check[0] = Mode8_Data(src, nation, type, ImageLoc);
	check[1] = Mode9_Data(src, nation, type, ImageLoc);
	check[2] = Mode10_Data(src, nation, type, ImageLoc);
	check[3] = Mode11_Data(src, nation, type, ImageLoc);

	if ((check[0] != 1)&(check[0] != 5))
		return check[0];

	if ((check[1] != 1)&(check[1] != 5))
		return check[1];

	if ((check[2] != 1)&(check[2] != 5))
		return check[2];

	if ((check[3] != 1)&(check[3] != 5))
		return check[3];

	if ((check[0] == 1) | (check[1] == 1) | (check[2] == 1) | (check[3] == 1))
		return 1;

	return 5;
}

int ContentCheck::Mode_Mix_12_13(Mat& src, int nation, int type, char* ImageLoc)
{
	int check[2] = { 0, 0};
	int satte = 0;
	check[0] = Mode12_Data(src, nation, type, ImageLoc);
	check[1] = Mode13_Data(src, nation, type, ImageLoc);

	if ((check[0] != 1)&(check[0] != 5))
		return check[0];

	if ((check[1] != 1)&(check[1] != 5))
		return check[1];

	if ((check[0] == 1) | (check[1] == 1))
		return 1;

	return 5;
}
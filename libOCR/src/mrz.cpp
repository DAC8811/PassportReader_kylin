#include "mrzAPI.h"
#include "utils.h"

using namespace std;
using namespace cv;
using namespace dnn;

#define LOG_TAG "PassportReaderCPP-OCR"

// #define LOG_DEBUG
// #define DEBUG_WRITEFILE
#include "internalLogging.h"

MRZ::MRZ()
{
    //initNet(modelPath);
}

MRZ::~MRZ()
{
}

std::string MRZ::OCR(const Mat &img, int type)
{
    m_mrzImg=findMrz(img, type);

#ifdef DEBUG_WRITEFILE
    // extern string workingFolder;
    imwrite("./debug/rawOCR.jpg", m_mrzImg);
#endif

    m_type=type;
    int count = segmentChars();
    LOGD("segmentChars: %d", count);

    recognize();
    if (0 == 0)
    {
        m_mrzStr.pop_back();
        return mrzStr();
    }
    else
        return string();
}

int MRZ::initNet(std::string modelPath)
{
    m_net = readNet(modelPath);
    return m_net.empty();
}
int MRZ::getMrzNums(const cv::Mat &img){
    mrzNum = findMRZ(img,lineImgs);
    return mrzNum;
}
/**
int MRZ::segmentChars()
{

    Mat img = m_mrzImg.clone();
    Mat imgBin = img.clone();
    threshold(imgBin, imgBin, 0, 255, THRESH_BINARY | THRESH_OTSU);
    imgBin = imgBin == 0;

#ifdef SHOW
    namedWindow("bin", 0);
    imshow("bin", imgBin);
    waitKey(0);
    destroyAllWindows();
#endif // SHOW
    int horizontalKernalWidth = 0;
    if (m_type == 0)
        horizontalKernalWidth = 50;
    else if (m_type == 1)
        horizontalKernalWidth = 75;
    else if (m_type == 2)
        horizontalKernalWidth = 100;

    Mat horizontalShort = getStructuringElement(MORPH_RECT, Size(horizontalKernalWidth, 1));
    Mat verticalKernel = getStructuringElement(MORPH_RECT, Size(1, 5));

    Mat blackhat;
    morphologyEx(img, blackhat, MORPH_BLACKHAT, horizontalShort);

    blackhat(Rect(0, 0, 30, img.rows)) = 0;

#ifdef SHOW
    namedWindow("blackhat", 1);
    imshow("blackhat", blackhat);
    waitKey(0);
    destroyAllWindows();
#endif
    Mat gradX;
    Sobel(blackhat, gradX, CV_32F, 1, 0, 3);
    convertScaleAbs(gradX, gradX);
    morphologyEx(gradX, gradX, MORPH_CLOSE, horizontalShort);
#ifdef SHOW
    namedWindow("gradX", 0);
    imshow("gradX", gradX);
    waitKey(0);
    destroyAllWindows();
#endif
    threshold(gradX, gradX, 0, 255, THRESH_BINARY | THRESH_OTSU);
    morphologyEx(gradX, gradX, MORPH_OPEN, verticalKernel);
    for (int i = 0; i < gradX.rows; ++i)
    {
        gradX.at<uchar>(i, 0) = 0;
        gradX.at<uchar>(i, gradX.cols - 1) = 0;
    }
#ifdef SHOW
    namedWindow("gradX", 0);
    imshow("gradX", gradX);
    waitKey(0);
    destroyAllWindows();
#endif
    vector<Mat> contours;
    findContours(gradX, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<Rect> rects;
    vector<Mat> lineImgs;
    vector<Mat> lineBinImgs;
    for (Mat contour : contours)
    {
        Rect rect = boundingRect(contour);
        rect.y -= 5;
        rect.height += 10;
        rect.y = max(0, rect.y);
        rect.height = min(img.rows - 1 - rect.y, rect.height);

        rect.x -= 20;
        rect.width += 40;
        rect.x = max(0, rect.x);
        rect.width = min(img.cols - 1 - rect.x, rect.width);

        if (m_type == 0)
        {
            if (rect.width > img.cols * 4 / 5 && rect.height >= 25 && rect.height <= 120)
                rects.push_back(rect);
        }
        else if (m_type == 1)
        {
            if (rect.width > img.cols * 4 / 5 && rect.height >= 25 && rect.height <= 120)
                rects.push_back(rect);
        }
        else if (m_type == 2)
        {
            if (rect.width > img.cols * 4 / 5 && rect.height >= 50 && rect.height <= 120)
                rects.push_back(rect);
        }
    }
    sort(rects.begin(), rects.end(), CompareRectByY);

    for (Rect rect : rects)
    {
        lineImgs.push_back(img(rect));
        lineBinImgs.push_back(imgBin(rect));
#ifdef SHOW
        namedWindow("line", 1);
        namedWindow("lineBin", 1);
        imshow("line", lineImgs.back());
        imshow("lineBin", lineBinImgs.back());
        waitKey(0);
        destroyAllWindows();
#endif // SHOW
    }

    contours.clear();
    m_charImgMat.clear();
    for (int i = 0; i < lineImgs.size(); ++i)
    {
        m_charImgMat.push_back(vector<Mat>());
        rects.clear();
        findContours(lineBinImgs[i], contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        for (Mat contour : contours)
        {
            Rect rect = boundingRect(contour);
            if (rect.height > lineImgs[i].rows * 0.35)
            {
                int toBePad = rect.height - rect.width;
                if (toBePad > 0)
                {
                    int left = toBePad / 2;
                    rect.x -= left;
                    rect.width += toBePad;
                    rect.x = max(0, rect.x);
                    rect.width = min(rect.width, lineBinImgs[i].cols - 1 - rect.x);
                }
                rects.push_back(rect);
            }
        }
        sort(rects.begin(), rects.end(), CompareRectByX);
#ifdef SHOW
        Mat tmp = lineImgs[i].clone();
        for (Rect rect : rects)
            rectangle(tmp, rect, 255);
        namedWindow("tmp", 0);
        imshow("tmp", tmp);
        waitKey(0);
        destroyAllWindows();
#endif
        for (Rect rect : rects)
        {
            m_charImgMat[i].push_back(lineImgs[i](rect));
#ifdef SHOW
            //namedWindow("char", 0);
            //imshow("char", lineImgs[i](rect));
            //waitKey(0);
            //destroyAllWindows();
#endif // SHOW
        }
    }
    int count = 0;
    for (vector<Mat> vec : m_charImgMat)
        count += vec.size();
    return count;
}
*/
int MRZ::segmentChars()
{

    Mat img = m_mrzImg.clone();
    /*Mat imgBin = img.clone();
    threshold(imgBin, imgBin, 0, 255, THRESH_BINARY | THRESH_OTSU);
    imgBin = imgBin == 0;
    imgBin(Rect(0, 0, 30, img.rows)) = 0;*/

    int horizontalKernalWidth = 0;
    if (m_type == 0)
        horizontalKernalWidth = 50;
    else if (m_type == 1)
        horizontalKernalWidth = 75;
    else if (m_type == 2)
        horizontalKernalWidth = 100;

    Mat horizontalShort = getStructuringElement(MORPH_RECT, Size(horizontalKernalWidth, 1));
    Mat verticalKernel = getStructuringElement(MORPH_RECT, Size(1, 5));

    Mat blackhat;
    morphologyEx(img, blackhat, MORPH_BLACKHAT, horizontalShort);

    blackhat(Rect(0, 0, 30, img.rows)) = 0;

#ifdef SHOW
    //namedWindow("blackhat", 0);
	//imshow("blackhat", blackhat);
	//waitKey(0);
	//destroyAllWindows();
#endif

    Mat imgBin = blackhat.clone();
    threshold(imgBin, imgBin, 0, 255, THRESH_BINARY | THRESH_OTSU);
    //imgBin = imgBin == 0;
    imgBin(Rect(0, 0, 30, img.rows)) = 0;

#ifdef SHOW
    //namedWindow("imgBin", 0);
	//imshow("imgBin", imgBin);
	//waitKey(0);
	//destroyAllWindows();
#endif
//
//	Mat gradX;
//	Sobel(blackhat, gradX, CV_32F, 1, 0, 3);
//	convertScaleAbs(gradX, gradX);
//#ifdef SHOW
//	namedWindow("gradX", 0);
//	imshow("gradX", gradX);
//	waitKey(0);
//	destroyAllWindows();
//#endif
    Mat tmp;
    morphologyEx(imgBin, tmp, MORPH_CLOSE, horizontalShort);
#ifdef SHOW
    //namedWindow("close", 0);
	//imshow("close", tmp);
	//waitKey(0);
	//destroyAllWindows();
#endif
//	水平方向开运算，垂直方向闭运算
//	threshold(gradX, gradX, 0, 255, THRESH_BINARY | THRESH_OTSU);//
    morphologyEx(tmp, tmp, MORPH_OPEN, verticalKernel);
#ifdef SHOW
    //namedWindow("open", 0);
	//imshow("open", tmp);
	//waitKey(0);
	//destroyAllWindows();
#endif
    for (int i = 0; i < tmp.rows; ++i)
    {
        tmp.at<uchar>(i, 0) = 0;
        tmp.at<uchar>(i, tmp.cols - 1) = 0;
    }
    tmp(Rect(0, 0, 30, img.rows)) = 0;
    auto tmpHorizontal = getStructuringElement(MORPH_RECT, Size(10, 1));
    morphologyEx(tmp, tmp, MORPH_OPEN, tmpHorizontal);

#ifdef SHOW
    //namedWindow("open", 0);
	//imshow("open", tmp);
	//waitKey(0);
	//destroyAllWindows();
#endif
    vector<Mat> contours;
    findContours(tmp, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<Rect> rects;
    vector<Mat> lineImgs;
    vector<Mat> lineBinImgs;
    //找出行的轮廓
    for (Mat contour : contours) {
        Rect rect = boundingRect(contour);
        rect.y -= 5;
        rect.height += 10;
        rect.y = max(0, rect.y);
        rect.height = min(img.rows - 1 - rect.y, rect.height);

        rect.x -= 20;
        rect.width += 40;
        rect.x = max(0, rect.x);
        rect.width = min(img.cols - 1 - rect.x, rect.width);

        if (m_type == 0) {
            if (rect.width > img.cols * 4 / 5 && rect.height >= 25 && rect.height <= 150)
                rects.push_back(rect);
        }
        else if (m_type == 1) {
            if (rect.width > img.cols * 4 / 5 && rect.height >= 25 && rect.height <= 150)
                rects.push_back(rect);
        }
        else if (m_type == 2) {
            if (rect.width > img.cols * 4 / 5 && rect.height >= 50 && rect.height <= 150)
                rects.push_back(rect);
        }
    }
    sort(rects.begin(), rects.end(), CompareRectByY);

    for (Rect rect : rects)
    {
        lineImgs.push_back(img(rect));

        auto tmpKernel1 = getStructuringElement(MORPH_RECT, Size(1, 10));
        auto tmpKernel2 = getStructuringElement(MORPH_RECT, Size(5, 1));

        morphologyEx(imgBin(rect), imgBin(rect), MORPH_OPEN, tmpKernel2);
        morphologyEx(imgBin(rect), imgBin(rect), MORPH_CLOSE, tmpKernel1);


        lineBinImgs.push_back(imgBin(rect));
#ifdef SHOW
        //namedWindow("line", 0);
		//namedWindow("lineBin", 0);
		//imshow("line", lineImgs.back());
		//imshow("lineBin", lineBinImgs.back());
		//waitKey(0);
		//destroyAllWindows();
#endif // SHOW
    }

    if (lineBinImgs.size() != lineImgs.size())
    {
        //LOG(ERROR) << "lineBinImgs.size() not equal tolineImgs.size()";
    }
    contours.clear();
    m_charImgMat.clear();
    for (int i = 0; i < lineImgs.size(); ++i)
    {
        m_charImgMat.push_back(vector<Mat>());
        rects.clear();
        findContours(lineBinImgs[i], contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        for (Mat contour : contours)
        {
            Rect rect = boundingRect(contour);
            if (rect.height > lineImgs[i].rows*0.35)
            {
                int toBePad = rect.height - rect.width;
                if (toBePad > 0)
                {
                    int left = toBePad / 2;
                    rect.x -= left;
                    rect.width += toBePad;
                    rect.x = max(0, rect.x);
                    rect.width = min(rect.width, lineBinImgs[i].cols - 1 - rect.x);
                }
                rects.push_back(rect);
            }
        }
        sort(rects.begin(), rects.end(), CompareRectByX);
#ifdef SHOW
        //Mat tmp = lineImgs[i].clone();
		//for (Rect rect : rects)
		//	rectangle(tmp, rect, 255);
		//namedWindow("tmp", 0);
		//imshow("tmp", tmp);
		//waitKey(0);
		//destroyAllWindows();
#endif
        for (Rect rect : rects)
        {
            m_charImgMat[i].push_back(lineImgs[i](rect));
#ifdef SHOW
            //namedWindow("char", 0);
			//imshow("char", lineImgs[i](rect));
			//waitKey(0);
			//destroyAllWindows();
#endif // SHOW
        }

    }
    int count = 0;
    for (vector<Mat> vec : m_charImgMat)
        count += vec.size();
    return count;
}
/**
void MRZ::recognize()
{
    m_mrzStr.clear();
    m_mrzStr.reserve(128);
    for (vector<Mat> vec : m_charImgMat)
    {
        for (Mat charImg : vec)
        {
            Mat input;
            resize(charImg, input, Size(32, 32));
            Mat blob;
            blobFromImage(input, blob, 1.0, Size(32, 32));
            m_net.setInput(blob);
            Mat logit = m_net.forward();

            int index = 0;
            float max = logit.at<float>(0, 0);
            for (int i = 1; i < 37; ++i)
            {
                if (logit.at<float>(0, i) > max)
                {
                    max = logit.at<float>(0, i);
                    index = i;
                }
            }
            m_mrzStr.push_back(charTable[index]);
        }
        m_mrzStr.push_back('\n');
    }
    //m_mrzStr.pop_back();
}
*/
void MRZ::recognize()
{
    m_mrzStr = "";
    for (vector<Mat> vec : m_charImgMat)
    {
        for (Mat charImg : vec)
        {
            int index = 0;
            Mat input;
            resize(charImg, input, Size(32, 32));
            Mat blob;
            blobFromImage(input, blob, 1.0, Size(32, 32));
            m_net.setInput(blob);
            Mat logit = m_net.forward();
            float max = logit.at<float>(0, 0);
            for (int i = 1; i < 37; ++i)
            {
                if (logit.at<float>(0, i) > max)
                {
                    max = logit.at<float>(0, i);
                    index = i;
                }
            }
            m_mrzStr.push_back(charTable[index]);
        }
        m_mrzStr.push_back('\n');
    }
}
std::string MRZ::mrzStr()
{
    return m_mrzStr;
}

void MRZ::alpha2num(char & c)
{
    if (c == 'O')
        c = '0';
    if (c == 'A')
        c = '4';
    if (c == 'Z')
        c = '2';
}

void MRZ::num2alpha(char & c)
{
    if (c == '0')
        c = 'O';
    if (c == '4')
        c = 'A';
    if (c == '2')
        c = 'Z';
}

int MRZ::checkMrzStr()
{
    //普通护照
    if (m_type == 0)
    {
        if (m_mrzStr.size() != 90)
        {
            LOGD ("m_mrzStr %d: %s", m_mrzStr.size(), m_mrzStr.c_str());
            LOGW ("type0 mrz size not equal to 90");
            return -1;
        }
        string line1Str = m_mrzStr.substr(0, 45);
        string line2Str = m_mrzStr.substr(45, 45);
        for (char &c : line1Str)
        {
            num2alpha(c);
        }
        for (int i = 0; i < 44; ++i)
        {
            //以下位置只可能是数字
            if (i == 9 || (i >= 13 && i < 20) || (i >= 21 && i < 28) || (i >= 42 && i < 44))
                alpha2num(line2Str[i]);
        }
        m_mrzStr.clear();
        m_mrzStr = (line1Str + line2Str);
    }

    else if (m_type == 1)
    {
        if (m_mrzStr.size() != 31)
        {
            LOGW ("m_mrzStr %d: %s", m_mrzStr.size(), m_mrzStr.c_str());
            LOGW ("type1 mrz size not equal to 31");
            return -2;
        }
        for (int i = 4; i < m_mrzStr.size(); ++i)
        {
            alpha2num(m_mrzStr[i]);
        }
    }
    else if (m_type == 2)
    {
        if (m_mrzStr.size() != 93)
        {
            LOGD ("m_mrzStr %d: %s", m_mrzStr.size(), m_mrzStr.c_str());
            LOGW ( "type2 mrz size not equal to 93");
            return -3;
        }
        string line1Str = m_mrzStr.substr(0, 31);
        string line2Str = m_mrzStr.substr(31, 31);
        string line3Str = m_mrzStr.substr(62, 31);
        //*********第一行********
        for (int i = 0; i < 2; ++i)
            num2alpha(line1Str[i]);
        for (int i = 3; i < 22; ++i)
            alpha2num(line1Str[i]);
        num2alpha(line1Str[22]);
        for (int i = 23; i < 30; ++i)
            alpha2num(line1Str[i]);
        //*********第二行********
        for (int i = 0; i < 20; ++i)
            num2alpha(line2Str[i]);
        for (int i = 20; i < 30; ++i)
            //todo
            //alpha2num(line2Str[i]);
            //*********第三行********
            for (char c : line3Str)
                num2alpha(c);
        //*********合并**********
        m_mrzStr.clear();
        m_mrzStr += line1Str;
        m_mrzStr += line2Str;
        m_mrzStr += line3Str;
    }

    return 0;
}

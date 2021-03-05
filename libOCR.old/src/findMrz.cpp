#include "findMrz.h"
#include "rotate.h"
#include <algorithm>

using namespace cv;
using namespace std;

cv::Mat removeOutlierContours(cv::Mat &img) {

    for (int i = 0; i < img.rows; ++i) {
        img.at<uchar>(i, 0) = 0;
        img.at<uchar>(i, img.cols - 1) = 0;
    }
    for (int j = 0; j < img.cols; ++j) {
        img.at<uchar>(0, j) = 0;
        img.at<uchar>(img.rows - 1, j) = 0;
    }

    Mat imgCopy = img.clone();
#ifdef SHOW
    namedWindow("rc0", 0);
    imshow("rc0", imgCopy);
    waitKey(0);
#endif // SHOW
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(imgCopy, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    for (vector<Point> contour :contours) {
        Rect contourRect = boundingRect(contour);
        imgCopy(contourRect) = 255;
    }

#ifdef SHOW
    namedWindow("rc1", 0);
    imshow("rc1", imgCopy);
    waitKey(0);
#endif // SHOW

    findContours(imgCopy, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    vector<Rect> rects;
    vector<int> heights;
    for (vector<Point> contour : contours) {
        Rect contourRect = boundingRect(contour);
        rects.push_back(contourRect);
        heights.push_back(contourRect.height);
    }
    sort(heights.begin(), heights.end());
    double refHeight = (double) heights[heights.size() / 2];

    for (Rect rect : rects) {
        double ratio = (double) rect.height / refHeight;

        if (ratio > 1.3 || ratio < 0.7)
            img(rect).setTo(0);
    }
#ifdef SHOW
    namedWindow("rc2", 0);
    imshow("rc2", img);
    waitKey(0);
#endif // SHOW
    return img;
}

cv::Mat findMrz(const Mat &img, int type, int threshold) {

    Mat imgCopy = img;

    if (type == 0 || type == -1)
        imgCopy = imgCopy(Rect(0, imgCopy.rows * 3 / 4, imgCopy.cols, imgCopy.rows - imgCopy.rows * 3 / 4));
    else if (type == 1)
        imgCopy = imgCopy(Rect(0, imgCopy.rows * 4 / 5, imgCopy.cols, imgCopy.rows - imgCopy.rows * 4 / 5));
    else if (type == 2)
        imgCopy = imgCopy(Rect(0, imgCopy.rows * 1 / 2, imgCopy.cols, imgCopy.rows - imgCopy.rows * 1 / 2));

    Mat grayImg = imgCopy.clone();

#ifdef SHOW
    namedWindow("fm1", 0);
    imshow("fm1", imgCopy);
    waitKey(0);
#endif

    //hsvBinaryzation(imgCopy, threshold);
    //adaptiveBinaryzation(imgCopy);
    otsuBinaryzation(imgCopy);
#ifdef SHOW
    namedWindow("fm2", 0);
    imshow("fm2", imgCopy);
    waitKey(0);
#endif // SHOW
    Mat binImg = imgCopy.clone();
    Canny(imgCopy, imgCopy, 30, 300);

#ifdef SHOW
    namedWindow("fm3", 0);
    imshow("fm3", imgCopy);
    waitKey(0);
#endif // SHOW

    vector<int> horizontalChangeVec(imgCopy.rows, 0);    //ˮƽ����仯����

    binaryImgHorizontalChange(imgCopy, horizontalChangeVec);

    int mean = vecSum(horizontalChangeVec) / horizontalChangeVec.size();

    vecHighValuePassFilter(horizontalChangeVec, mean);

#ifdef SHOW
    showVecHist(horizontalChangeVec);
#endif // SHOW

    vector<Seg> lines = vec2Segs(horizontalChangeVec);
    segsFilter(lines, lines.back().getWidth() * 4 / 5);

    if (lines.size() < 1)
        return Mat();

    int x1, x2;

    x1 = lines[0].getHead();
    x2 = lines.back().getTail();

    x1 = x1 - 20 >= 0 ? x1 - 20 : 0;
    x2 = x2 + 20 < horizontalChangeVec.size() ? x2 + 20 : horizontalChangeVec.size() - 1;
    int h = x2 - x1;
    imgCopy = imgCopy(Rect(0, x1, imgCopy.cols, h));
    binImg = binImg(Rect(0, x1, binImg.cols, h));
    grayImg = grayImg(Rect(0, x1, grayImg.cols, h));
#ifdef SHOW
    namedWindow("fm4", 0);
    imshow("fm4", imgCopy);
    waitKey(0);
    namedWindow("fm5", 0);
    imshow("fm5", grayImg);
    waitKey(0);
#endif // SHOW

    vector<int> verticalPixNum(imgCopy.cols, 0);
    for (int i = 0; i < imgCopy.rows; ++i)
        for (int j = 0; j < imgCopy.cols; ++j)
            if (imgCopy.at<uchar>(i, j) > 0)
                verticalPixNum[j]++;

#ifdef SHOW
    showVecHist(verticalPixNum);
#endif // SHOW

    vector<Seg> segs = vec2Segs(verticalPixNum);
    segsFilter(segs, 10);

#ifdef SHOW
    vecInSegPassFilter(verticalPixNum, segs);
    showVecHist(verticalPixNum);
#endif // SHOW

    if (segs.size() == 0)
        return Mat();

    int y1 = segs[0].getHead() - 20 > 0 ? segs[0].getHead() - 20 : 1;
    int y2 =
        segs.back().getTail() + 20 < verticalPixNum.size() - 1 ? segs.back().getTail() + 20 : verticalPixNum.size() - 2;

    int tmp = segs.size() / 2;
    int interval = segs[tmp].getHead() - segs[tmp - 1].getTail();

    double ratioThresh;
    if (type == 0)
        ratioThresh = 1.5;
    if (type == 1)
        ratioThresh = 2.0;

    for (int i = tmp; i > 0; --i)
        if ((double) (segs[i].getHead() - segs[i - 1].getTail()) / (double) interval > ratioThresh) {
            y1 = segs[i].getHead() - 20 > 0 ? segs[i].getHead() - 20 : 1;
            break;
        }
    for (int i = tmp + 1; i < segs.size() - 1; ++i)
        if ((double) (segs[i + 1].getHead() - segs[i].getTail()) / (double) interval > ratioThresh) {
            y2 =
                segs[i].getTail() + 20 < verticalPixNum.size() - 1 ? segs[i].getTail() + 20 : verticalPixNum.size() - 2;
            break;
        }

    grayImg = grayImg(Range(0, grayImg.rows), Range(y1, y2));
    binImg = binImg(Range(0, binImg.rows), Range(y1, y2));
    removeOutlierContours(binImg);


//
//#ifdef _DEBUG
//	imwrite("../mrzImg.bmp", grayImg);
//	imwrite("../binaryMrzImg.bmp", binImg);
//#else
//	imwrite("USB_TEMP/mrzImg.bmp", grayImg);
//	imwrite("USB_TEMP/binaryMrzImg.bmp", binImg);
//#endif

    return binImg;

}

cv::Mat findMrz(string imgPath, int type, int threshold) {
    Mat img = imread(imgPath);
    return findMrz(img, type, threshold);
}

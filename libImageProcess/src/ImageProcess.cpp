//
// Created by 张迪 on 2020/3/31.
//

#include "ImageProcess.h"
using namespace cv;
using namespace std;

#define MIN_AREA 5000
#define MAX_WIDTH 1000


RotatedRect getBoundingBox(const Mat & input, int img_threshold = 0)
{
    //转为灰度
    Mat gray, image_blur, img_binary, img_binary2, image_open;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    int tmpflag = 0;
    cvtColor(input, gray, cv::COLOR_BGR2GRAY);

    //平滑去噪
    cv::blur(gray, image_blur, Size(5, 5));

    //二值化
    cv::threshold(image_blur, img_binary, img_threshold, 255, THRESH_BINARY);
    cv::threshold(image_blur, img_binary2, 240, 255, cv::THRESH_BINARY_INV);
    cv::morphologyEx(img_binary, img_binary, MORPH_OPEN, Mat::ones(15, 15, CV_8UC1));

    img_binary.convertTo(img_binary, CV_32SC1);
    cv::findContours(img_binary, contours, hierarchy, CV_RETR_FLOODFILL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));



    //定位最大区块
    int max_size = -1, max_size_index = 0;
    for (int i = 0; i < contours.size(); i++) {
        int size = contourArea(contours[i]);
        if (size > max_size) {
            max_size = size;
            max_size_index = i;
        }
    }

    if (max_size < 400)
    {
        //LOG(WARNING) << "getBoundingBox() end,max_size < 400";
        return RotatedRect();
    }

    RotatedRect box = minAreaRect(contours[max_size_index]);

    return box;
}


//根据红外图像返回证件边缘大小Size，用于后面剪裁，并根据尺寸初步判断是护照还是卡片式的小证件
Mat edgeDetectionAny(int &docType, Size &outputSize,Mat& inputIr)
{
    //LOG(INFO) << "edgeDetectionAny()-run";
    Mat output;
    Mat input;

    inputIr.copyTo(input);

    double resize_rate = 1;
    Mat input_resize;
    //尺寸缩放
    if (input.cols > MAX_WIDTH) {
        resize_rate = (double)input.cols / MAX_WIDTH;
        resize(input, input_resize, Size(0, 0), 1 / resize_rate, 1 / resize_rate, cv::INTER_AREA);
    }
    else {
        resize_rate = 1.00;
        input_resize = input.clone();
    }

    RotatedRect box = getBoundingBox(input_resize, 170);

    Point2f pt[4];
    box.points(pt);

    if (resize_rate > 1)
        for (auto i = 0; i < 4; i++)
            pt[i] = pt[i] * resize_rate;
    Size boxSize = box.size;
    boxSize.height *= resize_rate;
    boxSize.width *= resize_rate;

    //LOG(INFO) << "=============! boxSize = " << boxSize << boxSize.height*boxSize.width << " !=============";

    if (boxSize.height*boxSize.width < 400)
    {
        //LOG(WARNING) << "edgeDetectionAny() end,max_size < 400";
        return Mat();
    }

    if (docType != 1)
    {
        //LOG(INFO) << "boxSize.height*boxSize.width: " << boxSize.height*boxSize.width;
        if (boxSize.height*boxSize.width > 460000)//大尺寸证件
            docType = 0;
        else//小尺寸证件
            docType = 2;
    }

    if (docType == 0) // passport
    {
        box = getBoundingBox(input_resize, 100);
        box.points(pt);

        if (resize_rate > 1)
            for (auto i = 0; i < 4; i++)
                pt[i] = pt[i] * resize_rate;
        boxSize = box.size;
        boxSize.height *= resize_rate;
        boxSize.width *= resize_rate;
        //LOG(INFO) << "=============! boxSize = " << boxSize << boxSize.height*boxSize.width << " !=============";
    }

    //==========================================
    //旋转！
    Point2f *targetPoint,
            NormalTargetPoint[4] = {
            Point2f(0, boxSize.height),
            Point2f(0, 0),
            Point2f(boxSize.width, 0),
            Point2f(boxSize.width, boxSize.height)
    },
            Rotated90TargetPoint[4] = {
            Point2f(boxSize.height, boxSize.width),
            Point2f(0, boxSize.width),
            Point2f(0, 0),
            Point2f(boxSize.height, 0)
    },
            Rotated180TargetPoint[4] = {
            Point2f(boxSize.width, 0),
            Point2f(boxSize.width, boxSize.height),
            Point2f(0, boxSize.height),
            Point2f(0, 0)
    },
            Rotated270TargetPoint[4] = {
            Point2f(0, 0),
            Point2f(boxSize.height, 0),
            Point2f(boxSize.height, boxSize.width),
            Point2f(0, boxSize.width)
    };

    if (boxSize.width < boxSize.height) {
        targetPoint = Rotated90TargetPoint;
        outputSize.height = boxSize.width;
        outputSize.width = boxSize.height;
    }
    else {
        targetPoint = NormalTargetPoint;
        outputSize = boxSize;
    }

    Mat AffineTransform = getAffineTransform(pt, targetPoint);

    /*if (docType == 0){
        LOG(INFO) << "edgeDetectionAny()-end";
        return AffineTransform;
    }*/

    //==========================================

    warpAffine(input, output, AffineTransform, outputSize, INTER_LINEAR, BORDER_REPLICATE);

    Mat gray, image_blur, img_binary, image_open;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //resize again
    resize(output, input_resize, Size(0, 0), 1 / resize_rate, 1 / resize_rate, cv::INTER_AREA);
    //cvtColor again
    cvtColor(input_resize, gray, cv::COLOR_BGR2GRAY);
    //blur again
    cv::blur(gray, image_blur, Size(5, 5));

    morphologyEx(image_blur, image_open, MORPH_OPEN, Mat::ones(20, 20, CV_8UC1));
    addWeighted(gray, 1, image_open, -1, 0, image_open, CV_8UC1);

    threshold(image_open, img_binary, 0, 255, THRESH_BINARY | THRESH_OTSU);

    //使用开运算和闭运算让图像边缘成为一个整体
    morphologyEx(img_binary, image_open, MORPH_CLOSE, Mat::ones(4, 19, CV_8UC1));
    morphologyEx(image_open, image_open, MORPH_CLOSE, Mat::ones(4, 19, CV_8UC1));

    //确定边界
    image_open.convertTo(img_binary, CV_32SC1);
    findContours(img_binary, contours, hierarchy, CV_RETR_FLOODFILL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    //寻找最大区块
    int max_size = -1, max_size_index = 0;
    for (int i = 0; i < contours.size(); i++) {
        int size = contourArea(contours[i]);
        if (size > max_size) {
            max_size = size;
            max_size_index = i;
        }
    }

    if (max_size < 50)
    {
        //LOG(INFO) << "edgeDetectionAny() end,max_size < 50";
        return Mat();
    }

    box = minAreaRect(contours[max_size_index]);
    // need rotate
    if (box.center.y < gray.rows / 2) {
        if (boxSize.width < boxSize.height) {
            targetPoint = Rotated270TargetPoint;
            outputSize.height = boxSize.width;
            outputSize.width = boxSize.height;
        }
        else {
            targetPoint = Rotated180TargetPoint;
            outputSize = boxSize;
        }
        if (outputSize.width < 2000)
            AffineTransform = getAffineTransform(pt, targetPoint);
        //warpAffine(input, output, AffineTransform, outputSize, INTER_LINEAR, BORDER_REPLICATE);
    }

    //LOG(INFO) << "edgeDetectionAny()-end";
    return AffineTransform;
}
int PassportBorderCut(const Mat &input, Mat &output, Mat &AffineTransform, Size &outputSize, int & docType) {
    Mat input_clone;
    if (input.data == output.data) {
        input_clone = input.clone();
    } else {
        input_clone = input;
    }

    double resize_rate = 1;
    Mat input_resize;
    //尺寸缩放
    if (input_clone.cols > MAX_WIDTH) {
        resize_rate = (double) input_clone.cols / MAX_WIDTH;
        resize(input_clone, input_resize, Size(0, 0), 1 / resize_rate, 1 / resize_rate,
               cv::INTER_AREA);
    } else {
        resize_rate = 1.00;
        input_resize = input_clone;
    }

    //转为灰度
    Mat gray;
    //cvtColor(input_resize, gray, cv::COLOR_BGR2GRAY);
    gray = input_resize;

    //平滑去噪
    Mat image_blur;
    cv::blur(gray, image_blur, Size(5, 5));

    //计算阈值
    Mat hist;
    int histSize = 256;
    float range[] = {0, 256};
    const float *histRange = {range};
    Mat ROI =
            image_blur(Rect(image_blur.cols * 0.1, image_blur.rows * 0.1, image_blur.cols * 0.8,
                            image_blur.rows * 0.8));
    int totalSize = image_blur.cols * 0.8 * image_blur.rows * 0.8, sum = 0, img_threshold = 190;
    calcHist(&ROI, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);
    for (auto i = 240; i > 120; i--) {
        sum += hist.at<float>(i);
        if (sum > totalSize * 0.3) {
            img_threshold = i * 0.9;
            break;
        }
    }
    //cout << img_threshold << endl;

    //二值化
    Mat img_binary, img_binary2;
    threshold(image_blur, img_binary, img_threshold, 255, THRESH_BINARY);
    threshold(image_blur, img_binary2, 240, 255, cv::THRESH_BINARY_INV);

    img_binary &= img_binary2;
    morphologyEx(img_binary, img_binary, MORPH_OPEN, Mat::ones(5, 5, CV_8UC1));

    //findContours
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    img_binary.convertTo(img_binary, CV_32SC1);
    findContours(img_binary, contours, hierarchy, RETR_FLOODFILL, CHAIN_APPROX_SIMPLE,
                 Point(0, 0));

    //定位最大区块
    int max_size = -1, max_size_index = 0;
    for (int i = 0; i < contours.size(); i++) {
        int size = contourArea(contours[i]);
        if (size > max_size) {
            max_size = size;
            max_size_index = i;
        }
    }
    //nothing found
    if (max_size < MIN_AREA)
        return -1;

    RotatedRect box = minAreaRect(contours[max_size_index]);

    //旋转！
    Point2f pt[4];
    box.points(pt);

    if (resize_rate > 1)
        for (auto i = 0; i < 4; i++)
            pt[i] = pt[i] * resize_rate;
    Size boxSize = box.size;
    boxSize.height *= resize_rate;
    boxSize.width *= resize_rate;

    Point2f *targetPoint,
            NormalTargetPoint[4] = {
            Point2f(0, boxSize.height),
            Point2f(0, 0),
            Point2f(boxSize.width, 0),
            Point2f(boxSize.width, boxSize.height)
    },
            Rotated90TargetPoint[4] = {
            Point2f(boxSize.height, boxSize.width),
            Point2f(0, boxSize.width),
            Point2f(0, 0),
            Point2f(boxSize.height, 0)
    },
            Rotated180TargetPoint[4] = {
            Point2f(boxSize.width, 0),
            Point2f(boxSize.width, boxSize.height),
            Point2f(0, boxSize.height),
            Point2f(0, 0)
    },

            Rotated270TargetPoint[4] = {
            Point2f(0, 0),
            Point2f(boxSize.height, 0),
            Point2f(boxSize.height, boxSize.width),
            Point2f(0, boxSize.width)
    };

    if (boxSize.width < boxSize.height) {
        targetPoint = Rotated90TargetPoint;
        outputSize.height = boxSize.width;
        outputSize.width = boxSize.height;
    } else {
        targetPoint = NormalTargetPoint;
        outputSize = boxSize;
    }

    AffineTransform = getAffineTransform(pt, targetPoint);
    warpAffine(input_clone, output, AffineTransform, outputSize, INTER_LINEAR, BORDER_REPLICATE);

    input_resize.release();
    gray.release();
    image_blur.release();
    hist.release();
    ROI.release();
    img_binary.release();

    //==========================================

    //resize again
    resize(output, input_resize, Size(0, 0), 1 / resize_rate, 1 / resize_rate, cv::INTER_AREA);
    //cvtColor again
    //cvtColor(input_resize, gray, cv::COLOR_BGR2GRAY);
    gray = input_resize;
    //blur again
    cv::blur(gray, image_blur, Size(5, 5));

    Mat image_open;
    morphologyEx(image_blur, image_open, MORPH_OPEN, Mat::ones(4, 20, CV_8UC1));
    addWeighted(gray, 1, image_open, -1, 0, image_open, CV_8UC1);

//    imwrite("/storage/emulated/0/Android/data/com.dscvlab.ooxx.passportreader/files/debug/image_open0.jpg", image_open);


    threshold(image_open, img_binary, 0, 255, THRESH_BINARY | THRESH_OTSU);

//    imwrite("/storage/emulated/0/Android/data/com.dscvlab.ooxx.passportreader/files/debug/img_binary.jpg", img_binary);

    //使用开运算和闭运算让图像边缘成为一个整体
    morphologyEx(img_binary, image_open, MORPH_CLOSE, Mat::ones(4, 19, CV_8UC1));
    morphologyEx(image_open, image_open, MORPH_CLOSE, Mat::ones(4, 19, CV_8UC1));

    //确定边界
    image_open.convertTo(img_binary, CV_32SC1);
    findContours(img_binary, contours, hierarchy, RETR_FLOODFILL, CHAIN_APPROX_SIMPLE,
                 Point(0, 0));

    imwrite("/storage/emulated/0/Android/data/com.dscvlab.ooxx.passportreader/files/debug/img_binary1.jpg", img_binary);

    //寻找最大区块
    max_size = -1, max_size_index = 0;
    for (int i = 0; i < contours.size(); i++) {
        int size = contourArea(contours[i]);
        if (size > max_size) {
            max_size = size;
            max_size_index = i;
        }
    }

    box = minAreaRect(contours[max_size_index]);

    // need rotate
    if (box.center.y < gray.rows / 2) {
        if (boxSize.width < boxSize.height) {
            targetPoint = Rotated270TargetPoint;
            outputSize.height = boxSize.width;
            outputSize.width = boxSize.height;
        } else {
            targetPoint = Rotated180TargetPoint;
            outputSize = boxSize;
        }

        AffineTransform = getAffineTransform(pt, targetPoint);
        warpAffine(input_clone, output, AffineTransform, outputSize, INTER_LINEAR,
                   BORDER_REPLICATE);
    }

    //this should work.
    int boxheight = box.size.height > box.size.width ? box.size.width:box.size.height;
    if (docType == 0) {
        if (outputSize.height > 1200)
            //passport
            docType = 2;
        else if (boxheight < 0.1 * gray.rows)
            //other cards
            docType = 3;
        else
            docType = 4;
    }

    return 0;
}

void serialize(fstream &fs, cv::Mat &mat, const unsigned int direction);

void serializeMat(const string &filename, Mat &mat) {
    fstream ofs(filename.c_str(), ios_base::binary | ios_base::out | ios_base::trunc);
    serialize(ofs, mat, 0);
    ofs.close();
}

void deserializeMat(Mat &mat, const string &filename) {
    fstream ifs(filename.c_str(), ios_base::binary | ios_base::in);
    serialize(ifs, mat, 1);
    ifs.close();
}

void serialize(fstream &fs, cv::Mat &mat, const unsigned int direction) {
    int cols, rows, type;
    if (!fs.good())
        throw "failed to open file";

    if (direction) {
        fs.read((char *) &cols, 4);
        fs.read((char *) &rows, 4);
        fs.read((char *) &type, 4);
        mat.create(rows, cols, type);
    } else {
        cols = mat.cols;
        rows = mat.rows;
        type = mat.type();
        fs.write((char *) &cols, 4);
        fs.write((char *) &rows, 4);
        fs.write((char *) &type, 4);
    }

    const unsigned int data_size = rows * cols * mat.elemSize();
    if (direction) {
        fs.read((char *) mat.data, data_size);
    } else {
        fs.write((char *) mat.data, data_size);
    }
}

Mat GenerateBin(const Mat &merged) {

    Mat offset;
    GenerateMaskColor(offset, merged);

    return offset;
}

Mat GenerateIrBin(const Mat &IrRefer) {

    Mat offset;
    GenerateMask(offset, IrRefer);

    return offset;
}

int GenerateMask(Mat &output, const Mat &input) {
    input.convertTo(output, CV_32F);

    for (auto i = output.begin<float>(); i != output.end<float>(); i++) {
        if (*i < 35)
            *i = 1.4; //???
        else
            *i = 220.0f / *i;
    }

    //Mat Temp;
    GaussianBlur(output, output, Size(55, 55), 0, 0, BORDER_REFLECT);
    GaussianBlur(output, output, Size(55, 55), 0, 0, BORDER_REFLECT);

    return 0;
}

int GenerateMaskColor(Mat &output, const Mat &input) {
    input.convertTo(output, CV_32FC3);

    for (auto i = output.data; i < output.dataend; i += 12) {
        float *temp = (float *) i;
        if (temp[0] < 35 || temp[1] < 30 || temp[2] < 30) {
            temp[0] = 1.2; //???
            temp[1] = 1.2; //???
            temp[2] = 1.2; //???
            continue;
        } else {
            temp[0] = 220.0 / temp[0];
            temp[1] = 220.0 / temp[1];
            temp[2] = 220.0 / temp[2];
        }
    }

    //Mat Temp;
    GaussianBlur(output, output, Size(55, 55), 0, 0, BORDER_REFLECT);
    GaussianBlur(output, output, Size(55, 55), 0, 0, BORDER_REFLECT);
    for (auto i = output.data; i < output.dataend; i += 12) {
        float *temp = (float *) i;
        temp[0] = min<float>(temp[0], 4.2);
        temp[1] = min<float>(temp[1], 4.2);
        temp[2] = min<float>(temp[2], 4.2);
    }
    return 0;

}

int ApplyMask(Mat &output, const Mat &offset) {
    if (offset.empty())
        return 1;

//    unsigned char * outputIter = output.data;
//    float *offsetIter = (float *)offset.data;
//#pragma omp parallel for
//    for (;outputIter < output.dataend;) {
//        outputIter[0] = saturate_cast<uchar>(outputIter[0] * offsetIter[0]);
//        outputIter[1] = saturate_cast<uchar>(outputIter[1] * offsetIter[1]);
//        outputIter[2] = saturate_cast<uchar>(outputIter[2] * offsetIter[2]);
//        outputIter[3] = saturate_cast<uchar>(outputIter[3] * offsetIter[3]);
//
//        offsetIter+=4;
//        outputIter+=4;
//    }
//
//    Mat temp=output.mul(offset);
//    output = temp;

    cv::multiply(output, offset, output, 1, CV_8UC1);
    return 0;
}

int ApplyMaskColor(Mat &output, const Mat &offset) {
    if (offset.empty())
        return 1;

//    unsigned char *outputIter = output.data;
//    float *offsetIter = (float *) offset.data;
//
//#pragma omp parallel for
//    for (; outputIter < output.dataend; ) {
//        outputIter[0] = saturate_cast<uchar>(outputIter[0] * offsetIter[0]);
//        outputIter[1] = saturate_cast<uchar>(outputIter[1] * offsetIter[1]);
//        outputIter[2] = saturate_cast<uchar>(outputIter[2] * offsetIter[2]);
//        offsetIter+=3;
//        outputIter+=3;
//    }

    cv::multiply(output, offset, output, 1, CV_8UC3);
    return 0;
}

int MergeImages(Mat &output, const Mat &imgLeft, const Mat &imgRight, const Mat &offset) {

    assert(imgLeft.size() == imgRight.size());

    //拷贝左右两侧的图
    output = Mat(imgLeft.size(), CV_8UC3);
    imgLeft(Rect(0, 0, imgLeft.cols / 2 - 50, imgLeft.rows)).copyTo(
            output(Rect(0, 0, imgLeft.cols / 2 - 50, imgLeft.rows)));
    imgRight(Rect(imgLeft.cols / 2 + 50, 0, imgLeft.cols / 2 - 50, imgLeft.rows)).copyTo(
            output(Rect(imgLeft.cols / 2 + 50, 0, imgLeft.cols / 2 - 50, imgLeft.rows)));

    //渐变过渡
#pragma omp parallel for
    for (auto i = 0; i < imgLeft.rows; i++) {
        float alpha = 0.01;
        for (auto j = imgRight.cols / 2 - 50; j < imgRight.cols / 2 + 50; j++) {
            output.at<Vec3b>(i, j) =
                    imgRight.at<Vec3b>(i, j) * alpha + imgLeft.at<Vec3b>(i, j) * (1 - alpha);
            alpha += 0.01;
        }
    }

    //=============================================================

    //乘以mask
    if (!offset.empty() && offset.cols == output.cols && offset.rows == output.rows) {
        if (offset.type() == CV_8UC1)
            ApplyMask(output, offset);
        else
            ApplyMaskColor(output, offset);
    }

    return 0;
}

int FadeInAndFadeOut(Mat &output, Mat &leftImage, Mat &rightImage, int location) {
    int begin = location - 50, end = location + 50;
#pragma omp parallel for
    for (auto i = 0; i < leftImage.rows; i++) {
        float alpha = 0, beta = 1;
        for (auto j = begin; j < end; j++) {
            output.at<Vec3b>(i, j) =
                    rightImage.at<Vec3b>(i, j) * alpha + leftImage.at<Vec3b>(i, j) * beta;
            alpha += 0.01;
            beta -= 0.01;
        }
    }
    return 0;
}
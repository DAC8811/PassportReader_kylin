
#include <opencv2/opencv.hpp>
#include <fstream>

#include "mrzAPI.h"
#include "mrz.h"
#include "findMrz.h"
#include "rotate.h"
#include "utils.h"

using namespace std;
using namespace cv;

std::string mrzOcrAPI(cv::Mat pic, int type, std::string templatePath) {
    featureIO *fio = new featureIO(templatePath);

    MRZ mrz(pic, type, 0, fio);
    string res = mrz.getMrzStr();
    return res;
}

cv::Mat findMrzAPI(const cv::Mat &pic, int type, int threshold) {

    Mat PicCopy;
//    if (type == 0) {
//        double theta = getTheta(pic);
//        PicCopy = rotate(pic, theta);
//    } else
    PicCopy = pic.clone();
    return findMrz(pic, type, threshold);
}



#include "mrz.h"
#include "utils.h"

using namespace std;
using namespace cv;

MRZ::MRZ(const cv::Mat &img, int type, int threshold, featureIO *fio)
    : mrzImg(img.clone()), lineAmount(0), fio(fio) {
    //mrzImgBinaryzation(threshold);

#ifdef SHOW
    cv::namedWindow("1", 0);
    cv::imshow("1", mrzImg);
    cv::waitKey(0);
#endif // SHOW
    mrzCharSegmentation();
    generateMrzStr();
    //checkMrzStr(type);
}

MRZ::MRZ(const std::string &imgPath, int type, int threshold, featureIO *fio) {
    cv::Mat img = cv::imread(imgPath, 0);
    MRZ
    a(img, type, threshold, fio);
    this->mrzImg = a.mrzImg.clone();
    this->mrzChars = a.mrzChars;
    this->lineAmount = a.lineAmount;
    this->mrzStr = a.mrzStr;
    this->fio = a.fio;
}

MRZ::~MRZ() {
}

void MRZ::showMrzImg(int windowMode) {
    cv::namedWindow("binary", 0);
    cv::namedWindow("binary", windowMode);
    cv::imshow("binary", mrzImg);
    cv::waitKey(0);
}

std::vector<mrzChar> MRZ::getMrzChars() {
    return mrzChars;
}

std::string MRZ::getMrzStr() {
    return mrzStr;
}

void MRZ::mrzImgBinaryzation(int threshold) {
    otsuBinaryzation(this->mrzImg);
}

void MRZ::mrzCharSegmentation() {
    std::vector<int> horizontalProjection(mrzImg.rows, 0);
    for (int i = 0; i < mrzImg.rows; i++) {
        for (int j = 0; j < mrzImg.cols; j++) {
            if (mrzImg.at<uchar>(i, j) > 0) {
                horizontalProjection[i]++;
            }
        }
    }

#ifdef SHOW
    showVecHist(horizontalProjection);
#endif // SHOW

    vector<Seg> lineSegs = vec2Segs(horizontalProjection);
    segsFilter(lineSegs, 15);

    lineAmount = lineSegs.size();

    std::vector<cv::Mat> lineImgs;
    for (auto lineSeg : lineSegs) {
        Mat
        lineImg(mrzImg(Range(lineSeg.getHead(), lineSeg.getTail()), Range(0, mrzImg.cols)));
        lineImgs.push_back(lineImg);
    }

    for (int n = 0; n < lineImgs.size(); ++n) {
        vector<int> verticalProjection(lineImgs[n].cols, 0);
        for (int i = 0; i < lineImgs[n].rows; i++)
            for (int j = 0; j < lineImgs[n].cols; j++)
                if (lineImgs[n].at<uchar>(i, j) > 0)
                    verticalProjection[j]++;
#ifdef SHOW
        showVecHist(verticalProjection);
#endif // SHOW

        vector<Seg> charSegs = vec2Segs(verticalProjection);
        segsFilter(charSegs, 15);
        for (int m = 0; m < charSegs.size(); ++m) {
            Mat mrzCharImg(
                lineImgs[n](Range(0, lineImgs[n].rows), Range(charSegs[m].getHead(), charSegs[m].getTail())));
            mrzChar mrzc(mrzCharImg, n, m, fio);
            mrzChars.push_back(mrzc);

            //#define SAVE
#ifdef  SAVE
            std::string name = randomStr();
            std::string path ="../chars/" + name + ".bmp";
            cv::imwrite(path, mrzc.getCharImg());

#endif //  SAVE

        }
    }
}

void MRZ::generateMrzStr() {
    mrzStr.clear();
    int count = 0;
    for (mrzChar mC : mrzChars) {
        if (count % (mrzChars.size() / lineAmount) == 0 && count != 0)
            mrzStr.push_back('\n');
        mrzStr.push_back(mC.getCharacter());
        ++count;
    }
    //mrzStr.push_back('\n');
}

void MRZ::checkMrzStr(int type) {

    if (type == 0) {
        string line1Str = mrzStr.substr(0, 44);
        string line2Str = mrzStr.substr(45, 44);
        for (char &s : line1Str) {
            if (s == '0')
                s = 'O';
            if (s == '4')
                s = 'A';
        }
        for (int i = 0; i < 44; ++i) {
            if (i == 9 || (i >= 13 && i < 20) || (i >= 21 && i < 28) || (i >= 42 && i < 44)) {
                if (line2Str[i] == 'O')
                    line2Str[i] = '0';
                if (line2Str[i] == 'A')
                    line2Str = '4';
            }
        }
    }
}

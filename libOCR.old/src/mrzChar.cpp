#include "mrzChar.h"
#include "utils.h"
#include <iostream>
#include <vector>
#include <numeric>


//#define SHOW


using namespace std;
using namespace cv;

mrzChar::mrzChar()
    : charImg(cv::Mat{}), height(0), width(0), ratioHeightWidth(0),
      rowPixelStatisticsFeature(), colPixelStatisticsFeature(), blockPixelStatisticsFeature(),
      feature(), featureAlternative(), character(' '), fio(NULL) {
}

mrzChar::mrzChar(cv::Mat &img, int x, int y, featureIO *fio, bool reverse)
    : charImg(img.clone()), TD(3), rowPixelStatisticsFeature(), colPixelStatisticsFeature(),
      blockPixelStatisticsFeature(), feature(), character(' '), fio(fio) {
    if (reverse)
        charImg = charImg < 100;
    setPos(x, y);
    segment();
    height = charImg.rows;
    width = charImg.cols;
    ratioHeightWidth = (double) height / (double) width;
    padAndResize();

    constructFeatureAlternative();
    classify();
#ifdef SHOW
    this->show();
#endif // SHOW
}

//mrzChar::mrzChar(std::string charImgPath, featureIO *fio, bool reverse) {
//    cv::Mat img = cv::imread(charImgPath, 0);
//    mrzChar a(img, reverse);
//    charImg = a.charImg.clone();
//    TD = a.TD;
//    posX = a.posX;
//    posY = a.posY;
//    height = a.height;
//    width = a.width;
//    ratioHeightWidth = a.ratioHeightWidth;
//    feature.swap(a.feature);
//    featureAlternative.swap(a.featureAlternative);
//    character = a.character;
//    fio = a.fio;
//}

void mrzChar::segment() {
    int x = 0, w = 0, y = 0, h = 0;
    bool inSpaceZone = true;
    for (int i = 0; i < charImg.rows; ++i) {
        for (int j = 0; j < charImg.cols; ++j) {
            if (inSpaceZone == true && charImg.at<uchar>(i, j) == 255) {
                inSpaceZone = false;
                y = i;
                break;
            }
            if (inSpaceZone == false && charImg.at<uchar>(i, j) == 255) {
                ++h;
                break;
            }
        }
    }

    inSpaceZone = true;
    for (int j = 0; j < charImg.cols; ++j) {
        for (int i = 0; i < charImg.rows; ++i) {
            if (inSpaceZone == true && charImg.at<uchar>(i, j) == 255) {
                inSpaceZone = false;
                x = j;
                break;
            }
            if (inSpaceZone == false && charImg.at<uchar>(i, j) == 255) {
                ++w;
                break;
            }
        }
    }

    const cv::Rect roi(x, y, w, h);
    cv::Mat dst(charImg, roi);
    charImg = dst.clone();
}

void mrzChar::padAndResize() {
    int padNum = charImg.rows - charImg.cols;
    int left = padNum / 2;
    int right = padNum - left;
    if (padNum > 0)
        cv::copyMakeBorder(charImg, charImg, 0, 0, left, right, cv::BORDER_CONSTANT, 0);
    cv::resize(charImg, charImg, cv::Size(30, 30));
    charImg = charImg > 0;
}

void mrzChar::constructRowPixelStatisticsFeature() {

    double weihght = 1;
    int count = 0;
    for (int i = 0; i < charImg.rows; ++i) {
        count = 0;
        for (int j = 0; j < charImg.cols; ++j)
            if (charImg.at<uchar>(i, j) == 255)
                ++count;
        rowPixelStatisticsFeature.push_back(count * weihght);
    }
}

void mrzChar::constructColPixelStatisticsFeature() {
    double weihght = 1;
    int count = 0;
    for (int j = 0; j < charImg.cols; ++j) {
        count = 0;
        for (int i = 0; i < charImg.rows; ++i)
            if (charImg.at<uchar>(i, j) == 255)
                ++count;
        colPixelStatisticsFeature.push_back(count * weihght);
    }
}

void mrzChar::constructBlockPixelStatisticsFeature() {
    double weight = 1;
    int count = 0;
    int blockHeight = charImg.rows / 5;
    int blockWidth = charImg.cols / 5;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            count = 0;
            for (int m = 0; m < blockHeight; ++m) {
                for (int n = 0; n < blockWidth; ++n) {
                    if (charImg.at<uchar>(i * 5 + m, j * 5 + n) == 255)
                        ++count;
                }
            }
            blockPixelStatisticsFeature.push_back(count * weight);
        }
    }
}

void mrzChar::constructFeature() {
    constructRowPixelStatisticsFeature();
    constructColPixelStatisticsFeature();
    constructBlockPixelStatisticsFeature();
    feature.insert(feature.end(), rowPixelStatisticsFeature.begin(), rowPixelStatisticsFeature.end());
    feature.insert(feature.end(), colPixelStatisticsFeature.begin(), colPixelStatisticsFeature.end());
    feature.insert(feature.end(), blockPixelStatisticsFeature.begin(), blockPixelStatisticsFeature.end());
}

void mrzChar::constructFeatureAlternative() {
    for (int i = 0; i < charImg.rows; ++i)
        for (int j = 0; j < charImg.cols; ++j)
            featureAlternative.push_back((double) (charImg.at<uchar>(i, j) > 0));
}

double mrzChar::calcSim(const std::vector<double> &feaTemplate) {
    std::vector<double> featureUsedNow;    //��ǰʹ�õ�����
    if (feaTemplate.size() == 85)
        featureUsedNow = feature;
    if (feaTemplate.size() == 900)
        featureUsedNow = featureAlternative;
    double res = 0.0;
    double innerProduct = 0;
    for (int i = 0; i < featureUsedNow.size(); ++i)
        innerProduct += (featureUsedNow[i] * feaTemplate[i]);
    double modeFeature = 0;
    for (int i = 0; i < featureUsedNow.size(); ++i)
        modeFeature += pow(featureUsedNow[i], 2);
    modeFeature = sqrt(modeFeature);
    double modeFeaTemplate = 0;
    for (int i = 0; i < feaTemplate.size(); ++i)
        modeFeaTemplate += pow(feaTemplate[i], 2);
    modeFeaTemplate = sqrt(modeFeaTemplate);
    res = 1 - innerProduct / (modeFeature * modeFeaTemplate);

    return res;
}

void mrzChar::classify() {
    compare(fio->table, fio->charTable);
    subdivide();

}

void mrzChar::compare(std::vector<std::vector<double>> &valueTable, std::vector<char> &charTable) {
    double min = 999999.0;
    int minIndex = -1;
    for (int i = 0; i < valueTable.size(); ++i) {
        double sim = calcSim(valueTable[i]);
        if (sim < min) {
            min = sim;
            minIndex = i;
        }
    }
    character = charTable[minIndex];
}

void mrzChar::subdivide() {
    if (character == 'A' || character == '4') {
        //subdivide_A4();
    } else if (character == '0' || character == 'O' || character == 'Q' || character == 'D') {
        subdivide_0ODQ();
    } else if (character == 'N' || character == 'H') {
        //subdivide_NH();
    } else if (character == '7' || character == 'Z' || character == '2') {
        //subdivide_7Z2();
    } else if (character == 'E' || character == 'F' || character == 'P') {
        subdivide_EFP();
    } else if (character == 'X' || character == 'V' || character == 'W') {
        //subdivide_XVW();
    } else if (character == 'T' || character == 'I') {
        //subdivide_TI();
    }
}

void mrzChar::subdivide_A4() {
    double leftPixNum = 0.0;
    double rightPixNum = 0.0;
    for (int i = charImg.rows - 4; i < charImg.rows; ++i) {
        for (int j = 0; j < charImg.cols / 2; ++j) {
            if (charImg.at<uchar>(i, j) == 255)
                ++leftPixNum;
            if (charImg.at<uchar>(i, j + charImg.cols / 2) == 255)
                ++rightPixNum;
        }
    }
    double leftRightPixNumRatio = (leftPixNum + 1) / (rightPixNum + 1);
    if (leftRightPixNumRatio < 0.7)
        character = '4';
    else
        character = 'A';
}

void mrzChar::subdivide_0ODQ() {
    double topPixNum = 0.0;
    double bottomPixNum = 0.0;
    for (int i = charImg.rows / 4; i < charImg.rows / 2; ++i) {
        for (int j = 0; j < charImg.cols; ++j) {
            if (charImg.at<uchar>(i, j) == 255)
                ++topPixNum;
            if (charImg.at<uchar>(i + charImg.rows / 4, j) == 255)
                ++bottomPixNum;
        }
    }
    double topBottomPixNumRatio = (topPixNum + 1) / (bottomPixNum + 1);
    if (topBottomPixNumRatio < 0.75) {
        character = 'Q';
        return;
    }

    std::vector<double> leftDist;
    std::vector<double> rightDist;
    for (int i = 0; i < charImg.rows; ++i) {
        int j = 0;
        while (charImg.at<uchar>(i, j) == 0)
            ++j;
        leftDist.push_back(j);
        int k = charImg.cols - 1;
        while (charImg.at<uchar>(i, k) == 0)
            --k;
        rightDist.push_back(k);
    }

    //��һ��
    vecNormalization(leftDist, charImg.cols);
    vecNormalization(rightDist, charImg.cols);

    double sumOfVarianceLeft = vecNorm(leftDist, 2.0);
    double sumOfVarianceRight = vecNorm(rightDist, 2.0);

    if (sumOfVarianceRight / sumOfVarianceLeft > 5)
        character = 'D';
    else if (sumOfVarianceLeft < 0.1)
        character = '0';
    else
        character = 'O';

}

void mrzChar::subdivide_NH() {
    int y2 = charImg.cols / 2;
    int y1 = y2 - 2;
    int y3 = y2 + 2;
    int x1 = 0, x2 = 0, x3 = 0;
    while (charImg.at<uchar>(x1, y1) == 0)
        ++x1;
    while (charImg.at<uchar>(x2, y2) == 0)
        ++x2;
    while (charImg.at<uchar>(x3, y3) == 0)
        ++x3;
    int diff = abs(x1 - x3);
    if (diff < 3)
        character = 'H';
    else
        character = 'N';
}

void mrzChar::subdivide_7Z2() {
    if (posX == 1 &&
        (posY == 9 || (posY >= 13 && posY <= 19) || (posY >= 21 && posY <= 26) || posY == 42 || posY == 43)) {
        double bottomLinePixNum = 0.0;
        for (int j = 0; j < charImg.cols; ++j)
            if (charImg.at<uchar>(charImg.rows - 2, j) > 0)
                ++bottomLinePixNum;
        double bottomLinePixRatio = bottomLinePixNum / (double) charImg.cols;
        if (bottomLinePixRatio < 0.3)
            character = '7';
        else
            character = '2';
    } else {
        double bottomLinePixNum = 0.0;
        for (int j = 0; j < charImg.cols; ++j)
            if (charImg.at<uchar>(charImg.rows - 2, j) > 0)
                ++bottomLinePixNum;
        double bottomLinePixRatio = bottomLinePixNum / (double) charImg.cols;
        if (bottomLinePixRatio < 0.3)
            character = '7';

            //����2��Z �ڶ��к͵����ڶ������ر���
        else {
            double topLinePixNum = 0.0;
            double bottomLinePixNum = 0.0;
            for (int j = 0; j < charImg.cols; ++j) {
                if (charImg.at<uchar>(1, j) > 0)
                    ++topLinePixNum;
                if (charImg.at<uchar>(charImg.rows - 2, j) > 0)
                    ++bottomLinePixNum;

            }
            double topBottomPixNumRatio = (topLinePixNum + 1) / (bottomLinePixNum + 1);

            if (topBottomPixNumRatio < 0.85)
                character = '2';
            else
                character = 'Z';
        }
    }
}

void mrzChar::subdivide_EFP() {
    double firstLinePixNum = 0;
    double lastLinePixNum = 0;

    for (int j = 0; j < charImg.cols; ++j) {
        if (charImg.at<uchar>(1, j) == 255)
            ++firstLinePixNum;
        if (charImg.at<uchar>(charImg.rows - 2, j) == 255)
            ++lastLinePixNum;
    }
    if (firstLinePixNum / lastLinePixNum < 1.5)
        character = 'E';
    else {
        double topRightDistance = 0.0;
        for (int i = 0; i < charImg.rows / 2 - 1; ++i) {
            int j = charImg.cols - 1;
            while (j-- > 0) {
                if (charImg.at<uchar>(i, j) > 0) {
                    topRightDistance += (charImg.cols - 1 - j) * 1.0 / charImg.cols;    //���й�һ������
                    break;
                }
            }
        }
        topRightDistance /= (charImg.rows / 2);    //��һ��ƽ������
        if (topRightDistance > 0.25)
            character = 'F';
        else
            character = 'P';
    }
}

void mrzChar::subdivide_XVW() {
    double lastLineCharWidth = charImg.cols * 1.0;    //����һ���ַ����
    int lastLineLeftMargin = 0;
    bool isLastLineLeftMarginGet = false;
    int lastLineRightMargin = 0;
    bool isLastLineRightMarginGet = false;
    for (int j = 0; j < charImg.cols; ++j) {
        if (isLastLineLeftMarginGet && isLastLineRightMarginGet)
            break;
        if (charImg.at<uchar>(charImg.rows - 1, j) > 0 && !isLastLineLeftMarginGet) {
            lastLineLeftMargin = j;
            isLastLineLeftMarginGet = true;
        }
        if (charImg.at<uchar>(charImg.rows - 1, charImg.cols - 1 - j) > 0 && !isLastLineRightMarginGet) {
            lastLineRightMargin = j;
            isLastLineRightMarginGet = true;
        }
    }
    lastLineCharWidth -= (lastLineLeftMargin + lastLineRightMargin);
    lastLineCharWidth = lastLineCharWidth / charImg.cols;    //��һ��
    if (lastLineCharWidth > 0.6)
        character = 'X';
    else if (lastLineCharWidth < 0.3)
        character = 'V';
    else
        character = 'W';
}

void mrzChar::subdivide_TI() {
    double topPixNum = 0.0;
    double bottomPixNum = 0.0;
    for (int i = 0; i < charImg.rows / 2 - 1; ++i) {
        for (int j = 0; j < charImg.cols; ++j) {
            if (charImg.at<uchar>(i, j) > 0)
                ++topPixNum;
            if (charImg.at<uchar>(charImg.rows - 1 - i, j) > 0)
                ++bottomPixNum;
        }
    }
    double topBottomPixNumRatio = (topPixNum + 1) / (bottomPixNum + 1);
    if (topBottomPixNumRatio > 1.3)
        character = 'T';
    else
        character = 'I';
}

cv::Mat mrzChar::getCharImg() {
    return charImg;
}

const char mrzChar::getCharacter() {
    return character;
}

int mrzChar::getPosX() {
    return posX;
}

int mrzChar::getPosY() {
    return posY;
}

void mrzChar::setPos(int x, int y) {
    this->posX = x;
    this->posY = y;
}

void mrzChar::show() {

    std::cout << posX << " " << posY << " " << character << std::endl;
    /*for (int i = 0; i < feature.size(); ++i)
        std::cout << feature[i] << " ";
    std::cout << std::endl;*/
    cv::namedWindow("char", 0);
    cv::imshow("char", charImg);
    cv::waitKey(0);
    cv::destroyWindow("char");
}

std::vector<double> mrzChar::getFeature() {
    return feature;
}

std::vector<double> mrzChar::getFeatureAlternative() {
    return featureAlternative;
}


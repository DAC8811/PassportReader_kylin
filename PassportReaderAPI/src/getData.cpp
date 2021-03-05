//
// Created by 张迪 on 2020/3/21.
//

// vim: set tabstop=4 shiftwidth=4 expandtab fileencoding=utf-8
#include <unistd.h>
#include <sstream>
#include <regex>
#include <sys/stat.h>
#include "utf8.h"
#include <time.h>
#include <opencv2/imgproc/types_c.h>
#include <sys/time.h>

#include "libChip.h"
#include "mrzAPI.h"
#include "MRTD.h"
#include "ImageProcess.h"
#include "utils_zhangdi.h"
#include "pyContentCheck.h"

#include "getData.h"

EXPORT_FUN
char LOGBUFFER[300];

// #define LOG_DEBUG
#define LOG_TAG "PassportReaderCPP-API"

#include "internalLogging.h"

using namespace std;
using namespace cv;

#ifndef USE_OLD_OCR
MRZ OCRworker;
#endif

string workingFolder;
struct MrzInfoStruct mrzInfo;
struct timeval start, stop;

string mrzCode, DG1, DG11, SFZinfo, SFZImage, jsonInfo;

char re[1024];

//-1 - error
// 0 - unknow
// 1 - sfz
// 2 - passport
// 3 - other 1-line MRZ cards
// 4 - other 3-line MRZ cards
int chipStatus, sfzStatus, insertType;
DOC_TYPE docType = UNKNOWN;

int borderResult = -1;

//1,SFZ;   0. Passport/HK
int getCardType(){
    int thisCardType = PassportTest();
    if(thisCardType == 0){
        return 1;//SFZ
    } else if(thisCardType == 1){
        return 0;//Passport
    } else if(thisCardType == -1){
        return -1;//无芯片
    } else {
        return thisCardType;//-2 设备没打开
    }
}
//保存最终图片（经过剪裁）并返回Mat, n取0~3分别代表 红外，白光，紫外 的路径;docType：0--护照；1--身份证；2--港澳通行证
Mat saveFinalImage(const Mat &trans, Mat& src, Size outputsize, int n, int docType)
{
    if (n<0 || n>2)
        return Mat();
    Mat Oring;
    src.copyTo(Oring);
    Mat FinalResize;
    warpAffine(Oring, FinalResize, trans, outputsize, INTER_LINEAR, BORDER_REPLICATE);
    imwrite(workingFolder+"/USB_TEMP/IR.bmp", FinalResize);
    return FinalResize;
}
extern "C" __attribute__((visibility("default"))) int initProgram(const char* folder){
    string stringFolder = folder;
    mkdir((stringFolder + "/USB_TEMP").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    workingFolder = stringFolder;
    time_t tt = time(NULL);
    struct tm* t = localtime(&tt);
    int year = t->tm_year + 1900;
    int mon = t->tm_mon;
    int day = t->tm_mday;
    //todo
    if(year >2021||mon>12||year<2020){
        LOGE("SDK使用权限到期，无法继续使用");
        return -1;
    }
    // choose the right model: mrz32_61.pb or mrz32_20190523.pb
#ifndef USE_OLD_OCR
    if (OCRworker.initNet("./model/mrz32_61.pb") != 0) {
        LOGE("OCR model failed to load");
        return -2;
    }
#endif
    return 0;
}

extern "C" __attribute__((visibility("default"))) int getData(const char* irPath){
    //cleanup everything
    docType = UNKNOWN, chipStatus = 0, sfzStatus = 0, insertType=-1;
    mrzCode.clear(), DG1.clear(), DG11.clear(), SFZinfo.clear(), SFZImage.clear(), jsonInfo.clear();

    int chipType = getCardType();
    if(chipType == 1){
        string output, img;
        sfzStatus = IDCardRead(output, img);
        if (sfzStatus == 1) {
            //convert to utf-8
            try {
                vector<unsigned short> utf16line((unsigned short *) output.data(),
                                                 (unsigned short *) output.data() + output.size()/2);
                utf8::utf16to8(utf16line.begin(), utf16line.end(), back_inserter(SFZinfo));
            }
            catch (const utf8::invalid_utf16 &) {
                // bad UTF-16 data!
            }
            docType = ID_CARD;
            SFZImage = img;
            const char* im = (const char*)SFZImage.data();
            return 1;
        } else{;
            docType = UNKNOWN;
            return -4;
        }
    }
    LOGI("-----开始读取护照数据-----");
    string stringIRPath = irPath;
    Mat irImg = imread(stringIRPath);
    if(irImg.empty()){
        docType = UNKNOWN;
        return -1;
    }
    printf("OCR start...\r\n");
    gettimeofday(&start,NULL);

    Size outputSize;
    Mat trans = edgeDetectionAny(insertType,outputSize,irImg);
    if(trans.empty()){
        docType = UNKNOWN;
        return -2;
    }
    //仿射变换并保存图片，返回的是裁剪之后的图片数据
    Mat finalIr = saveFinalImage(trans,irImg,outputSize,0,insertType);
    cvtColor(finalIr,finalIr,CV_RGB2GRAY);
    if(insertType == 2){
        insertType = OCRworker.getMrzNums(finalIr);
    }
    int OCRType = 0;
    if(insertType == 2){
        //港澳通行证
        OCRType = 1;
        docType = HK_MACAO_PASSPORT;
    } else if(insertType == 0){
        //护照
        OCRType = 0;
        docType = PASSPORT;
    } else if(insertType == 3){
        //三行证件
        OCRType = 2;
    }else{
        docType == UNKNOWN;
        return -2;
    }

#ifndef USE_OLD_OCR
    mrzCode = OCRworker.OCR(finalIr, OCRType);//2:三行证件，1，单行证件，0，双行证件
#else
        Mat MRZImage = findMrzAPI(camera.IRImage, 0, 0);
        mrzCode = mrzOcrAPI(MRZImage, 0, "");
#endif
    if (mrzCode.length() < 10) {
        LOGE("OCR found nothing, aborting...");
        docType = UNKNOWN;
        return -2;
    }
    LOGI("OCR RESULT: %s", mrzCode.c_str());
    printf("OCR RESULT: %s", mrzCode.c_str());

    gettimeofday(&stop,NULL);
    int timesss = (stop.tv_sec - start.tv_sec) * 1000000 +(stop.tv_usec - start.tv_usec);
    printf("OCR finished and used time: %d us\r\n",timesss);
    printf("Read chip start...\r\n");
    gettimeofday(&start,NULL);

    //Chip Read
    //mrzCode = "POCHNYUAN<<PEIPEI<<<<<<<<<<<<<<<<<<<<<<<<<<<\nE561240497CHN9008168F2507204NEKMMFOFMFOFA920";
    //docType = 0;
    if(OCRType == 2){
        //如果是三行证件，根据OCR的结果来判断是回乡证还是台胞证
        if(mrzCode.substr(0,2).compare("CT")==0){
            docType = TAIWAN_VISIT_MAINLAND;
        } else {
            docType = HK_MACAO_VISIT_MAINLAND;
        }

    }

//    mrzCode = "CSC531580954<2612199<9712087<2";
//    docType = HK_MACAO_PASSPORT;
    switch (docType) {
        case HK_MACAO_PASSPORT: //单行卡片
            chipStatus = EchipOneCardRead(mrzCode + "\n", DG1, DG11, workingFolder + "/USB_TEMP/DG2.bmp", mrzInfo);//DG2.bmp
            break;
        case PASSPORT://双行卡片
            chipStatus = EchipTwoCardRead(mrzCode + "\n", DG1, DG11,workingFolder + "/USB_TEMP/DG2.bmp", mrzInfo);//DG2.bmp
            break;
        default: //三行证件
            chipStatus = EchipThreeCardRead(mrzCode + "\n", DG1, DG11, workingFolder + "/USB_TEMP/DG2.bmp", mrzInfo);//DG2.bmp
            break;
    }
    gettimeofday(&stop,NULL);
    timesss = (stop.tv_sec - start.tv_sec) * 1000000 +(stop.tv_usec - start.tv_usec);
    printf("Read chip finished and used time %d us\r\n",timesss);
    if (chipStatus){
        LOGI("chip reader status: %d, DG1: %s, DG11: %s", chipStatus, DG1.c_str(),
             DG11.c_str());
    }else{
        LOGE("chip reader status: %d, read failed or no chip", chipStatus);
        chipStatus = -1;
        return -3;
    }
    return 0;
}

extern "C" __attribute__((visibility("default"))) int closeProgram(){
    return 0;
}

extern "C" __attribute__((visibility("default"))) const char* getJsonNew(){
    if (docType < 0) {
        //未识别出证件
        return  "";
    }
    if(docType == UNKNOWN){
        jsonInfo = "";
    }
    if(docType == ID_CARD){
        jsonInfo = processSFZInfo(SFZinfo);
    } else if(!DG1.empty()){
        string info = getEChipDG1();
        string fullName = getEChipDG11();
        jsonInfo = processDG(info,fullName,docType);
    } else if(!mrzCode.empty()){
        jsonInfo = processMRZ(mrzCode,docType);
    }
//    char re[1024];
    strcpy(re,jsonInfo.c_str());
    return re;
}
extern "C" __attribute__((visibility("default"))) string getEChipDG1() {
    if (!chipStatus)
        return ("");
    else
        return DG1;
}
extern "C" __attribute__((visibility("default"))) string getEChipDG11() {
    if (!chipStatus)
        return ("");

    //find second
    std::string UTF8_name_tag("\x5f\x0e", 2);
    size_t offset = DG11.find(UTF8_name_tag);
    if (offset == std::string::npos)
        return ("");
    offset = DG11.find(UTF8_name_tag, offset + 2);
    if (offset == std::string::npos)
        return ("");

    unsigned char size = DG11[offset + 2];

    return (DG11.substr(offset + 3, size));
}
extern "C" __attribute__((visibility("default"))) const char* getSFZImage(){
    if (docType != 1)
        return "";
    else
        return SFZImage.c_str();
}
extern "C" __attribute__((visibility("default"))) int getAuthenticity(const char* whitePath, const char* irPath, const char* uvPath){
    int RCode[14];
    for (int m = 0; m < 14; ++m)
        RCode[m] = 5;
    string CodePath = "./PatternCode/";
    pySourceSeperate_New(const_cast<char *>(whitePath),
                         const_cast<char *>(irPath),
                         const_cast<char *>(uvPath),
                         const_cast<char *>(mrzCode.c_str()),
                         const_cast<char *>(CodePath.c_str()),
                         RCode);
    int checkResult = 1;
    for(int i = 0; i < 14; i++){
        if(RCode[i]==2){
            checkResult = 0;
            break;
        }
    }
    return checkResult;
}

//int main(){
//    int result1 =  initProgram("/data/user");//初始化程序，在调用getData前调用，并且仅可调用一次
//    closeProgram();
//}

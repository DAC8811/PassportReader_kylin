// vim: set tabstop=4 shiftwidth=4 expandtab fileencoding=utf-8 
//
// Created by Comic Chang on 2018/8/24.
//

#include "CameraApi.h" //相机SDK的API头文件
//#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <dirent.h>
#include <malloc.h>
#include <unistd.h>
#include "libCamera.h"
#include "USBLED.h"
//#include "RFReader.h"
#include "ImageProcess.h"

#define LOG_TAG "PassportReaderCPP-camera"

//#define LOG_DEBUG
#include "internalLogging.h"

const int WHITE_LEFT_GAIN = 100,
    WHITE_RIGHT_GAIN = 100,
    IR_GAIN = 100,
    UV_GAIN = 1000;

using namespace cv;

USBLED LED;

MvCamera::MvCamera() :
    hCamera(0),
    pbyBuffer(NULL),
    exposureTime{20000, 20000, 20000, 20000} {
    CameraSdkInit(1);
}

int MvCamera::Open() {
    int iStatus;


    tSdkCameraDevInfo g_CameraEnumList[32];

    CameraEnumerateDevice(g_CameraEnumList, &(iCameraCounts));
    //没有连接设备
    if (iCameraCounts == 0) {
        LOGD("没有连接设备");
        return -1;
    }

    //相机初始化。初始化成功后，才能调用任何其他相机相关的操作接口
    iStatus = CameraInit(&g_CameraEnumList[0], -1, -1, &(hCamera));

    //初始化失败
    if (iStatus != CAMERA_STATUS_SUCCESS) {
        LOGD("初始化失败 %i", iStatus);
        return -1;
    }

    //获得相机的特性描述结构体。该结构体中包含了相机可设置的各种参数的范围信息。决定了相关函数的参数
    CameraGetCapability(hCamera, &(tCapability));

#ifdef __ANDROID__
    g_pRgbBuffer = (unsigned char *) CameraAlignMalloc(
            tCapability.sResolutionRange.iHeightMax * tCapability.sResolutionRange.iWidthMax * 4,
            16);
#elif __WINDOWS__
    g_pRgbBuffer = (unsigned char *) __mingw_aligned_malloc(16,
            tCapability.sResolutionRange.iHeightMax * tCapability.sResolutionRange.iWidthMax * 4);
#else
    g_pRgbBuffer = (unsigned char *) memalign(16,
            tCapability.sResolutionRange.iHeightMax * tCapability.sResolutionRange.iWidthMax * 4);
#endif
    if (g_pRgbBuffer == NULL) {
        return -2;
    }

    if (tCapability.sIspCapacity.bMonoSensor) {
        channel = 1;
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_MONO8);
    } else {
#ifdef __ANDROID__
        channel = 4;
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_RGBA8);
#else
        channel = 3;
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_RGB8);
#endif
    }

    CameraSetAeState(hCamera, FALSE);
    CameraSetExposureTime(hCamera, 5);
    CameraSetTriggerMode(hCamera, 1);
    //色温
    CameraSetClrTempMode(hCamera, 2);

    CameraPlay(hCamera);

    return 0;
}

MvCamera::~MvCamera() {
    //Close();
}

void MvCamera::Close() {
    CameraUnInit(hCamera);
    //注意，反初始化后再free
    if (g_pRgbBuffer) {
#ifdef __ANDROID__
        CameraAlignFree(g_pRgbBuffer);
#else
        free(g_pRgbBuffer);
#endif
        g_pRgbBuffer = NULL;
    }
}

Mat MvCamera::CaptureImage(double ExposureTime = -1, int AnalogGain = -1, int isGrayScale = 0) {
    int iStatus;
    Mat Iimag, image_out;
    tSdkFrameHead sFrameInfo;
    LOGV("Begin CaptureImage isGrayScale:%01d", isGrayScale);

    CameraStop(hCamera);
    CameraSetImageResolution(hCamera, &tCapability.pImageSizeDesc[0]);
    if (isGrayScale)
    {
        channel = 1;
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_MONO8);
    } else {
        channel = 4;
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_RGBA8);
    }
    if (ExposureTime > 0)
        CameraSetExposureTime(hCamera, ExposureTime);
    if (AnalogGain > 0)
        CameraSetAnalogGain(hCamera, AnalogGain);
    CameraPlay(hCamera);
    CameraSoftTrigger(hCamera);
    usleep(10000);

    LOGV("Begin CameraGetImageBuffer");

    for (int retry = 0; retry < 10; retry++) {
        iStatus = CameraGetImageBuffer(hCamera, &sFrameInfo, &pbyBuffer, 1000);
        if (iStatus == CAMERA_STATUS_SUCCESS) {
//            LOGV("Begin CameraImageProcess");
            CameraImageProcess(hCamera, pbyBuffer, g_pRgbBuffer, &sFrameInfo);
            Iimag = Mat (sFrameInfo.iHeight, sFrameInfo.iWidth, CV_8UC(channel), g_pRgbBuffer);

            LOGV("Begin CameraReleaseImageBuffer");

            //在成功调用CameraGetImageBuffer后，必须调用CameraReleaseImageBuffer来释放获得的buffer。
            //否则再次调用CameraGetImageBuffer时，程序将被挂起一直阻塞，直到其他线程中调用CameraReleaseImageBuffer来释放了buffer
            CameraReleaseImageBuffer(hCamera, pbyBuffer);
            break;
        }
        if (iStatus == CAMERA_STATUS_TIME_OUT) {
            if (retry > 4)
                LOGW("CAMERA_STATUS_TIME_OUT, retry %d", retry);
            //hard reset
            CameraStop(hCamera);
            CameraPlay(hCamera);
            CameraSoftTrigger(hCamera);
            //usleep(10000);
        }
    }

    if (iStatus != CAMERA_STATUS_SUCCESS) {
#ifdef __ANDROID__
        LOGE("CAMERA_STATUS_TIME_OUT return empty mat");
#endif
        return Mat();
    }

    if (isGrayScale)
        image_out=Iimag.clone();//cvtColor(Iimag, image_out, COLOR_RGBA2GRAY, 1);
    else {
        LOGV("Begin cvtColor");
#ifdef __ANDROID__
        cvtColor(Iimag, image_out, COLOR_RGBA2BGR, 3);
#else
        cvtColor(Iimag, image_out, COLOR_RGB2BGR, 3);
#endif
    }

    LOGV("End CaptureImage");
    return image_out;
}

Mat MvCamera::CaptureImageFast(double ExposureTime = -1, int AnalogGain = -1) {
    int iStatus;
    Mat Iimag, image_out;
    tSdkFrameHead sFrameInfo;
    LOGV("Begin CaptureImageFast");

    CameraStop(hCamera);

    channel = 1;
    CameraSetImageResolution(hCamera, &tCapability.pImageSizeDesc[5]);
    CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_MONO8);

    if (ExposureTime > 0)
        CameraSetExposureTime(hCamera, ExposureTime);
    if (AnalogGain > 0)
        CameraSetAnalogGain(hCamera, AnalogGain);
    CameraPlay(hCamera);
    CameraSoftTrigger(hCamera);
    usleep(10000);

    LOGV("Begin CameraGetImageBuffer");

    for (int retry = 0; retry < 10; retry++) {
        iStatus = CameraGetImageBuffer(hCamera, &sFrameInfo, &pbyBuffer, 1000);
        if (iStatus == CAMERA_STATUS_SUCCESS) {
            CameraImageProcess(hCamera, pbyBuffer, g_pRgbBuffer, &sFrameInfo);
            Iimag = Mat (sFrameInfo.iHeight, sFrameInfo.iWidth, CV_8UC(channel), g_pRgbBuffer);

            LOGV("Begin CameraReleaseImageBuffer");

            //在成功调用CameraGetImageBuffer后，必须调用CameraReleaseImageBuffer来释放获得的buffer。
            //否则再次调用CameraGetImageBuffer时，程序将被挂起一直阻塞，直到其他线程中调用CameraReleaseImageBuffer来释放了buffer
            CameraReleaseImageBuffer(hCamera, pbyBuffer);
            break;
        }
        if (iStatus == CAMERA_STATUS_TIME_OUT) {
            if (retry > 4)
                LOGW("CAMERA_STATUS_TIME_OUT, retry %d", retry);
            //hard reset
            CameraStop(hCamera);
            CameraPlay(hCamera);
            CameraSoftTrigger(hCamera);
            //usleep(10000);
        }
    }

    if (iStatus != CAMERA_STATUS_SUCCESS) {
        LOGE("CAMERA_STATUS_TIME_OUT return empty mat");
        return Mat();
    }

    image_out=Iimag.clone();//cvtColor(Iimag, image_out, COLOR_RGBA2GRAY, 1);

    LOGV("End CaptureImage");
    return image_out;
}


Mat MvCamera::GetWhiteLeft() {
    LOGV("Begin GetWhiteLeft");

    if (LED.LightLED(LED.CMD_LIGHT_WHITE_LEFT) != true)
        LOGE("LightLED error CMD_LIGHT_WHITE_LEFT");
    CameraSetGain(hCamera, 113, 100, 151);

    return CaptureImage(exposureTime[1], WHITE_LEFT_GAIN);
}

Mat MvCamera::GetWhiteRight() {
    LOGV("Begin GetWhiteRight");

    if (LED.LightLED(LED.CMD_LIGHT_WHITE_RIGHT) != true)
        LOGE("LightLED error CMD_LIGHT_WHITE_RIGHT");
    CameraSetGain(hCamera, 113, 100, 151);

    return CaptureImage(exposureTime[2], WHITE_RIGHT_GAIN);
}

Mat MvCamera::GetUVImage() {
    LOGV("Begin GetUVImage");

    if (LED.LightLED(LED.CMD_LIGHT_PURPLE) != true)
        LOGE("LightLED error CMD_LIGHT_PURPLE");
    CameraSetGain(hCamera, 100, 100, 100);

    return CaptureImage(exposureTime[3], UV_GAIN);
}

Mat MvCamera::GetIRImage() {

    LOGV("Begin GetIRImage");
    if (LED.LightLED(LED.CMD_LIGHT_INFRARED) != true)
        LOGE("LightLED error CMD_LIGHT_INFRARED");
    CameraSetGain(hCamera, 100, 100, 100);

    Mat image = CaptureImage(exposureTime[0], IR_GAIN, true);
    return image;
}

Mat MvCamera::GetIRImageFast() {

    LOGV("Begin GetIRImage");
    if (LED.LightLED(LED.CMD_LIGHT_INFRARED) != true)
        LOGE("LightLED error CMD_LIGHT_INFRARED");
    CameraSetGain(hCamera, 100, 100, 100);

    Mat image = CaptureImageFast(exposureTime[0], IR_GAIN);
    return image;
}

Mat MvCamera::GetIRMerged() {
    LOGV("Begin GetIRMerged");

    Mat image = GetIRImage();

    LOGV("Begin ApplyMask");
    ApplyMask(image, IROffset);
    LOGV("End ApplyMask");
    return image;
}

Mat MvCamera::GetWhiteMerged() {
    LOGV("Begin GetWhiteMerged");
    Mat left = GetWhiteLeft();
    Mat right = GetWhiteRight();

    LOGV("Begin MergeImages");
    Mat merged;
    MergeImages(merged, left, right, Mat());

    LOGV("Begin ApplyMaskColor");
    ApplyMaskColor(merged, WhiteOffset);

    LOGV("End ApplyMaskColor");
#ifdef DEBUG_CAMERA
    imwrite("/storage/emulated/0/Android/data/com.dscvlab.ooxx.passportreader/files/images/left.jpg",
            left);
    imwrite("/storage/emulated/0/Android/data/com.dscvlab.ooxx.passportreader/files/images/right.jpg",
            right);
#endif

    return merged;
}

int GetMaxValue(const Mat &input, USBLED::LIGHT_TYPE ColorCode) {
    double min, max;
    Mat roi, resized;
    switch (ColorCode) {
        case LED.CMD_LIGHT_WHITE_LEFT:
        case LED.CMD_LIGHT_INFRARED_LEFT:roi = input.colRange(Range(0, input.cols / 2));
            break;
        case LED.CMD_LIGHT_WHITE_RIGHT:
        case LED.CMD_LIGHT_INFRARED_RIGHT:roi = input.colRange(Range(input.cols / 2, input.cols));
            break;
        default:roi = input;
    }
    resize(roi, resized, Size(), 0.5, 0.5, cv::INTER_NEAREST);
    GaussianBlur(roi, roi, Size(55, 55), 0, 0);
    //mix channels
    roi = roi.reshape(1, 0);
    //find max value
    minMaxLoc(roi, &min, &max);
    return max;
}

double MvCamera::AutoFitParam(USBLED::LIGHT_TYPE ColorCode, int AnalogGain) {
    while (LED.LightLED(ColorCode) != true)
        LOGE("LightLED error %d", ColorCode);
    double low = 500, //5ms
        high = 400000, //400ms
        testValue;

    LOGI("fitting param for light :  %d", ColorCode);

    switch (ColorCode) {
        case LED.CMD_LIGHT_WHITE_LEFT:
        case LED.CMD_LIGHT_WHITE_RIGHT:
        case LED.CMD_LIGHT_WHITE:CameraSetGain(hCamera, 113, 100, 151);
            break;
        case LED.CMD_LIGHT_PURPLE:CameraSetGain(hCamera, 100, 100, 50);
            break;
        default:CameraSetGain(hCamera, 100, 100, 100);
    }

    Mat result;
    while (result.empty())
        result = CaptureImage(high, AnalogGain, ColorCode == LED.CMD_LIGHT_INFRARED);
    int maxValue = GetMaxValue(result, ColorCode);

    int threshold = maxValue * 0.95;
    LOGI("initial exposure time:  %f   result:   %d   target:   %d   ", high, maxValue, threshold);

    while (high - low > 10) {
        testValue = (low + high) / 2;
        result = Mat();
        while (result.empty())
            result = CaptureImage(testValue, AnalogGain, ColorCode == LED.CMD_LIGHT_INFRARED);

        maxValue = GetMaxValue(result, ColorCode);

        //cout << "exposure time:  "  << testValue << "  result:  " << maxValue << endl;
        LOGI("exposure time:  %f   result:   %d", testValue, maxValue);

        if (maxValue < threshold) {
            low = testValue;
        } else if (maxValue > threshold) {
            high = testValue;
        } else
            break;
    }
    CameraGetExposureTime(hCamera, &testValue);

    //cout << "final exposure time:  "  << testValue  <<endl << endl;
    LOGI("final exposure time:  %f", testValue);

    return testValue;
}

int MvCamera::AutoFitAllParam() {
    WhiteOffset = Mat();
    IROffset = Mat();

    exposureTime[0] = AutoFitParam(LED.CMD_LIGHT_INFRARED, IR_GAIN);
    exposureTime[1] = AutoFitParam(LED.CMD_LIGHT_WHITE_LEFT, WHITE_LEFT_GAIN);
    exposureTime[2] = AutoFitParam(LED.CMD_LIGHT_WHITE_RIGHT, WHITE_RIGHT_GAIN);
    exposureTime[3] = AutoFitParam(LED.CMD_LIGHT_PURPLE, UV_GAIN);

    LOGI("generating parameters...");

    WhiteImage = GetWhiteMerged();
    WhiteOffset = GenerateBin(WhiteImage);

    IRImage = GetIRMerged();
    IROffset = GenerateIrBin(IRImage);

    UVImage = GetUVImage();

    return 0;
}

#include <sys/stat.h>

inline bool exists_test(const std::string &name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

int MvCamera::loadConfigFromPath(string path) {
    if (exists_test(path + "/WhiteOffset.bin") == 0 ||
        exists_test(path + "/IROffset.bin") == 0 ||
        exists_test(path + "/exposure.bin") == 0
        )
        return -1;

    deserializeMat(WhiteOffset, path + "/WhiteOffset.bin");
    deserializeMat(IROffset, path + "/IROffset.bin");
    ifstream exposureconfig(path + "/exposure.bin", ios::binary);
    exposureconfig.read((char *) &exposureTime, sizeof(double) * 4);
    exposureconfig.close();

    return 0;
}

int MvCamera::saveConfigToPath(string path) {
    serializeMat(path + "/WhiteOffset.bin", WhiteOffset);
    serializeMat(path + "/IROffset.bin", IROffset);
    ofstream exposureconfig(path + "/exposure.bin", ios::binary | ios::trunc);
    exposureconfig.write((char *) exposureTime, sizeof(double) * 4);
    exposureconfig.close();

    if (!WhiteImage.empty()) {
        imwrite(path + "/UVRefer.jpg", UVImage);
        imwrite(path + "/IRRefer.jpg", IRImage);
        imwrite(path + "/whiteRefer.jpg", WhiteImage);
    }
    return 0;
}

int MvCamera::configLoaded()
{
    return ! IROffset.empty();
}

void MvCamera::testCamera() {
//    int testR, testG,testB;
//    CameraGetGain(hCamera, &testR, &testG,&testB);
//    int temp;
//    CameraGetClrTempMode(hCamera, &temp);
//    float temp2[30];
//    CameraGetUserClrTempMatrix(hCamera, temp2);
//    int temp3;
//    CameraGetWbMode(hCamera, &temp3);
//    int temp4;
//    CameraGetPresetClrTemp(hCamera, &temp4);

}

int MvCamera::inputDetect() {
    Mat input = GetIRImageFast();
    if (input.empty())
        return -3;

    //resize to speed up calc
    resize(input, input, Size(0, 0), 0.5, 0.5,
           cv::INTER_NEAREST);

    //cv::blur(input, input, Size(5, 5));

    //inner area
    // w&h: 10%~90%
    Mat ROI_test = input(Rect(input.cols * 0.1, input.rows * 0.1,
                              input.cols * 0.8, input.rows * 0.8));

    // count pixels between 100~230
    int count = countNonZero(ROI_test > 100 & ROI_test < 230);

    return count;
}

int MvCamera::clearImages() {
    if (!IRImage.empty())
        IRImage.release();
    if (!WhiteImage.empty())
        WhiteImage.release();
    if (!UVImage.empty())
        UVImage.release();

    if (!AffineTransform.empty())
        AffineTransform.release();

    return 1;
}

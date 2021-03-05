//
// Created by Comic Chang on 2018/8/24.
//

#ifndef PASSPORTREADER_CAMERAAPI_H
#define PASSPORTREADER_CAMERAAPI_H

#include "CameraApi.h" //相机SDK的API头文件
#include <opencv2/core/core.hpp>
#include "USBLED.h"

using namespace cv;

#pragma pack(push)
#pragma pack(8)

class MvCamera {
 private:
  CUSBHid lightControl;
  //tSdkCameraDevInfo tCameraEnumList[5];
  tSdkCameraCapbility tCapability;      //设备描述信息
  int iCameraCounts = 32;
  int hCamera = 0;
  int channel;
  unsigned char *pbyBuffer;
  unsigned char *g_pRgbBuffer; //处理后数据缓存区
  double exposureTime[4];
  Mat WhiteOffset, IROffset;

 public:
  Mat WhiteImage, IRImage, UVImage, FaceImg;
  Mat AffineTransform;
  Size outputSize;

  MvCamera();

  ~MvCamera();

  Mat GetWhiteMerged();

  Mat GetIRMerged();

  Mat GetUVImage();

  int Open();

  void Close();

  int IsOpened() { return hCamera == -1; }

  int AutoFitAllParam();

  int loadConfigFromPath(string);
  int saveConfigToPath(string);
  int configLoaded();

  void testCamera();

  int inputDetect();

  int clearImages();

 private:
  Mat GetWhiteLeft();
  Mat GetWhiteRight();
  Mat GetIRImage();
  Mat GetIRImageFast();

//    Mat GetIRLeft();
//    Mat GetIRRight();

  Mat CaptureImage(double, int, int);
  Mat CaptureImageFast(double, int);

  double AutoFitParam(USBLED::LIGHT_TYPE, int);

};


//void testCamera();

#endif //PASSPORTREADER_CAMERAAPI_H

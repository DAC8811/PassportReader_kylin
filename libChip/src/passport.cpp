//#include <iostream>
#include <fstream>
#include <vector>
//#include <codecvt>
#include <map>
#include <string>
#include "echip.h"
#include "libChip.h"

#define LOG_TAG "PassportReaderCPP-chip"
#include "internalLogging.h"

using namespace std;

/**
 *新增，获取卡类型
 * 返回值：
 * -2——设备未打开
 * -1——无芯片
 *  0——身份证
 *  1——护照/通行证
 */

int PassportTest(void){
	int ret = -2;

    // RFReader echipReader;

    CChipReader gChipReader;
    ret = gChipReader.PassportCardTest();
    gChipReader.CloseWire();
    // gChipReader.Close();
	return ret;
}


int EchipOneCardRead(string code,
                     string &DG1,
                     string &DG11,
                     string path,
                     struct MrzInfoStruct & mrzInfo) {
    char ret = false;
    CChipReader gChipReader;

//    gChipReader.AuthKeyFunc(A);
//    gChipReader.ChangeAuthKeyFunc(A,A);
//    unsigned char data[8];
//    unsigned char num[8] = {0xE4,0xBD,0x14,0x98,0xE0,0x3A,0x87,0xC3};
//    gChipReader.RF_WriteDevNum(num);
//    gChipReader.RF_ReadDevNum(data);
    gChipReader.SetCardType(CARD_TYPE_ONE);
    gChipReader.EF_DG2_SetResultPath(path);
    ret = gChipReader.ReadInfo(code);
    if (ret == true) {
        ret = gChipReader.GetResult(EF_DG1, DG1);
        if (ret == false) {
            LOGE("GetResult: EF_DG1 file get error!\n");
        }
        ret = gChipReader.GetResult(EF_DG11, DG11);
        if (ret == false) {
            LOGE("GetResult: EF_DG2 file get error!\n");
        }
    }
    mrzInfo=gChipReader.mrtd.mrzInfo;
    gChipReader.CloseWire();
    return ret;
}

int EchipTwoCardRead(string code,
                     string &DG1,
                     string &DG11,
                     string path,
                     struct MrzInfoStruct & mrzInfo) {
    char ret = false;
    CChipReader gChipReader;

    size_t index = code.find("\n");
    if (index == std::string::npos) {
        LOGE("%s", code.c_str());
        return -1;
    }
    code.replace(index, 1, "\n");
    gChipReader.SetCardType(CARD_TYPE_TWO);
    gChipReader.EF_DG2_SetResultPath(path);
    ret = gChipReader.ReadInfo(code);
    if (ret) {
        ret = gChipReader.GetResult(EF_DG1, DG1);
        if (!ret) {
            LOGE("GetResult: EF_DG1 file get error!\n");
        }
        //todo
        int ret1 = gChipReader.GetResult(EF_DG11, DG11);
        if (ret1 == false) {
            LOGE("GetResult: EF_DG11 file get error!\n");
        }
    }

    mrzInfo=gChipReader.mrtd.mrzInfo;
    gChipReader.CloseWire();
    return ret;
}

int EchipThreeCardRead(string code,
                       string &DG1,
                       string &DG11,
                       string path,
                     struct MrzInfoStruct & mrzInfo) {
    char ret = false;
    CChipReader gChipReader;

    size_t index1 = code.find("\n");
    if (index1 == std::string::npos) {
        LOGD("code");
        return -1;
    }
    code.replace(index1, 1, "\n");

    size_t index2 = code.find("\n");
    if (index2 == std::string::npos) {
        LOGD("code");
        return -1;
    }
    code.replace(index2, 1, "\n");

    gChipReader.SetCardType(CARD_TYPE_THREE);
    gChipReader.EF_DG2_SetResultPath(path);
    ret = gChipReader.ReadInfo(code);

    if (ret) {
        ret = gChipReader.GetResult(EF_DG1, DG1);
        if (!ret) {
            LOGE("GetResult: EF_DG1 file get error!\n");
        }
        ret = gChipReader.GetResult(EF_DG11, DG11);
        if (ret == false) {
            LOGE("GetResult: EF_DG2 file get error!\n");
        }
    }

    mrzInfo=gChipReader.mrtd.mrzInfo;
    gChipReader.CloseWire();
    return ret;
}

int IDCardRead(string &IDInfo,
               string &IDPic) {
    char ret = false;
    CChipReader gChipReader;
    IDInfo.clear();
    IDPic.clear();

    gChipReader.SetCardType(CARD_TYEP_ID);

    ret = gChipReader.ReadIDInfo();
    if (ret == true) {
        ret = gChipReader.GetResult(EF_IDINFO, IDInfo);
        if (!ret) {
            LOGE("GetResult: EF_IDINFO file get error!\n");
        }
        ret = gChipReader.GetResult(EF_IDPIC, IDPic);
        if (!ret) {
            LOGE("GetResult: EF_IDPIC file get error!\n");
        }
    }
    gChipReader.CloseWire();
    return ret;
}

int IDcardReadWithFinger(unsigned char *sInfo, unsigned int * sLength, unsigned char *cpPhoto, unsigned int *ipLength, unsigned char *cpFinger, unsigned int *ipFLength){
    char ret = false;
    CChipReader gChipReader;

    gChipReader.SetCardType(CARD_TYEP_ID);

    ret = gChipReader.ReadIDInfo(sInfo, sLength, cpPhoto, ipLength, cpFinger, ipFLength);
    if (ret == true) {
        LOGI("Read IDCard success");
    } else{
        LOGE("Read IDCard failed");
    }
    gChipReader.CloseWire();
    return ret;
}

int CloseWire(){
    char ret = false;
    CChipReader gChipReader;
    ret = gChipReader.CloseWire();
    return ret;
}

int AuthDev(){
    char ret = false;
    CChipReader gChipReader;
    std::string A("\x49\x45\x4D\x4B\x41\x45\x52\x42\x21\x4E\x41\x43\x55\x4F\x59\x46",16);
    ret = gChipReader.AuthKeyFunc(A);
    return ret;
}







//
// Created by 张迪 on 2020/5/15.
//

#ifndef PASSPORTREADER_UTILS_ZHANGDI_H
#define PASSPORTREADER_UTILS_ZHANGDI_H

#include <string>
#include <sstream>
#include <map>
#include <vector>

enum DOC_TYPE{
    UNKNOWN = -1,                 //未知
    ID_CARD = 1,                  //二代证
    PASSPORT = 0,                 //护照
    HK_MACAO_PASSPORT = 2,        //港澳通行证
    HK_MACAO_VISIT_MAINLAND = 3,  //回乡证，港澳居民来往内地通行证
    TAIWAN_VISIT_MAINLAND = 4     //台胞证，台湾居民来往大陆通行证
};
enum OCR_TYPE{
    UNKNOWN_LINE = -1,
    ONE_LINE = 1,
    TWO_LINE = 2,
    THREE_LINE = 3
};
extern std::map<std::string, std::string> ETHNICITY;
extern std::map<std::string, std::string> GENDER;
/**************切割字符串的方法 ****************/
void split(const std::string &s, std::vector<std::string> &tokens, const std::string &delimiters = " ");
/*************将mrzCode中间的\n 剔除，得到一行字符串*****************/
std::string delN(std::string mrzCode);
/***********处理mrzCode的方法，得到的是一定格式的json字符串，读卡失败，OCR成功会调用***********/
std::string processMRZ(std::string mrzCode, DOC_TYPE docType);
/***********处理DG1,GD11的方法，得到的是一定格式的json字符串，读卡成功会调用***********/
std::string processDG(std::string DG1, std::string DG11, DOC_TYPE docType);
/***********处理身份证电子信息的方法****************/
std::string processSFZInfo(std::string sfzInfo);

#endif //PASSPORTREADER_UTILS_ZHANGDI_H

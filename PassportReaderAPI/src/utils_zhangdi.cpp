//
// Created by 张迪 on 2020/5/15.
//

#include "utils_zhangdi.h"

std::map<std::string, std::string> ETHNICITY{
        {"01", "汉族"},{"02", "蒙古族"},{"03", "回族"},{"04", "藏族"},{"05", "维吾尔族"},{"06", "苗族"},{"07", "彝族"},
        {"08", "壮族"},{"09", "布依族"},{"10", "朝鲜族"},{"11", "满族"},{"12", "侗族"},{"13", "瑶族"},{"14", "白族"},
        {"15", "土家族"},{"16", "哈尼族"},{"17", "哈萨克族"},{"18", "傣族"},{"19", "黎族"},{"20", "傈傈族"},{"21", "佤族"},
        {"22", "畲族"},{"23", "高山族"},{"24", "拉祜族"},{"25", "水族"},{"26", "东乡族"},{"27", "纳西族"},{"28", "景颇族"},
        {"29", "柯尔克孜族"},{"30", "土族"},{"31", "达斡尔族"},{"32", "仫佬族"},{"33", "羌族"},{"34", "布朗族"},{"35", "撒拉族"},
        {"36", "毛难族"},{"37", "仡佬族"},{"38", "锡伯族"},{"39", "阿昌族"},{"40", "普米族"},{"41", "塔吉克族"},{"42", "怒族"},
        {"43", "乌孜别克族"},{"44", "俄罗斯族"},{"45", "鄂温克族"},{"46", "崩龙族"},{"47", "保安族"},{"48", "裕固族"},{"49", "京族"},
        {"50", "塔塔尔族"},{"51", "独龙族"},{"52", "鄂伦春族"},{"53", "赫哲族"},{"54", "门巴族"},{"55", "珞巴族"},{"56", "基洛族"},
        {"57", "其他"},{"58", "外国血统中国籍人士"},{"59", "港澳台"}};
std::map<std::string, std::string> GENDER{
        {"F", "女"},{"M", "男"},{"1", "男"},{"0", "女"}};

void split(const std::string &s, std::vector<std::string> &tokens, const std::string &delimiters) {
    std::string::size_type lastPos = s.find_first_not_of(delimiters, 0);
    std::string::size_type pos = s.find_first_of(delimiters, lastPos);
    while (std::string::npos != pos || std::string::npos != lastPos) {
        tokens.push_back(s.substr(lastPos, pos - lastPos));//use emplace_back after C++11
        lastPos = s.find_first_not_of(delimiters, pos);
        pos = s.find_first_of(delimiters, lastPos);
    }
}
std::string delN(std::string ocrInfo){
    std::string temp = ocrInfo;
    size_t index1 = temp.find("\n");//找到\n的下标
    if(index1 == std::string::npos){
        return temp;                //第一次就没有找到说明是一行机读码，直接返回
    }
    temp.replace(index1,1,"");      //将第一个\n替换
    size_t index2 = temp.find("\n");//开始找第2个
    if(index2 == std::string::npos){
        return temp;                //没有，说明是两行机读码，可以返回
    }
    temp.replace(index2,1,"");      //找到了，就是2行机读码，替换之后返回
    return temp;
}
std::string processMRZ(std::string mrzCode, DOC_TYPE docType){
    if(mrzCode.empty()||mrzCode.length() == 0)
        return "";
    std::stringstream json;
    std::vector<std::string> buffers;
    switch(docType) {
        case PASSPORT: {
            mrzCode = delN(mrzCode);
            split(mrzCode, buffers, "<");
            json << "{"
                 << "\"ChipReadResult\":\"" << "-1" << "\","
                 << "\"DocumentType\":\"" << docType << "\","
                 << "\"IDcardInfo\":\"" << "" << "\","
                 << "\"PassportInfo\":"
                 << "{"
                 << "\"BirthDate\":\"" << buffers[2].substr(13, 6) << "\","
                 //<< "\"DocType\":\"" << mrzInfo.Type << "\","
                 << "\"DocType\":\"" << "P" << "\","
                 << "\"FirstName\":\"" << buffers[1] << "\","
                 << "\"Gender\":\"" << GENDER[buffers[2].substr(20, 1)] << "\","
                 << "\"Issuing\":\"" << buffers[0].substr(2, 3) << "\","
                 << "\"LastName\":\"" << buffers[0].substr(5) << "\","
                 << "\"MiddleName\":\"" << "" << "\","
                 << "\"Nationality\":\"" << buffers[2].substr(10, 3) << "\","
                 << "\"Option1\":\"" << "" << "\","
                 << "\"SerialNum\":\"" << buffers[2].substr(0, 9) << "\","
                 << "\"Validity\":\"" << buffers[2].substr(21, 6) << "\","
                 << "\"FullName\":\"" << "" << "\""
                 << "}"
                 << "}" << std::ends;
            break;
        }
        case HK_MACAO_PASSPORT: {
            split(mrzCode, buffers);
            json << "{"
                 << "\"ChipReadResult\":\"" << "-1" << "\","
                 << "\"DocumentType\":\"" << docType << "\","
                 << "\"IDcardInfo\":\"" << "" << "\","
                 << "\"PassportInfo\":"
                 << "{"
                 << "\"BirthDate\":\"" << buffers[2].substr(0, 6) << "\","
                 //<< "\"DocType\":\"" << mrzInfo.Type << "\","
                 << "\"DocType\":\"" << "C" << "\","
                 << "\"FirstName\":\"" << "" << "\","
                 << "\"Gender\":\"" << "" << "\","
                 << "\"Issuing\":\"" << "" << "\","
                 << "\"LastName\":\"" << "" << "\","
                 << "\"MiddleName\":\"" << "" << "\","
                 << "\"Nationality\":\"" << "" << "\","
                 << "\"Option1\":\"" << "" << "\","
                 << "\"SerialNum\":\"" << buffers[0].substr(2, 9) << "\","
                 << "\"Validity\":\"" << buffers[1].substr(0, 6) << "\","
                 << "\"FullName\":\"" << "" << "\""
                 << "}"
                 << "}" << std::ends;
            break;
        }
        case HK_MACAO_VISIT_MAINLAND: {
            std::string info = delN(mrzCode);//把OCR字符串中间的换行符号去除
            std::string nameLine = info.substr(60);//这个是第三行，我把它给分割
            std::vector<std::string> bufferline;
            split(nameLine, bufferline, "<");
            int size = bufferline.size();
            std::string firstname = "";
            for (int i = 1; i < size; i++) {
                firstname = firstname + bufferline[i];
            }
            std::string hkid = info.substr(49, 9);
            if (hkid.find("<") != std::string::npos) {
                hkid = hkid.substr(0, 8);
            }
            json << "{"
                 << "\"ChipReadResult\":\"" << "-1" << "\","
                 << "\"DocumentType\":\"" << docType << "\","
                 << "\"IDcardInfo\":\"" << "" << "\","
                 << "\"PassportInfo\":"
                 << "{"
                 << "\"BirthDate\":\"" << info.substr(23, 6) << "\","
                 //<< "\"DocType\":\"" << mrzInfo.Type << "\","
                 << "\"DocType\":\"" << "C" << "\","
                 << "\"FirstName\":\"" << firstname << "\","
                 << "\"Gender\":\"" << GENDER[info.substr(22, 1)] << "\","
                 << "\"Issuing\":\"" << "" << "\","
                 << "\"LastName\":\"" << bufferline[0] << "\","
                 << "\"MiddleName\":\"" << "" << "\","
                 << "\"Nationality\":\"" << "CHN" << "\","
                 << "\"Option1\":\"" << hkid << "\","
                 << "\"SerialNum\":\"" << info.substr(2, 9) << "\","
                 << "\"Validity\":\"" << info.substr(15, 6) << "\","
                 << "\"FullName\":\"" << "" << "\""
                 << "}"
                 << "}" << std::ends;
            break;
        }
        case TAIWAN_VISIT_MAINLAND:{
            std::string info = delN(mrzCode);//把OCR字符串中间的换行符号去除
            std::string nameLine = info.substr(60);//这个是第三行，我把它给分割
            std::vector<std::string> bufferline;
            split(nameLine, bufferline, "<");
            int size = bufferline.size();
            std::string firstname = "";
            for (int i = 1; i < size - 1; i++) {
                firstname = firstname + bufferline[i];
            }
            std::string twid = info.substr(49, 10);
            //剔除后面的<
            if (twid.find("<") != std::string::npos) {
                //hkid = hkid.substr(0, 8);
                int index = twid.find("<");
                twid = twid.substr(0, index);
            }
            json << "{"
                 << "\"ChipReadResult\":\"" << "-1" << "\","
                 << "\"DocumentType\":\"" << docType << "\","
                 << "\"IDcardInfo\":\"" << "" << "\","
                 << "\"PassportInfo\":"
                 << "{"
                 << "\"BirthDate\":\"" << info.substr(23, 6) << "\","
                 //<< "\"DocType\":\"" << mrzInfo.Type << "\","
                 << "\"DocType\":\"" << "C" << "\","
                 << "\"FirstName\":\"" << firstname << "\","
                 << "\"Gender\":\"" << GENDER[info.substr(22, 1)] << "\","
                 << "\"Issuing\":\"" << bufferline[size - 1] << "\","
                 << "\"LastName\":\"" << bufferline[0] << "\","
                 << "\"MiddleName\":\"" << "" << "\","
                 << "\"Nationality\":\"" << "CHN" << "\","
                 << "\"Option1\":\"" << twid << "\","
                 << "\"SerialNum\":\"" << info.substr(2, 8) << "\","
                 << "\"Validity\":\"" << info.substr(15, 6) << "\","
                 << "\"FullName\":\"" << "" << "\""
                 << "}"
                 << "}" << std::ends;
            break;
        }
        default: {
            json << "{"
                 << "\"ChipReadResult\":\"" << "-1" << "\","
                 << "\"DocumentType\":\"" << -1 << "\","
                 << "\"IDcardInfo\":\"" << "" << "\","
                 << "\"PassportInfo\":\"" << "" << "\""
                 << "}" << std::ends;
            break;
        }
    }
    return json.str();
}
std::string processDG(std::string DG1, std::string DG11, DOC_TYPE docType){
    if(DG1.empty()||DG1.length()==0)
        return "";
    std::stringstream json;
    std::vector<std::string> buffers;
    switch (docType){
        case PASSPORT: {
            split(DG1, buffers, "<");
            json << "{"
                 << "\"ChipReadResult\":\"" << "1" << "\","
                 << "\"DocumentType\":\"" << docType << "\","
                 << "\"IDcardInfo\":\"" << "" << "\","
                 << "\"PassportInfo\":"
                 << "{"
                 << "\"BirthDate\":\"" << buffers[2].substr(13, 6) << "\","
                 //<< "\"DocType\":\"" << mrzInfo.Type << "\","
                 << "\"DocType\":\"" << "P" << "\","
                 << "\"FirstName\":\"" << buffers[1] << "\","
                 << "\"Gender\":\"" << GENDER[buffers[2].substr(20, 1)] << "\","
                 << "\"Issuing\":\"" << buffers[0].substr(2, 3) << "\","
                 << "\"LastName\":\"" << buffers[0].substr(5) << "\","
                 << "\"MiddleName\":\"" << "" << "\","
                 << "\"Nationality\":\"" << buffers[2].substr(10, 3) << "\","
                 << "\"Option1\":\"" << "" << "\","
                 << "\"SerialNum\":\"" << buffers[2].substr(0, 9) << "\","
                 << "\"Validity\":\"" << buffers[2].substr(21, 6) << "\","
                 << "\"FullName\":\"" << DG11 << "\""
                 << "}"
                 << "}" << std::ends;
            break;
        }
        case HK_MACAO_PASSPORT: {
            split(DG1, buffers, "<");
            if (buffers.size() == 7) {
                json << "{"
                     << "\"ChipReadResult\":\"" << "1" << "\","
                     << "\"DocumentType\":\"" << docType << "\","
                     << "\"IDcardInfo\":\"" << "" << "\","
                     << "\"PassportInfo\":"
                     << "{"
                     << "\"BirthDate\":\"" << buffers[2].substr(0, 6) << "\","
                     //<< "\"DocType\":\"" << mrzInfo.Type << "\","
                     << "\"DocType\":\"" << "CS" << "\","
                     << "\"FirstName\":\"" << buffers[5] << "\","
                     << "\"Gender\":\"" << GENDER[buffers[0].substr(22, 1)] << "\","
                     << "\"Issuing\":\"" << "" << "\","
                     << "\"LastName\":\"" << buffers[4] << "\","
                     << "\"MiddleName\":\"" << "" << "\","
                     << "\"Nationality\":\"" << "" << "\","
                     << "\"Option1\":\"" << "" << "\","
                     << "\"SerialNum\":\"" << buffers[0].substr(2, 9) << "\","
                     << "\"Validity\":\"" << buffers[1].substr(0, 6) << "\","
                     << "\"FullName\":\"" << DG11 << "\""
                     << "}"
                     << "}" << std::ends;
            } else {
                //LOGI("size = %d",buffers.size());
                json << "{"
                     << "\"ChipReadResult\":\"" << "1" << "\","
                     << "\"DocumentType\":\"" << docType << "\","
                     << "\"IDcardInfo\":\"" << "" << "\","
                     << "\"PassportInfo\":"
                     << "{"
                     << "\"BirthDate\":\"" << buffers[2].substr(0, 6) << "\","
                     //<< "\"DocType\":\"" << mrzInfo.Type << "\","
                     << "\"DocType\":\"" << "CS" << "\","
                     << "\"FirstName\":\"" << buffers[4] << "\","
                     << "\"Gender\":\"" << GENDER[buffers[5].substr(0, 1)] << "\","
                     << "\"Issuing\":\"" << "" << "\","
                     << "\"LastName\":\"" << buffers[3].substr(13) << "\","
                     << "\"MiddleName\":\"" << "" << "\","
                     << "\"Nationality\":\"" << "" << "\","
                     << "\"Option1\":\"" << "" << "\","
                     << "\"SerialNum\":\"" << buffers[0].substr(2, 9) << "\","
                     << "\"Validity\":\"" << buffers[1].substr(0, 6) << "\","
                     << "\"FullName\":\"" << DG11 << "\""
                     << "}"
                     << "}" << std::ends;
            }
            break;
        }
        case HK_MACAO_VISIT_MAINLAND: {
            std::string nameLine = DG1.substr(60);//这个是第三行，我把它给分割
            split(nameLine, buffers, "<");
            int size = buffers.size();
            std::string firstname = "";
            for (int i = 1; i < size; i++) {
                firstname = firstname + buffers[i];
            }
            std::string hkid = DG1.substr(49, 9);
            if (hkid.find("<") != std::string::npos) {
                hkid = hkid.substr(0, 8);
            }
            json << "{"
                 << "\"ChipReadResult\":\"" << "1" << "\","
                 << "\"DocumentType\":\"" << docType << "\","
                 << "\"IDcardInfo\":\"" << "" << "\","
                 << "\"PassportInfo\":"
                 << "{"
                 << "\"BirthDate\":\"" << DG1.substr(23, 6) << "\","
                 //<< "\"DocType\":\"" << mrzInfo.Type << "\","
                 << "\"DocType\":\"" << "C" << "\","
                 << "\"FirstName\":\"" << firstname << "\","
                 << "\"Gender\":\"" << GENDER[DG1.substr(22, 1)] << "\","
                 << "\"Issuing\":\"" << "" << "\","
                 << "\"LastName\":\"" << buffers[0] << "\","
                 << "\"MiddleName\":\"" << "" << "\","
                 << "\"Nationality\":\"" << "CHN" << "\","
                 << "\"Option1\":\"" << hkid << "\","
                 << "\"SerialNum\":\"" << DG1.substr(2, 9) << "\","
                 << "\"Validity\":\"" << DG1.substr(15, 6) << "\","
                 << "\"FullName\":\"" << DG11 << "\""
                 << "}"
                 << "}" << std::ends;
            break;
        }
        case TAIWAN_VISIT_MAINLAND: {
            std::string nameLine = DG1.substr(60);//这个是第三行，我把它给分割
            split(nameLine, buffers, "<");
            int size = buffers.size();
            std::string firstname = "";
            for (int i = 1; i < size - 1; i++) {
                firstname = firstname + buffers[i];
            }
            std::string twid = DG1.substr(49, 10);
            //剔除后面的<
            if (twid.find("<") != std::string::npos) {
                //hkid = hkid.substr(0, 8);
                int index = twid.find("<");
                twid = twid.substr(0, index);
            }
            json << "{"
                 << "\"ChipReadResult\":\"" << "1" << "\","
                 << "\"DocumentType\":\"" << docType << "\","
                 << "\"IDcardInfo\":\"" << "" << "\","
                 << "\"PassportInfo\":"
                 << "{"
                 << "\"BirthDate\":\"" << DG1.substr(23, 6) << "\","
                 //<< "\"DocType\":\"" << mrzInfo.Type << "\","
                 << "\"DocType\":\"" << "C" << "\","
                 << "\"FirstName\":\"" << firstname << "\","
                 << "\"Gender\":\"" << GENDER[DG1.substr(22, 1)] << "\","
                 << "\"Issuing\":\"" << buffers[size - 1] << "\","
                 << "\"LastName\":\"" << buffers[0] << "\","
                 << "\"MiddleName\":\"" << "" << "\","
                 << "\"Nationality\":\"" << "CHN" << "\","
                 << "\"Option1\":\"" << twid << "\","
                 << "\"SerialNum\":\"" << DG1.substr(2, 8) << "\","
                 << "\"Validity\":\"" << DG1.substr(15, 6) << "\","
                 << "\"FullName\":\"" << DG11 << "\""
                 << "}"
                 << "}" << std::ends;
            break;
        }
        default: {
            json << "{"
                 << "\"ChipReadResult\":\"" << "-1" << "\","
                 << "\"DocumentType\":\"" << -1 << "\","
                 << "\"IDcardInfo\":\"" << "" << "\","
                 << "\"PassportInfo\":\"" << "" << "\""
                 << "}" << std::ends;
            break;
        }
    }
    return json.str();
}
std::string processSFZInfo(std::string sfzInfo){
    if(sfzInfo.empty() || sfzInfo.length() == 0)
        return "";
    std::vector<std::string> buffers;
    std::stringstream json;
    split(sfzInfo,buffers," ");
    json << "{"
         << "\"ChipReadResult\":\"" << "1" << "\","
         << "\"DocumentType\":\"" << "1" << "\","
         << "\"IDcardInfo\":"
         << "{"
         << "\"Address\":\"" << buffers[1].substr(11, std::string::npos) << "\","
         << "\"Authority\":\"" << buffers[2].substr(18, std::string::npos) << "\","
         << "\"Birthdate\":\"" << buffers[2].substr(6, 8) << "\","
         << "\"Ethnicity\":\"" << ETHNICITY[buffers[1].substr(1, 2)] << "\","
         << "\"Gender\":\"" << GENDER[buffers[1].substr(0, 1)] << "\","
         << "\"IDnumber\":\"" << buffers[2].substr(0, 18) << "\","
         << "\"Name\":\"" << buffers[0] << "\","
         << "\"Validity_Start\":\"" << buffers[3].substr(0, 8) << "\","
         << "\"Validity_Termi\":\"" << buffers[3].substr(8, std::string::npos) << "\""
         << "},"
         << "\"PassportInfo\":\"" << "" << "\""
         << "}" << std::ends;
    return json.str();
}

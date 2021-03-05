//#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <iomanip>
#include <locale.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include <openssl/des.h>
#include <exception>
#include <iosfwd>
#include "Ptypes.h"
#include "MRTD.h"
#include "RFReader.h"
#include "echip.h"
#include "EFFile.h"
#include "JP2.h"
#include "CryptoDES.h"

#define LOG_TAG "PassportReaderCPP-chip"
//#define OOXX_DEBUG_LOG 1
//#define LOG_DEBUG
#include "internalLogging.h"

using namespace std;

//#define CHECK_OK(x) if ((!x)) return false;
//const static unsigned char SW_SUCCESS[] = "\x90\x00";
#define KLEN 16

//////////////////////////////////////////////////////////////////////////////////
char EFFileDummyParse(std::string &data, STEFFileSystem *fileSystem) {
    return false;
}

char EF_COM_FileParse(std::string &data, STEFFileSystem *fileSystem) {
    return true;
}

char EF_DG1_FileParse(std::string &data, STEFFileSystem *fileSystem) {
    std::string flag("\x5F\x1F", 2);
    size_t it = data.find(flag);
    if (it == std::string::npos) {
        LOGE("EF_DG1_FileParse:it == std::string::npos\n");
        return false;
    }

    std::string mrz = data.substr(it + 3);
#if USE_LOG_LEVEL1
    LOGV("EF_DG1_File::FileParse: " << mrz);
#endif

    fileSystem->stEFFiles[EF_DG1].resultLen = 0;
    for (int i = 0; i < mrz.size(); i++) {
        fileSystem->stEFFiles[EF_DG1].result[i] = mrz[i];
        fileSystem->stEFFiles[EF_DG1].resultLen++;
    }
    fileSystem->stEFFiles[EF_DG1].result[mrz.size()] = '\0';
    //LOGV("EF_DG1_FileParse: resultLen  == %d\n",fileSystem->stEFFiles[EF_DG1].resultLen);
    //LOGV("EF_DG1_FileParse: result  == %s\n",fileSystem->stEFFiles[EF_DG1].result);
    return true;
}

char EF_DG2_FileParse(std::string &data, STEFFileSystem *fileSystem) {
//	char path[256];
//	int len = MakeFullPath(path,DG2_FILE_NAME);

#if USE_OPENJPEG
    //LOGV("DG2.bmp path == %s\n",fileSystem->stEFFiles[EF_DG2].resultPath);
    return jp2_to_bmp(data, fileSystem->stEFFiles[EF_DG2].resultPath);
#else
    return true;
#endif
}

char EF_DG11_FileParse(std::string &data, STEFFileSystem *fileSystem) {

    fileSystem->stEFFiles[EF_DG11].resultLen = 0;
    for (int i = 0; i < data.size(); i++) {
        fileSystem->stEFFiles[EF_DG11].result[i] = data[i];
        fileSystem->stEFFiles[EF_DG11].resultLen++;
    }
    fileSystem->stEFFiles[EF_DG11].result[data.size()] = '\0';

    return true;
}

char EF_IDINFO_FileParse(std::string &data, STEFFileSystem *fileSystem) {

    fileSystem->stEFFiles[EF_IDINFO].resultLen = 0;
    for (int i = 0; i < data.size(); i++) {
        fileSystem->stEFFiles[EF_IDINFO].result[i] = data[i];
        fileSystem->stEFFiles[EF_IDINFO].resultLen++;
    }
    fileSystem->stEFFiles[EF_IDINFO].result[data.size()] = '\0';

    /*
    LOGV("EF_IDINFO: " << fileSystem->stEFFiles[EF_IDINFO].resultLen);
    for(int i = 0;i < fileSystem->stEFFiles[EF_IDINFO].resultLen ;i++){
        LOGV("%x ",fileSystem->stEFFiles[EF_IDINFO].result[i] & 0xff );
    }
    std::cout << std::endl;
    */
    return true;
}

char EF_IDPIC_FileParse(std::string &data, STEFFileSystem *fileSystem) {

    fileSystem->stEFFiles[EF_IDPIC].resultLen = 0;
    for (int i = 0; i < data.size(); i++) {
        fileSystem->stEFFiles[EF_IDPIC].result[i] = data[i];
        fileSystem->stEFFiles[EF_IDPIC].resultLen++;
    }
    fileSystem->stEFFiles[EF_IDPIC].result[data.size()] = '\0';

/*
    LOGV("EF_IDINFO: " << fileSystem->stEFFiles[EF_IDPIC].resultLen);
    for(int i = 0;i < fileSystem->stEFFiles[EF_IDPIC].resultLen ;i++){
        LOGV("%x ",fileSystem->stEFFiles[EF_IDPIC].result[i] & 0xff );
    }
    std::cout << std::endl;
    */
    return true;
}

char EFFileDummyValid() {
    return false;
}

char STDefaultValid() {
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
static void SelectFunc(STEFFile *stFile) {
    switch (stFile->Index) {
        case EF_COM :
            stFile->FileParse = EF_COM_FileParse;
            stFile->Valid = STDefaultValid;
            break;
        case EF_DG1 :
            stFile->FileParse = EF_DG1_FileParse;
            stFile->Valid = STDefaultValid;
            break;
        case EF_DG2 :
            stFile->FileParse = EF_DG2_FileParse;
            stFile->Valid = STDefaultValid;
            break;
        case EF_DG11:
            stFile->FileParse = EF_DG11_FileParse;
            stFile->Valid = STDefaultValid;
            break;
        case EF_IDINFO:
            stFile->FileParse = EF_IDINFO_FileParse;
            stFile->Valid = STDefaultValid;
            break;
        case EF_IDPIC:
            stFile->FileParse = EF_IDPIC_FileParse;
            stFile->Valid = STDefaultValid;
            break;
        case EF_DG3:
        case EF_DG4:
        case EF_DG5:
        case EF_DG6:
        case EF_DG7:
        case EF_DG8:
        case EF_DG9:
        case EF_DG10:
        case EF_DG12:
        case EF_DG13:
        case EF_DG14:
        case EF_DG15:
        case EF_DG16:
        case EF_SOD:
        case EF_UNKNOWN :
        default:
            stFile->FileParse = EFFileDummyParse;
            stFile->Valid = EFFileDummyValid;
            break;
    }
}

static void MyNotifyShowMRZ(int type, char *Mrz, string path) {
    CHIPMRTD mrtd;
    std::string tempMrz;

    //

    std::string strMrz(Mrz);
    LOGV("%s", Mrz);
    if (!mrtd.Parse(strMrz, type)) {

        LOGE("Parse Error ");
        return;
    }

//	char path[256];
//	MakeFullPath(path, DG1_FILE_NAME);
//	LOGV("ChipMRZ.txt: " << path);
//    std::ofstream Output;
//    Output.open(path + "/mrz.txt");


    //std::string Gender = (mrtd.Gender == 'F' ? "Female" : "Male");;
    //std::string PassportType = ((mrtd.Type == 'p' || mrtd.Type == 'P') ? "Passport" : "Unknow");

//    Output << mrtd.mrzInfo.PassportNo << std::endl;
//    Output << mrtd.mrzInfo.DateOfBirth << std::endl;
//    Output << mrtd.mrzInfo.ExpiryDate << std::endl;
//    Output << mrtd.mrzInfo.Issuer << std::endl;
//    Output << mrtd.mrzInfo.Country << std::endl;
//    Output << mrtd.mrzInfo.FirstName << std::endl;
//    Output << mrtd.mrzInfo.MiddleName << std::endl;
//    Output << mrtd.mrzInfo.LastName << std::endl;
//    Output << mrtd.mrzInfo.Type << std::endl;
//    Output << mrtd.mrzInfo.Opential_1 << std::endl;
//    //LOGV("strOption2 == " << mrtd.strOption2);
//    Output << mrtd.mrzInfo.Gender << std::endl;
//    Output << mrtd.code.c_str() << std::endl;
//
//    Output.close();
}

//生成指定长度的随机数,长度需要在调用resize方法之前甚至好
static void BuildRandomData(std::string &data) {
    srand((unsigned) time(NULL));
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = rand() % 255 + 1;
    }
}

static void CheckParity(
        const std::string &srcChar,
        std::string &dstChar,
        int nLen) {
    unsigned char sinChar;
    short minBit = 0;
    short count = 0;

    if (dstChar.size() != srcChar.size())
        dstChar.resize(nLen);//分配内存

    for (int i = 0; i < nLen; i++) {
        count = 0;
        sinChar = srcChar[i];
        minBit = sinChar % 2;
        for (int j = 0; j < 8; j++) {
            if (sinChar % 2 == 1)
                count++;

            sinChar >>= 1;
        }
        if (count % 2 == 1)
            dstChar[i] = srcChar[i];
        else if (minBit == 1)
            dstChar[i] = srcChar[i] - 1;
        else
            dstChar[i] = srcChar[i] + 1;
    }
}

static void KencTDES(
        const std::string &strInputData,
        const std::string &Kenc,
        std::string &strOutputData,
        int enc) {
    DES_cblock IV = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    DES_cblock key1, key2, key3;
    DES_key_schedule schKey1, schKey2, schKey3;
    int byteNum = strInputData.size(), r = 0;
    unsigned char *inputData = (unsigned char *) strInputData.c_str();
    std::string hexKa1, hexKb1;

    hexKa1.append(Kenc.data(), 8);
    hexKb1.append(Kenc.data() + 8, 8);

    memcpy(key1, hexKa1.data(), hexKa1.size());
    memcpy(key2, hexKb1.data(), hexKb1.size());
    memcpy(key3, hexKa1.data(), hexKa1.size());

    if (-2 == (
            DES_set_key_checked(&key1, &schKey1) |
            DES_set_key_checked(&key2, &schKey2) |
            DES_set_key_checked(&key3, &schKey3))) {
        //throw std::exception("weak key");
    }

    strOutputData.resize(byteNum);

    DES_ede3_cbc_encrypt(inputData, (unsigned char *) strOutputData.data(), strOutputData.size(),
                         &schKey1, &schKey2, &schKey3, &IV, enc);
}

static void KmacDES(
        const std::string &inputData,
        const std::string &Kmac,
        std::string &encryptData) {
    int iter = inputData.size() / 8;
    std::string enKey, deKey;

    enKey.append(Kmac.data(), 8);
    deKey.append(Kmac.data() + 8, 8);
    std::string inBuffer(8, 0), outBuffer(8, 0);

    DES_cblock IV = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    DES_key_schedule enSchKey, deSchKey;

    DES_set_key_unchecked((const_DES_cblock *) enKey.data(), &enSchKey);
    DES_set_key_unchecked((const_DES_cblock *) deKey.data(), &deSchKey);

    for (int i = 0; i < iter; i++) {

        inBuffer.assign(inputData.data() + i * 8, 8);

        for (int j = 0; j < 8; j++) {
            inBuffer[j] ^= outBuffer[j];
        }

        DES_cbc_encrypt((unsigned char *) inBuffer.data(), (unsigned char *) outBuffer.data(),
                        8, &enSchKey, &IV, DES_ENCRYPT);
    }

    encryptData.resize(8);

    DES_cbc_encrypt((unsigned char *) outBuffer.data(), (unsigned char *) inBuffer.data(), 8,
                    &deSchKey, &IV, DES_DECRYPT);
    DES_cbc_encrypt((unsigned char *) inBuffer.data(), (unsigned char *) encryptData.data(), 8,
                    &enSchKey, &IV, DES_ENCRYPT);
}

static void SHA1ToKencKmac(
        std::string &Kseed,
        std::string &Kenc,
        std::string &Kmac) {
    int byteNumOfKseed = Kseed.size();

    std::string c1("\x00\x00\x00\x01", 4);
    std::string c2("\x00\x00\x00\x02", 4);
    std::string D1, D2, HD1(20, 0), HD2(20, 0);

    D1.append(Kseed.data(), Kseed.size());
    D2.append(Kseed.data(), Kseed.size());
    D1.append(c1.data(), c1.size());
    D2.append(c2.data(), c2.size());

    SHA1((unsigned char *) D1.data(), D1.size(), (unsigned char *) HD1.data());
    SHA1((unsigned char *) D2.data(), D2.size(), (unsigned char *) HD2.data());
    CheckParity(HD1, Kenc, 16);
    CheckParity(HD2, Kmac, 16);
}

static void DesAddPaddingBytes(std::string &data) {
    size_t dsize = data.size();
    dsize = DES_ALIGN(dsize, 8);
    data.push_back('\x80');
    for (size_t i = data.size(); i < dsize; i++) {
        data.push_back(0x00);
    }
}

static void DesRemovePaddingBytes(std::string &data) {
    size_t dsize = data.size();
    for (size_t i = dsize - 1; i >= 0; i--) {
        unsigned char c = data[i];
        if (c == 0x00) {
            continue;
        } else if (c == 0x80) {
            data.erase(i, dsize - i);
            break;
        } else {
            break;
        }
    }
}

static void IncreaseSSC(std::string &ssc, int len) {
    int i = len - 1;
    if (i < 0) {
        return;
    }
    unsigned char b = ssc[i];
    if (b == 0xFF) {
        ssc[i] = 0x00;
        IncreaseSSC(ssc, i);
    } else {
        ssc[i] = b + 1;
    }
}

static void IncreaseSSC(std::string &ssc) {
    IncreaseSSC(ssc, ssc.size());
}

//按BER_TLV格式解析Length值,其中tlLen返回Tag-Length字段的字节数,从2到4
static char ParseFileLengthIn4Bytes(const std::string &data, unsigned short *len, int *tlLen = NULL) {
    if (data.size() < 4) {
        return false;
    }
    unsigned char b = data[1];

    if (b == 0x82) {
        short *nlen = (short *) ((char *) data.data() + 2);
        *len =  HostToNetworkUINT16(*nlen) + 4;
        if (tlLen) *tlLen = 4;
    } else if (b == 0x81) {
        *len = (unsigned char) data[2] + 3;
        if (tlLen) *tlLen = 3;
    } else if (b <= 0x7F) {
        *len = (unsigned char) data[1] + 2;
        if (tlLen) *tlLen = 2;
    }
    return true;
}

// 从RAPDU中解析DO的TLV数据
static std::string RAPDUParse(std::string &rapdu, unsigned char DO, int *tlLen = NULL) {
    // 按照TLV结构查找
    for (size_t i = 0; i < rapdu.size();) {
        if (i + 4 > rapdu.size() - 1) {
            break;
        }
        std::string head4bytes(rapdu.data() + i, 4);

        unsigned char b = rapdu[i];
        unsigned short nLen;
        ParseFileLengthIn4Bytes(head4bytes, &nLen, tlLen);

        if (b == DO) {
            return rapdu.substr(i, nLen);
        } else {
            i += nLen;
        }
    }
    //std::stringstream ss;
    //ss << "not found DO" << std::hex << std::setw(2) << std::setfill('0') << (short) DO;
    //throw std::exception(ss.str().c_str());
    return string();
}

// 解析EF.COM文件可用文件的Tag列表
static std::string EF_COM_TagsParse(std::string &ef_com_data) {
    size_t it = ef_com_data.rfind(0x5C);
    if (it == std::string::npos) {
        //throw std::exception("not found tag 0x5c in EF.COM");
    }
    int tagCount = ef_com_data.size() - it - 2;
    return ef_com_data.substr(it + 2, tagCount);
}

char CChipReader::EF_DG2_SetResultPath(string path) {
    strncpy(st_efs.stEFFiles[EF_DG2].resultPath, path.c_str(), path.length());
    baseFolder = path;
    return true;
}

CChipReader::CChipReader() : cb(NULL), m_dwTimeout(CONTROL_RW_TIMEOUT_MS) {
    STEFilesInit(&st_efs);
}

CChipReader::~CChipReader() {
    LOGV("CChipReader::~CChipReader()\n");
    if (echipReader.IsOpen())
        echipReader.Close();
}

char CChipReader::SetPassportComeIn() {
    unsigned char cpInfo[300];
    unsigned char cpLen;
    int ret;

    memset(cpInfo, 0, sizeof(cpInfo));
    for (int i = 0; i < 5; i++) {
        ret = echipReader.RF_14443_Reset(0, cpInfo, &cpLen);
        if (RF_ERR_SUCCESS != ret) {
            echipReader.Close();
        } else {
            break;
        }
    }

    if (RF_ERR_SUCCESS != ret) {
        LOGV("RF_14443_Reset Error: %d,%s,%s", __LINE__, __FUNCTION__, __FILE__);
        return false;
    }
    return true;
}

char CChipReader::GetResult(EF_NAME efName, string &retData) {

    if (efName == EF_DG2) return true;
    if (st_efs.stEFFiles[efName].resultLen > 0) {
//		if((*retLen)  <  st_efs.stEFFiles[efName].resultLen){
//			LOGV("retData size Len to small!\n");
//			return false;
//		}
//
//		memcpy(retData,
//				st_efs.stEFFiles[efName].result,
//				st_efs.stEFFiles[efName].resultLen);
//
//    	retData[st_efs.stEFFiles[efName].resultLen] = '\0';
//		*retLen = st_efs.stEFFiles[efName].resultLen;
        retData.assign(st_efs.stEFFiles[efName].result, st_efs.stEFFiles[efName].resultLen);
/*
        LOGV("result: ");
        for(int i = 0;i <  st_efs.stEFFiles[efName].resultLen;i++){
            LOGV("%x ",st_efs.stEFFiles[efName].result[i] & 0xff);
        }
        std::cout << std::endl;

        LOGV("retData: ");
        for(int i = 0;i <  retData.size();i++){
            LOGV("%x ",retData[i] & 0xff);
        }
        std::cout << std::endl;
*/

        return true;
    }
    return false;
}

char CChipReader::ReadEchipInfo(std::string &codetonfc) {
    std::string Kenc;
    std::string Kmac;
    std::string RND_ICC;
    std::string RND_IFD;
    std::string Kifd;
    std::string KSenc;
    std::string KSmac;
    std::string SSC;
#if OOXX_DEBUG_LOG
    LOGV("激活护照");
#endif

    //发送激活命令
    if (!this->SetPassportComeIn()) {
#if USE_LOG_LEVEL1
        LOGV("激活护照失败");
#endif
        return false;
    }

#if OOXX_DEBUG_LOG
    LOGV("激活完毕");
#endif

    if (codetonfc.size() < 24) {
#if USE_LOG_LEVEL1
        LOGV("非法的MRZ格式，MRZ至少需要24个字节");
#endif
        return false;
    }
    CHECK_OK(BuildKencAndKmac(codetonfc, Kenc, Kmac));

#if OOXX_DEBUG_LOG
    LOGV("选择签发方的应用");
#endif
    // 选择签发方的应用
    CHECK_OK(ICCSelectApp());

    // 请求随机数
    CHECK_OK(ICCRequestRandomNumber(RND_ICC));
//	LOGV("CHECK_OK( ICCRequestRandomNumber(RND_ICC) );\n");

    CHECK_OK(BuildIFD(RND_IFD, Kifd));
//	LOGV("CHECK_OK(BuildIFD(RND_IFD, Kifd));\n");

    // 发起MUTUAL认证
    CHECK_OK(ICCMutualAuthenticate(RND_IFD, RND_ICC, Kifd, Kenc, Kmac, KSenc, KSmac));
//	LOGV("CHECK_OK(ICCMutualAuthenticate(RND_IFD, RND_ICC, Kifd, Kenc, Kmac, KSenc, KSmac));\n");


    CHECK_OK(BuildSSC(RND_IFD, RND_ICC, SSC));
    //LOGV("CHECK_OK(BuildSSC(RND_IFD, RND_ICC, SSC));\n");

    // 准备好 KSenc KSmac SSC 后，开始进行安全通信
    std::string EF_COM_Data;
    // 读取EF.COM文件，并检查该护照存在哪些文件
    CHECK_OK(ICCReadEF(KSenc, KSmac, SSC, EF_COM, EF_COM_Data));
    std::string tags = EF_COM_TagsParse(EF_COM_Data);

    //EFFileSystem efs;
    STEFFile *stFile = NULL;
    // 读取其他文件
    for (size_t i = 0; i < tags.size(); i++) {
        unsigned char b = tags[i];
        stFile = StTagFindEFFile(b, &st_efs);
        if (NULL == stFile) {
            LOGV("error!\n");
            continue;
        }

        std::string ef_data;
        if (stFile->Index == EF_COM) {
            continue;
        }

        // 如果该类型EF解析器未实现则不读该文件
        SelectFunc(stFile);
        if (!stFile->Valid()) {
            continue;
        }
        // 读取文件
        CHECK_OK(ICCReadEF(KSenc, KSmac, SSC, stFile->Index, ef_data));
        // 解析文件
        if (stFile->FileParse(ef_data, &st_efs)) {

            ChipReaderReadFileResultOperate(stFile->Index, stFile->result, this->GetCardType());
            /*
                if (this->cb) {
                    this->cb->OnChipReaderReadFileResult(stFile->Index, stFile->result,this->GetCardType());
                }*/
        }
    }
    return true;
}

char CChipReader::AuthKeyFunc(std::string &cpKey){
    std::string RND_CMD("\x01",1);//请求随机数发送的指令
    std::string RND_NUM;//进行设备认证请求的随机数
    std::string cipherText;//将请求到的随机数进行加密后的数据
    std::string RAPUD;
    //请求随机数
    char ret = this->PostPassportCommand(RF_CMD_AUTH, RND_CMD,RND_NUM);
    if (!ret || RND_NUM.size() != 8) {
        LOGE("设备认证：请求随机数失败！");
        LOG(ERROR)<<"设备认证：请求随机数失败！"<<endl;
        return false;
    }
    unsigned char key[16] = {0x00};//用来保存用户密钥
    unsigned char rnd[8] = {0x00};//用来保存请求到的随机数和经过3DES加密后的随机数
    memcpy(key,cpKey.data(),cpKey.size());
    memcpy(rnd,RND_NUM.data(),RND_NUM.size());
    //std::string A("\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",16);
    //std::string B("\xE4\xBD\x14\x98\xE0\x3A\x87\xC3",8);
    //memcpy(key,A.data(),A.size());
    //memcpy(rnd,B.data(),B.size());
    //将请求到的随机数使用用户密钥进行3DES加密
    ret = Crypto_3DES_ECB(CRYPTO_ENCRYPT,key, RND_NUM.size(), rnd);
    if(ret != 0){
        LOGE("设备认证：随机数加密失败！");
        LOG(ERROR)<<"设备认证：随机数加密失败！"<<endl;
        return false;
    }
    //进行外部认证，将加密的随机数发送给设备
    std::string request("\x02",1);//进行外部认证需要的参数
    char* temp = (char*)rnd;
    cipherText.assign(temp,8);
    request.append(cipherText);//将加密后的随机数拼接到参数后面
    ret = this->PostPassportCommand(RF_CMD_AUTH, request,RAPUD);
    if (!ret) {
        LOGE("设备认证：认证失败！");
        LOG(ERROR)<<"设备认证：认证失败！"<<endl;
        return false;
    }
    LOGI("设备认证成功！");
    LOG(INFO)<<"设备认证成功！"<<endl;
    return true;
}

char CChipReader::ChangeAuthKeyFunc(std::string &oldKey, std::string &newKey){
    std::string cipherText;
    std::string response;
    unsigned char oldKeys[16] = {0};
    unsigned char newKeys[16] = {0};
    memcpy(oldKeys,oldKey.data(),oldKey.size());
    memcpy(newKeys,newKey.data(),newKey.size());
    //使用原密钥将新密钥进行3DES加密
    char ret = Crypto_3DES_ECB(CRYPTO_ENCRYPT,oldKeys, 16, newKeys);
    if(ret != 0)
        return false;
    //发送修改密钥的指令
    std::string request("\x10",1);
    char* temp = (char*)newKeys;
    cipherText.assign(temp,16);
    request.append(cipherText);
    ret = this->PostPassportCommand(RF_CMD_AUTH,request,response);
    if(!ret){
        LOGE("修改用户密钥失败！");
        LOG(ERROR)<<"修改用户密钥失败！"<<endl;
        return false;
    }
    LOGI("修改用户密钥成功！");
    LOG(INFO)<<"修改用户密钥成功！"<<endl;
    return true;
}

char CChipReader::ReadDevNum(unsigned char* curDevNum){
    std::string request("\x00",1);//读取设备自定义编号的命令
    std::string response;
    char ret = this->PostPassportCommand(RF_CMD_RW_DEV_NUM, request, response);
    if(!ret){
        LOGE("读取设备自定义编号失败！");
        LOG(ERROR)<<"读取设备自定义编号失败！"<<endl;
        return false;
    }
    memcpy(curDevNum, response.data(),response.size());
    LOGI("读取设备自定义编号成功!");
    LOG(INFO)<<"读取设备自定义编号成功!"<<endl;
    return true;
}

char CChipReader::WriteDevNum(unsigned char* newDevNum){
    std::string request("\x01",1);//写入设备自定义编号的命令
    std::string response;
    char* temp = (char*)newDevNum;
    std::string cmd;
    cmd.assign(temp, 8);
    request.append(cmd);
    char ret = this->PostPassportCommand(RF_CMD_RW_DEV_NUM, request, response);
    if(!ret){
        LOGE("写入设备自定义编号失败！");
        LOG(ERROR)<<"写入设备自定义编号失败！"<<endl;
        return false;
    }
    LOGI("写入设备自定义编号成功!");
    LOG(INFO)<<"写入设备自定义编号成功!"<<endl;
    return true;
}

char CChipReader::CloseWire(){
    std::string request("\x00",1);//写入设备自定义编号的命令
    std::string response;
    char ret = this->PostPassportCommand(RF_CMD_CLOSE_WIRE, request, response);
    if(!ret){
        LOGE("关闭射频天线失败！");
        LOG(ERROR)<<"关闭射频天线失败！"<<endl;
        return false;
    }
    LOGI("关闭射频天线成功!");
    LOG(INFO)<<"关闭射频天线成功!"<<endl;
    return true;
}

void CChipReader::ChipReaderReadFileResultOperate(EF_NAME name, char *result, unsigned char type) {
    switch (name) {
        case EF_COM :
            break;
        case EF_DG1 : {
            //MyNotifyShowMRZ(type, result, baseFolder);
            LOGV("DG1: %s", result);
            break;
        }

        case EF_DG2 : {
            LOGV("护照头像保存到%s文件中", result);
            break;
        }
        default:
            break;
    }
}

static int
utf16lt_to_wchar_t(const unsigned char *a_szSrc, int a_nSize, wchar_t *a_szDest, int a_nDestSize) {

    memcpy(a_szDest, (const wchar_t *) a_szSrc, a_nSize);
    return a_nSize;
}

char CChipReader::ReadIDInfo(void) {
    unsigned char *sInfo;
    unsigned char *cpPhoto;
    unsigned int ipLength;
    unsigned char *cpFinger;
    unsigned int ipFLength;

    ipLength = 1024;
    ipFLength = 1024;
    cpPhoto = (unsigned char *) malloc(ipLength);
    //cpFinger = (unsigned char *)malloc(ipFLength);
    sInfo = (unsigned char *) malloc(ipFLength);

    int count = 0;
    unsigned int ret = RF_ERR_FAILURE;
    while (ret != RF_ERR_SUCCESS && count < 3) {
        ret = echipReader.RF_IDCard_ReadInfo(sInfo, cpPhoto, &ipLength);
        echipReader.Close();
        count++;
    }
    if (ret != RF_ERR_SUCCESS) {
        LOGV("RF_IDCard_ReadInfo error");
        free(sInfo);
        free(cpPhoto);
        return false;
    }

    std::string sInfoString;
    sInfoString.resize(sizeof(TIDCardInfo));
    for (int i = 0; i < sizeof(TIDCardInfo); i++) {
        sInfoString[i] = sInfo[i];
    }

    std::string cpPhotoString;
    cpPhotoString.resize(1024 + 1);
    for (int i = 0; i < ipLength; i++) {
        cpPhotoString[i] = cpPhoto[i];
    }

    SelectFunc(&st_efs.stEFFiles[EF_IDINFO]);
    st_efs.stEFFiles[EF_IDINFO].FileParse(sInfoString, &st_efs);
    SelectFunc(&st_efs.stEFFiles[EF_IDPIC]);
    st_efs.stEFFiles[EF_IDPIC].FileParse(cpPhotoString, &st_efs);

    free(sInfo);
//	free(cpFinger);
    free(cpPhoto);
    return true;
}
char CChipReader::ReadIDInfo(unsigned char *sInfo, unsigned int * sLength, unsigned char *cpPhoto, unsigned int *ipLength, unsigned char *cpFinger, unsigned int *ipFLength){
    unsigned int ret = RF_ERR_FAILURE;
    int count = 0;
    while (ret != RF_ERR_SUCCESS && count < 3) {
        ret = echipReader.RF_IDCard_ReadFinger(sInfo, sLength, cpPhoto, ipLength, cpFinger, ipFLength);;
        echipReader.Close();
        count++;
    }
    if (ret != RF_ERR_SUCCESS) {
        LOGV("RF_IDCard_ReadInfo error");
        return false;
    }
    return true;
}

char CChipReader::ReadInfo(std::string &code) {
    char ret;

    mrtd.CardType = this->GetCardType();
    if (!mrtd.Parse(code)) {
        LOGE("mrtd.Parse(code) error");
    }

#if USE_LOG_LEVEL1
    LOGV("CodeToNFC = " << std::endl << mrtd.mrzInfo.codetonfc);
#endif

    std::string codetonfc = mrtd.mrzInfo.codetonfc;
    LOGV("codetonfc = %s", codetonfc.c_str());
    try {
        ret = this->ReadEchipInfo(codetonfc);
    } catch (std::exception &e) {
#if USE_LOG_LEVEL1
        LOGV("致命错误: " << e.what());
#endif
        return false;
    }
    if (ret == true) {
        LOGV("ReadInfo: ret == true\n");
    } else {
        LOGV("ReadInfo: ret == false\n");
    }
    return ret;
}


/**
	* 新增函数 PassportCardTest
	* 返回值：0——身份证，1——护照/通行证 ，-1——无芯片
	* 复位，发出复位请求，在返回值中有卡类型信息
	* 复位成功后发送签发方应用
*/
int  CChipReader::PassportCardTest(void){
	unsigned  char cpInfo[300];
	unsigned  char cpLen;
	//unsigned  char cpType;

	// if (!this->IsOpen()) {
	// 	LOGE("Device is not Open ");
	// 	return -2;
	// }
	memset(cpInfo, 0, sizeof(cpInfo));
	int retInt = -1;
	retInt = echipReader.RF_14443_Reset(0xFF, cpInfo, &cpLen); //软复位
	LOGV("RF_14443_Reset Return: %d", retInt);
	//LOGV("cpType: " << (cpType & 0xff));

	if (retInt != RF_ERR_SUCCESS) { //复位失败，返回-1
		return -1;
	}

	/// 复位成功，选择签发方应用
	std::string selectAppCmd("\x00\xA4\x04\x0C\x07\xA0\x00\x00\x02\x47\x10\x01", 12);
	std::string selectAppRAPDU;
	char ret = false;
	LOGV("PostPassportCommand start: *** ");
	
	ret = this->PostPassportCommand(RF_CMD_APP, selectAppCmd, selectAppRAPDU);//发送命令
	LOGV("PostPassportCommand ret = %d", ret );
	
	if (!ret) { //命令返回值为false，则为身份证
		return 0;
	}
	return 1;
}

/**
 
 */
char CChipReader::PostPassportCommand(
        unsigned char cmd,
        const std::string &request,
        std::string &response) {
    int ret = RF_ERR_FAILURE;
    int count = 0;
    unsigned char cpAPDU[300];
    unsigned int ipLen = request.size();

    memcpy(cpAPDU, request.data(), ipLen);
    while (ret == RF_ERR_FAILURE && count < 10) {
        ret = echipReader.RF_14443_Apdu_With_Cmd(cmd,cpAPDU, &ipLen);
        echipReader.Close();
        count++;
        if (count > 1)
                LOGV("ret == RF_ERR_FAILURE");
    }

    if (RF_ERR_SUCCESS == ret) {
        std::string s((char *) &cpAPDU[0], ipLen);
        response.append(s);
        if(cmd == RF_CMD_AUTH || cmd ==RF_CMD_RW_DEV_NUM || cmd == RF_CMD_CLOSE_WIRE){
            //如果是认证命令，就不需要判断是否有成功标志位，可以直接返回true
            return true;
        }
        //智能卡数据交互命令需要判断成功标志位
        char *sw = (char *) response.data() + response.size() - 2;//查看最后2个字节
        if (memcmp(sw, SW_SUCCESS, 2) != 0) {
            const char *sw = response.data();
            const short *ssw = (short *) sw;
            LOGE("Post failed, SW=0x %04X",  HostToNetworkUINT16(*ssw));
            LOGE("未找到成功标志");
            return false;
        }
        response.erase(response.size() - 2, 2);
        return true;
    }

    return false;
}

char CChipReader::BuildKencAndKmac(
        const std::string &mrzInfo,
        std::string &Kenc,
        std::string &Kmac) {
    std::string Kseed(16, 0);
    std::string mrzInfoSha1(20, 0);

    SHA1((unsigned char *) mrzInfo.data(), mrzInfo.size(), (unsigned char *) mrzInfoSha1.data());
#if USE_LOG_LEVEL1
    LOGV("H_SHA1=" << BinaryToHexString(mrzInfoSha1));
#endif
    memcpy((unsigned char *) Kseed.data(), mrzInfoSha1.data(), Kseed.size());
#if USE_LOG_LEVEL1
    LOGV("K_SEED=" << BinaryToHexString(Kseed));
#endif
    // SHA1 to Kenc Kmac
    std::string c1("\x00\x00\x00\x01", 4);
    std::string c2("\x00\x00\x00\x02", 4);
    std::string D1, D2, HD1(20, 0), HD2(20, 0);

    // Kseed concat c1,c2 into D1, D2
    D1.append(Kseed.data(), Kseed.size());
    D1.append(c1.data(), c1.size());
    D2.append(Kseed.data(), Kseed.size());
    D2.append(c2.data(), c2.size());


    // SHA D1 and D2
    SHA1((unsigned char *) D1.data(), D1.size(), (unsigned char *) HD1.data());
    SHA1((unsigned char *) D2.data(), D2.size(), (unsigned char *) HD2.data());

    CheckParity(HD1, Kenc, 16);
    CheckParity(HD2, Kmac, 16);
#if USE_LOG_LEVEL1
    LOGV("Kenc=" << BinaryToHexString(Kenc));
    LOGV("Kmac=" << BinaryToHexString(Kmac));
#endif
    return true;
}

char CChipReader::ICCSelectApp() {
    std::string selectAppCmd("\x00\xA4\x04\x0C\x07\xA0\x00\x00\x02\x47\x10\x01", 12);
    std::string selectAppRAPDU;

#if USE_LOG_LEVEL1
    LOGV("正在选择签发应用...");
#endif
    char ret = this->PostPassportCommand(RF_CMD_APP,selectAppCmd, selectAppRAPDU);
    if (!ret) {
        LOGE("选择签发方应用失败");
        LOG(ERROR)<<"选择签发方应用失败"<<endl;
        return false;
    }

#if USE_LOG_LEVEL1
    LOGV("选择签发应用方成功");
#endif
    return true;
}

char CChipReader::ICCRequestRandomNumber(std::string &RND_ICC) {
    std::string randCmd("\x00\x84\x00\x00\x08", 5);
    int respLen = 0;

#if USE_LOG_LEVEL1
    LOGV("正在获取随机数...");
#endif
    char ret = this->PostPassportCommand(RF_CMD_APP, randCmd, RND_ICC);
    if (!ret || RND_ICC.size() != 8) {
        LOGE("获取随机数失败");
        LOG(ERROR)<<"获取随机数失败"<<endl;
        return false;
    }
#if USE_LOG_LEVEL1
    LOGV("获取随机数成功, RND_ICC=" << BinaryToHexString(RND_ICC));
#endif
    return true;
}

char CChipReader::ICCMutualAuthenticate(
        std::string &RND_IFD,
        std::string &RND_ICC,
        std::string &Kifd,
        std::string &Kenc,
        std::string &Kmac,
        std::string &KSenc,
        std::string &KSmac) {
    char ret = false;

    // S = concat(RND_IFD, RND_ICC, Kifd)
    std::string S;
    S.append(RND_IFD);
    S.append(RND_ICC);
    S.append(Kifd);
#if USE_LOG_LEVEL1
    LOGV("S=" << BinaryToHexString(S));
#endif
    if (S.size() != 32) {
        return false;
    }

    // 用密钥Kenc对S进行CBC模式的3DES（TDES）加密
    std::string Eifd;
    KencTDES(S, Kenc, Eifd, DES_ENCRYPT);
#if USE_LOG_LEVEL1
    LOGV("Eifd= %s", BinaryToHexString(Eifd));
#endif
    if (Eifd.size() != 32) {
        return false;
    }

    // 用密钥Kmac计算Eifd(填充)的MAC
    std::string Mifd;
    DesAddPaddingBytes(Eifd);

    if (Eifd.size() != 40) {
        return false;
    }

    KmacDES(Eifd, Kmac, Mifd);

#if USE_LOG_LEVEL1
    LOGV("Mifd= %s", BinaryToHexString(Mifd));
#endif

    if (Mifd.size() != 8) {
        return false;
    }

    // 构建MUTUAL AUTHENTICATE命令数据
    std::string cmd;
    cmd.append(Eifd.data(), 32);
    cmd.append(Mifd.data(), 8);
    // 构建MUTUAL AUTHENTICATE命令
    std::string catHeader("\x00\x82\x00\x00\x28", 5);
    std::string APDU;
    APDU.append(catHeader);
    APDU.append(cmd);
    /*
    这里应该多加一个字节Le值，中国护照可以不需要，但是国外的护照如果不加
    这个字节，调用会失败
    */
    APDU.push_back(0x28);

    // 将命令 APDU发送给机读旅行证件芯片
    std::string RAPDU;

#if USE_LOG_LEVEL1
    LOGV("APDU= %s", BinaryToHexString(APDU));
#endif

#if USE_LOG_LEVEL1
    LOGV("正在请求MUTUAL AUTHENTICATE命令...");
#endif
    ret = this->PostPassportCommand(RF_CMD_APP, APDU, RAPDU);
    if (!ret || RAPDU.size() != 40) {
#if USE_LOG_LEVEL1
        LOGV ("MUTUAL AUTHENTICATE命令执行失败，ret= %s,RAPDU size is  %s", ret, APDU.size());
#endif
        LOGE("MUTUAL AUTHENTICATE命令执行失败");
        LOG(ERROR)<<"MUTUAL AUTHENTICATE命令执行失败"<<endl;
        return false;
    }
#if USE_LOG_LEVEL1
    LOGV("MUTUAL AUTHENTICATE命令执行成功");
#endif

    std::string Eicc, Micc;
    Eicc.append(RAPDU.data(), 32);
    Micc.append(RAPDU.data() + 32, 8);

    // 用密钥Kenc对Eicc进行3DES解密得到R
    std::string R;
    KencTDES(Eicc, Kenc, R, DES_DECRYPT);

    std::string RNDifdr(R.data() + 8, 8);
    // R由RNDiCCN(已知)、RNDifd（已知）、KiCCN(未知）组成，KiCCN为后16个字节
    if (memcmp(RND_IFD.data(), RNDifdr.data(), RNDifdr.size()) != 0) {
        // 接收到的RNDifdr和生成的RNDifd不一致
#if USE_LOG_LEVEL1
        LOGE("接收到的RNDifdr和生成的RNDifd不一致");
#endif
        return false;
    }
#if USE_LOG_LEVEL1
    LOGV("RNDifdr 校验成功");
#endif
    std::string Kicc(R.data() + 16, 16);
    // Kifd与Kicc按位异或得到SKseed
    std::string SKseed(16, 0);
    for (size_t i = 0; i < SKseed.size(); i++) {
        SKseed[i] = Kifd[i] ^ Kicc[i];
    }

    // 用SKseed分别并置c1与c2求SHA1取高位16字节，再调整奇偶校验位得到SKenc和SKmac
    KSenc.resize(16, 0);
    KSmac.resize(16, 0);
    SHA1ToKencKmac(SKseed, KSenc, KSmac);

#if USE_LOG_LEVEL1
    LOGV("KSenc= %s", BinaryToHexString(KSenc));
    LOGV("KSmac= %s", BinaryToHexString(KSmac));
#endif

    return true;
}

char CChipReader::BuildIFD(
        std::string &RND_IFD,
        std::string &Kifd) {
    RND_IFD.resize(8);
    Kifd.resize(16);
    BuildRandomData(RND_IFD);
    BuildRandomData(Kifd);

    if (RND_IFD.size() != 8 || Kifd.size() != 16) {
        return false;
    }
    return true;
}

char CChipReader::BuildSSC(
        std::string &RND_IFD,
        std::string &RND_ICC,
        std::string &SSC) {
    // SSC为RNDicc的低4个字节与RNDifd的低4个字节的并置
    SSC.resize(0);
    SSC.append(RND_ICC.data() + 4, 4);
    SSC.append(RND_IFD.data() + 4, 4);

#if USE_LOG_LEVEL1
    LOGV("SSC= %s", BinaryToHexString(SSC));
#endif
    return true;
}

char CChipReader::ICCReadEF(std::string &KSenc, std::string &KSmac, std::string &SSC, EF_NAME name,
                            std::string &EF_Data) {
    //EFFile &file = m_efs[name];
    STEFFile *stFile = StIndexFindEFFile(name, &st_efs);

#if OOXX_DEBUG_LOG
    LOGV("正在读取  %s 文件...", stFile->name);
#endif

    char ret = this->SecureCommunication(stFile, KSenc, KSmac, SSC, EF_Data);
    if (false == ret || EF_Data.size() == 0) {
#if USE_LOG_LEVEL1
        LOGE("读取  %s 文件失败,  ret =  %d, 文件大小 %d 字节", stFile->name, ret, EF_Data.size());
#endif
        return false;
    }

#if OOXX_DEBUG_LOG
    LOGV("读取  %s 文件成功,  ret =  %d, 文件大小 %d 字节", stFile->name, ret, EF_Data.size());
#endif
    return true;

}

char CChipReader::SecureCommunication(
        STEFFile *file,
        const std::string &KSenc,
        const std::string &KSmac,
        std::string &SSC,
        std::string &data) {
    char ret = false;

    // 1. 选择要读取的文件
#if USE_LOG_LEVEL2
    LOGV("[ChipReader]正在选择文件 %s", file->name);
#endif

    ret = this->SecureCommunicationSelectFile(file, KSenc, KSmac, SSC);
    if (!ret) {
#if USE_LOG_LEVEL2
        LOGV("[ChipReader]选择文件 %s 失败", file->name);
#endif
        return false;
    }
#if USE_LOG_LEVEL2
    LOGV("[ChipReader]选择文件 %s 成功", file->name);
#endif
    // 2. 读取该文件的前4个字节
    std::string head4bytes;
#if USE_LOG_LEVEL2
    LOGV("[ChipReader]正在读取文件 %s 的前4个字节", file->name);
#endif
    ret = this->SecureCommunicationReadBinary(KSenc, KSmac, SSC, 0, 4, head4bytes);
    if (!ret) {
#if USE_LOG_LEVEL2
        LOGV("[ChipReader]读取失败");
#endif
        return false;
    }
#if USE_LOG_LEVEL2
        LOGV("[ChipReader]读取成功");
#endif
    /*
      读文件有两种方式
      (1)要先读头4字节获取文件大小，再循环读后续数据
      (2)要先读头4字节获取文件大小，再从偏移0开始读数据，数据最大230字节
      本方法的实现采用第2种方法
    */
    // 3. 解析EF文件头4字节, 获取文件总大小(包括上面读的4个字节)
    unsigned short len = 0;
    ret = ParseFileLengthIn4Bytes(head4bytes, &len);
    if (!ret) {
#if USE_LOG_LEVEL2
        LOGV("[ChipReader]解析文件头4个字节失败");
#endif
        return false;
    }
#if USE_LOG_LEVEL2
    LOGV("[ChipReader]解析文件头4个字节成功，文件长度 %d 字节", len);
#endif
#if 0
    // 4. 计算剩余的未读文件长度
    len -= 4;
    data.append(head4bytes);
#endif

    // 5. 读取剩余的数据
    if (len > 0) {
        std::string rest;
#if USE_LOG_LEVEL2
        LOGV("[ChipReader]准备请求文件 %s 内容", file->name);
#endif
        ret = this->SecureCommunicationReadFile(KSenc, KSmac, SSC, 0, len, rest);
        if (!ret) {
            return false;
        }
        data.append(rest);
    } else if (len < 0) {
        // 非法的文件长度
        //throw std::exception("invalid EF file length");
        LOGV("SecureCommunication: invalid EF file length\n");
        return false;
    }
#if USE_LOG_LEVEL2
    LOGV("[ChipReader]文件 %s 读取完毕", file->name);
#endif
    return true;
}

char CChipReader::SecureCommunication(
        EFFile &file,
        const std::string &KSenc,
        const std::string &KSmac,
        std::string &SSC,
        std::string &data) {
    char ret = false;

    // 1. 选择要读取的文件
#if USE_LOG_LEVEL2
    LOGV("[ChipReader]正在选择文件 %s", file.name);
#endif
    ret = this->SecureCommunicationSelectFile(file, KSenc, KSmac, SSC);
    if (!ret) {
#if USE_LOG_LEVEL2
        LOGV("[ChipReader]选择文件 %s 失败", file.name);
#endif
        return false;
    }
#if USE_LOG_LEVEL2
    LOGV("[ChipReader]选择文件 %s 成功", file.name);
#endif
    // 2. 读取该文件的前4个字节
    std::string head4bytes;
#if USE_LOG_LEVEL2
    LOGV("[ChipReader]正在读取文件 %s 的前4个字节", file.name);
#endif
    ret = this->SecureCommunicationReadBinary(KSenc, KSmac, SSC, 0, 4, head4bytes);
    if (!ret) {
#if USE_LOG_LEVEL2
        LOGV("[ChipReader]读取失败");
#endif
        return false;
    }
#if USE_LOG_LEVEL2
        LOGV("[ChipReader]读取成功， 4个字节内容为 %s", BinaryToHexString(head4bytes));
#endif
    /*
      读文件有两种方式
      (1)要先读头4字节获取文件大小，再循环读后续数据
      (2)要先读头4字节获取文件大小，再从偏移0开始读数据，数据最大230字节
      本方法的实现采用第2种方法
    */
    // 3. 解析EF文件头4字节, 获取文件总大小(包括上面读的4个字节)
    unsigned short len = 0;
    ret = ParseFileLengthIn4Bytes(head4bytes, &len);
    if (!ret) {
#if USE_LOG_LEVEL2
        LOGV("[ChipReader]解析文件头4个字节失败");
#endif
        return false;
    }
#if USE_LOG_LEVEL2
    LOGV("[ChipReader]解析文件头4个字节成功，文件长度 %s 字节");
#endif
#if 0
    // 4. 计算剩余的未读文件长度
    len -= 4;
    data.append(head4bytes);
#endif
    // 5. 读取剩余的数据
    if (len > 0) {
        std::string rest;
#if USE_LOG_LEVEL2
        LOGV("[ChipReader]准备请求文件 %s 内容", file.name);
#endif
        ret = this->SecureCommunicationReadFile(KSenc, KSmac, SSC, 0, len, rest);
        if (!ret) {
            return false;
        }
        data.append(rest);
    } else if (len < 0) {
        // 非法的文件长度
        //throw std::exception("invalid EF file length");
        LOGV("SecureCommunication:invalid EF file length\n");
    }
    return true;
}

char CChipReader::SecureCommunicationSelectFile(
        STEFFile *file,
        const std::string &KSenc,
        const std::string &KSmac,
        std::string &SSC) {
    // 对未受保护的APDU命令进行填充
    std::string unprotectedAPDU("\x0C\xA4\x02\x0C", 4);
    std::string CmdHeader(unprotectedAPDU);
    DesAddPaddingBytes(CmdHeader);
    std::string strFillData;
    //if(strlen(file->Id) >= 2){
    //LOGV("file.Id.size() ==  %s", file.Id.size());
    //	LOGV("fild->Id:  %s", file->Id);
    //	LOGV("file->Id[0] == %02x;file->Id[1] == %02x\n",file->Id[0],file->Id[1]);
    strFillData.push_back(file->Id[0]);
    strFillData.push_back(file->Id[1]);
    /*}else{

        LOGV("file.Id.size() ==  %s", strlen(file->Id));
        LOGV("fild.Id:  %s", file->Id);
        return  false;
    }
*/

    DesAddPaddingBytes(strFillData);
    // 用SKenc加密数据
    std::string strEncData;
    KencTDES(strFillData, KSenc, strEncData, DES_ENCRYPT);

    // 构建DO87,并置cmdheader和DO87得到M
    std::string DO87;
    unsigned char L = (unsigned char) strFillData.size() + 1;
    unsigned char x87 = 0x87;
    DO87.push_back(x87);
    DO87.push_back(L);
    DO87.push_back(0x01);
    DO87.append(strEncData);
    std::string M = CmdHeader + DO87;

    IncreaseSSC(SSC); //SSC += 1

    // 连接SSC和M，并增加填充得到N
    std::string N;
    N.append(SSC.data(), SSC.size());
    N.append(M.data(), M.size());
    DesAddPaddingBytes(N);

    // 用SKmac计算N的消息认证码MAC
    std::string CCN;
    KmacDES(N, KSmac, CCN);

#if USE_LOG_LEVEL1
    LOGV("CC_N= %s", BinaryToHexString(CCN));
#endif
    // 用CCN构建DO8E
    std::string DO8E("\x8E\x08", 2);
    DO8E.append(CCN.data(), CCN.size());

    // 构建受保护的APDU
    std::string APDU;
    std::string unprotectedAPDU2("\x0C\xA4\x02\x0C", 4);
    APDU.append(unprotectedAPDU2.data(), unprotectedAPDU2.size());
    APDU.append("\x15", 1);
    APDU.append(DO87.data(), DO87.size());
    APDU.append(DO8E.data(), DO8E.size());
    /*
     这里应该多加一个字节，中国护照可以不需要，但是国外的护照如果不加
     这个字节，调用会失败
     */
    APDU.push_back(0);
    // 发送APDU
    int dwLen = APDU.size();
    std::string RAPDU;
    char ret = this->PostPassportCommand(RF_CMD_APP,APDU, RAPDU);
    if (!ret) {
#if USE_LOG_LEVEL1
        LOGV("Select File failed.");
#endif
        return false;
    }
    IncreaseSSC(SSC); //SSC += 1
    // 连接SSC和DO99， 并填充得到K
    std::string DO99 = RAPDUParse(RAPDU, 0x99);
    std::string K(SSC);
    K += DO99;
    DesAddPaddingBytes(K);
    // 用SKmac计算K的消息认证码MAC
    std::string CCK;
    KmacDES(K, KSmac, CCK);
    // 从RAPDU中提取出DO8Er，验证是否等于CCK
    std::string RAPDU_DO8E = RAPDUParse(RAPDU, 0x8E);
    if (memcmp(RAPDU_DO8E.data() + 2, CCK.data(), 8) != 0) {
#if USE_LOG_LEVEL1
        LOGV("DO8E and CCK mismatch in Selecting file");
#endif
        return false;
    }
    return true;
}

char CChipReader::SecureCommunicationSelectFile(
        EFFile &file,
        const std::string &KSenc,
        const std::string &KSmac,
        std::string &SSC) {
    // 对未受保护的APDU命令进行填充
    std::string unprotectedAPDU("\x0C\xA4\x02\x0C", 4);
    std::string CmdHeader(unprotectedAPDU);
    DesAddPaddingBytes(CmdHeader);
    std::string strFillData;
    if (strlen(file.Id) >= 2) {
        //LOGV("file.Id.size() ==  %s", file.Id.size());
        //LOGV("fild.Id:  %s", file.Id);
        strFillData.push_back(file.Id[0]);
        strFillData.push_back(file.Id[1]);
    } else {

        LOGV("file.Id.size() ==  %s", strlen(file.Id));
        LOGV("fild.Id:  %s", file.Id);
        return false;
    }

    DesAddPaddingBytes(strFillData);
    // 用SKenc加密数据
    std::string strEncData;
    KencTDES(strFillData, KSenc, strEncData, DES_ENCRYPT);

    // 构建DO87,并置cmdheader和DO87得到M
    std::string DO87;
    unsigned char L = (unsigned char) strFillData.size() + 1;
    unsigned char x87 = 0x87;
    DO87.push_back(x87);
    DO87.push_back(L);
    DO87.push_back(0x01);
    DO87.append(strEncData);
    std::string M = CmdHeader + DO87;

    IncreaseSSC(SSC); //SSC += 1

    // 连接SSC和M，并增加填充得到N
    std::string N;
    N.append(SSC.data(), SSC.size());
    N.append(M.data(), M.size());
    DesAddPaddingBytes(N);

    // 用SKmac计算N的消息认证码MAC
    std::string CCN;
    KmacDES(N, KSmac, CCN);

#if USE_LOG_LEVEL1
    LOGV("CC_N= %s", BinaryToHexString(CCN));
#endif
    // 用CCN构建DO8E
    std::string DO8E("\x8E\x08", 2);
    DO8E.append(CCN.data(), CCN.size());

    // 构建受保护的APDU
    std::string APDU;
    std::string unprotectedAPDU2("\x0C\xA4\x02\x0C", 4);
    APDU.append(unprotectedAPDU2.data(), unprotectedAPDU2.size());
    APDU.append("\x15", 1);
    APDU.append(DO87.data(), DO87.size());
    APDU.append(DO8E.data(), DO8E.size());
    /*
     这里应该多加一个字节，中国护照可以不需要，但是国外的护照如果不加
     这个字节，调用会失败
     */
    APDU.push_back(0);
    // 发送APDU
    int dwLen = APDU.size();
    std::string RAPDU;
    char ret = this->PostPassportCommand(RF_CMD_APP,APDU, RAPDU);
    if (!ret) {
#if USE_LOG_LEVEL1
        LOGV("Select File failed.");
#endif
        return false;
    }
    IncreaseSSC(SSC); //SSC += 1
    // 连接SSC和DO99， 并填充得到K
    std::string DO99 = RAPDUParse(RAPDU, 0x99);
    std::string K(SSC);
    K += DO99;
    DesAddPaddingBytes(K);
    // 用SKmac计算K的消息认证码MAC
    std::string CCK;
    KmacDES(K, KSmac, CCK);
    // 从RAPDU中提取出DO8Er，验证是否等于CCK
    std::string RAPDU_DO8E = RAPDUParse(RAPDU, 0x8E);
    if (memcmp(RAPDU_DO8E.data() + 2, CCK.data(), 8) != 0) {
#if USE_LOG_LEVEL1
        LOGV("DO8E and CCK mismatch in Selecting file");
#endif
        return false;
    }
    return true;
}

char CChipReader::SecureCommunicationReadFile(
        const std::string &KSenc,
        const std::string &KSmac,
        std::string &SSC,
        unsigned short offset,
        unsigned short len,
        std::string &data) {
    const int chunkSize = 230;

    //assert(chunkSize >0 && chunkSize <= 230);

    int lastBytes = len % chunkSize;
    int chunks = len / chunkSize;

#if USE_LOG_LEVEL2
    int requestOffset = 1;
    LOGV("[ChipReader]开始读取文件全部内容，需要 %s", chunks + 1 << "次请求");
#endif

    for (int i = 0; i < chunks; i++) {
        std::string chunkData;

        //std::cout << ".";
#if USE_LOG_LEVEL2
        std::cout << "[ChipReader]正在读取第 %s", requestOffset << "个包，偏移为"
            << offset << "开始的 %s", chunkSize << "个字节 %s", std::endl;
#endif
        char ret = this->SecureCommunicationReadBinary(KSenc, KSmac, SSC, offset, chunkSize,
                                                       chunkData);
        if (!ret) {
#if USE_LOG_LEVEL2
            LOGV("[ChipReader]第 %s", requestOffset << "个包读取失败");
#endif
            return false;
        }
#if USE_LOG_LEVEL2
        LOGV("[ChipReader]第 %s", requestOffset << "个包读取成功，实际读取 %s", chunkData.size() << "字节");
        ++requestOffset;
#endif
        data.append(chunkData);
        offset += (unsigned short) chunkData.size();
    }
    //std::cout << std::endl;

    if (lastBytes) {
        std::string chunkData;
#if USE_LOG_LEVEL2
        std::cout << "[ChipReader]正在读取第 %s", requestOffset << "个包，偏移为" <<
            offset << "开始的 %s", lastBytes << "个字节 %s", std::endl;
#endif
        char ret = this->SecureCommunicationReadBinary(KSenc, KSmac, SSC, offset, lastBytes,
                                                       chunkData);
        if (!ret) {
#if USE_LOG_LEVEL2
            LOGV("[ChipReader]第 %s", requestOffset << "个包读取失败");
#endif
            return false;
        }
#if USE_LOG_LEVEL2
        LOGV("[ChipReader]第 %s", requestOffset << "个包读取成功，实际读取 %s", chunkData.size() << "字节");
#endif
        data.append(chunkData);
    }

    return true;
}

char CChipReader::SecureCommunicationReadBinary(
        const std::string &KSenc,
        const std::string &KSmac,
        std::string &SSC,
        unsigned short offset,
        unsigned short chunkSize,
        std::string &data) {
    // 创建未受保护的命令APDU, P1, P2未指定
    std::string unprotectedAPDU("\x0C\xB0\x00\x00", 4);

    // a.1设置读取偏移, 设置P1, P2
    unsigned short *apdu_len = (unsigned short *) (unprotectedAPDU.data() + 2);
    *apdu_len =  HostToNetworkUINT16(offset);

    // a. 设置读取偏移， 创建并填充命令报头
    std::string CmdHeader(unprotectedAPDU);
    // a.2 创建并填充命令报头
    DesAddPaddingBytes(CmdHeader);

    // b. 建立DO97
    std::string DO97("\x97\x01", 2);
    DO97.push_back((unsigned char) chunkSize);

    // c. 并置CmdHeader和DO97为M
    std::string M(CmdHeader);
    M += DO97;

    // --------------------------------------------
    // d.计算M的MAC
    // d.1 用1为SSC增值
    IncreaseSSC(SSC);

    // d.2并置SSC和M，并增加填充
    std::string N(SSC);
    N.append(M);
    DesAddPaddingBytes(N);

    // d.3用KSmac计算N的MAC
    std::string CC;
    KmacDES(N, KSmac, CC);
    // ---------------------------------------------

    // e.建立DO8E
    std::string DO8E("\x8E\x08", 2);
    DO8E.append(CC);
    // f.建立保护的APDU
    std::string APDU(unprotectedAPDU);
    int size = DO97.size() + DO8E.size();
    APDU.push_back(size);
    APDU += DO97;
    APDU += DO8E;
    APDU.push_back(0);

    // g.接收机读旅行证件芯片的响应APDU
    std::string RAPDU;
    char ret = this->PostPassportCommand(RF_CMD_APP,APDU, RAPDU);
    if (!ret) {
#if USE_LOG_LEVEL1
        LOGV("Read Binary failed.");
#endif
        return false;
    }

#define DO99_COMPATIBLE 1
    // ------------------------------------------------------------
    // h.通过计算DO87和DO99并置的MAC, 验证RAPDU CC
    int tlLen = 0;
    std::string RAPDU_DO87 = RAPDUParse(RAPDU, 0x87, &tlLen);

    /* 有时候请求200字节的大包时，读写器返回的数据包里没有DO99 TLV标签，
       只有DO87和DO8E，目前看来DO99是固定的4个字节\x99\E02\x90\x00，这里
       直接使用4字节常量，而不去RAPDU中取DO99，以免失败。
       按规范Doc9303 Part1 Vol2.PDF之第IV节公钥基础设施IV-45的描述，DO99
       应该是必须强制存在的，这里提供兼容性处理
       */
#ifdef DO99_COMPATIBLE
    std::string RAPDU_DO99("\x99\x02\x90\x00", 4);
#else
    std::string RAPDU_DO99 = RAPDUParse(RAPDU, 0x99);
#endif
    std::string RAPDU_DO8E = RAPDUParse(RAPDU, 0x8E);

    // h.1 用1为SSC增值
    IncreaseSSC(SSC);

    // h.2 并置SSC, DO87和DO99，并增加填充
    std::string K(SSC);
    K += RAPDU_DO87;
    K += RAPDU_DO99;
    DesAddPaddingBytes(K);
    // h.3 用KSmac计算MAC
    std::string CC2;
    KmacDES(K, KSmac, CC2);
    // h.4 将CC与RAPDU的DO8E数据作比较
    if (memcmp(CC2.data(), RAPDU_DO8E.data() + 2, 8) != 0) {
#if USE_LOG_LEVEL1
        LOGV("DO8E and CC2 mismatch in Reading binary");
#endif
        return false;
    }
    // i. 用KSenc解密DO87数据
    std::string RAPDU_DO87_DATA = RAPDU_DO87.substr(tlLen + 1);
    KencTDES(RAPDU_DO87_DATA, KSenc, data, DES_DECRYPT);
    DesRemovePaddingBytes(data);

    return true;
}

#if 0

char CChipMRTD::Parse(std::string &code,unsigned char type){
    char  ret;
    switch (type) {
    case CARD_TYPE_ONE:
         ret = ParseOneCard(code);
        break;
    case CARD_TYPE_TWO:
        ret = ParseTwoCard(code);
        break;
    case CARD_TYPE_THREE:
        ret = ParseThreeCard(code);
        break;

    }
    return ret;
}
char CChipMRTD::ParseOneCard(std::string &code){
    size_t iter = code.find(" ");
        while (iter != std::string::npos) {
            code = code.erase(iter, 1);
            iter = code.find(" ");
        }
        if (code.size() < 90) {
            LOGV("code.size() == %d\n",code.size());
            return false;
        }

        std::string splitstr(code);

        char2int(splitstr, 3, 11);
        char2int(splitstr, 13, 19);
        char2int(splitstr, 21, 27);

        Country[0] = 'C';
        Country[1] = 'H';
        Country[2] = 'N';
        Issuer[0] = 'C';
        Issuer[1] = 'H';
        Issuer[2] = 'N';

        Type = splitstr[0];
        this->code = splitstr.c_str();

        for (int i = 2; i < 11; i++) {
            PassportNo[i - 2] = splitstr[i];
            codetonfc[i - 2] = splitstr[i];//10
        }
        codetonfc[9] = splitstr[11];//10

        for (int i = 21; i < 27; i++) {
            DateOfBirth[i - 21] = splitstr[i];
            codetonfc[i - 21 + 10] = splitstr[i];//
        }
        codetonfc[16] = splitstr[27];//7

        for (int i = 13; i < 19; i++) {
            ExpiryDate[i - 13] = splitstr[i];
            codetonfc[i + 17 - 13] = splitstr[i];//7
        }
        codetonfc[23] = splitstr[19];//7
        codetonfc[24] = '\0';

        //Last Name
        int countLN = 0;
        for (int i = 42; i< 90; i++) {
            if (splitstr[i] == '<') {
                break;
            }
            countLN++;
        }
        for (int i = 0; i< countLN; i++) {
            LastName[i] = splitstr[i + 42];
        }
        LastName[countLN] = '\0';

        //First Name
        int countFN = 0;
        for (int i = countLN + 2 + 42; i<90; i++) {
            if (splitstr[i] == '<') {
                break;
            }
            countFN++;
        }
        for (int i = 0; i<countFN; i++) {
            FirstName[i] = splitstr[countLN + 2 + 42 + i];
        }
        FirstName[countFN] = '\0';


        for (int i = countLN + 2 + 42 + countFN; i < 90; i++) {
            if (splitstr[i] == '<')
                continue;
            else {
                Gender = splitstr[i];
                break;
            }
        }


        return true;
}
char CChipMRTD::ParseTwoCard(std::string &code){
    std::string str = code.insert(44, 1, '\n');


    //清除前面的空格
    size_t iter = str.find(" ");
    while (iter != std::string::npos) {
        str = str.erase(iter, 1);
        iter = str.find(" ");
    }

    std::vector<std::string> splitstr;

    size_t location = str.find("\n");

    if (location < 44 || location == std::string::npos)
    {
        return false;
    }
    splitstr.push_back(std::string(str, 0, 44));//机读码有两行，每一行机读码是44个字符
    splitstr.push_back(std::string(str, location + 1, 44));

    if (splitstr[1].length() < 44)
    {
        return false;
    }


    //Line 1
    //Positions Length Characters Meaning
    //1         1      alpha      P, indicating a passport
    //2         1      alpha+<    Type (for countries that distinguish between different types of passports)
    //3–5       3      alpha+<    Issuing country or organization (ISO 3166-1 alpha-3 code with modifications)
    //6–44      39     alpha+<    Surname, followed by two filler characters, followed by given names. Given names are separated by single filler characters
    int2char(splitstr[0], 1, 44);


    //Line 2
    //Positions Length Characters  Meaning
    //1–9       9      alpha+num+< Passport number
    //10        1      numeric     Check digit over digits 1–9
    char2int(splitstr[1], 2, 10);
    //11–13     3      alpha+<     Nationality (ISO 3166-1 alpha-3 code with modifications)
    int2char(splitstr[1], 11, 13);
    //14–19     6      numeric     Date of birth (YYMMDD)
    //20        1      num         Check digit over digits 14–19
    char2int(splitstr[1], 14, 20);
    //21        1      alpha+<     Sex (M, F or < for male, female or unspecified)
    //22–27     6      numeric     Expiration date of passport (YYMMDD)
    //28        1      numeric     Check digit over digits 22–27
    char2int(splitstr[1], 22, 28);
    //29–42     14     alpha+num+< Personal number (may be used by the issuing country as it desires)
    //int2char(splitstr[1], 29, 42);
    //43        1      numeric+<   Check digit over digits 29–42 (may be < if all characters are <)
    //44        1      numeric     Check digit over digits 1–10, 14–20, and 22–43
    char2int(splitstr[1], 43, 44);

    str = splitstr[0] + std::string("\r\n") + splitstr[1];

    code = str.c_str();

    Type = str[0];

    Country[0] = str[2];
    Country[1] = str[3];
    Country[2] = str[4];
    //Last Name
    int countLN = 0;
    for (int i = 5; i<10 + 5; i++) {
        if (str[i] == '<') {
            break;
        }
        countLN++;
    }
    for (int i = 0; i<countLN; i++) {
        LastName[i] = str[i + 5];
    }
    LastName[countLN] = '\0';

    //First Name
    int countFN = 0;
    for (int i = 5 + countLN + 2; i<5 + countLN + 2 + 10; i++) {
        if (str[i] == '<') {
            break;
        }
        countFN++;
    }
    for (int i = 0; i<countFN; i++) {
        FirstName[i] = str[5 + countLN + 2 + i];
    }
    FirstName[countFN] = '\0';

    for (size_t i = 0; i<str.length(); i++) {
        if (i<(str.length() - 5) && str[i] == '\n') {
            //护照号码提取
            for (int j = 0; j<9; j++) {
                PassportNo[j] = str[i + j + 1];
                codetonfc[j] = str[i + j + 1];
            }
            codetonfc[9] = str[i + 10];

            //出生年月日提取
            for (int k = 0; k<6; k++) {
                DateOfBirth[k] = str[i + k + 14];
                codetonfc[k + 10] = str[i + k + 14];
            }
            codetonfc[16] = str[i + 20];

            //发行者
            for (int m = 0; m<3; m++) {
                Issuer[m] = str[i + m + 11];
            }
            //性别
            Gender = str[i + 21];
            for (int n = 0; n <= 14; n++) {
                Opential_1[n] = str[i + n + 29];
            }
            for (int e = 0; e<6; e++) {
                ExpiryDate[e] = str[i + e + 22];
                codetonfc[e + 17] = str[i + e + 22];
            }

            codetonfc[23] = str[i + 28];
            codetonfc[24] = '\0';
        }
    }

    return true;
}
char CChipMRTD::ParseThreeCard(std::string &str){
    //清除前面的空格
        size_t iter = str.find(" ");
        while (iter != std::string::npos) {
            str = str.erase(iter, 1);
            iter = str.find(" ");
        }

        std::vector<std::string> splitstr;

        size_t location1 = str.find("\n");
        if (location1 < 30 || location1 == std::string::npos)
        {
            return false;
        }
        splitstr.push_back(std::string(str, 0, 30));//机读码有两行，每一行机读码是44个字符

        size_t location2 = str.find("\n", location1);
        if (location2 < 60 || location2 == std::string::npos)
        {
            return false;
        }
        splitstr.push_back(std::string(str, location1 + 1, 30));
        splitstr.push_back(std::string(str, location2 + 1, 30));
        if (splitstr[2].length() < 30)
        {
            return false;
        }

        str = splitstr[0] + std::string("\r\n") + splitstr[1] + std::string("\r\n") + splitstr[2];
        code = str.c_str();

        Type = splitstr[0][0];

        Country[0] = splitstr[0][2];
        Country[1] = splitstr[0][3];
        Country[2] = splitstr[0][4];
        /*
        char PassportNo[10] = { '\0' };//存储护照号码
        char DateOfBirth[7] = { '\0' };//存储护照出生年月日
        char Issuer[4] = { '\0' };//发行者
        char Gender;
        char Opential_1[16] = { '\0' };
        char ExpiryDate[7] = { '\0' };//有效日期
        char Type;
        char Country[4] = { '\0' };//国家

        char codetonfc[28] = { '\0' };
        char LastName[256] = { 0 };
        char FirstName[256] = { 0 };
        char MiddleName[256] = { 0 };
        */
        int index = 0;
        for (int i = 5; i < 14; i++) {
            PassportNo[i - 5] = splitstr[0][i];
            codetonfc[index++] = splitstr[0][i];
        }
        if (isalnum(splitstr[0][14])) {
            codetonfc[index++] = splitstr[0][14];
            PassportNo[9] = '\0';
        }
        else {
            for (int i = 15; i < 18; i++) {

                PassportNo[i - 6] = splitstr[0][i];
                codetonfc[index++] = splitstr[0][i];
            }
            codetonfc[index++] = splitstr[0][17];
            PassportNo[12] = '\0';
        }



        for (int i = 0; i < 6; i++) {
            DateOfBirth[i] = splitstr[1][i];
            codetonfc[index++] = splitstr[1][i];
        }
        codetonfc[index++] = splitstr[1][6];
        DateOfBirth[6] = '\0';

        Gender = splitstr[1][7];

        for (int i = 8; i < 14; i++) {
            ExpiryDate[i - 8] = splitstr[1][i];
            codetonfc[index++] = splitstr[1][i];
        }
        codetonfc[index++] = splitstr[1][14];
        ExpiryDate[14 - 8] = '\0';
        codetonfc[index] = '\0';

        Issuer[0] = splitstr[1][15];
        Issuer[1] = splitstr[1][16];
        Issuer[2] = splitstr[1][17];
        Issuer[3] = '\0';


        //Last Name
        int countLN = 0;
        for (int i = 0; i < 30; i++) {
            if (splitstr[2][i] == '<') {
                break;
            }
            countLN++;
        }
        for (int i = 0; i< countLN; i++) {
            LastName[i] = splitstr[2][i];
        }
        LastName[countLN] = '\0';

        //First Name
        int countFN = 0;
        for (int i = countLN + 2; i< 30; i++) {
            if (splitstr[2][i] == '<') {
                break;
            }
            countFN++;
        }
        for (int i = 0; i< countFN; i++) {
            FirstName[i] = splitstr[2][countLN + 2 + i];
        }
        FirstName[countFN] = '\0';
        return true;
}
#endif
#ifndef ECHIP_H_
#define ECHIP_H_

#include "Ptypes.h"
#include "EFFile.h"
#include "RFReader.h"
#include "STEFFile.h"
#include "MRTD.h"

class CChipReaderCallback {
public:
    virtual void OnChipReaderReadFileResult(EF_NAME name, std::map<std::string, std::string> &result, unsigned char type) = 0;
};

class CChipReader {
private:
    unsigned char CardType;
    EFFileSystem m_efs;
    STEFFileSystem st_efs;
    int m_dwTimeout;
    RFReader echipReader;

public:
    MRTD mrtd;
    std::string code;
    std::string baseFolder;
    CChipReaderCallback *cb;

    void ChipReaderReadFileResultOperate(EF_NAME name, char *result, unsigned char type);
    //新增成员函数声明
    int PassportCardTest();
public:
    CChipReader();
    ~CChipReader();

    char SetCardType(unsigned char type) {
        this->CardType = type;
        return true;
    }

    unsigned char GetCardType(void) {
        return this->CardType;
    }

    char Open() {
        return echipReader.Open();
    }

    void Close() {
        echipReader.Close();
    }

    char IsOpen() {
        return echipReader.IsOpen();
    }

//    char GetResult(EF_NAME efName,unsigned char *retData,unsigned int *retLen);
    char GetResult(EF_NAME efName, string &retData);

    char EF_DG2_SetResultPath(string path);
    //------------------------------------------------------------------------------
    // IN * cpKey:					固定16字节，默认密钥 49 45 4D 4B 41 45 52 42 21 4E 41 43 55 4F 59 46
    // return:						成功返回 true
    // 动态库产生一个随机数给设备加密，验证返回的加密结果，判断是否和输入密钥加密结果相同。
    // 如果相同则返回成功。
    // 该函数功能用于软件验证设备是否合法，不影响设备正常使用。
    // 软件可以根据验证结果，选择是否支持该设备正常操作。
    char AuthKeyFunc(std::string &cpKey);
    //------------------------------------------------------------------------------
    // IN * cpSourceKey:			固定16字节，原密钥, 保留参数，
    // IN * cpChangeKey:			固定16字节，修改密钥
    // return:						成功返回 true
    // 修改密钥，成功后主控密钥修改为cpChangeKey输入的密钥。
    // 原密钥是保留参数，只需要给一个buffer即可。
    char ChangeAuthKeyFunc(std::string &oldKey, std::string &newKey);
    //------------------------------------------------------------------------------
    // return:						成功返回 RF_ERR_SUCCESS
    // 读取设备编号，固定为8字节
    char ReadDevNum(unsigned char* curDevNum);
    //------------------------------------------------------------------------------
    // IN * newDevNum:			固定8字节，待写入的8字节设备编号
    // return:						成功返回 RF_ERR_SUCCESS
    // 修改设备编号，固定为8字节
    char WriteDevNum(unsigned char* newDevNum);

    char CloseWire();

public:
    char ReadIDInfo(void);

    char ReadIDInfo(unsigned char *sInfo, unsigned int * sLength, unsigned char *cpPhoto, unsigned int *ipLength, unsigned char *cpFinger, unsigned int *ipFLength);

    char ReadInfo(std::string &code);

private:

    char ReadEchipInfo(std::string &codetonfc);

    char SetPassportComeIn();

    char PostPassportCommand(
            unsigned char cmd,
            const std::string &request,
            std::string &response);

    char BuildKencAndKmac(
            const std::string &mrzInfo,
            std::string &Kenc,
            std::string &Kmac);

    char ICCSelectApp();

    char ICCRequestRandomNumber(std::string &RND_ICC);

    char ICCMutualAuthenticate(
            std::string &RND_IFD,
            std::string &RND_ICC,
            std::string &Kifd,
            std::string &Kenc,
            std::string &Kmac,
            std::string &KSenc,
            std::string &KSmac);

    char BuildIFD(
            std::string &RND_IFD,
            std::string &Kifd);

    char BuildSSC(
            std::string &RND_IFD,
            std::string &RND_ICC,
            std::string &SSC);

    char ICCReadEF(
            std::string &KSenc,
            std::string &KSmac,
            std::string &SSC,
            EF_NAME name,
            std::string &EF_Data);

    char SecureCommunication(
            EFFile &file,
            const std::string &KSenc,
            const std::string &KSmac,
            std::string &SSC,
            std::string &data);

    //STEFFile
    char SecureCommunication(
            STEFFile *file,
            const std::string &KSenc,
            const std::string &KSmac,
            std::string &SSC,
            std::string &data);

    char SecureCommunicationSelectFile(
            EFFile &file,
            const std::string &SKenc,
            const std::string &SKmac,
            std::string &SSC);

    //STEFFile
    char SecureCommunicationSelectFile(
            STEFFile *file,
            const std::string &SKenc,
            const std::string &SKmac,
            std::string &SSC);

    char SecureCommunicationReadFile(
            const std::string &KSenc,
            const std::string &KSmac,
            std::string &SSC,
            unsigned short offset,
            unsigned short len,
            std::string &data);

    char SecureCommunicationReadBinary(
            const std::string &KSenc,
            const std::string &KSmac,
            std::string &SSC,
            unsigned short offset,
            unsigned short chunkSize,
            std::string &data);
};

#endif

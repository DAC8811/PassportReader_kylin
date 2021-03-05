/*
 * RFReader.cpp
 *
 *  Created on: Sep 5, 2018
 *      Author: biac
 */
#include "RFReader.h"
#include "Ptypes.h"
//#define DEBUG
//#define OOXX_DEBUG_LOG 1
#define LOG_TAG "PassportReaderCPP-chip"

#include "internalLogging.h"

RFReader::RFReader() {
    m_VID = RF_VID;
    m_PID = RF_PID;
    _IsOpened = false;
}

RFReader::~RFReader() {
    m_usbHid.close();
}

bool RFReader::Open() {
    return ((0 == m_usbHid.open(RF_VID, RF_PID)) ? true : false);
}

void RFReader::Close() {
    m_usbHid.close();
}

unsigned char RFReader::xorChecksum(unsigned char *data, int len) {
    unsigned char check = 0;
    if (len < RF_RECV_PACKET_LEN_POS) {
        return -1;
    }
    //check xor sum
    for (int i = RF_RECV_PACKET_LEN_POS; i < len; i++) {
        check ^= data[i];
    }
    return check;
}

//passport card reset
//aa aa aa 96  69
//00 11  
//00 00 f0 02 50  7a 89 91 e3  e0 f3 5e 11  77 81 b3 2b    

//id card reset
//aa aa aa 96  69 
//00 12
//00 00 f0 03 50  00 00 00 00  d1 03 00 81  00 70 c0 08  5a 
int RFReader::RF_ResetCheck_Receive(unsigned char *data, int datlen){
    int len = -1;
    unsigned char check = 0;

    //check freameHeader
    if (memcmp((void *) data, RF_FRAME_HEADER, sizeof(RF_FRAME_HEADER))) {
        LOGV("frameHeader Error !");
        return -1;
    }

    //get length
    len = (data[RF_RECV_PACKET_LEN_POS] << 8);
    len |= (data[RF_RECV_PACKET_LEN_POS + 1]);

    if(len < 5 || datlen < len + RF_RECV_PACKET_SW_POS)
    {
        LOGV("RF reset receive error len data,len == %02x,datlen == %02x",len,datlen);
        return -1;
    }
    
    if ((0x00 != data[RF_RECV_PACKET_SW_POS + 1])) {
            LOGD("Error: sw1 = %02x,sw2 = %02x,sw3 = %02x",
                 data[RF_RECV_PACKET_SW_POS],
                 data[RF_RECV_PACKET_SW_POS + 1],
                 data[RF_RECV_PACKET_SW_POS + 2]);
            return -1;
    }
//    if((0x50 !=  data[RF_RECV_PACKET_SW_POS + 4])){
//        LOGV("RF reset error,unkonw card type");
//        return -1;
//    }

    for (int i = RF_RECV_PACKET_LEN_POS; i < datlen - 1; i++) {
        check ^= data[i];
    }
    if (check != data[datlen - 1]) {
        LOGV("Check Error: check = %02x,data[tail] == %02x", check, data[datlen - 1]);
        return -1;
    }
    return len;
}
int RFReader::RF_Check_Receive(unsigned char *data, int datlen) {
    int len = -1;
    unsigned char check = 0;

    //check freameHeader
    if (memcmp((void *) data, RF_FRAME_HEADER, sizeof(RF_FRAME_HEADER))) {
        LOGV("frameHeader Error !");
        return -1;
    }

    //get length
    len = (data[RF_RECV_PACKET_LEN_POS] << 8);
    len |= (data[RF_RECV_PACKET_LEN_POS + 1]);

    //unsigned char t = data[RF_RECV_PACKET_SW_POS + 2];
    if(0x00 != data[RF_RECV_PACKET_SW_POS + 1]){
        //直接判断为失败
        LOGE("Error: sw1 = %02x,sw2 = %02x,sw3 = %02x",
             data[RF_RECV_PACKET_SW_POS],
             data[RF_RECV_PACKET_SW_POS + 1],
             data[RF_RECV_PACKET_SW_POS + 2]);
        return -1;
    }
    if ((0xf0 != data[RF_RECV_PACKET_SW_POS + 2])) {//校验位在新的读卡器固件里面更改了，上面是旧的读卡器对应的
        if (0x9f != data[RF_RECV_PACKET_SW_POS + 2]) {
            if ((0x90 != data[RF_RECV_PACKET_SW_POS + 2])) {
                LOGE("Error: sw1 = %02x,sw2 = %02x,sw3 = %02x",
                     data[RF_RECV_PACKET_SW_POS],
                     data[RF_RECV_PACKET_SW_POS + 1],
                     data[RF_RECV_PACKET_SW_POS + 2]);
                return -1;
            }
        }
    }

    for (int i = RF_RECV_PACKET_LEN_POS; i < datlen - 1; i++) {
        check ^= data[i];
    }
    if (check != data[datlen - 1]) {
        LOGV("Check Error: check = %02x,data[tail] == %02x", check, data[datlen - 1]);
        return -1;
    }
    return len;
}

unsigned int
RFReader::RF_14443_Reset(unsigned char cPara, unsigned char *cpInfo, unsigned char *cpLen) {
    int ret = 0;
    //RF_SPEED s = RF_SPEED_848K_B;
    RF_SPEED s = RF_SPEED_848K;
    unsigned char data[64];
    RF_SEND_PACKET packet;

#if OOXX_DEBUG_LOG
    LOGV("open usb device");
#endif

    if (!this->IsOpen()) {
        if (!this->Open()) {
            LOGV("RFReader Error: USB is can not open!");
            return RF_ERR_FAILURE;
        }
    }

    while (1) {
        memcpy((void *) &packet.frameHeader, RF_FRAME_HEADER, sizeof(RF_FRAME_HEADER));
        packet.len1 = 0;
        packet.len2 = 4;
        packet.ext = 0xF0;
        packet.cmd = RF_CMD_RESET;
        memcpy(data, (void *) &packet, sizeof(packet));
        data[sizeof(packet)] = s;
        data[sizeof(packet) + 1] = xorChecksum(data, sizeof(packet) + 1);

#if USE_CHIPCARD_LOG_LEVEL2
        LOGV("RF_14443_Reset Send:");
        for(int i = 0;i < 64;i++){
            LOGV("%02x ",data[i]);
        }
        LOGV("");
#endif
#if OOXX_DEBUG_LOG
        LOGV("write");
#endif

        ret = m_usbHid.controlWriteData(data, 64);
        if (ret < 0) {
            //cannot send data to usb
            LOGV("cannot send data to usb");
            return RF_ERR_FAILURE;
        }
#if OOXX_DEBUG_LOG
        LOGV("read");
#endif
        //read ack data
        memset(data, 0, sizeof(data));
        ret = m_usbHid.controlReadData(data, 64);
        if (ret < 0) {
            LOGV("Error:usb can't read!");
            return RF_ERR_FAILURE;
        }

#if USE_CHIPCARD_LOG_LEVEL2
        LOGV("RF_14443_Reset Recv:");
        for(int i = 0;i < 64;i++){
            LOGV("%02x ",data[i]);
        }
        LOGV("");
#endif
#if OOXX_DEBUG_LOG
        LOGV("check");
#endif
        //判断是否复位成功
        //int recvLen = RF_Check_Receive(data, ret);
        int recvLen = RF_ResetCheck_Receive(data, ret);
        if (recvLen < 0) {
            LOGD("RF_ResetCheck_Receive Check Error!");
            switch (s) {
                case RF_SPEED_106K:
                    LOGI("can't reset RF reader");
                    return RF_ERR_FAILURE;
                case RF_SPEED_212K:
                    LOGI("Change speed and try:106k...");
                    s = RF_SPEED_106K;
                    break;
                case RF_SPEED_424K:
                    s = RF_SPEED_212K;
                    LOGI("Change speed and try:212k...");
                    break;
                case RF_SPEED_848K:
                    s = RF_SPEED_424K;
                    LOGI("Change speed and try:424k...");
                    break;
                case RF_SPEED_848K_B:
                    s= RF_SPEED_424K;
                    LOGD("Change speed and try:424k A...");
                    break;
                case RF_SPEED_424K_A:
                    s= RF_SPEED_424K;
                    return RF_ERR_FAILURE;
                    //LOGD("Change speed and try:424k...");
                    //break;
                default:
                    break;
            }
            continue;
        } else if (recvLen > 0) {
            *cpLen = recvLen - (RF_RECV_PACKET_SW_LEN + RF_RECV_PACKET_CHECK_LEN);
            memcpy(cpInfo, (void *) (data + RF_RECV_PACKET_DAT_POS), *cpLen);//  changdu cuowu ?
#if OOXX_DEBUG_LOG
            LOGV("success");
#endif
            return RF_ERR_SUCCESS;
        }
    }
    return RF_ERR_FAILURE;
}

unsigned int RFReader::RF_14443_Apdu(unsigned char *cpAPDU, unsigned int *ipLen) {
    int ret = 0;
    unsigned char data[1024] = {0x00};
    RF_SEND_PACKET packet;

    if (!this->IsOpen()) {
        if (!this->Open()) {
            LOGE("RFReader Error: USB is not open!");
            return RF_ERR_FAILURE;
        }
    }

    int packetLen = *ipLen + 3;
    memset(data, 0, sizeof(data));
    memcpy((void *) &packet.frameHeader, RF_FRAME_HEADER, sizeof(RF_FRAME_HEADER));
    packet.len1 = (packetLen >> 8) & 0xff;
    packet.len2 = packetLen & 0xff;
    packet.ext = 0xF0;
    packet.cmd = RF_CMD_APP;
    //packet.para = 0x00;
    memcpy(data, (void *) &packet, sizeof(packet));
    memcpy((void *) (data + sizeof(packet)), (void *) cpAPDU, *ipLen);
    data[sizeof(packet) + *ipLen] = xorChecksum(data, sizeof(packet) + *ipLen);;

#if USE_CHIPCARD_LOG_LEVEL2
    LOGV("RF_14443_Apdu Send: ");
    for(int i=0; i<64; i++)
    {
        LOGV("%02x ",data[i]);
    }
    LOGV("");
#endif
    ret = m_usbHid.controlWriteData(data, 64);
    if (ret < 0) {
        LOGE("cannot send data to usb");
        return RF_ERR_FAILURE;
    }

    memset(data, 0, sizeof(data));
    ret = m_usbHid.controlReadData(data, 320);
    if (ret < 0) {
        LOGE("Error:usb can't read!");
        return RF_ERR_FAILURE;
    }

#if USE_CHIPCARD_LOG_LEVEL2
    LOGV("RF_14443_Apdu Recv: ");
    for(int i=0; i< ret; i++)
    {
        LOGV("%02x ",data[i]);
    }
    LOGV("");
#endif


    int recvLen = RF_Check_Receive(data, ret);

    if (recvLen < 0) return RF_ERR_FAILURE;
    if (recvLen > 0) {
        //	LOGV("recvLen == %d",recvLen);
        *ipLen = recvLen - (RF_RECV_PACKET_SW_LEN + RF_RECV_PACKET_CHECK_LEN);
        //	LOGV("*ipLen == %d",*ipLen);
        memcpy(cpAPDU, data + RF_RECV_PACKET_DAT_POS, *ipLen);
    }

#if USE_CHIPCARD_LOG_LEVEL2
    for(int i=0; i < recvLen-1; i++)
    {
        LOGV("%02x ",cpAPDU[i]);
    }
    LOGV("");
#endif
    return RF_ERR_SUCCESS;
}

unsigned int RFReader::RF_14443_Apdu_With_Cmd(unsigned char cPara, unsigned char *cpAPDU, unsigned int *ipLen){
    int ret = 0;
    unsigned char data[1024] = {0x00};
    RF_SEND_PACKET packet;

    if (!this->IsOpen()) {
        if (!this->Open()) {
            LOGE("RFReader Error: USB is not open!");
            return RF_ERR_FAILURE;
        }
    }

    int packetLen = *ipLen + 3;
    memset(data, 0, sizeof(data));
    memcpy((void *) &packet.frameHeader, RF_FRAME_HEADER, sizeof(RF_FRAME_HEADER));
    packet.len1 = (packetLen >> 8) & 0xff;
    packet.len2 = packetLen & 0xff;
    packet.ext = 0xF0;
    packet.cmd = cPara;
    //packet.para = 0x00;
    memcpy(data, (void *) &packet, sizeof(packet));
    memcpy((void *) (data + sizeof(packet)), (void *) cpAPDU, *ipLen);
    data[sizeof(packet) + *ipLen] = xorChecksum(data, sizeof(packet) + *ipLen);;

#if USE_CHIPCARD_LOG_LEVEL2
    LOGV("RF_14443_Apdu Send: ");
    for(int i=0; i<64; i++)
    {
        LOGV("%02x ",data[i]);
    }
    LOGV("");
#endif
    ret = m_usbHid.controlWriteData(data, 64);
    if (ret < 0) {
        LOGE("cannot send data to usb");
        return RF_ERR_FAILURE;
    }

    memset(data, 0, sizeof(data));
    ret = m_usbHid.controlReadData(data, 320);
    if (ret < 0) {
        LOGE("Error:usb can't read!");
        return RF_ERR_FAILURE;
    }

#if USE_CHIPCARD_LOG_LEVEL2
    LOGV("RF_14443_Apdu Recv: ");
    for(int i=0; i< ret; i++)
    {
        LOGV("%02x ",data[i]);
    }
    LOGV("");
#endif


    int recvLen = RF_Check_Receive(data, ret);

    if (recvLen < 0) return RF_ERR_FAILURE;
    if (recvLen > 0) {
        //	LOGV("recvLen == %d",recvLen);
        *ipLen = recvLen - (RF_RECV_PACKET_SW_LEN + RF_RECV_PACKET_CHECK_LEN);
        //	LOGV("*ipLen == %d",*ipLen);
        memcpy(cpAPDU, data + RF_RECV_PACKET_DAT_POS, *ipLen);
    }

#if USE_CHIPCARD_LOG_LEVEL2
    for(int i=0; i < recvLen-1; i++)
    {
        LOGV("%02x ",cpAPDU[i]);
    }
    LOGV("");
#endif
    return RF_ERR_SUCCESS;
}

unsigned int
RFReader::RF_IDCard_ReadInfo(unsigned char *sInfo, unsigned char *cpPhoto, unsigned int *ipLength) {

    int ret = 0;
    unsigned char *tempBuf = NULL;
    unsigned char data[64] = {0x00};
    IDCARD_SEND_PACKET packet;

    if (!this->IsOpen()) {
        if (!this->Open()) {
            LOGE("RFReader Error: USB is not open!");
            return RF_ERR_FAILURE;
        }
    }

    if (*ipLength < 1024) {
        LOGE("ipLength too small");
        return RF_ERR_FAILURE;
    }

    memset(data, 0, sizeof(data));
    memcpy((void *) &packet.frameHeader, RF_FRAME_HEADER, sizeof(RF_FRAME_HEADER));
    packet.len1 = 0;
    packet.len2 = 3;
    packet.cmd = RF_CMD_FIND_IDCARD;
    packet.para = 0x01;
    packet.xor_sum = 0x22;
    memcpy(data, (void *) &packet, sizeof(packet));

#if USE_IDCARD_LOG_LEVEL2
    LOGV("RF_IDCard_ReadInfo Search Card Send: ");
    for(int i=0; i< 64; i++)
    {
        LOGV("%02x ",data[i]);
    }
    LOGV("");
#endif

    ret = m_usbHid.controlWriteData(data, 64);
    if (ret < 0) {
        LOGE("cannot send data to usb");
        return RF_ERR_FAILURE;
    }

    memset(data, 0, sizeof(data));
    ret = m_usbHid.controlReadData(data, 64);
    if (ret < 0) {
        LOGE("Error:usb can't read!");
        return RF_ERR_FAILURE;
    }

#if USE_IDCARD_LOG_LEVEL2
    LOGV("RF_IDCard_ReadInfo Search Card Recv: ");
    for(int i=0; i< ret; i++)
    {
        LOGV("%02x ",data[i]);
    }
    LOGV("");
#endif

    int recvLen1 = RF_Check_Receive(data, ret);
    if (recvLen1 < 0) {
        LOGV("search card error!");
        return RF_ERR_FAILURE;
    }

    memset(data, 0, sizeof(data));
    memcpy((void *) &packet.frameHeader, RF_FRAME_HEADER, sizeof(RF_FRAME_HEADER));
    packet.len1 = 0;
    packet.len2 = 3;
    packet.cmd = RF_CMD_FIND_IDCARD;
    packet.para = 0x02;
    packet.xor_sum = 0x21;
    memcpy(data, (void *) &packet, sizeof(packet));

#if USE_IDCARD_LOG_LEVEL2
    LOGV("RF_IDCard_ReadInfo Select Card Send: ");
    for(int i=0; i< 64; i++)
    {
        LOGV("%02x ",data[i]);
    }
    LOGV("");
#endif

    ret = m_usbHid.controlWriteData(data, 64);
    if (ret < 0) {
        LOGE("cannot send data to usb");
        return RF_ERR_FAILURE;
    }

    memset(data, 0, sizeof(data));
    ret = m_usbHid.controlReadData(data, 64);
    if (ret < 0) {
        LOGE("Error:usb can't read!");
        return RF_ERR_FAILURE;
    }

#if USE_IDCARD_LOG_LEVEL2
    LOGV("RF_IDCard_ReadInfo Select Card Recv: ");
    for(int i=0; i< ret; i++)
    {
        LOGV("%02x ",data[i]);
    }
    LOGV("");
#endif

    int recvLen2 = RF_Check_Receive(data, ret);
    if (recvLen2 < 0) {
        LOGV("select card error!");
        return RF_ERR_FAILURE;
    }

    memset(data, 0, sizeof(data));
    memcpy((void *) &packet.frameHeader, RF_FRAME_HEADER, sizeof(RF_FRAME_HEADER));
    packet.len1 = 0;
    packet.len2 = 3;
    packet.cmd = RF_CMD_READ_IDCARD;
    packet.para = 0x01;
    packet.xor_sum = 0x32;
    memcpy(data, (void *) &packet, sizeof(packet));

#if USE_IDCARD_LOG_LEVEL2
    LOGV("RF_IDCard_ReadInfo Read   Card Send: ");
    for(int i=0; i< 64; i++)
    {
        LOGV("%02x ",data[i]);
    }
    LOGV("");
#endif

    ret = m_usbHid.controlWriteData(data, 64);
    if (ret < 0) {
        LOGE("cannot send data to usb");
        return RF_ERR_FAILURE;
    }

    tempBuf = (unsigned char *) malloc(1024 * 2);
    ret = m_usbHid.controlReadData(tempBuf, 2048);
    if (ret < 0) {
        LOGE("Error:usb can't read!");
        free(tempBuf);
        return RF_ERR_FAILURE;
    }

#if USE_IDCARD_LOG_LEVEL2
    LOGV("RF_IDCard_ReadInfo Read Card Data Len == %d",ret);
    LOGV("RF_IDCard_ReadInfo Read   Card Recv: ");
    for(int i=0; i< ret; i++)
    {
        LOGV("%02x ",tempBuf[i]);
    }
    LOGV("");
#endif

    int recvLen = RF_Check_Receive(tempBuf, ret);
    if (recvLen < 0) {
        LOGE("read card error!");
        free(tempBuf);
        return RF_ERR_FAILURE;
    }

    if (recvLen > 0) {
        memcpy(sInfo, (void *) (tempBuf + RF_RECV_IDCARD_INFO_POS), RF_RECV_IDCARD_INFO_LEN);
        *ipLength = MIN(recvLen - RF_RECV_IDCARD_INFO_LEN - 1, *ipLength);
        //std::cout << "ipLength == " << *ipLength << std::endl;
        memcpy(cpPhoto, (void *) (tempBuf + RF_RECV_IDCARD_PIC_POS), *ipLength);
        free(tempBuf);
        return RF_ERR_SUCCESS;
    }

    free(tempBuf);
    return RF_ERR_FAILURE;
}

unsigned int
RFReader::RF_IDCard_ReadFinger(unsigned char *sInfo, unsigned int * sLength, unsigned char *cpPhoto, unsigned int *ipLength, unsigned char *cpFinger, unsigned int *ipFLength){

}
unsigned int
RFReader::RF_IDCard_ReadInfo(TIDCardInfo *sInfo, unsigned char *cpPhoto, unsigned int *ipLength) {

    int ret = 0;
    unsigned char *tempBuf = NULL;
    unsigned char data[64] = {0x00};
    IDCARD_SEND_PACKET packet;

    if (!this->IsOpen()) {
        if (!this->Open()) {
            LOGE("RFReader Error: USB is not open!");
            return RF_ERR_FAILURE;
        }
    }

    if (*ipLength < 1024) {
        LOGE("ipLength too small");
        return RF_ERR_FAILURE;
    }

    memset(data, 0, sizeof(data));
    memcpy((void *) &packet.frameHeader, RF_FRAME_HEADER, sizeof(RF_FRAME_HEADER));
    packet.len1 = 0;
    packet.len2 = 3;
    packet.cmd = RF_CMD_FIND_IDCARD;
    packet.para = 0x01;
    packet.xor_sum = 0x22;
    memcpy(data, (void *) &packet, sizeof(packet));

#if USE_IDCARD_LOG_LEVEL2
    LOGV("RF_IDCard_ReadInfo Search Card Send: ");
    for(int i=0; i< 64; i++)
    {
        LOGV("%02x ",data[i]);
    }
    LOGV("");
#endif

    ret = m_usbHid.controlWriteData(data, 64);
    if (ret < 0) {
        LOGE("cannot send data to usb");
        return RF_ERR_FAILURE;
    }

    memset(data, 0, sizeof(data));
    ret = m_usbHid.controlReadData(data, 64);
    if (ret < 0) {
        LOGE("Error:usb can't read!");
        return RF_ERR_FAILURE;
    }

#if USE_IDCARD_LOG_LEVEL2
    LOGV("RF_IDCard_ReadInfo Search Card Recv: ");
    for(int i=0; i< ret; i++)
    {
        LOGV("%02x ",data[i]);
    }
    LOGV("");
#endif

    int recvLen1 = RF_Check_Receive(data, ret);
    if (recvLen1 < 0) {
        LOGV("search card error!");
        return RF_ERR_FAILURE;
    }

    memset(data, 0, sizeof(data));
    memcpy((void *) &packet.frameHeader, RF_FRAME_HEADER, sizeof(RF_FRAME_HEADER));
    packet.len1 = 0;
    packet.len2 = 3;
    packet.cmd = RF_CMD_FIND_IDCARD;
    packet.para = 0x02;
    packet.xor_sum = 0x21;
    memcpy(data, (void *) &packet, sizeof(packet));

#if USE_IDCARD_LOG_LEVEL2
    LOGV("RF_IDCard_ReadInfo Select Card Send: ");
    for(int i=0; i< 64; i++)
    {
        LOGV("%02x ",data[i]);
    }
    LOGV("");
#endif

    ret = m_usbHid.controlWriteData(data, 64);
    if (ret < 0) {
        LOGE("cannot send data to usb");
        return RF_ERR_FAILURE;
    }

    memset(data, 0, sizeof(data));
    ret = m_usbHid.controlReadData(data, 64);
    if (ret < 0) {
        LOGE("Error:usb can't read!");
        return RF_ERR_FAILURE;
    }

#if USE_IDCARD_LOG_LEVEL2
    LOGV("RF_IDCard_ReadInfo Select Card Recv: ");
    for(int i=0; i< ret; i++)
    {
        LOGV("%02x ",data[i]);
    }
    LOGV("");
#endif

    int recvLen2 = RF_Check_Receive(data, ret);
    if (recvLen2 < 0) {
        LOGV("select card error!");
        return RF_ERR_FAILURE;
    }

    memset(data, 0, sizeof(data));
    memcpy((void *) &packet.frameHeader, RF_FRAME_HEADER, sizeof(RF_FRAME_HEADER));
    packet.len1 = 0;
    packet.len2 = 3;
    packet.cmd = RF_CMD_READ_IDCARD;
    packet.para = 0x01;
    packet.xor_sum = 0x32;
    memcpy(data, (void *) &packet, sizeof(packet));

#if USE_IDCARD_LOG_LEVEL2
    LOGV("RF_IDCard_ReadInfo Read   Card Send: ");
    for(int i=0; i< 64; i++)
    {
        LOGV("%02x ",data[i]);
    }
    LOGV("");
#endif

    ret = m_usbHid.controlWriteData(data, 64);
    if (ret < 0) {
        LOGE("cannot send data to usb");
        return RF_ERR_FAILURE;
    }

    tempBuf = (unsigned char *) malloc(1024 * 2);
    ret = m_usbHid.controlReadData(tempBuf, 2048);
    if (ret < 0) {
        LOGE("Error:usb can't read!");
        free(tempBuf);
        return RF_ERR_FAILURE;
    }

#if USE_IDCARD_LOG_LEVEL2
    LOGV("RF_IDCard_ReadInfo Read Card Data Len == %d",ret);
    LOGV("RF_IDCard_ReadInfo Read   Card Recv: ");
    for(int i=0; i< ret; i++)
    {
        LOGV("%02x ",tempBuf[i]);
    }
    LOGV("");
#endif

    int recvLen = RF_Check_Receive(tempBuf, ret);
    if (recvLen < 0) {
        LOGE("read card error!");
        free(tempBuf);
        return RF_ERR_FAILURE;
    }

    if (recvLen > 0) {
        memcpy(sInfo, (void *) (tempBuf + RF_RECV_IDCARD_INFO_POS), RF_RECV_IDCARD_INFO_LEN);
        *ipLength = MIN(recvLen - RF_RECV_IDCARD_INFO_POS - RF_RECV_IDCARD_INFO_LEN - 1, *ipLength);
        memcpy(cpPhoto, (void *) (tempBuf + RF_RECV_IDCARD_PIC_POS), *ipLength);
        free(tempBuf);
        return RF_ERR_SUCCESS;
    }

    free(tempBuf);
    return RF_ERR_FAILURE;
}

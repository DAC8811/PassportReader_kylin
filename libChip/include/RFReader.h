/*
 * RFReader.h
 *
 *  Created on: Sep 10, 2018
 *      Author: echo
 */

#ifndef RFREADER_H_
#define RFREADER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "USB_HAL.h"

/*******************************************************************************
第二代居民身份证卡功能
********************************************************************************/
typedef struct tagIDCradInfo {
  unsigned char name[30];                // 姓名
  unsigned char sex[2];                // 性别
  unsigned char nation[4];            // 民族
  unsigned char birthday[16];            // 出生日期 YYYYMMDD
  unsigned char addr[70];                // 地址
  unsigned char IDSn[36];                // 身份证号
  unsigned char signOrg[30];            // 签发单位
  unsigned char startDate[16];        // 签发日期
  unsigned char endDate[16];            // 有效日期
  unsigned char newAddr[70];            // 新地址
} TIDCardInfo;

#define RF_VID                        0x0483
#define RF_PID                        0x5751

#define RF_RECV_PACKET_LEN_POS    0x05
#define RF_RECV_PACKET_SW_POS     0x07
#define RF_RECV_PACKET_DAT_POS    0x0A

#define RF_RECV_PACKET_LEN_LEN    0x02
#define RF_RECV_PACKET_SW_LEN     0x03
#define RF_RECV_PACKET_CHECK_LEN  0x01

#define RF_RECV_IDCARD_INFO_LEN    0x100

#define RF_RECV_IDCARD_INFO_POS    0x0E
#define RF_RECV_IDCARD_PIC_POS     (RF_RECV_IDCARD_INFO_POS + RF_RECV_IDCARD_INFO_LEN)

/*******************************************************************************
错误码定义
********************************************************************************/
#define RF_ERR_SUCCESS                0x0000        // 成功
#define RF_ERR_FAILURE                0x0001        // 失败，数据收发成功，命令执行失败。

// 设备给出的错误状态定义 参照PCD.h文件定义
#define RF_ERR_NO_CARD                0x00FF        // 无卡

// PC软件给出的错误状态定义
#define RF_ERR_DEVICE_INIT            0x1000        // 设备未初始化
#define RF_ERR_DEVICE_CONNECT        0x1001        // 通讯接口连接错误：
#define RF_ERR_DEVICE_PORT            0x1002        // 设备接口错误
#define RF_ERR_COMM_PROTOCOL        0x1003        // 通信协议错误，需要配置通信协议。
#define RF_ERR_COMM_OVERTIME        0x1004        // 通讯超时错误
#define RF_ERR_COMM_VERIFY            0x1005        // 通讯校验错误
#define RF_ERR_COMM_LENGTH            0x1006        // 通讯长度错误
#define RF_ERR_COMM_COMMAND            0x1007        // 通讯命令错误，返回命令和发送命令不相同，且不是取反。

#define RF_ERR_COMM_PARAMETER        0x1010        // 参数错误
#define RF_ERR_COMM_ADDRESS            0x1011        // 地址错误

/*******************************************************************************
设备通讯接口控制类
********************************************************************************/
// 通讯协议类型定义
#define RF_COMM_PROTOCOL_NULL        0x00u        // 未知通讯协议
#define RF_COMM_PROTOCOL_DQ300        0x01u        // 详见 DQ300通讯协议
#define RF_COMM_PROTOCOL_GA467        0x02u        // 详解 GA467标准文件
#define RF_COMM_PROTOCOL_MODBUS        0x03u        // 详见 MODBUS通讯协议


// 设备接口类型定义
#define RF_DEVCIE_TYPE_NULL            0x00        // 未知通讯接口
#define RF_DEVCIE_TYPE_COM            0x01        // 串口通讯接口
#define RF_DEVCIE_TYPE_HID            0x02        // USB-HID通讯接口
#define RF_DEVCIE_TYPE_PCSC            0x03        // USB-PCSC通讯接口
#define RF_DEVCIE_TYPE_TCP            0x04        // TCP/IP通讯接口

//RF命令字
#define RF_CMD_RESET                0x41        //RF reset command
#define RF_CMD_APP                    0x43        //RF application data exchange
#define RF_CMD_FIND_IDCARD            0x20
#define RF_CMD_READ_IDCARD            0x30
#define RF_CMD_CLOSE_WIRE             0x06
#define RF_CMD_RW_DEV_NUM             0x07      //读写设备自定义编号的命令
#define RF_CMD_AUTH                   0x08       //设备认证的命令

//射频卡发送数据头
typedef struct RF_SEND_PACKET {
  unsigned char frameHeader[5];            //“AA AA AA 96 69”
  unsigned char len1;                        //从扩展字开始至校验字
  unsigned char len2;
  unsigned char ext;                        //扩展字
  unsigned char cmd;                        //指定设备动作内容
  //unsigned char para;                       //参数
  //data
  //check sum
} RF_SEND_PACKET;
//射频卡返回数据头
typedef struct RF_RECV_PACKET {
  unsigned char frameHeader[5];            //“AA AA AA 96 69”
  unsigned char len1;                        //从扩展字开始至校验字
  unsigned char len2;
  unsigned char sw1;
  unsigned char sw2;
  unsigned char sw3;
  //data
  //checksum
} RF_RECV_PACKET;

typedef struct IDCARD_SEND_PACKET {
  unsigned char frameHeader[5];            //“AA AA AA 96 69”
  unsigned char len1;                        //从扩展字开始至校验字
  unsigned char len2;
  unsigned char cmd;
  unsigned char para;
  unsigned char xor_sum;
} IDCARD_SEND_PACKET;

typedef struct IDCARD_RECV_PACKET {
  unsigned char frameHeader[5];            //“AA AA AA 96 69”
  unsigned char len1;                        //从扩展字开始至校验字
  unsigned char len2;
  unsigned char sw1;
  unsigned char sw2;
  unsigned char sw3;
} IDCARD_RECV_PACKET;

//RF14443 speed enum
typedef enum RF_SPEED {
  RF_SPEED_106K = 0x0F,                    //bit7-4:0000  --> 106Kb/s
  RF_SPEED_212K = 0x5F,                    //bit7-4:0101  --> 212Kb/s
  RF_SPEED_424K = 0xAF,                    //bit7-4:1010  --> 424Kb/s
  RF_SPEED_848K = 0xFF,                    //bit7-4:1111  --> 848Kb/s
  RF_SPEED_424K_A = 0xA9,
  RF_SPEED_848K_B = 0xFA
} RF_SPEED;

const unsigned char RF_RESET_CMD = 0x0B;    //硬复位
const unsigned char RF_FRAME_HEADER[5] = {0xAA, 0xAA, 0xAA, 0x96, 0x69};

class RFReader {
 private:
  CUSBHid m_usbHid;                            //libusb handle
  int m_VID;                            //USB LED VID
  int m_PID;                            //USB LED PID
  bool _IsOpened;

 public:
  RFReader();
  ~RFReader();

  bool Open();
  void Close();

  bool IsOpen() {
    return m_usbHid.IsOpened();
  }

  /****************************************************************
   * @Description: 异或校验
   * @	  Input: unsigned char*data: input data
   * 		  Input: int len: input data length
   * @	 return: check sum
   * @	   Date: 2018-09-06
   * @   notation:
   * modify:
   *	name	date	email
  ****************************************************************/
  unsigned char xorChecksum(unsigned char *data, int len);
  int RF_ResetCheck_Receive(unsigned char *data, int datlen);



  int RF_Check_Receive(unsigned char *data, int datlen);
  //----------------------------------------------------------
  // IN cPara:					复位参数,位有效
  //								bit7 = 1:硬复位; bit7 = 0:软复位，复位成功后，无法再次复位。
  //								bit6-2 RFU
  //								bit1 = 1:Type B CPU卡复位
  //								bit0 = 1:Type A CPU卡复位
  // OUT *cpInfo:					复位信息
  // OUT *cpLen:					信息长度，最大长度64字节
  // return:						成功返回 RF_ERR_SUCCESS
  // ISO14443 Type A & B 智能卡复位
  //----------------------------------------------------------
  unsigned int RF_14443_Reset(unsigned char cPara, unsigned char *cpInfo, unsigned char *cpLen);

  //----------------------------------------------------------
  // IN *cpAPDU:					发送给卡的交互数据
  // IN iAPDULen					交互信息长度
  // OUT *cpDATA:					卡片响应数据
  // OUT *ipDATALen				响应数据长度
  // return:						成功返回 RF_ERR_SUCCESS
  // ISO14443 - 4  智能卡APDU数据交互
  //----------------------------------------------------------
  unsigned int RF_14443_Apdu(unsigned char *cpAPDU, unsigned int *ipLen);

  //带有命令参数的APDU函数,可以进行数据的交互，设备的认证等命令
  unsigned int RF_14443_Apdu_With_Cmd(unsigned char cPara, unsigned char *cpAPDU, unsigned int *ipLen);

  //----------------------------------------------------------
  // OUT *sInfo:					结构体指针TIDCardInfo
  // OUT *cpPhoto:				相片数据
  // OUT *ipLength:				相片数据长度
  // return:						成功返回 RF_ERR_SUCCESS
  // 读取身份证文件信息和图像信息
  //----------------------------------------------------------
  unsigned int RF_IDCard_ReadInfo(TIDCardInfo *sInfo, unsigned char *cpPhoto, unsigned int *ipLength);
  unsigned int RF_IDCard_ReadInfo(unsigned char *sInfo, unsigned char *cpPhoto, unsigned int *ipLength);
  unsigned int RF_IDCard_ReadFinger(unsigned char *sInfo, unsigned int * sLength, unsigned char *cpPhoto, unsigned int *ipLength, unsigned char *cpFinger, unsigned int *ipFLength);
  //----------------------------------------------------------
  // OUT *sInfo:					结构体指针TIDCardInfo
  // OUT *cpPhoto:				相片数据，至少定义1024字节缓存
  // OUT *ipLength:				相片数据长度
  // OUT *cpFinger:				指纹数据，至少定义1024字节缓存
  // OUT *ipFLength:				指纹数据长度
  // return:						成功返回 RF_ERR_SUCCESS
  // 读取身份证文件信息，图像信息，指纹信息（预留）
  //----------------------------------------------------------
  unsigned int RF_IDCard_ReadFinger(TIDCardInfo *sInfo,
                                    unsigned char *cpPhoto,
                                    unsigned int *ipLength,
                                    unsigned char *cpFinger,
                                    unsigned int *ipFLength);

  bool RF_DevAuthKey(unsigned char* cpKey);

  bool RF_DevAuthChange(unsigned char* cpSourceKey, unsigned char* cpChangeKey);
};

#endif /* RFREADER_H_ */

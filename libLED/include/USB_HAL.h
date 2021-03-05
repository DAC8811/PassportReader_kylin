/*********************************************************
@Filename: libUSB.h
@Function:Linux C++ USB HID library
@Author:Xiong.Guo
@Company:HUST BIAC
@Email:1250281649@qq.com
@Version:V1.0
@Date:2018-09-04
	Last update date
if you find any bug, please send to my Email. Thanks!
*********************************************************/

#ifndef USB_HAL_H_
#define USB_HAL_H_

//#include <iostream>

#include <string>
//C
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

#include <libusb-1.0/libusb.h>

using namespace std;

#define STM32_CHIP_ID_LEN 12
#define STM32_FIRMWARE_UPDATA_PACKET_MAX_SIZE  1024

const static unsigned char CMD_LIGHT_PURPLE = 0x01;
const static unsigned char CMD_LIGHT_INFRARED_LEFT = 0x02;
const static unsigned char CMD_LIGHT_INFRARED_RIGHT = 0x07;
const static unsigned char CMD_LIGHT_INFRARED = 0x09;
const static unsigned char CMD_LIGHT_KEEP_ALIVE = 0x04;
const static unsigned char CMD_LIGHT_WHITE_LEFT = 0x03;
const static unsigned char CMD_LIGHT_WHITE_RIGHT = 0x08;
const static unsigned char CMD_LIGHT_WHITE = 0x0A;


const static unsigned char CMD_APP_FACTORY_SET = 0xA1;
const static unsigned char CMD_APP_FACTORY_SET2 = 0xB1;

const static unsigned char CMD_APP_AUTHENTICATE = 0xA2;
const static unsigned char CMD_APP_AUTHENTICATE2 = 0xB2;

const static unsigned char CMD_APP_GET_MOD = 0xA5;

const static unsigned char CMD_APP_SET_JUMP_MOD = 0xA4;

const static unsigned char CMD_APP_CODE_DOWLOAD = 0xA6;
const static unsigned char CMD_APP_CODE_DOWLOAD2 = 0xB6;

const static unsigned char CMD_APP_CODE_UPLOAD = 0xA7;
const static unsigned char CMD_APP_CODE_UPLOAD2 = 0xB7;

const static unsigned char CMD_APP_GET_VERSION = 0xA8;

const static char Version[] = "VERSION_1.1";
const static char VersionStr[] = "VERSION_";
const static char MachineName[] = "HUST-DSCV";//用于version_1.0版本的固件验证
const static char MachineName2[] = "HUST-DSCF"; //用于version_1.1版本的固件验证
const static char IapMode[] = "IAP";
const static char RunMode[] = "RUN";
const static char FIRMWARE[] = ".\\USB_TEMP\\firmware.bin";

const static unsigned char RUN_MODE = 0xAA;
const static unsigned char IAP_MODE = 0x55;
const unsigned char SW_SUCCESS[] = "\x90\x00";

#define LED_VID                    0x0483
#define LED_PID                    0x1850
#define LED_ENDPOINT_IN             0x82
#define LED_ENDPOINT_OUT            0x01

class CUSBHid {
private:
    int m_dwTimeOut;
    libusb_context *ctx;
    libusb_device_handle *dev_handle;    //a device handle
    libusb_device *m_dev;
    struct libusb_config_descriptor *conf_desc;
    uint8_t endpoint_in, endpoint_out;
    int m_dwInputLen;                    //USB input data length
    int m_dwOutputLen;                    //USB output data length
    unsigned char m_descId;
    int m_dwTimeout;

    std::string m_recvBuff;

    char FirmwareUpload();

    char SystemSetIapMode();

    char SystemSetRunMode();

    char WriteData(unsigned char *pData, int *length, int dwTimeoutMs = 500);

    char ReadData(unsigned char *pData, int *length, int dwTimeoutMs = 500);
    char ReadChunk(unsigned char * data, int dataLen, int dwTimeoutMs);
    char WriteChunk(unsigned char * data, int dataLen, int dwTimeoutMs);

    char Post(unsigned char tag, const std::string &request, std::string &response);


    unsigned char CalculateCheckByte(unsigned char *data, int len);

    char VerifyCheckByte(unsigned char *data, int len, unsigned char checkByte);

    char ReadFull(int dwTimeoutMs = 500);

    char ReadFullEx(int dwTimeoutMs = 500);

    int SystemAuthentic();
    int SystemGetVersion();
    std::string  Authentic_ChipId;

private:
    //register callback functions
    libusb_hotplug_callback_handle usb_connect_handle;
    libusb_hotplug_callback_handle usb_disconnect_handle;

    //USB设备断开的回调函数
    static int LIBUSB_CALL
    usb_left_callback(
            struct libusb_context *ctx,
            struct libusb_device *dev,
            libusb_hotplug_event event,
            void *userdata);

    //USB设备插入的回调函数
    static int LIBUSB_CALL
    usb_arrived_callback(
            struct libusb_context *ctx,
            struct libusb_device *dev,
            libusb_hotplug_event event,
            void *userdata);

public:
    CUSBHid();

    ~CUSBHid();

    /****************************************************************
     * @Description: open usb hid device according to VID and PID
     * @	  Input: int vid: USB VID
     * @	  Input: int pid: USB PID
     * @	 return: status
     * @	   Date: 2018-09-04
     * modify:
     *	name	date	email
    ****************************************************************/
    int open(int vid, int pid);

    /****************************************************************
     * @Description: close usb hid device
     * @	  Input: none
     * @	 return: none
     * @	   Date: 2018-09-04
     * modify:
     *  name	date	email
    ****************************************************************/
    void close();

    /****************************************************************
     * @Description: judge the device status,opened or not
     * @	  Input: none
     * @	 return: opened: true; not opened: false
     * @	   Date: 2018-09-04
     * modify:
     *  name	date	email
    ****************************************************************/
    bool IsOpened();

    /****************************************************************
     * @Description: set send or receive data timeout
     * @	  Input: int t: t ms
     * @	 return: none
     * @	   Date: 2018-09-04
     * modify:
     *  name	date	email
    ****************************************************************/
    void setTimeOut(int t);

    /****************************************************************
     * @Description: print this opened device infomation
     * @	  Input: none
     * @	 return: none
     * @	   Date: 2018-09-04
     * @   notation: if this device is not opened, no message output
     * modify:
     *  name	date	email
    ****************************************************************/
    void printDevInfo(libusb_device *d);

    libusb_device *getDeviceHandle() {
        return m_dev;
    }

    /****************************************************************
     * @Description: do events looply
     * @	  Input: none
     * @	 return: none
     * @	   Date: 2018-09-04
     * @   notation: call this function in your main loop manually
     * modify:
     *  name	date	email
    ****************************************************************/
    void loopEvents();

    /****************************************************************
     * @Description: USB HID interrupt send data
     * @	  Input: unsigned char endpoint: endpoint address
     * 		  Input: unsigned char *pData: send data address
     * 		  Input: int length: send data length
     * 		  Inpute: int dwTimeoutMs = 500: timeout ,ms
     * @	 return: send status, 0:success
     * @	   Date: 2018-09-05
     * @   notation:
     * modify:
     *  name	date	email
    ****************************************************************/
    int interruptWriteData(unsigned char endpoint, unsigned char *pData, int length, int dwTimeoutMs = 200);

    /****************************************************************
     * @Description: HID USB中断传输方式读取数据
     * @	  Input: unsigned char endpoint: endpoint address
     * 		  Input: unsigned char *pData: send data address
     * 		  Input: int length: read data length
     * 		  Inpute: int dwTimeoutMs = 1000: timeout ,ms
     * @	 return: read count really
     * @	   Date: 2018-09-06
     * @   notation:
     * modify:
     *  name	date	email
    ****************************************************************/
    int interruptReadData(unsigned char endpoint, unsigned char *pData, int length, int dwTimeoutMs = 200);

    /****************************************************************
     * @Description: HID USB控制传输方式发送数据
     * @	  Input: unsigned char *pData: send data address
     * 		  Input: int length: read data length
     * 		  Inpute: int dwTimeoutMs = 1000: timeout ,ms
     * @	 return: write count really
     * @	   Date: 2018-09-06
     * @   notation:
     * modify:
     *  name	date	email
    ****************************************************************/
    int controlWriteData(unsigned char *pData, int length, int dwTimeoutMs = 1000);

    int controlReadData(unsigned char *pData, int length, int dwTimeoutMs = 1000);

    int writeData(unsigned char *pData, int length, int dwTimeoutMs = 500);

    int readData(unsigned char *pData, int length, int dwTimeoutMs = 500);

};

#endif /* LIBUSB_H_ */

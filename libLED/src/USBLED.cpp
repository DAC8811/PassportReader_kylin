/*********************************************************
@Filename: USBLED.cpp
@Function:Linux C++ USB HID LED ligth class
@Author:Xiong.Guo
@Company:HUST BIAC
@Email:1250281649@qq.com
@Version:V1.0
@Date:2018-09-05
	Last update date
if you find any bug, please send to my Email. Thanks!
*********************************************************/
#include "USBLED.h"

#define LOG_TAG "PassportReaderCPP-USBLED"

// #define LOG_DEBUG
#include "internalLogging.h"


USBLED::USBLED() {
    m_VID = LED_VID;
    m_PID = LED_PID;
    m_ENDPOINT_IN = LED_ENDPOINT_IN;
    m_ENDPOINT_OUT = LED_ENDPOINT_OUT;
    _IsOpened = false;
}

USBLED::~USBLED() {
    m_usbHid.close();
}

bool USBLED::Open() {
    int ret = 0;
    ret = m_usbHid.open(m_VID, m_PID);
    if (ret != 0) {
        _IsOpened = false;
        LOGD("Cannot open usb hid device:(%0x, %0x)\n", m_VID, m_PID);
    } else
        _IsOpened = true;


    return !_IsOpened;
}

void USBLED::close() {
    m_usbHid.close();
}

void USBLED::toBigEnd(USB_LED_PACKET *packet) {
    //USB_LED_PACKET p;
    //memcpy(&p, packet, sizeof(p));
    //change length
    packet->Length = (packet->Length << 8) | (packet->Length >> 8);

    //chage command
    unsigned char *ptr = (unsigned char *) (&packet->Led_msg),
            tmp = ptr[0];
    ptr[0] = ptr[1];
    ptr[1] = tmp;

//	*(unsigned char*)(&packet->Led_msg) = *(((unsigned char*)&p.Led_msg) + 1);
//	*(((unsigned char*)&packet->Led_msg) + 1) = *(((unsigned char*)&p.Led_msg) + 0);
}

void USBLED::fillPacket(USB_LED_PACKET *packet, unsigned char LED_Type) {
    memset((void *) packet, 0x00, sizeof(USB_LED_PACKET));
//    for (int i = 0; i < sizeof(USB_LED_PACKET); i ++)
//        ((unsigned char *) (packet))[i] = i;
    packet->Length = sizeof(USB_LED_MSG);
    packet->Led_msg.CMD = LED_CMD;
    packet->Led_msg.Length = 0x01;
    packet->Led_msg.LED = LED_Type;
    for (int i = 0; i < sizeof(USB_LED_PACKET) - 1; i++) {
        packet->checkSum ^= ((unsigned char *) (packet))[i];
    }
    toBigEnd(packet);
}

bool USBLED::LightLED(LIGHT_TYPE LED_type) {
    int ret = false, retry = 10;
    if (!_IsOpened) return false;
    USB_LED_PACKET packet;
    fillPacket(&packet, LED_type);
    ret = m_usbHid.interruptWriteData(LED_ENDPOINT_OUT, (unsigned char *) &packet, sizeof(packet));
    while (ret != 0) {
        retry--;
        if (retry <= 0)
            return false;
        //LOGE("libusb_interrupt_transfer timeout, retry");
        ret = m_usbHid.interruptWriteData(LED_ENDPOINT_OUT, (unsigned char *) &packet, sizeof(packet));
    }

//	unsigned char readData[64] = {0};
//	int len = 0;
//	len = m_usbHid.interruptReadData(LED_ENDPOINT_IN, readData, 64);
//	if(len>0)
//	{
//		if(readData[4] == SW_SUCCESS[0] &&
//		   readData[5] == SW_SUCCESS[1]	)
//			return true;
//	}
//	return false;
    return true;
}

bool USBLED::IndicatorLED(INDICATOR_TYPE LED_type) {
    int ret = false;
    if (!_IsOpened) return false;
    USB_LED_PACKET packet;
    fillPacket(&packet, LED_type);
    ret = m_usbHid.interruptWriteData(LED_ENDPOINT_OUT, (unsigned char *) &packet, sizeof(packet));
    return ret == 0;
}


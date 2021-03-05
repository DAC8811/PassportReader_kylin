/*********************************************************
@Filename: USBLED.h
@Function:Linux C++ USB HID LED ligth class
@Author:Xiong.Guo
@Company:HUST BIAC
@Email:1250281649@qq.com
@Version:V1.0
@Date:2018-09-05
	Last update date
if you find any bug, please send to my Email. Thanks!
*********************************************************/

#ifndef HAL_INCLUDE_USBLED_H_
#define HAL_INCLUDE_USBLED_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "USB_HAL.h"



#define LED_CMD                        0x8
//const unsigned char SW_SUCCESS[] = "\x90\x00";
//LED Control message, 3 bytes

#pragma pack(16)

typedef struct USB_LED_MSG {
    unsigned char Length:12;
    unsigned char CMD:4;
    unsigned char LED;
} USB_LED_MSG;

//64Bytes message packet
typedef struct USB_LED_PACKET {
    unsigned short Length;            //message length,less then 61
    USB_LED_MSG Led_msg;            //3 bytes msg
    unsigned char reserved[58];        //58 bytes 00
    unsigned char checkSum;
} USB_LED_PACKET;

class USBLED {
private:
    CUSBHid m_usbHid;                //libusb handle
    unsigned int m_VID;                //USB LED VID
    unsigned int m_PID;                //USB LED PID
    unsigned char m_ENDPOINT_IN;    //USB receive endpoint
    unsigned char m_ENDPOINT_OUT;    //USB send endpoint
    bool _IsOpened;

public:
    //Light LED type
    typedef enum {
        CMD_LIGHT_WHITE_LEFT = 0x03,
        CMD_LIGHT_WHITE_RIGHT = 0x8,
        CMD_LIGHT_WHITE = 0xA,
        CMD_LIGHT_INFRARED_LEFT = 0x2,
        CMD_LIGHT_INFRARED_RIGHT = 0x7,
        CMD_LIGHT_INFRARED = 0x9,
        CMD_LIGHT_PURPLE = 0x1,
    } LIGHT_TYPE;

    //Indicator LED type
    typedef enum {
        CMD_INDICATOR_BLUE_ON = 0x80,
        CMD_INDICATOR_BLUE_OFF = 0x81,
        CMD_INDICATOR_GREEN_ON = 0x82,
        CMD_INDICATOR_GREEN_OFF = 0x83,
        CMD_INDICATOR_RED_ON = 0x84,
        CMD_INDICATOR_RED_OFF = 0x85,
        CMD_INDICATOR_WHITE_ON = 0x86,
        CMD_INDICATOR_WHITE_OFF = 0x87,
    } INDICATOR_TYPE;

public:
    USBLED();

    ~USBLED();

    /****************************************************************
     * @Description: open USB device
     * @	  Input: none
     * @	 return: bool status
     * @	   Date: 2018-09-05
     * @   notation:
     * modify:
     *	name	date	email
    ****************************************************************/
    bool Open();

    void close();

    bool IsOpened() {
        return _IsOpened;
    }

    /****************************************************************
     * @Description: convert struct message to bigend data
     * @	  Input: USB_LED_PACKET *packet: packet data address
     * @	 return: none
     * @	   Date: 2018-09-05
     * @   notation: modify this function if you modify strut of LED
     * modify:
     *	name	date	email
    ****************************************************************/
    void toBigEnd(USB_LED_PACKET *packet);

    /****************************************************************
     * @Description: fill packet
     * @	  Input: USB_LED_PACKET *packet: packet data address
     * @	  Input: LIGHT_TYPE LED_Type: enum type LED
     * @	 return: none
     * @	   Date: 2018-09-05
     * @   notation:
     * modify:
     *	name	date	email
    ****************************************************************/
    void fillPacket(USB_LED_PACKET *packet, unsigned char LED_Type);

    /****************************************************************
     * @Description: light LED according to led type
     * @	  Input: LIGHT_TYPE LED_Type: enum type LED
     * @	 return: if it is light on
     * @	   Date: 2018-09-05
     * @   notation:
     * modify:
     *	name	date	email
    ****************************************************************/
    bool LightLED(LIGHT_TYPE LED_type);

    /****************************************************************
     * @Description: light Indicator LED according to led type
     * @	  Input: INDICATOR_TYPE LED_Type: enum type LED
     * @	 return: if it is light on
     * @	   Date: 2018-09-05
     * @   notation:
     * modify:
     *	name	date	email
    ****************************************************************/
    bool IndicatorLED(INDICATOR_TYPE LED_type);
};

#endif /* HAL_INCLUDE_USBLED_H_ */

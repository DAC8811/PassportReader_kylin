/*********************************************************
@Filename: libUSB.cpp
@Function:Linux C++ USB HID library
@Author:Xiong.Guo
@Company:HUST BIAC
@Email:1250281649@qq.com
@Version:V1.0
@Date:2018-09-04
	Last update date
if you find any bug, please send to my Email. Thanks!
*********************************************************/

#include "USB_HAL.h"
#include "rsaAPI.h"
#include <libusb-1.0/libusb.h>

#define LOG_TAG "PassportReaderCPP-USBHAL"
//#define LOG_DEBUG

#include "internalLogging.h"

int CUSBHid::LIBUSB_CALL
usb_arrived_callback(
        struct libusb_context *ctx,
        struct libusb_device *dev,
        libusb_hotplug_event event,
        void *userdata) {
    struct libusb_device_descriptor desc;
    libusb_get_device_descriptor(dev, &desc);
    LOGD("Add usb device: \n");
    LOGD("\tCLASS(0x%x) SUBCLASS(0x%x) PROTOCOL(0x%x)\n",
         desc.bDeviceClass, desc.bDeviceSubClass, desc.bDeviceProtocol);
    LOGD("\tVENDOR(0x%x) PRODUCT(0x%x)\n", desc.idVendor, desc.idProduct);

    return 0;
}

int CUSBHid::LIBUSB_CALL
usb_left_callback(
        struct libusb_context *ctx,
        struct libusb_device *dev,
        libusb_hotplug_event event,
        void *userdata) {
    struct libusb_device_descriptor desc;
    libusb_get_device_descriptor(dev, &desc);
    LOGD("Remove usb device: CLASS(0x%x) SUBCLASS(0x%x) iSerialNumber(0x%x)\n",
         desc.bDeviceClass, desc.bDeviceSubClass, desc.iSerialNumber);

    return 0;
}

CUSBHid::CUSBHid() {
    //initialize private data
    m_dwTimeOut = 100;
    ctx = NULL;
    dev_handle = NULL;
    m_dev = NULL;
    conf_desc = NULL;
    m_dwInputLen = 0;;
    m_dwOutputLen = 0;
    m_descId = 0;
    endpoint_in = 0;
    endpoint_out = 0;
    m_dwTimeout=1000; 

    usb_connect_handle = 0;
    usb_disconnect_handle = 0;
}

CUSBHid::~CUSBHid() {
    this->close();
}

int CUSBHid::open(int vid, int pid) {
    int ret = 0;            //for return values

    if (this->IsOpened()) {
        return 0;
    }

    ret = libusb_init(&ctx);
    if (ret < 0) {
        LOGE("Init Error\n");
        return -1;
    }
    libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_INFO);

    dev_handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if (dev_handle == NULL) {
        LOGD("Cannot open device\n");
        return -1;
    }

    m_dev = libusb_get_device(dev_handle);

    //detach kernel
    if (libusb_kernel_driver_active(dev_handle, 0) == 1) {
        LOGD("Kernel Driver Active\n");
        if (libusb_detach_kernel_driver(dev_handle, 0) == 0)
                LOGD("Kernel Driver Detached!\n");
    }
    //claim interface
    ret = libusb_claim_interface(dev_handle, 0);
    if (ret < 0) {
        LOGD("Cannot claim interface\n");
        return ret;
    }

    ret = libusb_hotplug_register_callback(
            ctx,
            LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED,
            LIBUSB_HOTPLUG_NO_FLAGS,
            LIBUSB_HOTPLUG_MATCH_ANY,
            LIBUSB_HOTPLUG_MATCH_ANY,
            LIBUSB_HOTPLUG_MATCH_ANY,
            usb_arrived_callback,
            NULL,
            &usb_connect_handle);
    if (ret != 0) {
        LOGD("Error to register usb arrived callback\n");
        return ret;
    }
    ret = libusb_hotplug_register_callback(
            ctx,
            LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
            LIBUSB_HOTPLUG_NO_FLAGS,
            LIBUSB_HOTPLUG_MATCH_ANY,
            LIBUSB_HOTPLUG_MATCH_ANY,
            LIBUSB_HOTPLUG_MATCH_ANY,
            usb_left_callback,
            NULL,
            &usb_disconnect_handle);
    if (ret != 0) {
        LOGD("Error to register usb left callback\n");
        return ret;
    }

    if (vid == LED_VID && pid == LED_PID) {
        struct libusb_config_descriptor *config;
        ret = libusb_get_config_descriptor(m_dev, 0, &config);

        if (config->interface && config->interface->altsetting->endpoint) {
            m_dwInputLen = config->interface->altsetting->endpoint->wMaxPacketSize;
        }
        else
        {
            m_dwInputLen=64;
        }
        m_dwOutputLen=m_dwInputLen;

        for (int i = 5; i > 0; i --) {
            ret = !SystemAuthentic();
            if (ret == 0) {
                LOGI("????????????");
                break;
            }
            LOGD("Error to auth hardware... retrying... \n");
        }
    }

    return ret;
}

void CUSBHid::close() {
    //LOGV("CUSBHid::close() \n");
    if (dev_handle) {
        libusb_attach_kernel_driver(dev_handle, 0);
        libusb_hotplug_deregister_callback(ctx, usb_connect_handle);
        libusb_hotplug_deregister_callback(ctx, usb_disconnect_handle);
        libusb_release_interface(dev_handle, 0);

        //	libusb_free_config_descriptor(conf_desc);
        libusb_close(dev_handle);
        dev_handle = NULL;
    }

    if (ctx) {
        libusb_exit(ctx);
        ctx = NULL;
    }
}

bool CUSBHid::IsOpened() {
    return (this->dev_handle == NULL ? false : true);
}

void CUSBHid::setTimeOut(int t) {
    m_dwTimeOut = t;
}


//?????????????????????????
void CUSBHid::loopEvents() {
    libusb_handle_events_completed(ctx, NULL);
}

int CUSBHid::interruptWriteData(unsigned char endpoint, unsigned char *pData, int length, int dwTimeoutMs) {
    int ret = 0;
    int sendLen = 0;
    if (!this->IsOpened()) {
        LOGD("interruptReadData Open Error!\n");
        return -1;
    }

    ret = libusb_interrupt_transfer(
            dev_handle,
            endpoint,
            pData,
            length,
            &sendLen,
            dwTimeoutMs);
    if (ret == 0 && sendLen == length)
        return ret;
    else return ret;

    return 0;
}

int CUSBHid::interruptReadData(unsigned char endpoint, unsigned char *pData, int length, int dwTimeoutMs) {
    int readLen = 0;
    int ret = 0;
    if (!this->IsOpened()) {
        LOGD("interruptReadData Open Error!\n");
        return -1;
    }

    ret = libusb_interrupt_transfer(
            dev_handle,
            endpoint,
            pData,
            length,
            &readLen,
            dwTimeoutMs);
    
    return ret;
}

int CUSBHid::controlWriteData(unsigned char *pData, int length, int dwTimeoutMs) {
    int ret = 0;
    if (!this->IsOpened()) {
        LOGD("controlWriteData: control usb open error!\n");
        return -1;
    }

    ret = libusb_control_transfer(
            dev_handle,
            0x21,
            0x09,
            0x0200,
            0,
            pData,
            length,
            dwTimeoutMs);
    return ret;
}

int CUSBHid::controlReadData(unsigned char *pData, int length, int dwTimeoutMs) {
    int ret = 0;
    if (!this->IsOpened()) {
        LOGD("controlReadData: control usb open error!\n");
        return -1;
    }

    ret = libusb_control_transfer(
            dev_handle,
            0xA1,
            0x01,
            0x0100,
            0,
            pData,
            length,
            dwTimeoutMs);
    return ret;
}


//static std::string HexStringToBinary(const std::string &src) {
//    unsigned char nibble[2];
//    unsigned int destlen = src.size() / 2;
//    unsigned int i = 0, j = 0;
//    std::string dest(src.size() / 2, 0);
//
//    for (; i < destlen; i++) {
//        nibble[0] = src[2 * i];
//        nibble[1] = src[2 * i + 1];
//        for (j = 0; j < 2; j++) {
//            if (nibble[j] >= 'A' && nibble[j] <= 'F')
//                nibble[j] = nibble[j] - 'A' + 10;
//            else if (nibble[j] >= 'a' && nibble[j] <= 'f')
//                nibble[j] = nibble[j] - 'a' + 10;
//            else if (nibble[j] >= '0' && nibble[j] <= '9')
//                nibble[j] = nibble[j] - '0';
//            else
//                return 0;
//        }
//        dest[i] = nibble[0] << 4;    // Set the high nibble
//        dest[i] |= nibble[1];        // Set the low nibble
//    }
//    return dest;
//}

// ?????????????????????(BigEndian)????????????
static unsigned short HostToNetworkUINT16(unsigned short n) {
    return (n << 8) | (n >> 8);
}

char CUSBHid::VerifyCheckByte(unsigned char *data, int len, unsigned char checkByte) {
    return true;
}

char CUSBHid::ReadChunk(unsigned char * data, int dataLen, int dwTimeoutMs)
{
    int ret = 0;
    while (dataLen > 0 && ret == 0) {
        ret = this->interruptReadData(LED_ENDPOINT_IN, data, m_dwInputLen, dwTimeoutMs);
        data += m_dwInputLen;
        dataLen -= m_dwInputLen;
    }
    return ret;
}

char CUSBHid::WriteChunk(unsigned char * data, int dataLen, int dwTimeoutMs)
{
    int ret = 0;
    while (dataLen > 0 && ret == 0) {
        ret = this->interruptWriteData(LED_ENDPOINT_OUT, data, m_dwOutputLen, dwTimeoutMs);
        data += m_dwOutputLen;
        dataLen -= m_dwOutputLen;
    }
    return ret;
}


char CUSBHid::ReadFull(int dwTimeoutMs) {
    int dwSize = 0, chunkNum = 0;;
    char ret = false;

    std::string data(m_dwInputLen, 0);

    // ??????????????????
    dwSize = m_dwInputLen;


    ret = this->ReadChunk( (unsigned char *) data.data(), dwSize, dwTimeoutMs);
    if (ret) {
        return false;
    }

    unsigned char desc_id = data[0];
    unsigned char check_byte = data[m_dwInputLen - 1];
    unsigned short *plen = (unsigned short *) ((unsigned char *) data.data() + 1);
    unsigned short len = HostToNetworkUINT16(*plen); // ????????????????????????
    int chunkSize = m_dwInputLen - 4; // ????????????????????????

    ret = this->VerifyCheckByte((unsigned char *) data.data() + 1, data.size() - 2, check_byte);
    if (!ret) {
        return false;
    }
    this->m_recvBuff.resize(0);
    this->m_recvBuff.append(data.data() + 3, min(chunkSize, int(len)));
    if (len > chunkSize) {
        int left = len - chunkSize;

        chunkSize = m_dwInputLen - 2;
        chunkNum = (left / chunkSize) + ((left % chunkSize) ? 1 : 0);

        for (int i = 0; i < chunkNum; i++) {
            dwSize = m_dwInputLen;
            ret = this->ReadChunk( (unsigned char *) data.data(), dwSize, dwTimeoutMs);
            if (ret) {

                return false;
            }
            check_byte = data[m_dwInputLen - 1];
            ret = this->VerifyCheckByte((unsigned char *) data.data() + 1, data.size() - 2, check_byte);
            if (ret) {
                return false;
            }
            int cpyLen = min(chunkSize, left);
            this->m_recvBuff.append(data.data() + 1, cpyLen);
            left -= cpyLen;
        }
    }
    return true;
}

char CUSBHid::ReadFullEx(int dwTimeoutMs) {
    int dwSize = 0, chunkNum = 0;;
    char ret = false;

    std::string data(m_dwInputLen, 0);

    // ??????????????????
    dwSize = m_dwInputLen;//65

    ret = this->ReadChunk( (unsigned char *) data.data(), dwSize, dwTimeoutMs);
    if (ret) {
        return false;
    }

    unsigned char desc_id = data[0];
    unsigned char check_byte = data[m_dwInputLen - 1];
    unsigned short *plen = (unsigned short *) ((unsigned char *) data.data());
    unsigned short len = HostToNetworkUINT16(*plen); // ????????????????????????
    int chunkSize = m_dwInputLen - 3; // ????????????????????????

//    ret = this->VerifyCheckByte(( unsigned char*)data.data() + 1, data.size() - 2, check_byte);
//    if (ret) {
//    	return false;
//    }
    this->m_recvBuff.resize(0);
    this->m_recvBuff.append(data.data() + 2, min(chunkSize, int(len)));
    if (len > chunkSize) {

        int left = len - chunkSize;

        chunkSize = m_dwInputLen - 2;
        chunkNum = (left + chunkSize - 1) / chunkSize;

        int alignLeft = chunkNum * m_dwInputLen;

        std::string data(alignLeft, 0);

        dwSize = data.size();
        int dwRead = 0;
        ret = this->ReadChunk( (unsigned char *) data.data(), dwSize, dwTimeoutMs);
        if (ret) {
            return false;
        }

        //this->m_recvBuff.append(data.data(), left);

        for (int i = 0; i < alignLeft / m_dwInputLen; i++) {
            char *buf = (char *) data.data() + i * m_dwInputLen;

            int cpyLen = min((m_dwInputLen - 1), left);
            this->m_recvBuff.append(buf, cpyLen);
            left -= cpyLen;
        }
    }

    return true;
}

unsigned char CUSBHid::CalculateCheckByte(unsigned char *data, int len) {
    unsigned char ret = 0;

    if (!data || !len) {
        return ret;
    }
    for (int i = 0; i < len; i++) {
        ret ^= data[i];
    }
    return ret;
}

char CUSBHid::WriteData(unsigned char *pData, int *length, int dwTimeoutMs /* = 500 */) {
    char ret = true;
    int dwWritten = 0;
    std::string data(m_dwOutputLen, 0);

    if (!pData || !length) {
        return false;
    }
    // USB-hid?????????????????????64????????????2??????????????????????????????, ?????????????????????
    // ????????????????????????61?????????????????????????????????????????????????????????61??????????????????
    // ??????????????????????????????????????????2??????????????????????????????????????????????????????????????????
    // ??????Bit64????????????????????????????????????????????????????????????????????????????????????61????????????
    // ???????????????????????????????????????63?????????????????????????????????????????????????????????????????????
    // ???????????????
    // ??????????????????????????????????????????63??????????????????????????????????????????????????????????????????0
    // ???????????????

    // ???????????????????????????id,?????????0,????????????????????????????????????
    unsigned char *pOffset = (unsigned char *) data.data(); // ?????????????????????????????????id
    int dwSize = *length;
    int offset = 0;
    int chunkNum = 0;

    // ???????????????id
    //data.at(0) = m_descId;

    // ??????????????????
    int chunkSize = m_dwOutputLen - 4; // ??????????????????
    int cpyLen = min(chunkSize, dwSize);
    // ??????length
    unsigned short *plen = (unsigned short *) pOffset;
    *plen = HostToNetworkUINT16((unsigned short) dwSize);
    // ????????????
    memset(pOffset + 2, 0, chunkSize);
    memcpy(pOffset + 2, pData, cpyLen);
    // ????????????????????????
    pOffset[chunkSize + 2] = this->CalculateCheckByte(pOffset, chunkSize + 2);
    // ??????????????????
    dwWritten = m_dwOutputLen;

    ret = this->WriteChunk((unsigned char *) data.data(), dwWritten, dwTimeoutMs);
    if (ret) {
        LOGE( "writeData Error!" );
        return false;
    }

    offset += chunkSize;
    // ???????????????
    if (dwSize > chunkSize) {
        chunkSize = m_dwOutputLen - 2;//65????????????chunkSize???63
        chunkNum = ((dwSize - offset) / chunkSize) + ((((dwSize - offset) % chunkSize) > 0) ? 1 : 0);

        for (int i = 0; i < chunkNum; i++) {
            memset(pOffset, 0, chunkSize);
            memcpy(pOffset, pData + offset, min(chunkSize, dwSize - offset));
            pOffset[chunkSize] = this->CalculateCheckByte(pOffset, chunkSize);

            dwWritten = m_dwOutputLen;

            ret = this->WriteChunk((unsigned char *) data.data(), dwWritten, dwTimeoutMs);
            if (ret) {
                return false;
            }
            offset += chunkSize;
        }
    }

    return true;
}

char CUSBHid::ReadData(unsigned char *pData, int *length, int dwTimeoutMs /* = 500 */) {
    char ret = false;
    int dwSize = 0;

    if (!length) {
        return false;
    }
    dwSize = *length;

    if (this->m_recvBuff.size() == 0) {
        ret = this->ReadFullEx(dwTimeoutMs);
        if (!ret) {
            return false;
        }
    }
    int cpyLen = min(dwSize, int(this->m_recvBuff.size()));
    memcpy(pData, this->m_recvBuff.data(), cpyLen);
    this->m_recvBuff.erase(0, cpyLen);
    *length = cpyLen;

    return true;
}

int CheckSumOk(std::string &str){

	char sum = 0;
	for (int i = 0; i < str.size() - 1; i++){
		sum ^= str[i];
	}

	return sum == str[str.size() - 1];
}

//????????????
int CUSBHid::SystemAuthentic() {
    std::string request;
    std::string response;
    std::string pubkey;
    pubkey.append("-----BEGIN RSA PUBLIC KEY-----\n");
    pubkey.append("MIGHAoGBAMExqW/nJaB/AcKpYAFsuvZx+T+9Z6eTvvZxt00++PoORQOgSsPR6Yxw\n");
    pubkey.append("03cTG9evabRUm4vIgilA2OwpGMRe/VXhA6SNbol0mGnrv1rCgn6MS0ZqWLlZbWcS\n");
    pubkey.append("aUjguTIHIceJh/aBGXVHqsZ2itP/Gtv2d6aAFhMRL9pbaK84/LuJAgED\n");
    pubkey.append("-----END RSA PUBLIC KEY-----\n");
    request.push_back(CMD_APP_AUTHENTICATE);
    int ret = this->Post(0x01, request, response);//??????????????????
    if (ret) {
        std::string cipherText(response.begin(), response.end());

        LOGD("cipherText %d: %s", cipherText.size(), cipherText.c_str());
        
        std::string decryptedText = rsaPubDecryptAPI(cipherText, pubkey);//??????
        //std::string decryptedText = rsaPubDecryptAPI(cipherText, readFromPem(pubkeyPath));//??????D

        LOGD("decryptedText %d: %s", decryptedText.size(), decryptedText.c_str());

        ret = SystemGetVersion();
        int pos = 0;
        if (ret) {
            pos = decryptedText.find(MachineName2);//???????????????????????? version_1.1
        } else {
            pos = decryptedText.find(MachineName);//???????????????????????? version_1.0
        }

        if (pos != std::string::npos) {
            request.resize(0);
            response.resize(0);
            //???????????????
            ret = CheckSumOk(decryptedText);
            if (!ret) {
                LOGI( "??????????????????????????????" );
            }

            this->Authentic_ChipId = std::string(decryptedText.begin(), decryptedText.begin() + STM32_CHIP_ID_LEN);
            request.push_back(CMD_APP_AUTHENTICATE2);
            request.append(decryptedText.begin(), decryptedText.begin() + STM32_CHIP_ID_LEN);
            ret = this->Post(0x01, request, response);
            if (!ret) {
                LOGI( "???????????????STM32 ID????????????" );
                return false;
            }
        } else {
            LOGI( "?????????????????????????????????????????????" );
            request.resize(0);
            response.resize(0);
            request.push_back(CMD_APP_AUTHENTICATE2);
            this->Post(0x01, request, response);//????????????????????????????????????
            return false;
        }
    } else {
        LOGI( "???????????????????????????" );
        request.resize(0);
        response.resize(0);
        request.push_back(CMD_APP_AUTHENTICATE2);
        this->Post(0x01, request, response);//????????????????????????????????????
        return false;
    }
    return true;
}
int CUSBHid::SystemGetVersion(){
	std::string request;
	std::string response;

	request.push_back(CMD_APP_GET_VERSION);
	int  ret = this->Post(0x01, request, response);

    if (!ret) {
        LOGE( "?????????????????????????????????" );
        return -1;
    }

    int pos = response.find(VersionStr);

    if (pos == std::string::npos){
        LOGE( "?????????????????????????????????" );
        return -1;
    }

    LOGD( "Version: %s", response.data() );

    pos = response.find(Version);

    if (pos == std::string::npos){
        //version 1.0
        return 0;
    }
    else
        //version 1.1
        return 1;
}
static void BuildTagLength(unsigned char tag, short length, std::string& tl)
{
	             //C
	unsigned char msb = (((tag | 0x02) & 0x03) << 6) | ((length & 0x0F00) >> 8);
	unsigned char lsb = (length & 0x00FF);

	tl.push_back(msb);
	tl.push_back(lsb);
}
static char ParseTagLength(std::string& data, unsigned char& tag, short& length)
{
	if (data.size() < 2) {
		return false;
	}
	unsigned char msb = data[0];
	unsigned char lsb = data[1];
	tag = ((msb & 0x40) >> 6);
	length = ((msb & 0x03) << 8) | lsb;

	return true;
}

char CUSBHid::Post(unsigned char tag, const std::string& request, std::string& response)
{
	char ret = false;
	unsigned char *pData = NULL;
	unsigned char *pResponse = NULL;

	if (!this->IsOpened()) {
		return false;
	}

	do {
		std::string pData;

		BuildTagLength(tag, (short)request.size(), pData);
		if (request.size() > 0) {
			pData.append(request);
		}

		// Send Request
		int dwWritten = pData.size();
		try {
			ret = this->WriteData((unsigned char*)pData.data(), &dwWritten, m_dwTimeout);
		}
		catch (...) {
            LOGE( "WriteData Error" );
			ret = false;
		}
		if (!ret) {
			break;
		}
		// Receive Response
		int head_len = 2;
		short resp_len = 0;
		std::string head(head_len, 0);

		try {
			ret = this->ReadData((unsigned char*)head.data(), &head_len, m_dwTimeout);
		}
		catch (...) {
			ret = false;
		}
		if (!ret) {
			break;
		}
		unsigned char tag_res;
		// ??????TagLength???????????????Tag?????????????????????
		ret = ParseTagLength(head, tag_res, resp_len);
		if (!ret) {
			break;
		}
		// ??????????????????Tag????????????
		if (tag_res != tag) {

			ret = false;
			break;
		}
		// ????????????TagLength???????????????0??????????????????????????????
		if (resp_len == 0) {
			break;
		}

		response.resize(resp_len);

		int dwRead = resp_len;

		try {
			//if (tag == 0x01 && request[0] == 0xa4){
			//	ret = this->ReadData((unsigned char*)response.data(), &dwRead, 10);
			//	LOGE( "this->ReadData((unsigned char*)response.data(), &dwRead, 10);" );
			//}
			//else
				ret = this->ReadData((unsigned char*)response.data(), &dwRead, m_dwTimeout);
		}
		catch (...) {
			ret = false;
		}
		if (!ret) {
			break;
		}

		ret = true;

		if (response.size() >= 2) {
			char *sw = (char*)response.data() + response.size() - 2;
			if (memcmp(sw, SW_SUCCESS, 2) != 0) {
				const char *sw = response.data() + response.size() - 2;
				const short *ssw = (short*)sw;
				LOGD( "ssw error!" );
				ret = false;
			}
			response.erase(response.size() - 2, 2);
		}
	} while (0);


	return ret;
}

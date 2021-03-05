/*******************************************************************************
 * FileName: CryptoDES.h
 *   Author: wenlei整理
 *     Date: 2010.12.30
 *  COMMENT: 支持 DES  加密或解密功能
 *           支持 3DES 电子密本方式 加密或解密功能
 *           支持 DES CBC  密文分组链接方式 对数据加密链接，或解密链接
 *           支持 3DES CBC 密文分组链接方式 对数据加密链接，或解密链接
 *
 *           使用宏定义，可以取舍后三种功能。注意3DES_CBC需要3DES的支持。
 *
 ******************************************************************************/
#ifndef _CRYPTO_DES_H_
#define _CRYPTO_DES_H_

//------------------------------------------------------------------------------
// DES和3DES加密或解密方式
//------------------------------------------------------------------------------
#define CRYPTO_ENCRYPT		0x01		// 加密
#define CRYPTO_DECRYPT		0x02		// 解密


//------------------------------------------------------------------------------
// Functions Support
//------------------------------------------------------------------------------
#define CRYPTO_DES_ECB					// 定义是否支持函数编译
#define CRYPTO_DES_CBC
#define CRYPTO_3DES_ECB
#define CRYPTO_3DES_CBC

//****************************************************************************** 
// IN:		cMode						// CRYPTO_ENCRYPT = 加密; CRYPTO_DECRYPT = 解密.
// IN:		*cpKey						// 8字节密钥
// IN OUT:	*cpnData					// 加密或解密的数据
// RETURN:	void
// COMMENT:	DES加密或解密功能, 外部调用。
//******************************************************************************
void Crypto_DES(unsigned char cMode, unsigned char *cpKey, unsigned char *cpData);


//******************************************************************************
// IN:		cMode						// CRYPTO_ENCRYPT = 加密; CRYPTO_DECRYPT = 解密.
// IN:		*cpKey						// 8字节密钥
// IN:		iInLen						// 加密或解密的数据长度，必须是8的整数倍。
// IN OUT:	*cpData						// 加密或解密数据的输入口，和输出口。
// RETURN:	unsigned char 				// SUCCESS = 0x00; FAILURE = 0xFF.
// COMMENT:	DES ECB加密或解密功能, 外部调用。
//******************************************************************************
unsigned char Crypto_DES_ECB(unsigned char cMode, unsigned char *cpKey, unsigned int iInLen, unsigned char *cpData);

//******************************************************************************
// IN:		cMode						// CRYPTO_ENCRYPT = 加密; CRYPTO_DECRYPT = 解密.
// IN:		*cpKey						// 8字节密钥
// IN OUT:	*cpIV						// 初始向量
// IN:		iInLen						// 加密或解密的数据长度，必须是8的整数倍。
// IN OUT:	*cpData						// 加密或解密数据的输入口，和输出口。
// RETURN:	unsigned char 				// SUCCESS = 0x00; FAILURE = 0xFF.
// COMMENT: DES CBC加密或解密功能
//******************************************************************************
unsigned char Crypto_DES_CBC(unsigned char cMode, unsigned char *cpKey, unsigned char *cpIV, unsigned int iInLen, unsigned char *cpData);


//******************************************************************************
// IN:		cMode						// CRYPTO_ENCRYPT = 加密; CRYPTO_DECRYPT = 解密.
// IN:		*cpKey						// 16字节密钥
// IN:		iInLen						// 加密或解密的数据长度，必须是8的整数倍。
// IN OUT:	*cpData						// 加密或解密数据的输入口，和输出口。
// RETURN:	unsigned char 				// SUCCESS = 0x00; FAILURE = 0xFF.
// COMMENT: 3DES ECB加密或解密功能
//******************************************************************************
unsigned char Crypto_3DES_ECB(unsigned char cMode, unsigned char *cpKey, unsigned int iInLen, unsigned char *cpData);


//******************************************************************************
// IN:		cMode						// ENCRYPT = 加密; DECRYPT = 解密.
// IN:		*cpKey						// 16字节密钥
// IN OUT:	*cpIV						// 初始向量
// IN:		iInLen						// 加密或解密的数据长度，必须是8的整数倍。
// IN OUT:	*cpData						// 加密或解密数据的输入口，和输出口。
// RETURN:	unsigned char				// SUCCESS = 0; FAILURE = 1.
// COMMENT:	3DES的CBC模式
//******************************************************************************
unsigned char Crypto_3DES_CBC(unsigned char cMode, unsigned char *cpKey, unsigned char *cpIV, unsigned int iInLen, unsigned char *cpData);

#endif

// CryptoDES.cpp : 应用程序。
//

//#include "stdafx.h"
//#include "HexString.h"
#include "CryptoDES.h"
#include <string>
/*******************************************************************************
 * FileName: DES.C
 *   Author: wenlei整理
 *     Date: 2010.12.30
 *  COMMENT: 支持 DES  加密或解密功能
 *           支持 3DES 加密或解密功能
 *           支持 DES CBC  模式 对数据加密链接，或解密链接
 *           支持 3DES CBC 模式 对数据加密链接，或解密链接
 *  头文件中可以取舍后三种功能。
 ******************************************************************************/


//------------------------------------------------------------------------------
// List Define
//------------------------------------------------------------------------------
unsigned char  PC1[56] = {
	57, 49, 41, 33, 25, 17,  9,
	 1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27,
	19, 11,  3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	 7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29,
	21, 13,  5, 28, 20, 12,  4
	};
unsigned char  PC2[48] = {
	14, 17, 11, 24,  1,  5,
	 3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8,
	16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
	};
unsigned char  IPC[64] = {
	58, 50, 42, 34, 26, 18, 10,  2,
	60, 52, 44, 36, 28, 20, 12,  4,
	62, 54, 46, 38, 30, 22, 14,  6,
	64, 56, 48, 40, 32, 24, 16,  8,
	57, 49, 41, 33, 25, 17,  9,  1,
	59, 51, 43, 35, 27, 19, 11,  3,
	61, 53, 45, 37, 29, 21, 13,  5,
	63, 55, 47, 39, 31, 23, 15,  7
	};
unsigned char  RFP[64] = {
	 8, 40, 16, 48, 24, 56, 32, 64,
	 7, 39, 15, 47, 23, 55, 31, 63,
	 6, 38, 14, 46, 22, 54, 30, 62,
	 5, 37, 13, 45, 21, 53, 29, 61,
	 4, 36, 12, 44, 20, 52, 28, 60,
	 3, 35, 11, 43, 19, 51, 27, 59,
	 2, 34, 10, 42, 18, 50, 26, 58,
	 1, 33,  9, 41, 17, 49, 25, 57
	};
unsigned char  EN[48] = {
	32,  1,  2,  3,  4,  5,
	 4,  5,  6,  7,  8,  9,
	 8,  9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32,  1
	};
unsigned char  PP[32] = {
	16,  7, 20, 21,
	29, 12, 28, 17,
	 1, 15, 23, 26,
	 5, 18, 31, 10,
	 2,  8, 24, 14,
	32, 27,  3,  9,
	19, 13, 30,  6,
	22, 11,  4, 25
	};
unsigned char  S[8][64] = {
	14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
	 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
	15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,

	15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
	 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
	 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
	13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,

	10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
	 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,

	 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
	 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,

	 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
	 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
	11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,

	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
	 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,

	 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
	 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
	 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,

	13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
	 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
	};

unsigned char  shifts[16] = 
	{ 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};



//------------------------------------------------------------------------------
// Local Functions Define
//------------------------------------------------------------------------------

void Pack8(unsigned char *packed, unsigned char *binary);
void UnPack8(unsigned char *packed, unsigned char *binary);
void DES_SetkeySingle(unsigned char sw,unsigned char *pkey ,unsigned char line, unsigned char *cpKsd);
void Xor_8Byte(unsigned char *cpIn, unsigned char *cpInOut);


//******************************************************************************
// OUT:		*packed
// IN:		*binary
// RETURN:	void
// COMMENT:	64bit => 8Byte
//******************************************************************************
void Pack8(unsigned char *packed, unsigned char *binary)
{
	unsigned char i,j,k;
	for(i=0; i<8; i++)
	{
		k = 0;
		for(j=0; j<8; j++)
		{
			k = (k<<1) + *binary++;
		}
		*packed++ = k;
	}
	return;
}
//******************************************************************************
// IN:		*packed
// OUT:		*binary
// RETURN:	void
// COMMENT:	8Byte => 64bit
//******************************************************************************
void UnPack8(unsigned char *packed, unsigned char *binary)
{
	unsigned char i, j, k, ky;
	for(i=0; i<8; i++)
	{
		k = *packed++;
		for(j=0; j<8; j++)
		{
			ky = (k>>(7-j)) & 0x01;
			*binary++ = ky;
		}
	}
	return;
}
//******************************************************************************
// IN:		sw							// CRYPTO_ENCRYPT = 加密; CRYPTO_DECRYPT = 解密.
// IN:		*pkey				
// IN:		line
// OUT: 	*cpKsd
// RETURN:	void
// COMMENT: 
//******************************************************************************
void DES_SetkeySingle(unsigned char sw, unsigned char *pkey, unsigned char line, unsigned char *cpKsd)
{
	unsigned char i, j, k, t1, t2;
	unsigned char key[64];
	unsigned char CD[56];

	UnPack8(pkey, key);
	for(i=0; i<56; i++)
	{
		CD[i] = key[PC1[i] - 1];
	}
	for(i=0; i<16; i++)
	{
		for(j=0; j<shifts[i]; j++)
		{
			t1 = CD[0];
			t2 = CD[28];
			for(k=0; k<27; k++)
			{
				CD[k] = CD[k+1];
				CD[k+28] = CD[k+29];
			}
			CD[27] = t1;
			CD[55] = t2;
		}
		if(sw == CRYPTO_ENCRYPT)
			j = i;
		else
			j = 15-i;

		if(j == line)
		{
			for(k=0; k<48; k++)
			{
				cpKsd[k] = CD[PC2[k] - 1];
			}
			return;
		}
	}
	return;
}
//******************************************************************************
// IN:		*cpIn						// A串输入
// IN OUT:	*cpInOut					// B串输入，异或后输出
// RETURN:	void
// COMMENT: 两串8字节数据异或。
//******************************************************************************
void Xor_8Byte(unsigned char *cpIn, unsigned char *cpInOut)
{
	unsigned char i;
	for (i=0; i<8; i++)
	{
		cpInOut[i] ^= cpIn[i];
	}
	return;
}

//****************************************************************************** 
// IN:		cMode						// CRYPTO_ENCRYPT = 加密; CRYPTO_DECRYPT = 解密.
// IN:		*cpKey						// 8字节密钥
// IN OUT:	*cpnData					// 加密或解密的数据
// RETURN:	void
// COMMENT:	DES加密或解密功能, 外部调用。
//******************************************************************************
void Crypto_DES(unsigned char cMode, unsigned char *cpKey, unsigned char *cpData)
{
	unsigned char i, j, k, t;
	unsigned char cBlock[64];
	unsigned char cLr[64];
	unsigned char cF[32];
	unsigned char cPreS[48];

	UnPack8(cpData, cBlock);
	for(i=0; i<64; i++)
	{
		cLr[i] = cBlock[IPC[i] - 1];
	}
	for(i=0; i<16; i++)
	{
		DES_SetkeySingle(cMode, cpKey, i, cPreS);
		for(j=0; j<48; j++)
		{
			cPreS[j] ^= cLr[EN[j]+31];
		}
		for(j=0; j<8; j++)
		{
			k = 6 * j;
			t = cPreS[k];
			t = (t<<1) | cPreS[k+5];
			t = (t<<1) | cPreS[k+1];
			t = (t<<1) | cPreS[k+2];
			t = (t<<1) | cPreS[k+3];
			t = (t<<1) | cPreS[k+4];
			t = S[j][t];
			k = 4 * j;
			cF[k]   = (t>>3) & 1;
			cF[k+1] = (t>>2) & 1;
			cF[k+2] = (t>>1) & 1;
			cF[k+3] = t & 1;
		}

		for(j=0; j<32; j++)
		{
			t = cLr[j+32];
			cLr[j+32] = cLr[j] ^ cF[PP[j] - 1];
			cLr[j] = t;
		}
	}
	for(j=0; j<64; j++)
	{
		cBlock[j] = cLr[RFP[j] - 1];
	}
	Pack8(cpData, cBlock);
	return;
}
#ifdef CRYPTO_DES_ECB
unsigned char Crypto_DES_ECB(unsigned char cMode, unsigned char *cpKey, unsigned int iInLen, unsigned char *cpData)
{
	unsigned int i;
	// 加密数据长度必须是8的倍数
	if ((iInLen % 0x08) != 0x00)
	{
		return 0xFF;
	}
	for (i=0; i<iInLen/8; i++)
	{
		Crypto_DES(cMode, cpKey, &cpData[i*8]);	// DES运算   8字节密钥
	}
	return 0x00;
}
#endif

#ifdef CRYPTO_DES_CBC
unsigned char Crypto_DES_CBC(unsigned char cMode, unsigned char *cpKey, unsigned char *cpIV, unsigned int iInLen, unsigned char *cpData)
{
	unsigned int i;
	unsigned char cTempIV[8];
	// 加密数据长度必须是8的倍数
	if ((iInLen % 0x08) != 0x00)				
	{
		return 0xFF;
	}
	for (i=0; i<iInLen/8; i++)
	{
		if (cMode & CRYPTO_ENCRYPT)
		{
			Xor_8Byte(cpIV, &cpData[i*8]);	// 密文分组链接
		}
		else
		{
			memcpy(cTempIV, &cpData[i*8], 8);
		}

		Crypto_DES(cMode, cpKey, &cpData[i*8]);	// DES运算   8字节密钥

		if (cMode & CRYPTO_ENCRYPT)
		{
			memcpy(cpIV, &cpData[i*8], 8);
		}
		else
		{
			Xor_8Byte(cpIV, &cpData[i*8]);	// 密文分组还原
			memcpy(cpIV, cTempIV, 0x08);
		}
	}
	return 0x00;
}
#endif
#ifdef CRYPTO_3DES_ECB
unsigned char Crypto_3DES_ECB(unsigned char cMode, unsigned char *cpKey, unsigned int iInLen, unsigned char *cpData)
{
	unsigned int i;
	// 数据长度必须是8的整数倍
	if (iInLen%8 != 0x00)
	{
		return 0xFF;
	}
	if (cMode & CRYPTO_ENCRYPT)
	{
		for (i=0; i<iInLen/8; i++)
		{
			Crypto_DES(CRYPTO_ENCRYPT, cpKey, &cpData[i*8]);
			Crypto_DES(CRYPTO_DECRYPT, &cpKey[8], &cpData[i*8]);
			Crypto_DES(CRYPTO_ENCRYPT, cpKey, &cpData[i*8]);
		}
	}
	else
	{
		for (i=0; i<iInLen/8; i++)
		{
			Crypto_DES(CRYPTO_DECRYPT, cpKey, &cpData[i*8]);
			Crypto_DES(CRYPTO_ENCRYPT, &cpKey[8], &cpData[i*8]);
			Crypto_DES(CRYPTO_DECRYPT, cpKey, &cpData[i*8]);
		}
	}
	return 0x00;
}
#endif

#ifdef CRYPTO_3DES_CBC
unsigned char Crypto_3DES_CBC(unsigned char cMode, unsigned char *cpKey, unsigned char *cpIV, unsigned int iInLen, unsigned char *cpData)
{
	unsigned int i;
	unsigned char cTempIV[8];

	// 加密数据长度必须是8的倍数
	if ((iInLen % 0x08) != 0x00)
	{
		return 0xFF;
	}
	for (i=0; i<iInLen/8; i++)
	{
		if (cMode & CRYPTO_ENCRYPT)
		{
			Xor_8Byte(cpIV, &cpData[i*8]);	// 密文分组链接
		}
		else
		{
			memcpy(cTempIV, &cpData[i*8], 8);
		}

		Crypto_3DES_ECB(cMode, cpKey, 8, &cpData[i*8]);	// 3DES运算 16字节密钥

		if (cMode & CRYPTO_ENCRYPT)
		{
			memcpy(cpIV, &cpData[i*8], 8);
		}
		else
		{
			Xor_8Byte(cpIV, &cpData[i*8]);	// 密文分组还原
			memcpy(cpIV, cTempIV, 0x08);
		}
	}
	return 0x00;
}
#endif

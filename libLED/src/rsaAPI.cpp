//
// Created by ooxx on 3/8/19.
//

#include <cstring>
#include "rsaAPI.h"


std::string rsaPubDecrypt(const std::string & cipherText, const std::string & pubKey)
{
	std::string strRet;
	RSA *rsa = RSA_new();
	BIO *keybio;
	keybio = BIO_new_mem_buf((unsigned char *)pubKey.c_str(), -1);

	// 此处有三种方法
	// 1, 读取内存里生成的密钥对，再从内存生成rsa
	// 2, 读取磁盘里生成的密钥对文本文件，在从内存生成rsa
	// 3，直接从读取文件指针生成rsa
	rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);
	if (rsa == nullptr)
		return "";
	int len = RSA_size(rsa);
	char *decryptedText = (char *)malloc(len + 1);
	memset(decryptedText, 0, len + 1);

	// 解密函数
	int ret = RSA_public_decrypt(cipherText.length(), (const unsigned char*)cipherText.c_str(), (unsigned char*)decryptedText, rsa, RSA_PKCS1_PADDING);
	if (ret >= 0)
		strRet = std::string(decryptedText, ret);

	// 释放内存
	free(decryptedText);
	BIO_free_all(keybio);
	RSA_free(rsa);

	return strRet;
}

std::string rsaPubDecryptAPI(const std::string cipherText, const std::string pubKeyStr)
 {
	  return rsaPubDecrypt(cipherText, pubKeyStr);
 }

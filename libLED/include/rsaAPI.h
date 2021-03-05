//
// Created by ooxx on 3/8/19.
//

#ifndef PROJECT_RSAAPI_H
#define PROJECT_RSAAPI_H

#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <string>

std::string rsaPubDecryptAPI(const std::string cipherText, const std::string pubKeyStr);
#endif //PROJECT_RSAAPI_H

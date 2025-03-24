#pragma once

#ifdef ENABLE_CRYPTO_HANDLER

#include <Arduino.h>
#include "mbedtls/aes.h"

class CryptoHandler
{
private:
    static String encryptAES(const String &plainText, const String &key);
    static String decryptAES(const String &cipherText, const String &key);

public:
    static void init();
};

#else

class CryptoHandler
{
public:
    static void init() {}
};

#endif // ENABLE_CRYPTO_HANDLER

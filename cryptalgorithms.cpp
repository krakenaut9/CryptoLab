#include <pch.h>
#include "cryptalgorithms.h"

static inline QString kEngSupportedSymbols{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" };
static inline QString kUkrSupportedSymbols{ "АБВГҐДЕЄЖЗИІЇЙКЛМНОПРСТУФХЦЧШЩЬЮЯабвгґдеєжзиіїйклмнопрстуфхцчшщьюя" };
static inline QString kOtherSupportedSymbols{ " ~`!@\"№#$;%^:&?*()-+={}[]\\|/<>\',.1234567890" };

static inline int positiveModulo(int base, int mod)
{
    return (base % mod + mod) % mod;
}

void CaesarCipher::tryEncryptChar(QChar& ch, QStringView symbols, qint8 key)
{
    if(auto index = symbols.indexOf(ch); index != -1)
    {
        ch = symbols[positiveModulo(index + key, symbols.size())];
    }
}

QString CaesarCipher::encrypt(QStringView src, qint8 key)
{
    QString result{ src.toString() };

    for(auto& ch : result)
    {
        tryEncryptChar(ch, kEngSupportedSymbols, key);
        tryEncryptChar(ch, kUkrSupportedSymbols, key);
        tryEncryptChar(ch, kOtherSupportedSymbols, key);
    }

    return result;
}

QString CaesarCipher::decrypt(QStringView src, qint8 key)
{
    return encrypt(src, -key);
}

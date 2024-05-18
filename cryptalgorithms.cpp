#include <pch.h>
#include "cryptalgorithms.h"

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

TrithemiusCipher::TrithemiusCipher(QVector<int> coefficients, CipherType type)
    : m_coefficients(std::move(coefficients)), m_type(type) {}

TrithemiusCipher::TrithemiusCipher(QStringView keyword)
    : m_keyword(keyword.toString()), m_type(CipherType::KEYWORD) {}

QString TrithemiusCipher::encrypt(QStringView src)
{
    QString ciphertext{ src.size(), '\0' };

    for (qsizetype i{ 0 }; i < src.size(); ++i)
    {
        ciphertext[i] = encryptChar(src[i], i);
    }

    return ciphertext;
}

QString TrithemiusCipher::decrypt(QStringView src)
{
    QString plaintext{ src.size(), '\0' };

    for (qsizetype i{ 0 }; i < src.size(); ++i)
    {
        plaintext[i] = decryptChar(src[i], i);
    }

    return plaintext;
}

QChar TrithemiusCipher::encryptChar(QChar c, int pos)
{
    auto index{ kSupportedSymbols.indexOf(c) };

    if (index == -1)
    {
        return c; // Return the original character if it's not in the supported symbols
    }

    auto shift{ calculateShift(pos) };
    auto newIndex{ (index + shift) % kSupportedSymbols.size() };

    return kSupportedSymbols[newIndex];
}

QChar TrithemiusCipher::decryptChar(QChar c, int pos)
{
    auto index{ kSupportedSymbols.indexOf(c) };

    if (index == -1)
    {
        return c; // Return the original character if it's not in the supported symbols
    }

    auto shift{ calculateShift(pos) };
    auto newIndex{ (index - shift + kSupportedSymbols.size()) % kSupportedSymbols.size() };

    return kSupportedSymbols[newIndex];
}

qsizetype TrithemiusCipher::calculateShift(qsizetype pos)
{
    qsizetype shift{ 0 };

    switch (m_type)
    {
    case CipherType::LINEAR:
        shift = m_coefficients[0] * pos + m_coefficients[1];
        break;
    case CipherType::NONLINEAR:
        shift = m_coefficients[0] * m_coefficients[0] * pos + m_coefficients[1] * pos + m_coefficients[2];
        break;
    case CipherType::KEYWORD:
        shift = m_keyword[pos % m_keyword.size()].unicode();
        break;
    }

    return shift % kSupportedSymbols.size();
}



GammaCipher::GammaCipher(QStringView key) : m_gamma{ key.toString() } {}

QString GammaCipher::encrypt(QStringView plainText)
{
    QString cipherText;

    for (int i = 0; i < plainText.size(); ++i)
    {
        int x = kSupportedSymbols.indexOf(plainText.at(i));
        int g = kSupportedSymbols.indexOf(m_gamma.at(i % m_gamma.size()));
        int y = (x + g) % kSupportedSymbols.size();
        cipherText.append(kSupportedSymbols.at(y));
    }

    return cipherText;
}

QString GammaCipher::decrypt(QStringView cipherText)
{
    QString plainText;

    for (int i = 0; i < cipherText.size(); ++i)
    {
        int y = kSupportedSymbols.indexOf(cipherText.at(i));
        int g = kSupportedSymbols.indexOf(m_gamma.at(i % m_gamma.size()));
        int x = (y - g + kSupportedSymbols.size()) % kSupportedSymbols.size();
        plainText.append(kSupportedSymbols.at(x));
    }

    return plainText;
}

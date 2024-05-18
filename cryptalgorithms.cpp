#include <pch.h>
#include "cryptalgorithms.h"
#include <cryptopp/osrng.h>
#include <cryptopp/modes.h>
#include <cryptopp/hex.h>

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

QString CaesarCipher::encrypt(QStringView sourceText, qint8 key)
{
    QString result{ sourceText.toString() };

    for(auto& ch : result)
    {
        tryEncryptChar(ch, kEngSupportedSymbols, key);
        tryEncryptChar(ch, kUkrSupportedSymbols, key);
        tryEncryptChar(ch, kOtherSupportedSymbols, key);
    }

    return result;
}

QString CaesarCipher::decrypt(QStringView encryptedText, qint8 key)
{
    return encrypt(encryptedText, -key);
}

TrithemiusCipher::TrithemiusCipher(QVector<int> coefficients, CipherType type)
    : m_coefficients(std::move(coefficients)), m_type(type) {}

TrithemiusCipher::TrithemiusCipher(QStringView keyword)
    : m_keyword(keyword.toString()), m_type(CipherType::KEYWORD) {}

QString TrithemiusCipher::encrypt(QStringView sourceText)
{
    QString ciphertext{ sourceText.size(), '\0' };

    for (qsizetype i{ 0 }; i < sourceText.size(); ++i)
    {
        ciphertext[i] = encryptChar(sourceText[i], i);
    }

    return ciphertext;
}

QString TrithemiusCipher::decrypt(QStringView encryptedText)
{
    QString plaintext{ encryptedText.size(), '\0' };

    for (qsizetype i{ 0 }; i < encryptedText.size(); ++i)
    {
        plaintext[i] = decryptChar(encryptedText[i], i);
    }

    return plaintext;
}

QChar TrithemiusCipher::encryptChar(QChar c, int pos)
{
    auto index{ kSupportedSymbols.indexOf(c) };

    if (index == -1)
    {
        return c;
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
        return c;
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

QString GammaCipher::encrypt(QStringView sourceText)
{
    QString encryptedText{ sourceText.size(), '\0' };;

    for (qsizetype i{ 0 }; i < sourceText.size(); ++i)
    {
        int x = kSupportedSymbols.indexOf(sourceText.at(i));
        int g = kSupportedSymbols.indexOf(m_gamma.at(i % m_gamma.size()));
        int y = (x + g) % kSupportedSymbols.size();
        encryptedText[i] = kSupportedSymbols.at(y);
    }

    return encryptedText;
}

QString GammaCipher::decrypt(QStringView encryptedText)
{
    QString plainText{ encryptedText.size(), '\0' };

    for (qsizetype i{ 0 }; i < encryptedText.size(); ++i)
    {
        int y = kSupportedSymbols.indexOf(encryptedText.at(i));
        int g = kSupportedSymbols.indexOf(m_gamma.at(i % m_gamma.size()));
        int x = (y - g + kSupportedSymbols.size()) % kSupportedSymbols.size();
        plainText[i] = kSupportedSymbols.at(x);
    }

    return plainText;
}


DESCipher::DESCipher(QStringView key)
{
    auto keyByteArray{ key.toLocal8Bit() };
    memcpy(&m_iv, keyByteArray.data(), CryptoPP::DES_EDE2::BLOCKSIZE);
    m_keyByteBlock = CryptoPP::SecByteBlock(reinterpret_cast<const CryptoPP::byte*>(keyByteArray.data()), keyByteArray.size());
}

QString DESCipher::encrypt(const QString& sourceText)
{
    std::string encryptedText{};

    try
    {
        CryptoPP::CBC_Mode<CryptoPP::DES_EDE2>::Encryption e;
        e.SetKeyWithIV(m_keyByteBlock, m_keyByteBlock.size(), m_iv);
        CryptoPP::StringSource ss1{ sourceText.toStdString(), true, new CryptoPP::StreamTransformationFilter(e, new CryptoPP::StringSink(encryptedText) ) };
    }
    catch(const CryptoPP::Exception& e)
    {
        qDebug() << "Exception while encrypting using DES: " << e.what();
    }

    std::string encodedEncryptedText{};

    CryptoPP::StringSource ss2{ encryptedText, true, new CryptoPP::HexEncoder(new CryptoPP::StringSink(encodedEncryptedText)) };

    return QString::fromStdString(encodedEncryptedText);
}

QString DESCipher::decrypt(const QString& encryptedText)
{
    std::string decodedEncryptedText{};
    CryptoPP::StringSource ss2{ encryptedText.toStdString(), true, new CryptoPP::HexDecoder( new CryptoPP::StringSink(decodedEncryptedText) ) };

    std::string decrypted{};

    try
    {
        CryptoPP::CBC_Mode<CryptoPP::DES_EDE2>::Decryption d;
        d.SetKeyWithIV(m_keyByteBlock, m_keyByteBlock.size(), m_iv);

        CryptoPP::StringSource ss3{ decodedEncryptedText, true, new CryptoPP::StreamTransformationFilter(d, new CryptoPP::StringSink(decrypted) ) }; // StringSource

        qDebug() << "recovered text: " << decrypted;
    }
    catch(const CryptoPP::Exception& e)
    {
        qDebug() << "Exception while decrypting using DES: " << e.what();
    }

    return QString::fromStdString(decrypted);
}

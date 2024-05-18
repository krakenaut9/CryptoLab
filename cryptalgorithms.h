#pragma once

#include <QString>
#include <QStringView>
#include <QVector>
#include <cryptopp/des.h>
static inline QString kEngSupportedSymbols{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" };
static inline QString kUkrSupportedSymbols{ "АБВГҐДЕЄЖЗИІЇЙКЛМНОПРСТУФХЦЧШЩЬЮЯабвгґдеєжзиіїйклмнопрстуфхцчшщьюя" };
static inline QString kOtherSupportedSymbols{ " ~`!@\"№#$;%^:&?*()-+={}[]\\|/<>\',.1234567890" };
static inline QString kSupportedSymbols{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
                                        "АБВГҐДЕЄЖЗИІЇЙКЛМНОПРСТУФХЦЧШЩЬЮЯабвгґдеєжзиіїйклмнопрстуфхцчшщьюя"
                                        " ~`!@\"№#$;%^:&?*()-+={}[]\\|/<>\',.1234567890"};

enum class CryptoAlgorithm : quint8
{
    Caesar,
    Trithemius_Linear,
    Trithemius_Nonlinear,
    Trithemius_Keyword,
    Gamma,
    DES,
};

class CaesarCipher
{
public:
    CaesarCipher() = delete;

    static QString encrypt(QStringView sourceText, qint8 key);
    static QString decrypt(QStringView encryptedText, qint8 key);

private:
    static void tryEncryptChar(QChar& ch, QStringView symbols, qint8 key);
};


class TrithemiusCipher {
public:
    enum class CipherType : quint8
    {
        LINEAR,
        NONLINEAR,
        KEYWORD
    };

    TrithemiusCipher(QVector<int> coefficients, CipherType type);
    TrithemiusCipher(QStringView src);

    QString encrypt(QStringView sourceText);
    QString decrypt(QStringView encryptedText);

private:
    QChar encryptChar(QChar c, int pos);
    QChar decryptChar(QChar c, int pos);
    qsizetype calculateShift(qsizetype pos);

private:
    QVector<int> m_coefficients;
    QString m_keyword;
    CipherType m_type;
};



class GammaCipher {
public:
    GammaCipher(QStringView key);
    QString encrypt(QStringView sourceText);
    QString decrypt(QStringView encryptedText);

private:
    QString m_gamma;
};

class DESCipher {
public:
    DESCipher(QStringView key);

    QString encrypt(const QString& sourceText);
    QString decrypt(const QString& encryptedText);

private:
    CryptoPP::byte m_iv[CryptoPP::DES_EDE2::BLOCKSIZE]{};
    CryptoPP::SecByteBlock m_keyByteBlock{};
};

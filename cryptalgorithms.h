#pragma once

#include <QString>
#include <QStringView>
#include <QVector>

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
};

class CaesarCipher
{
public:
    CaesarCipher() = delete;

    static QString encrypt(QStringView src, qint8 key);
    static QString decrypt(QStringView src, qint8 key);

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

    QString encrypt(QStringView src);
    QString decrypt(QStringView src);

private:
    QChar encryptChar(QChar c, int pos);
    QChar decryptChar(QChar c, int pos);
    qsizetype calculateShift(qsizetype pos);

private:
    QVector<int> m_coefficients;
    QString m_keyword;
    CipherType m_type;
};

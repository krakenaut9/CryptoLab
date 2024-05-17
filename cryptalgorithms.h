#pragma once

#include <QString>
#include <QStringView>

enum class CryptoAlgorithm : quint8
{
    CAESAR,
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

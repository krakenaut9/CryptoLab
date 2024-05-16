#pragma once

#include <QString>
#include <QStringView>

class CaesarCipher
{
public:
    CaesarCipher() = default;

    static QString encrypt(QStringView src, qint8 key);
    static QString decrypt(QStringView src, qint8 key);
};

#include <pch.h>
#include "cryptalgorithms.h"

static inline constexpr QStringView kSupportedSymbols{ u" AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZzАаБбВвГгҐґДдЕеЄєЖжЗзИиІіЇїЙйКкЛлМмНнОоПпРрСсТтУуФфХхЦцЧчШшЩщЬьЮюЯя	" };

QString CaesarCipher::encrypt(QStringView src, qint8 key)
{
    QString result{ src.toString() };
    for(auto& ch : result)
    {
        if(ch.isLetter() || ch.isSpace())
        {
            auto index = kSupportedSymbols.indexOf(ch.toLower());
            ch = kSupportedSymbols[(index + key) % kSupportedSymbols.size()];
        }
    }
    return result;
}

QString CaesarCipher::decrypt(QStringView src, qint8 key)
{
    return encrypt(src, -key);
}

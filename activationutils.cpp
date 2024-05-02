#include "pch.h"
#include "activationutils.h"

static constexpr int  kMaxLaunchCountTrial{ 10 };
static constexpr char kLaunchCounterFileName[]{ "activation.json" };
static constexpr char kLaunchTimes[]{ "Launch times" };
static constexpr char kActivated[]{ "Activated" };
static constexpr char kActivatedByUser[]{ "Activated by user" };
static constexpr char kEncryptedKey[]{ "qyioc Zkurftr" };
static constexpr char kEnctyptionPassword[]{ "sergei korolev" };

std::string_view kAlphabet{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz !-+*/=_@#№;$%^:&?()[]\'\"1234567890" };

static QString encryptVigenere(const QString &plaintext, const QString &keyword)
{
    QString ciphertext = "";
    int plaintextLength = plaintext.length();
    int keywordLength = keyword.length();

    for (int i = 0; i < plaintextLength; i++) {
        QChar currentChar = plaintext[i];

        // Преобразование символа ключа в соответствующий номер (0-25)
        int key = keyword[i % keywordLength].toLatin1() - 'a';

        // Если текущий символ - буква английского алфавита
        if (currentChar.isLetter()) {
            QChar base = currentChar.isUpper() ? 'A' : 'a';
            // Шифрование символа с учетом смещения ключа
            QChar encryptedChar = QChar((currentChar.toLatin1() - base.toLatin1() + key) % 26 + base.toLatin1());
            ciphertext += encryptedChar;
        } else {
            // Если текущий символ - не буква, оставляем его без изменений
            ciphertext += currentChar;
        }
    }

    return ciphertext;
}

static QString decryptVigenere(const QString &ciphertext, const QString &keyword)
{
    QString plaintext = "";
    int ciphertextLength = ciphertext.length();
    int keywordLength = keyword.length();

    for (int i = 0; i < ciphertextLength; i++) {
        QChar currentChar = ciphertext[i];

        // Преобразование символа ключа в соответствующий номер (0-25)
        int key = keyword[i % keywordLength].toLatin1() - 'a';

        // Если текущий символ - буква английского алфавита
        if (currentChar.isLetter()) {
            QChar base = currentChar.isUpper() ? 'A' : 'a';
            // Расшифровка символа с учетом смещения ключа
            QChar decryptedChar = QChar((currentChar.toLatin1() - base.toLatin1() - key + 26) % 26 + base.toLatin1());
            plaintext += decryptedChar;
        } else {
            // Если текущий символ - не буква, оставляем его без изменений
            plaintext += currentChar;
        }
    }

    return plaintext;
}

static void setNewActivationData(QFile& launchCounterFile, int launchTimes, bool activated = false, const QString activatedByUser = "")
{
    launchCounterFile.resize(0);
    QJsonObject rootObject;
    rootObject.insert(kLaunchTimes, launchTimes);
    rootObject.insert(kActivated, activated);
    rootObject.insert(kActivatedByUser, activatedByUser);
    QJsonDocument jsonDoc(rootObject);
    qDebug() << jsonDoc.toJson(QJsonDocument::Indented);
    launchCounterFile.write(jsonDoc.toJson(QJsonDocument::Indented));
}

static void parseActivationRootObject(const QJsonObject& rootObject, int& launchCount, bool& activated, QString& activatedByUser)
{
    launchCount = rootObject[kLaunchTimes].toInt();
    activated = rootObject[kActivated].toBool();
    activatedByUser = rootObject[kActivatedByUser].toString();
}

bool Activation::activate(QFile& launchCounterFile, const QString& userName, bool atStart)
{
    static const QString commonMessage{ "Please enter the activation key:" };
    static const QString messageAtStart{ "You have exceeded the number of launches.\n" + commonMessage };
    bool ok{ true };
    QString inputKey{};
    do
    {
        inputKey = QInputDialog::getText(nullptr, "Key", atStart ? messageAtStart : commonMessage, QLineEdit::Normal, "", &ok);
        if(!ok)
        {
            return false;
        }

        if(encryptVigenere(inputKey, kEnctyptionPassword) == kEncryptedKey)
        {
            qDebug() << "The key is correct. Activvating...";
            PLOGI << "The key is correct. Activvating...";

            setNewActivationData(launchCounterFile, 0, true, userName);

            QMessageBox::information(nullptr, "Activated", "The product successfully activated");
            return true;
        }
        else
        {
            qDebug() << "The key is not correct: " << inputKey << ". Encrypted " << encryptVigenere(inputKey, kEnctyptionPassword) << " != " << kEncryptedKey;
            PLOGI  << "The key is not correct: " << inputKey << ". Encrypted " << encryptVigenere(inputKey, kEnctyptionPassword) << " != " << kEncryptedKey;

            QMessageBox::warning(nullptr, "Incorrect key", "This key is not correct");
        }
    } while(ok);

    return false;
}

bool Activation::getActivationFile(QFile& launchCounterFile, bool* newFile)
{
    launchCounterFile.setFileName(kLaunchCounterFileName);
    bool toCreate = !launchCounterFile.exists();

    if(newFile)
    {
        *newFile = toCreate;
    }

    if(!launchCounterFile.open(QIODeviceBase::ReadWrite | QIODeviceBase::Text | (toCreate ? QIODevice::NewOnly : QIODevice::ExistingOnly)))
    {
        qDebug() << "Failed to open launchFileCounter: " << kLaunchCounterFileName;
        PLOGE    << "Failed to open launchFileCounter: " << kLaunchCounterFileName;

        QMessageBox::information(0, "error", launchCounterFile.errorString());
        return false;
    }

    return true;
}

bool Activation::tryIncrementLaunchCounter(const QString& userName, bool& activated)
{
    static_assert(kMaxLaunchCountTrial >= 1, "kMaxLaunchCountTrial must be greater then 0");

    QFile launchCounterFile{};
    bool newFile{ false };
    if(!getActivationFile(launchCounterFile, &newFile))
    {
        return false;
    }

    if(newFile)
    {
        setNewActivationData(launchCounterFile, 1);
        return true;
    }


    QJsonParseError JsonParseError;
    auto jsonDocument = QJsonDocument::fromJson(launchCounterFile.readAll(), &JsonParseError);

    QJsonObject rootObject = jsonDocument.object();
    int launchCount{ INT_MAX };
    QString activatedByUser{};
    parseActivationRootObject(rootObject, launchCount, activated, activatedByUser);
    if(activated)
    {
        qDebug() << "Bypass activated user";
        PLOGE    << "Bypass activated user";
        return true;
    }
    else if(launchCount >= kMaxLaunchCountTrial)
    {
        return activated = activate(launchCounterFile, userName, true);
    }

    setNewActivationData(launchCounterFile, ++launchCount);

    return true;
}

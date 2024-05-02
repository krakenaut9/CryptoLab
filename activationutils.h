#pragma once

namespace Activation
{
    bool tryIncrementLaunchCounter(const QString& userName, bool& activated);
    bool activate(QFile& launchCounterFile, const QString& userName, bool atStart);
    bool getActivationFile(QFile& launchCounterFile, bool* newFile = nullptr);
}

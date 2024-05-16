#pragma once

#include <QDialog>

namespace Ui {
class FileInfoWindow;
}

class FileInfoWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FileInfoWindow(const QString& fileName, QWidget *parent = nullptr);
    ~FileInfoWindow();

private:
    Ui::FileInfoWindow *ui;
    QString m_fileName;
};

#pragma once

#include <QMainWindow>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void aboutAuthor();
    void usedTechnologies();
    void openFile();
    void newFile();
    void saveFile();
    void saveAsFile();
    void parameters();
    void printFile();

    void encryptButtonPressed();
    void decryptButtonPressed();

    void chosenAlgorithmChanged(int index);

    void swapPlainTexts();
    void clearSourcePlainText();
    void clearResultPlainText();

private:
    Ui::MainWindow *ui;
    QFile m_sourceFile;
    QFile m_resultFile;
};

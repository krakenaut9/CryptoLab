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

private:
    Ui::MainWindow *ui;
    QFile m_file;
};

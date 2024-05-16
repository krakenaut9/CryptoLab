#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <aboutwindow.h>
#include <technologieswindow.h>
#include <fileinfowindow.h>
#include <pch.h>
#include <QFileDialog>
#include <QProgressDialog>
#include <QProgressBar>
#include <QThread>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->statusbar->showMessage("User : " + m_userName);

    ui->actionNew->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newFile);

    ui->actionOpen->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);

    ui->actionSave->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveFile);

    ui->actionSave_as->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
    connect(ui->actionSave_as, &QAction::triggered, this, &MainWindow::saveAsFile);

    ui->actionParameters->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    connect(ui->actionParameters, &QAction::triggered, this, &MainWindow::parameters);

    ui->actionPrint->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));
    connect(ui->actionPrint, &QAction::triggered, this, &MainWindow::printFile);


    auto menuMyAccount = ui->menubar->addMenu("My account");

    //auto actionChangePassword = menuMyAccount->addAction("Change password");
    //connect(actionChangePassword, &QAction::triggered, this, &MainWindow::changeMyPassword);

    menuMyAccount->addSeparator();

    auto actionLogOut = menuMyAccount->addAction("Log out");
    connect(actionLogOut, &QAction::triggered, this, &QApplication::quit);

    auto menuAbout = ui->menubar->addMenu("About");

    auto actionAboutAuthor = menuAbout->addAction("About author");
    connect(actionAboutAuthor, &QAction::triggered, this, &MainWindow::aboutAuthor);
    auto actionUsedTechnologies = menuAbout->addAction("Used technologies");
    connect(actionUsedTechnologies, &QAction::triggered, this, &MainWindow::usedTechnologies);

    setWindowTitle(QString("Main Window :  (") + UNTITLED + ')');
}

MainWindow::~MainWindow()
{
    delete ui;
    PLOGI << "Program finished : ";
}

void MainWindow::openFile()
{
    qDebug() << "Main window : Open file";
    PLOGI <<    "Main window : Open file";
    if(ui->plainTextEdit->document()->isModified())
    {
        auto answer = QMessageBox::question(this, "Save file", "Do you want to save changes?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if(answer == QMessageBox::Yes)
        {
            saveFile();
        }
        else if(answer == QMessageBox::No)
        {
            qDebug() << "Main window : Don't save file";
            PLOGI <<  "Main window : Don't save file";
        }
        else
        {
            qDebug() << "Main window : New file canceled";
            PLOGI << "Main window : New file canceled";
            return;
        }
    }

    QString filePath = QFileDialog::getOpenFileName(this, "Open file", QDir::currentPath());
    if(!filePath.isEmpty())
    {
        qDebug() << "Main window :  tries to open file : " << filePath;
        PLOGI << "Main window :  tries to open file : " << filePath;
        m_file.setFileName(filePath);
        if(!m_file.open(QFile::ReadOnly | QIODevice::Text | QIODevice::ExistingOnly))
        {
            qDebug() << "Main window : Unable to open file " << m_file.fileName();
            PLOGE << "Main window : Unable to open file " << m_file.fileName();
            QMessageBox::critical(this, "Open failure", "Unable to open file" + m_file.fileName());
            return;
        }
        ui->plainTextEdit->setPlainText(m_file.readAll());
        ui->plainTextEdit->moveCursor(QTextCursor::End);
        setWindowTitle("Main Window : (" + QFileInfo(filePath).fileName() + ')');
    }
    else
    {
        qDebug() << "Main window : File open rejected";
        PLOGI << "Main window : File open rejected";
    }

}

void MainWindow::newFile()
{
    qDebug() << "Main window : New file";
    PLOGI <<    "Main window : New file";
    if(ui->plainTextEdit->document()->isModified())
    {
        auto answer = QMessageBox::question(this, "Save file", "Do you want to save changes?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if(answer == QMessageBox::Yes)
        {
            saveFile();
        }
        else if(answer == QMessageBox::No)
        {
            qDebug() << "Main window : Don't save file";
            PLOGI << "Main window : Don't save file";
        }
        else
        {
            qDebug() << "Main window : New file canceled";
            PLOGI << "Main window : New file canceled";
            return;
        }
    }
    ui->plainTextEdit->clear();
    m_file.close();
    m_file.setFileName("");
    setWindowTitle(QString("Main Window :  (") + UNTITLED + ')');
}

void MainWindow::saveFile()
{
    qDebug() << "Main window : Save file";
    PLOGI <<    "Main window : Save file";
    if(!m_file.exists())
    {
        qDebug() << "Main window : File doesn't exist";
        PLOGI << "Main window : File doesn't exist";
        saveAsFile();
        return;
    }
    m_file.flush();
    m_file.close();
    if(!m_file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
    {
        qDebug() << "Main window : Unable to open file " << m_file.fileName();
        PLOGE << "Main window : Unable to open file " << m_file.fileName();
        QMessageBox::critical(this, "Open failure", "Unable to open file" + m_file.fileName());
        return;
    }
    QTextStream fileStream(&m_file);
    fileStream << ui->plainTextEdit->toPlainText();
    ui->plainTextEdit->document()->setModified(false);
}

void MainWindow::saveAsFile()
{
    qDebug() << "Main window : Save as file";
    PLOGI <<    "Main window : Save as file";
    auto newFilePath = QFileDialog::getSaveFileName(this, "Save as", QDir::currentPath(), "Text files (*.txt)");
    if(newFilePath.isEmpty())
    {
        qDebug() << "Save as window rejected";
        PLOGI << "Save as window rejected";
        return;
    }
    qDebug() << "Main window : Save as path " << newFilePath;
    if(m_file.isOpen())
    {
        m_file.flush();
        m_file.close();
    }
    m_file.setFileName(newFilePath);
    if(!m_file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
    {
        qDebug() << "Main window : Unable to open file " << m_file.fileName();
        PLOGE << "Main window : Unable to open file " << m_file.fileName();
        QMessageBox::critical(this, "Open failure", "Unable to open file" + m_file.fileName());
        return;
    }
    QTextStream fileStream(&m_file);
    fileStream << ui->plainTextEdit->toPlainText();
    ui->plainTextEdit->document()->setModified(false);
    setWindowTitle(QString("Main Window : ") + " (" + QFileInfo(newFilePath).fileName() + ')');
}

void MainWindow::parameters()
{
    qDebug() << "Main window : File parameters";
    PLOGI <<    "Main window : File parameters";
    if(!m_file.exists())
    {
        qDebug() << "You haven't opened any file yet";
        PLOGW << "You haven't opened any file yet";
        QMessageBox::warning(this, "No file", "You haven't opened any file yet");
        return;
    }
    FileInfoWindow fileInfoW(m_file.fileName());
    fileInfoW.setModal(true);
    fileInfoW.exec();
}
void MainWindow::printFile()
{
    qDebug() << "Main window : Print file";
    PLOGI <<    "Main window : Print file";
    QProgressDialog pd(this);
    pd.setRange(0,100);
    pd.setLabelText("Printing the text");
    pd.setWindowTitle("Printing");
    pd.setFixedSize(400, 100);
    QProgressBar pb;
    pd.setBar(&pb);
    pd.setModal(true);
    pd.setCancelButton(nullptr);
    pd.show();
    int progress = 0;

    for(int i=0;i<100; ++i)
    {
        QThread::msleep(30);
        QApplication::processEvents();
        ++progress;
        pd.setValue(progress);
    }
    pd.deleteLater();
}

void MainWindow::aboutAuthor()
{
    qDebug()<<"About author";
    AboutWindow aboutWindow;
    aboutWindow.setModal(true);
    aboutWindow.exec();
}

void MainWindow::usedTechnologies()
{
    qDebug()<<"Used technologies";
    TechnologiesWindow techWindow;
    techWindow.setModal(true);
    techWindow.exec();
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <aboutwindow.h>
#include <technologieswindow.h>
#include <fileinfowindow.h>
#include <pch.h>
#include <QRadioButton>
#include <cryptalgorithms.h>

static constexpr char kUntitledName[] = "untitled";

static void clearLayout(QLayout* layout)
{
    QLayoutItem *child{};
    while ((child = layout->takeAt(0)))
    {
        delete child->widget();
        delete child;
    }
}

static void setupCaesarKeyGroupBoxLayout(QGroupBox* gbox)
{
    auto layout{ gbox->layout() };

    auto spinBox{ new QSpinBox(gbox) };
    //spinBox->setRange(CaesarCipher::getMaxShift(), CaesarCipher::getMaxShift());
    spinBox->setValue(1);

    layout->addWidget(spinBox);
}

static void setupTrithemiusLinearKeyGroupBoxLayout(QGroupBox* gbox)
{
    auto layout{ gbox->layout() };

    auto spinBoxA{ new QSpinBox(gbox) };
    layout->addWidget(spinBoxA);

    auto spinBoxB{ new QSpinBox(gbox) };
    layout->addWidget(spinBoxB);
}

static void setupTrithemiusNonlinearKeyGroupBoxLayout(QGroupBox* gbox)
{
    auto layout{ gbox->layout() };

    auto spinBoxA{ new QSpinBox(gbox) };
    layout->addWidget(spinBoxA);

    auto spinBoxB{ new QSpinBox(gbox) };
    layout->addWidget(spinBoxB);

    auto spinBoxC{ new QSpinBox(gbox) };
    layout->addWidget(spinBoxC);
}

static void setupTrithemiusKeywordKeyGroupBoxLayout(QGroupBox* gbox)
{
    auto layout{ gbox->layout() };

    auto keywordLineEdit{ new QLineEdit(gbox) };
    keywordLineEdit->setPlaceholderText("Keyword");

    layout->addWidget(keywordLineEdit);
}

static void setupTrithemiusGammaKeyGroupBoxLayout(QGroupBox* gbox)
{
    auto layout{ gbox->layout() };

    auto GammaLineEdit{ new QLineEdit(gbox) };
    GammaLineEdit->setPlaceholderText("Gamma");

    layout->addWidget(GammaLineEdit);
}

static void setupKeyGroupBox(QGroupBox* gbox, CryptoAlgorithm algorithm = CryptoAlgorithm::Caesar)
{
    if(auto layout{ gbox->layout() }; layout)
    {
        clearLayout(layout);
    }
    else
    {
        gbox->setLayout(new QHBoxLayout);
    }

    switch (algorithm)
    {
    case CryptoAlgorithm::Caesar:
        setupCaesarKeyGroupBoxLayout(gbox);
        break;
    case CryptoAlgorithm::Trithemius_Linear:
        setupTrithemiusLinearKeyGroupBoxLayout(gbox);
        break;
    case CryptoAlgorithm::Trithemius_Nonlinear:
        setupTrithemiusNonlinearKeyGroupBoxLayout(gbox);
        break;
    case CryptoAlgorithm::Trithemius_Keyword:
        setupTrithemiusKeywordKeyGroupBoxLayout(gbox);
        break;
    case CryptoAlgorithm::Gamma:
        setupTrithemiusGammaKeyGroupBoxLayout(gbox);
        break;
    default:
        qDebug() << "Unknown algorithm id: " << static_cast<int>(algorithm);
        clearLayout(gbox->layout());
        break;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    auto menuAbout = ui->menubar->addMenu("About");

    auto actionAboutAuthor = menuAbout->addAction("About author");
    connect(actionAboutAuthor, &QAction::triggered, this, &MainWindow::aboutAuthor);
    auto actionUsedTechnologies = menuAbout->addAction("Used technologies");
    connect(actionUsedTechnologies, &QAction::triggered, this, &MainWindow::usedTechnologies);


    auto actionExit = ui->menubar->addAction("Exit");
    connect(actionExit, &QAction::triggered, this, &QApplication::quit);

    ui->sourcePlainTextEdit->setPlaceholderText("Your text to process. Press Encrypt button or CTRL+E to encrypt text and Decrypt button or CTRL+D to decrypt.");
    ui->destinationPlainTextEdit->setPlaceholderText("Processed text");
    connect(ui->swapButton, &QPushButton::released, this, &MainWindow::swapPlainTexts);

    connect(ui->encryptButton, &QPushButton::released, this, &MainWindow::encryptButtonPressed);
    connect(ui->decryptButton, &QPushButton::released, this, &MainWindow::decryptButtonPressed);
    ui->encryptButton->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    ui->decryptButton->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));

    setupKeyGroupBox(ui->keyGroupBox);
    ui->keyGroupBox->setMaximumWidth(150);

    ui->algorithmComboBox->addItem("Caesar");
    ui->algorithmComboBox->addItem("Trithemius (Linear)");
    ui->algorithmComboBox->addItem("Trithemius (Nonlinear)");
    ui->algorithmComboBox->addItem("Trithemius (Keyword)");
    ui->algorithmComboBox->addItem("Gamma");
    //ui->algorithmComboBox->addItem("Test");
    connect(ui->algorithmComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::chosenAlgorithmChanged);

    setWindowTitle(QString("Main Window :  (") + kUntitledName + ')');
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

    QString filePath = QFileDialog::getOpenFileName(this, "Open file", QDir::currentPath());
    if(!filePath.isEmpty())
    {
        qDebug() << "Main window :  tries to open file : " << filePath;
        PLOGI << "Main window :  tries to open file : " << filePath;
        m_sourceFile.setFileName(filePath);
        if(!m_sourceFile.open(QFile::ReadOnly | QIODevice::Text | QIODevice::ExistingOnly))
        {
            qDebug() << "Main window : Unable to open file " << m_sourceFile.fileName();
            PLOGE << "Main window : Unable to open file " << m_sourceFile.fileName();
            QMessageBox::critical(this, "Open failure", "Unable to open file" + m_sourceFile.fileName());
            return;
        }
        ui->sourcePlainTextEdit->setPlainText(m_sourceFile.readAll());
        //ui->sourcePlainTextEdit->moveCursor(QTextCursor::End);
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

    ui->sourcePlainTextEdit->clear();
    m_sourceFile.close();
    m_sourceFile.setFileName("");
    setWindowTitle(QString("Main Window :  (") + kUntitledName + ')');
}

void MainWindow::saveFile()
{
    qDebug() << "Main window : Save file";
    PLOGI <<    "Main window : Save file";
    if(!m_resultFile.exists())
    {
        qDebug() << "Main window : File doesn't exist";
        PLOGI << "Main window : File doesn't exist";
        saveAsFile();
        return;
    }
    m_resultFile.flush();
    m_resultFile.close();
    if(!m_resultFile.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
    {
        qDebug() << "Main window : Unable to open file " << m_resultFile.fileName();
        PLOGE << "Main window : Unable to open file " << m_resultFile.fileName();
        QMessageBox::critical(this, "Open failure", "Unable to open file" + m_resultFile.fileName());
        return;
    }
    QTextStream fileStream(&m_resultFile);
    fileStream << ui->destinationPlainTextEdit->toPlainText();
    ui->destinationPlainTextEdit->document()->setModified(false);
    ui->statusbar->showMessage("Result file : " + m_resultFile.fileName());
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
    if(m_resultFile.isOpen())
    {
        m_resultFile.flush();
        m_resultFile.close();
    }
    m_resultFile.setFileName(newFilePath);
    if(!m_resultFile.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
    {
        qDebug() << "Main window : Unable to open file " << m_resultFile.fileName();
        PLOGE << "Main window : Unable to open file " << m_resultFile.fileName();
        QMessageBox::critical(this, "Open failure", "Unable to open file" + m_resultFile.fileName());
        return;
    }
    QTextStream fileStream(&m_resultFile);
    fileStream << ui->destinationPlainTextEdit->toPlainText();
    ui->destinationPlainTextEdit->document()->setModified(false);
    setWindowTitle(QString("Main Window : ") + " (" + QFileInfo(newFilePath).fileName() + ')');
    ui->statusbar->showMessage("Result file : " + m_resultFile.fileName());
}

void MainWindow::parameters()
{
    qDebug() << "Main window : File parameters";
    PLOGI <<    "Main window : File parameters";
    if(!m_sourceFile.exists())
    {
        qDebug() << "You haven't opened any file yet";
        PLOGW << "You haven't opened any file yet";
        QMessageBox::warning(this, "No file", "You haven't opened any file yet");
        return;
    }
    FileInfoWindow fileInfoW(m_sourceFile.fileName());
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


void MainWindow::encryptButtonPressed()
{
    qDebug() << "Encrypt button pressed";
    auto sourceText{ ui->sourcePlainTextEdit->toPlainText() };

    if(sourceText.isEmpty())
    {
        qDebug() << "Source plain text is empty. Do nothing";
        return;
    }

    auto keyGroupBoxChildren{ ui->keyGroupBox->children() };
    auto algorithm{ static_cast<CryptoAlgorithm>(ui->algorithmComboBox->currentIndex()) };

    qDebug() << "Children count: " << keyGroupBoxChildren.size();

    if(algorithm == CryptoAlgorithm::Caesar)
    {
        auto spinBox{ qobject_cast<QSpinBox*>(keyGroupBoxChildren.at(1)) };
        assert(spinBox);
        qDebug() << spinBox->value();
        auto encryptedText{ CaesarCipher::encrypt(sourceText, spinBox->value()) };
        ui->destinationPlainTextEdit->setPlainText(encryptedText);
    }
    else if(algorithm == CryptoAlgorithm::Trithemius_Linear)
    {
        auto spinBoxA{ qobject_cast<QSpinBox*>(keyGroupBoxChildren.at(1)) };
        assert(spinBoxA);
        qDebug() << spinBoxA->value();

        auto spinBoxB{ qobject_cast<QSpinBox*>(keyGroupBoxChildren.at(2)) };
        assert(spinBoxB);
        qDebug() << spinBoxB->value();

        TrithemiusCipher cipher{ { spinBoxA->value(), spinBoxB->value() }, TrithemiusCipher::CipherType::LINEAR };

        auto encryptedText{cipher.encrypt(sourceText) };
        ui->destinationPlainTextEdit->setPlainText(encryptedText);
    }
    else if(algorithm == CryptoAlgorithm::Trithemius_Nonlinear)
    {
        auto spinBoxA{ qobject_cast<QSpinBox*>(keyGroupBoxChildren.at(1)) };
        assert(spinBoxA);
        qDebug() << spinBoxA->value();

        auto spinBoxB{ qobject_cast<QSpinBox*>(keyGroupBoxChildren.at(2)) };
        assert(spinBoxB);
        qDebug() << spinBoxB->value();

        auto spinBoxC{ qobject_cast<QSpinBox*>(keyGroupBoxChildren.at(3)) };
        assert(spinBoxC);
        qDebug() << spinBoxC->value();

        TrithemiusCipher cipher{ { spinBoxA->value(), spinBoxB->value(), spinBoxC->value() }, TrithemiusCipher::CipherType::NONLINEAR };

        auto encryptedText{cipher.encrypt(sourceText) };
        ui->destinationPlainTextEdit->setPlainText(encryptedText);
    }
    else if(algorithm == CryptoAlgorithm::Trithemius_Keyword)
    {
        auto keywordLineEdit{ qobject_cast<QLineEdit*>(keyGroupBoxChildren.at(1)) };
        assert(keywordLineEdit);
        qDebug() << keywordLineEdit->text();

        TrithemiusCipher cipher{ keywordLineEdit->text() };

        auto encryptedText{ cipher.encrypt(sourceText) };
        ui->destinationPlainTextEdit->setPlainText(encryptedText);
    }
    else if(algorithm == CryptoAlgorithm::Gamma)
    {
        auto gammaLineEdit{ qobject_cast<QLineEdit*>(keyGroupBoxChildren.at(1)) };
        assert(gammaLineEdit);
        qDebug() << gammaLineEdit->text();

        GammaCipher cipher{ gammaLineEdit->text() };

        auto encryptedText{ cipher.encrypt(sourceText) };
        ui->destinationPlainTextEdit->setPlainText(encryptedText);
    }
}

void MainWindow::decryptButtonPressed()
{
    qDebug() << "Decrypt button pressed";
    auto sourceText{ ui->sourcePlainTextEdit->toPlainText() };

    if(sourceText.isEmpty())
    {
        qDebug() << "Source plain text is empty. Do nothing";
        return;
    }

    auto keyGroupBoxChildren{ ui->keyGroupBox->children() };
    auto algorithm{ static_cast<CryptoAlgorithm>(ui->algorithmComboBox->currentIndex()) };
    qDebug() << "Children count: " << keyGroupBoxChildren.size();
    if(algorithm == CryptoAlgorithm::Caesar)
    {
        auto spinBox{ qobject_cast<QSpinBox*>(keyGroupBoxChildren.at(1)) };
        assert(spinBox);
        qDebug() << spinBox->value();
        auto decryptedText{ CaesarCipher::decrypt(sourceText, spinBox->value()) };
        ui->destinationPlainTextEdit->setPlainText(decryptedText);
    }
    else if(algorithm == CryptoAlgorithm::Trithemius_Linear)
    {
        auto spinBoxA{ qobject_cast<QSpinBox*>(keyGroupBoxChildren.at(1)) };
        assert(spinBoxA);
        qDebug() << spinBoxA->value();

        auto spinBoxB{ qobject_cast<QSpinBox*>(keyGroupBoxChildren.at(2)) };
        assert(spinBoxB);
        qDebug() << spinBoxB->value();

        TrithemiusCipher cipher{ { spinBoxA->value(), spinBoxB->value() }, TrithemiusCipher::CipherType::LINEAR };

        auto encryptedText{cipher.decrypt(sourceText) };
        ui->destinationPlainTextEdit->setPlainText(encryptedText);
    }
    else if(algorithm == CryptoAlgorithm::Trithemius_Nonlinear)
    {
        auto spinBoxA{ qobject_cast<QSpinBox*>(keyGroupBoxChildren.at(1)) };
        assert(spinBoxA);
        qDebug() << spinBoxA->value();

        auto spinBoxB{ qobject_cast<QSpinBox*>(keyGroupBoxChildren.at(2)) };
        assert(spinBoxB);
        qDebug() << spinBoxB->value();

        auto spinBoxC{ qobject_cast<QSpinBox*>(keyGroupBoxChildren.at(3)) };
        assert(spinBoxC);
        qDebug() << spinBoxC->value();

        TrithemiusCipher cipher{ { spinBoxA->value(), spinBoxB->value(), spinBoxC->value() }, TrithemiusCipher::CipherType::NONLINEAR };

        auto encryptedText{cipher.decrypt(sourceText) };
        ui->destinationPlainTextEdit->setPlainText(encryptedText);
    }
    else if(algorithm == CryptoAlgorithm::Trithemius_Keyword)
    {
        auto keywordLineEdit{ qobject_cast<QLineEdit*>(keyGroupBoxChildren.at(1)) };
        assert(keywordLineEdit);
        qDebug() << keywordLineEdit->text();

        auto keyword{ keywordLineEdit->text() };

        TrithemiusCipher cipher{ keyword };

        auto encryptedText{ cipher.decrypt(sourceText) };
        ui->destinationPlainTextEdit->setPlainText(encryptedText);
    }
    else if(algorithm == CryptoAlgorithm::Gamma)
    {
        auto gammaLineEdit{ qobject_cast<QLineEdit*>(keyGroupBoxChildren.at(1)) };
        assert(gammaLineEdit);
        qDebug() << gammaLineEdit->text();

        GammaCipher cipher{ gammaLineEdit->text() };

        auto encryptedText{ cipher.decrypt(sourceText) };
        ui->destinationPlainTextEdit->setPlainText(encryptedText);
    }
}

void MainWindow::chosenAlgorithmChanged(int index)
{
    auto algorithm{ static_cast<CryptoAlgorithm>(index) };
    setupKeyGroupBox(ui->keyGroupBox, algorithm);
}

void MainWindow::swapPlainTexts()
{
    qDebug() << "Swap plain texts button pressed";

    auto srcText{ ui->sourcePlainTextEdit->toPlainText() };
    ui->sourcePlainTextEdit->setPlainText(ui->destinationPlainTextEdit->toPlainText());
    ui->destinationPlainTextEdit->setPlainText(srcText);
}

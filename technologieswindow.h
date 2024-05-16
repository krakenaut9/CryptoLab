#pragma once

#include <QDialog>

namespace Ui {
class TechnologiesWindow;
}

class TechnologiesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TechnologiesWindow(QWidget *parent = nullptr);
    ~TechnologiesWindow();
public slots:
    void aboutQT();
private:
    Ui::TechnologiesWindow *ui;
};

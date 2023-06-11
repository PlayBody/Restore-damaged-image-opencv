#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TransShift.h"

class TransShift : public QMainWindow
{
    Q_OBJECT

public:
    TransShift(QWidget *parent = nullptr);
    ~TransShift();

private:
    Ui::TransShiftClass ui;

private slots:
    void onBtnLoadImageClicked();
    void onBtnLoadPatchClicked();
    void onBtnStart();
    void onBtnSave();
};

#include "TransShift.h"
#include <QFileDialog>

#include "EngineCV.h"

TransShift::TransShift(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.BTN_LOAD_IMG, SIGNAL(clicked()), this, SLOT(onBtnLoadImageClicked()));
    connect(ui.BTN_LOAD_PATCH, SIGNAL(clicked()), this, SLOT(onBtnLoadPatchClicked()));
    connect(ui.BTN_START, SIGNAL(clicked()), this, SLOT(onBtnStart()));
}

TransShift::~TransShift()
{}


void TransShift::onBtnLoadImageClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "D:\\venezia\\venezia", ("Images (*.png *.xpm *.jpg)"));
    if (fileName.length() == 0) {
        return;
    }
    EngineCV::setImagePath(fileName.toStdString());
    ui.EDIT_PATH->setText(fileName);
}

void TransShift::onBtnLoadPatchClicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Image"), "D:\\venezia\\venezia\\patch1", ("Images (*.png *.xpm *.jpg)"));
    if (fileNames.count() == 0) {
        return;
    }

    ui.TBL_LIST->clearContents();
    ui.TBL_LIST->setRowCount(fileNames.count());
    int i, len = fileNames.count();
    vector<string> patches;
    for (i = 0; i < len; i++) {
        ui.TBL_LIST->setItem(i, 0, new QTableWidgetItem(fileNames[i]));
        patches.push_back(fileNames[i].toStdString());
    }
    EngineCV::setpatchsPath(patches);
}

Mat matOut;

void TransShift::onBtnStart()
{
    string err;
    int iter = ui.SPIN_ITER->value();
    float ratio = ui.SPIN_RATIO->value();
    double thresh = ui.SPIN_THRESH->value();
    bool flag = EngineCV::run(err, matOut, ratio, iter, thresh);
    ui.EDIT_LOG->setText(QString::fromStdString(err));
}

void TransShift::onBtnSave()
{
    if (matOut.empty()) {
        ui.EDIT_LOG->setText("No Result");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "fix.png", ("Images (*.png *.jpg)"));
    string name;
    if (fileName.length() > 0) {
        name = fileName.toStdString();
    }
    imwrite(name, matOut);
}

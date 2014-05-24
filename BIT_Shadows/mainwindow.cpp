#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Utilities.h"

#include <QFileDialog>
#include <QFileInfo>

#include <QMessageBox>

// opencv
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_originalDir(),
    m_method1Dir(),
    m_method2Dir(),
    method1Done(false),
    method2Done(false)
{
    ui->setupUi(this);
    ui->pushButtonImage->setEnabled(false);
    ui->pushButtonMethod1->setEnabled(false);
    ui->pushButtonMethod2->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


bool MainWindow::updateCombobox(){

    ui->comboBoxImage->clear();

    for (int i = 0; i < m_frameList.size(); ++i){
        QString tmp = m_frameList.at(i).split("/").last();
        ui->comboBoxImage->addItem(tmp, QVariant(m_frameList.at(i)));
    }
    return true;
}

void MainWindow::extractImages()
{
    bool failed(false);
    // let the user choose a video
    QString file = QFileDialog::getOpenFileName(this, QString::fromUtf8("Video wählen"), QDir::home().dirName());
    if(!file)
        return;

    // create a directory for the frames
    QFileInfo info(file);
    QString name = info.fileName().split(".")[0];
    m_originalDir = info.absoluteDir();
    failed = !(m_originalDir.mkpath(name));
    m_originalDir.setPath(m_originalDir.path()+"/"+name);
    QString path = m_originalDir.path();

    // call in Utilities.cpp for method
    // extract the frames
    m_frameList = extractFrames(file, path);

    if(m_frameList.count() < 2){
         std::cout << "Fehler beim Speichern der Frames." << std::endl;
         failed = true;
    }

    // update combobox with image names
    if(!this->updateCombobox()){
        std::cout << "Fehler beim Aktualisieren der Combobox." << std::endl;
        failed = true;
    }


    // display error if not successful
    if(failed)
        QMessageBox::critical(this, "Fehler", "Das Video konnte nicht geladen werden.");
    else
    {
        ui->pushButtonImage->setEnabled(true);
        ui->pushButtonMethod1->setEnabled(true);
        ui->pushButtonMethod2->setEnabled(true);
    }
}

// combobox value is path to frame
bool MainWindow::loadOriginalImage(){
    //   ui.comboBoxSheetSize->itemData(index).toInt();

    int index = ui->comboBoxImage->currentIndex();
    QString path = ui->comboBoxImage->itemData(index).toString();

    QImage originalImage;
    originalImage.load(path);

    ui->labelOriginal->setPixmap(QPixmap::fromImage(originalImage));
    return true;
}


void MainWindow::displayImage()
{
    // load original image into pixmap and display
    if(!loadOriginalImage()){
        ui->labelOriginal->setText(QString::fromUtf8("Hier wäre das Originalbild"));
    }

    // load segmentation image made with method 1 into pixmap and display
    if(method1Done)
    {
        // todo: load into pixmap and use setPixmap() instead of setText()
        //ui->labelMethod1->setPixmap();
        ui->labelMethod1->setText(QString::fromUtf8("Hier wäre Segmentierung 1"));
    }
    else
        ui->labelMethod1->setText("Keine Segmentierung vorhanden");

    // load segmentation image made with method 2 into pixmap and display
    if(method2Done)
    {
        // todo: load into pixmap and use setPixmap() instead of setText()
        //ui->labelMethod1->setPixmap();
        ui->labelMethod2->setText(QString::fromUtf8("Hier wäre Segmentierung 2"));
    }
    else
        ui->labelMethod2->setText("Keine Segmentierung vorhanden");
}

void MainWindow::executeMethod1()
{
    bool failed(false);
    // create a new sub-directory
    failed = !(m_originalDir.mkpath("Method1"));
    m_method1Dir.setPath(m_originalDir.path()+"/Method1");

    // todo: execute the algorithm and save every frame into the sub-directory

    // display error if not successful
    if(failed)
        QMessageBox::critical(this, "Fehler", "Die Segmentierung konnte nicht durchgeführt werden.");
    else
        method1Done = true;
}

void MainWindow::executeMethod2()
{
    bool failed(false);
    // create a new sub-directory
    failed = !(m_originalDir.mkpath("Method2"));
    m_method2Dir.setPath(m_originalDir.path()+"/Method2");

    // todo: execute the algorithm and save every frame into the sub-directory

    // display error if not successful
    if(failed)
        QMessageBox::critical(this, "Fehler", "Die Segmentierung konnte nicht durchgeführt werden.");
    else
        method2Done = true;
}

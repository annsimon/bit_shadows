#include "sebgdialog.h"
#include "ui_sebgdialog.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/highgui.h>

SebgDialog::SebgDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SebgDialog)
{
    ui->setupUi(this);
    ui->labelOrig->hide();
}

SebgDialog::~SebgDialog()
{
    delete ui;
}

void SebgDialog::testUserParams()
{
    // load the selected image
    int index = ui->comboBoxImage->currentIndex();
    QString path = ui->comboBoxImage->itemData(index).toString();

    cv::Mat image = cv::imread(path.toStdString());

    // only continue if the images are there
    if( image.empty() )
        return;

    m_sebg->setFrame(image);
    m_sebg->setBackgroundParameter(ui->spinBoxHistory->value(),
                                   ui->spinBoxQuality->value());

    m_sebg->findInitialBackground(m_originals);

    m_sebg->setShadowParams(ui->spinBoxDilatationObject->value(),
                            ui->spinBoxErosionObject->value(),
                            ui->spinBoxThreshold->value());
    m_sebg->findSegmentation();

    // display original
        cv::Mat background;
        cv::Mat segmentation;
        cv::Mat img;
        cv::Mat preview;
        cv::cvtColor(image, img, CV_BGR2RGB);

        int w = ui->labelOrig->width();
        int h = ui->labelOrig->height();
        QPixmap pic = QPixmap::fromImage(QImage((unsigned char*) img.data,
                                                img.cols, img.rows,
                                                img.step, QImage::Format_RGB888));

        ui->labelOrig->setPixmap(pic.scaled(w,h,Qt::KeepAspectRatio));

        // display background
        m_sebg->getBackground(background);
        cv::cvtColor(background, background, CV_BGR2RGB);

        w = ui->labelBack->width();
        h = ui->labelBack->height();
        pic = QPixmap::fromImage(QImage((unsigned char*) background.data,
                                        background.cols, background.rows,
                                        background.step, QImage::Format_RGB888));

        ui->labelBack->setPixmap(pic.scaled(w,h,Qt::KeepAspectRatio));

        // display preview
        preview = cv::Mat(image.clone());

        std::vector<std::vector<cv::Point> > contours;
        // draw shadow outlines
        m_sebg->getShadowFrame(segmentation);
        cv::findContours(segmentation,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
        cv::drawContours(preview,contours,-1,cv::Scalar(0,255,0),1);

        // draw segmentation outlines
        m_sebg->getContourFrame(segmentation);
        cv::findContours(segmentation,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
        cv::drawContours(preview,contours,-1,cv::Scalar(0,0,255),1);

        cv::cvtColor(preview, segmentation, CV_BGR2RGB);

        w = ui->labelTest->width();
        h = ui->labelTest->height();
        QPixmap pic2 = QPixmap::fromImage(QImage((unsigned char*) segmentation.data,
                                                 segmentation.cols, segmentation.rows,
                                                 segmentation.step, QImage::Format_RGB888));

        ui->labelTest->setPixmap(pic2.scaled(w,h,Qt::KeepAspectRatio));

}

// SETTERS ###################################

void SebgDialog::setSebg(SebgUtils *sebg )
{
    m_sebg = sebg;
}

void SebgDialog::setImages(QStringList *list)
{
    m_originals = list;

    for (int i = 0; i < m_originals->size(); ++i)
    {
        QString tmp = m_originals->at(i).split("/").last();
        ui->comboBoxImage->addItem(tmp, QVariant(m_originals->at(i)));
    }
}

#include "parameterdialog.h"
#include "ui_parameterdialog.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/highgui.h>

parameterDialog::parameterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::parameterDialog)
{
    ui->setupUi(this);
    ui->labelOrig->hide();
}

parameterDialog::~parameterDialog()
{
    delete ui;
}

void parameterDialog::testParams()
{
    // load the selected image
    int index = ui->comboBoxImage->currentIndex();
    QString path = ui->comboBoxImage->itemData(index).toString();

    cv::Mat image = cv::imread(path.toStdString());

    // only continue if the images are there
    if( image.empty() )
        return;

    m_sakbot->setImage(image);
    m_sakbot->initBackground( m_originals );
    m_sakbot->setShadowParams( ui->doubleSpinBoxA->value(),
                               ui->doubleSpinBoxB->value(),
                               ui->spinBoxSat->value(),
                               ui->spinBoxHue->value(),
                               ui->spinBoxErosion->value(),
                               ui->spinBoxDilatation->value() );
    m_sakbot->findSegmentation();

    // display original
    cv::Mat temp;
    cv::Mat preview;
    cv::cvtColor(image, temp, CV_BGR2RGB);

    int w = ui->labelOrig->width();
    int h = ui->labelOrig->height();
    QPixmap pic = QPixmap::fromImage(QImage((unsigned char*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888));

    ui->labelOrig->setPixmap(pic.scaled(w,h,Qt::KeepAspectRatio));

    // display background
    m_sakbot->getBackground(temp);
    cv::cvtColor(temp, temp, CV_BGR2RGB);

    w = ui->labelBack->width();
    h = ui->labelBack->height();
    pic = QPixmap::fromImage(QImage((unsigned char*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888));

    ui->labelBack->setPixmap(pic.scaled(w,h,Qt::KeepAspectRatio));

    // display preview
    m_sakbot->getShadowMask(temp);
    cv::cvtColor(temp, temp, CV_RGB2GRAY);

    preview = cv::Mat(image.clone());

    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(temp,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    cv::drawContours(preview,contours,-1,cv::Scalar(0,255,0),1);

    cv::cvtColor(preview, temp, CV_BGR2RGB);

    w = ui->labelTest->width();
    h = ui->labelTest->height();
    QPixmap pic2 = QPixmap::fromImage(QImage((unsigned char*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888));

    ui->labelTest->setPixmap(pic2.scaled(w,h,Qt::KeepAspectRatio));
}

// SETTERS ###################################

void parameterDialog::setSakbot( SakbotUtils *sakbot )
{
    m_sakbot = sakbot;
}

void parameterDialog::setImages(QStringList *list)
{
    m_originals = list;

    for (int i = 0; i < m_originals->size(); ++i)
    {
        QString tmp = m_originals->at(i).split("/").last();
        ui->comboBoxImage->addItem(tmp, QVariant(m_originals->at(i)));
    }
}

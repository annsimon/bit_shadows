#include "sebg.h"


Sebg::Sebg():m_sebg(), m_sebgDialog()
{
    m_sebgDialog.setSebg(&m_sebg);
}


bool Sebg::run(QStringList* originals)
{
    m_originals = originals;

    cv::Mat currentFrame = cv::imread(m_originals->at(0).toStdString());
    if(currentFrame.empty())
        return false;

    if( !findShadowParams() )
        return false;

    m_sebg.findInitialBackground(originals);

    for(int i = 0; i < originals->length(); i++)
    {
        currentFrame = cv::imread(originals->at(i).toStdString());
        if(currentFrame.empty())
            continue;

        m_sebg.setFrame(currentFrame);
        m_sebg.findSegmentation();
        saveResult(originals->at(i));

        if(i == 15){
            m_sebg.showSpecificFrame();
        }
    }

    return true;
}

void Sebg::saveResult(QString path)
{
    // get the path
    QStringList components = path.split('/');
    components.insert(components.length()-1,"Method2");
    path = components.join("/");

    std::vector<std::vector<cv::Point> > contours;

    // get frames
    cv::Mat shadowFrame;
    cv::Mat frame;
    cv::Mat contourFrame;

    m_sebg.getShadowFrame(shadowFrame);
    m_sebg.getFrame(frame);
    m_sebg.getContourFrame(contourFrame);

    // draw the shadow's contours
    cv::findContours(shadowFrame,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    cv::drawContours(frame,contours,-1,cv::Scalar(0,255,0),1);

    // draw the segmentation's contours
    cv::findContours(contourFrame,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    cv::drawContours(frame,contours,-1,cv::Scalar(0,0,255),1);

    // save the image
    cv::imwrite(path.toStdString(), frame);
}

bool Sebg::findShadowParams()
{
    m_sebgDialog.setImages( m_originals );
    if( m_sebgDialog.exec() == QDialog::Accepted )
        return true;

    return false;
}

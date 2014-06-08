#include "sakbot.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

Sakbot::Sakbot():
    m_sakbot(),
    m_paramDialog()
{
    m_paramDialog.setSakbot(&m_sakbot);
}

bool Sakbot::run(QStringList* originals)
{
    m_originals = originals;

    m_sakbot.initBackground( originals );

    cv::Mat currentFrame = cv::imread(originals->at(0).toStdString());
    if(currentFrame.empty())
        return false;

    if( !findShadowParams() )
        return false;

    for(int i = 0; i < originals->length(); i++)
    {
        currentFrame = cv::imread(originals->at(i).toStdString());
        if(currentFrame.empty())
            continue;
        m_sakbot.setImage(currentFrame);
        m_sakbot.findSegmentation();
        saveResult(originals->at(i));
    }
    return true;
}

void Sakbot::saveResult(QString path)
{
    // get the path
    QStringList components = path.split('/');
    components.insert(components.length()-1,"Method1");
    path = components.join("/");

    cv::Mat image;
    cv::Mat shadow;
    cv::Mat segmentation;

    m_sakbot.getImage( image );
    m_sakbot.getShadowMask( shadow );
    m_sakbot.getSegmentationMask( segmentation );

    // draw the shadow's contours
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(shadow,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    cv::drawContours(image,contours,-1,cv::Scalar(0,255,0),1);

    // draw the segmentation's contours
    cv::findContours(segmentation,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    cv::drawContours(image,contours,-1,cv::Scalar(0,0,255),1);

    // save the image
    cv::imwrite(path.toStdString(), image);
}

//################################################################

bool Sakbot::findShadowParams()
{
    m_paramDialog.setImages( m_originals );
    if( m_paramDialog.exec() == QDialog::Accepted )
        return true;

    return false;
}

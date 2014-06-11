#include "sakbotwrapper.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

SakbotWrapper::SakbotWrapper():
    m_sakbot(),
    m_paramDialog()
{
    m_paramDialog.setSakbot(&m_sakbot);
}

SakbotWrapper::~SakbotWrapper()
{
}

bool SakbotWrapper::run(QStringList* originals)
{
    m_originals = originals;

    // create an initial background estimation
    m_sakbot.initBackground( originals );

    // only run if there is at least one frame
    cv::Mat currentFrame = cv::imread(originals->at(0).toStdString());
    if(currentFrame.empty())
        return false;

    // only run if parameters have been chosen by the user
    if( !findShadowParams() )
        return false;

    // run for every frame
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

void SakbotWrapper::saveResult(QString path)
{
    // get the path
    QStringList components = path.split('/');
    components.insert(components.length()-1,"Method1");
    path = components.join("/");

    // get the image and masks
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

bool SakbotWrapper::findShadowParams()
{
    m_paramDialog.setImages( m_originals );
    if( m_paramDialog.exec() == QDialog::Accepted )
        return true;

    return false;
}

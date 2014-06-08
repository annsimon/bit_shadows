#include "SakbotUtils.h"

#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>

SakbotUtils::SakbotUtils():
    m_aParam(0),
    m_bParam(1),
    m_satThresh(50),
    m_hueThresh(50),
    m_shadowErode(1),
    m_shadowDilate(1)
{
}

void SakbotUtils::findSegmentation()
{
    // init
    createGreyscale();

    // colored background
    m_bgSubtractorColor(m_image,m_segmentation);
    m_bgSubtractor.getBackgroundImage(m_background);

    // initial segmentation
    m_bgSubtractor(m_imageGrey,m_segmentation);
    m_bgSubtractor.getBackgroundImage(m_backgroundGrey);

    // create frames needed for shadow detection
    cv::cvtColor(m_image,m_imageHSV,CV_BGR2HSV);
    cv::cvtColor(m_background,m_backgroundHSV,CV_BGR2HSV);

    // get rid of the shadows
    findShadows();

    // combine
    cv::Mat temp;
    cv::bitwise_not(m_shadowSegmentation, temp);
    cv::bitwise_and(m_segmentation, temp, m_segmentation);

    // do a final second segmentation based on the mask and difference to the background
    finalizeSegmentation();

    // close holes
    cv::dilate(m_segmentation,m_segmentation,cv::Mat(),cv::Point(-1,-1),2);
    cv::erode(m_segmentation,m_segmentation,cv::Mat(),cv::Point(-1,-1),2);

    // get rid of small fragments
    cv::erode(m_segmentation,m_segmentation,cv::Mat(),cv::Point(-1,-1),3);
    cv::dilate(m_segmentation,m_segmentation,cv::Mat(),cv::Point(-1,-1),3);
}

// SEGMENTATION #####################################################
void SakbotUtils::findShadows()
{
    // formula:
    // B = background image (HSV)
    // I = main image (HSV)
    // a = control parameter for lightsource strength (brighter the source => smaller a)
    // b = control parameter for noise sensitivity
    // if a < V(I)/V(B) < b continue
    // if S(I)-S(B) <= satThresh continue
    // if |H(I)-H(B)| <= hueThresh shadowpoint found

    int rows = m_imageHSV.rows;
    int cols = m_imageHSV.cols;

    m_shadowSegmentation = cv::Mat(m_imageHSV.clone());

    for(int i=0; i<rows; i++)
    for(int j=0; j<cols; j++)
    {
        // apply formula for each pixel
        double v;
        int iV = m_imageHSV.at<cv::Vec3b>(i,j)[2];
        int bV = m_backgroundHSV.at<cv::Vec3b>(i,j)[2];
        iV = (iV == 0)? 1 : iV;
        bV = (bV == 0)? 1 : bV;
        v = iV/(double)bV;
        if( m_aParam > v || m_bParam < v)
        {
            m_shadowSegmentation.at<cv::Vec3b>(i,j)[0] = 0;
            m_shadowSegmentation.at<cv::Vec3b>(i,j)[1] = 0;
            m_shadowSegmentation.at<cv::Vec3b>(i,j)[2] = 0;
            continue;
        }
        int s = m_imageHSV.at<cv::Vec3b>(i,j)[1]-m_backgroundHSV.at<cv::Vec3b>(i,j)[1];
        if( s > m_satThresh )
        {
            m_shadowSegmentation.at<cv::Vec3b>(i,j)[0] = 0;
            m_shadowSegmentation.at<cv::Vec3b>(i,j)[1] = 0;
            m_shadowSegmentation.at<cv::Vec3b>(i,j)[2] = 0;
            continue;
        }
        int h = m_imageHSV.at<cv::Vec3b>(i,j)[0]-m_backgroundHSV.at<cv::Vec3b>(i,j)[0];
        h = (h < 0)? -h : h;
        if( h > m_hueThresh )
        {
            m_shadowSegmentation.at<cv::Vec3b>(i,j)[0] = 0;
            m_shadowSegmentation.at<cv::Vec3b>(i,j)[1] = 0;
            m_shadowSegmentation.at<cv::Vec3b>(i,j)[2] = 0;
            continue;
        }

        // test passed -> mask pixel
        m_shadowSegmentation.at<cv::Vec3b>(i,j)[0] = 255;
        m_shadowSegmentation.at<cv::Vec3b>(i,j)[1] = 255;
        m_shadowSegmentation.at<cv::Vec3b>(i,j)[2] = 255;
    }

    cv::cvtColor(m_shadowSegmentation, m_shadowSegmentation, CV_RGB2GRAY);

    cv::dilate(m_shadowSegmentation,m_shadowSegmentation,cv::Mat(),cv::Point(-1,-1),m_shadowDilate);
    cv::erode(m_shadowSegmentation,m_shadowSegmentation,cv::Mat(),cv::Point(-1,-1),m_shadowErode);
}

void SakbotUtils::finalizeSegmentation()
{
    int rows = m_imageHSV.rows;
    int cols = m_imageHSV.cols;

    cv::Mat tempMask = cv::Mat(m_imageHSV.clone());

    for(int i=0; i<rows; i++)
    for(int j=0; j<cols; j++)
    {
        // only use pixels if the difference to the background is big enough
        // use only value and saturation because hue is too easily changed by compression and noise

        // value
        double v;
        int iV = m_imageHSV.at<cv::Vec3b>(i,j)[2];
        int bV = m_backgroundHSV.at<cv::Vec3b>(i,j)[2];
        iV = (iV == 0)? 1 : iV;
        bV = (bV == 0)? 1 : bV;
        v = iV/(double)bV;

        // saturation
        double s;
        int iS = m_imageHSV.at<cv::Vec3b>(i,j)[1];
        int bS = m_backgroundHSV.at<cv::Vec3b>(i,j)[1];
        iS = (iS == 0)? 1 : iS;
        bS = (bS == 0)? 1 : bS;
        s = iS/(double)bS;

        if( (v > 1.05 || v < 0.95) && (s > 1.05 || s < 0.95) )
        {
            tempMask.at<cv::Vec3b>(i,j)[0] = 0;
            tempMask.at<cv::Vec3b>(i,j)[1] = 0;
            tempMask.at<cv::Vec3b>(i,j)[2] = 0;
            continue;
        }

        // test passed -> mask pixel
        tempMask.at<cv::Vec3b>(i,j)[0] = 255;
        tempMask.at<cv::Vec3b>(i,j)[1] = 255;
        tempMask.at<cv::Vec3b>(i,j)[2] = 255;
    }

    cv::cvtColor(tempMask, tempMask, CV_RGB2GRAY);

    // combine
    cv::bitwise_not(tempMask, tempMask);
    cv::bitwise_and(m_segmentation, tempMask, m_segmentation);
}

// UTILS ##############################################################
void SakbotUtils::initBackground(QStringList* originals)
{
    cv::Mat curr;
    cv::Mat fg;
    int frames = (originals->length() < 30)? originals->length() : 30;
    // let the background subtractor learn for a few runs
    // to start with a decent background
    for(int j = 0; j < 4; j++)
    for(int i = 0; i < frames; i++)
    {
        curr = cv::imread(originals->at(i).toStdString());
        if(curr.empty())
            continue;
        m_bgSubtractorColor(curr,fg);
        createGreyscale(curr, curr);
        m_bgSubtractor(curr,fg);
    }
}

void SakbotUtils::createGreyscale(cv::Mat src, cv::Mat dest)
{
    // create an image from the original's "V" (value) channel
    cv::vector<int> mixVec;
    mixVec.push_back(2);
    mixVec.push_back(0);
    mixVec.push_back(2);
    mixVec.push_back(1);
    mixVec.push_back(2);
    mixVec.push_back(2);

    if(!src.empty())
    {
        cv::cvtColor(src,dest,CV_BGR2HSV);
        cv::mixChannels(dest,dest,mixVec);
        return;
    }

    cv::cvtColor(m_image,m_imageGrey,CV_BGR2HSV);
    cv::mixChannels(m_imageGrey,m_imageGrey,mixVec);
}

// SETTERS #############################################################
void SakbotUtils::setImage( cv::Mat current )
{
    m_image = current.clone();
}

void SakbotUtils::setShadowParams(double a, double b, int sat, int hue, int erode, int dilate)
{
    m_aParam = a;
    m_bParam = b;
    m_satThresh = sat;
    m_hueThresh = hue;
    m_shadowErode = erode;
    m_shadowDilate = dilate;
}

// GETTERS #############################################################

void SakbotUtils::getImage( cv::Mat image )
{
    image = m_image.clone();
}

void SakbotUtils::getShadowMask( cv::Mat shadow )
{
    shadow = m_shadowSegmentation.clone();
}

void SakbotUtils::getSegmentationMask( cv::Mat segmentation )
{
    segmentation = m_segmentation.clone();
}

void SakbotUtils::getBackground( cv::Mat background )
{
    background = m_background.clone();
}

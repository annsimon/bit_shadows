#include "sakbot.h"

#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>

Sakbot::Sakbot():
    m_aParam(0),
    m_bParam(1),
    m_satThresh(50),
    m_hueThresh(50),
    m_shadowErode(1),
    m_shadowDilate(1),
    m_segmentationErode(2),
    m_segmentationDilate(2),
    m_backgroundDiff(0.05),
    m_withDebug(true)
{
}

// debug
void Sakbot::showPics(cv::Mat pic, QString name, int x, int y)
{
    if( !m_withDebug )
        return;
    cv::namedWindow(name.toStdString());
    cv::imshow(name.toStdString(), pic);
    cv::moveWindow(name.toStdString(), x, y);
}

void Sakbot::findSegmentation()
{
    // init
    createGreyscale();

    // colored background
    m_bgSubtractorColor(m_image,m_segmentation);
    m_bgSubtractorColor.getBackgroundImage(m_background);

    // initial segmentation
    m_bgSubtractor(m_imageGrey,m_segmentation);
    m_bgSubtractor.getBackgroundImage(m_backgroundGrey);

    // close holes
    cv::dilate(m_segmentation,m_segmentation,cv::Mat(),cv::Point(-1,-1),2);
    cv::erode(m_segmentation,m_segmentation,cv::Mat(),cv::Point(-1,-1),2);

    showPics(m_segmentation, "mask1", 20, 20);

    // create frames needed for shadow detection
    cv::cvtColor(m_image,m_imageHSV,CV_BGR2HSV);
    cv::cvtColor(m_background,m_backgroundHSV,CV_BGR2HSV);

    // get rid of the shadows
    findShadows();

    // combine
    cv::Mat temp;
    cv::bitwise_not(m_shadowSegmentation, temp);
    cv::bitwise_and(m_segmentation, temp, m_segmentation);

    showPics(m_segmentation, "mask-shadow", 20, 400);

    // close holes
    cv::dilate(m_segmentation,m_segmentation,cv::Mat(),cv::Point(-1,-1),2);
    cv::erode(m_segmentation,m_segmentation,cv::Mat(),cv::Point(-1,-1),2);

    // get rid of small fragments
    cv::erode(m_segmentation,m_segmentation,cv::Mat(),cv::Point(-1,-1),m_segmentationErode);
    cv::dilate(m_segmentation,m_segmentation,cv::Mat(),cv::Point(-1,-1),m_segmentationDilate);

    // do a final second segmentation based on the mask and difference to the background
    finalizeSegmentation();

    // last "smoothing"
    cv::dilate(m_segmentation,m_segmentation,cv::Mat(),cv::Point(-1,-1),2);
    cv::erode(m_segmentation,m_segmentation,cv::Mat(),cv::Point(-1,-1),2);
}

// SEGMENTATION #####################################################
void Sakbot::findShadows()
{
    // formula:
    // B = background image (HSV)
    // I = main image (HSV)
    // a = control parameter for lightsource strength (brighter the source => smaller a)
    // b = control parameter for noise sensitivity
    // if the pixel is part of the segmentation continue
    // if a < V(I)/V(B) < b continue
    // if |S(I)-S(B)| <= satThresh continue
    // if |H(I)-H(B)| <= hueThresh shadowpoint found

    int rows = m_imageHSV.rows;
    int cols = m_imageHSV.cols;

    m_shadowSegmentation = cv::Mat(m_imageHSV.clone());

    for(int i=0; i<rows; i++)
    for(int j=0; j<cols; j++)
    {
        // apply formula for each pixel
        if( m_segmentation.at<uchar>(i,j) == 0 )
        {
            m_shadowSegmentation.at<cv::Vec3b>(i,j)[0] = 0;
            m_shadowSegmentation.at<cv::Vec3b>(i,j)[1] = 0;
            m_shadowSegmentation.at<cv::Vec3b>(i,j)[2] = 0;
            continue;
        }
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
        s = (s < 0)? -s : s;
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

    showPics(m_shadowSegmentation, "shadow1", 420, 20);

    cv::cvtColor(m_shadowSegmentation, m_shadowSegmentation, CV_RGB2GRAY);

    cv::dilate(m_shadowSegmentation,m_shadowSegmentation,cv::Mat(),cv::Point(-1,-1),m_shadowDilate);
    cv::erode(m_shadowSegmentation,m_shadowSegmentation,cv::Mat(),cv::Point(-1,-1),m_shadowErode);

    showPics(m_shadowSegmentation, "shadowPost", 420, 400);

    m_shadowSegmentationFull = m_shadowSegmentation.clone();
}

void Sakbot::finalizeSegmentation()
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

        if( (v > 1+m_backgroundDiff || v < 1-m_backgroundDiff) &&
            (s > 1+m_backgroundDiff || s < 1-m_backgroundDiff) )
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
void Sakbot::initBackground(QStringList* originals, bool simple)
{
    cv::Mat curr;
    cv::Mat fg;
    int frames = (originals->length() < 30)? originals->length() : 30;
    int numRepeats = (simple)? 2 : 4;
    // let the background subtractor learn for a few runs
    // to start with a decent background
    for(int j = 0; j < numRepeats; j++)
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

void Sakbot::createGreyscale(cv::Mat src, cv::Mat dest)
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
void Sakbot::setImage( cv::Mat current )
{
    m_image = current.clone();
}

void Sakbot::setShadowParams(double a, double b, int sat, int hue, int erode, int dilate)
{
    m_aParam = a;
    m_bParam = b;
    m_satThresh = sat;
    m_hueThresh = hue;
    m_shadowErode = erode;
    m_shadowDilate = dilate;
}

void Sakbot::setSegmentationParams(int erode, int dilate, double backgroundDiff)
{
    m_segmentationErode = erode;
    m_segmentationDilate = dilate;
    m_backgroundDiff = backgroundDiff;
}

void Sakbot::setSubtractorParams(int history, int thresh)
{
    m_bgSubtractor = cv::BackgroundSubtractorMOG2(history,thresh,false);
    m_bgSubtractorColor = cv::BackgroundSubtractorMOG2(history,thresh,false);
}

// GETTERS #############################################################

void Sakbot::getImage( cv::Mat &image )
{
    image = m_image.clone();
}

void Sakbot::getShadowMask( cv::Mat &shadow )
{
    shadow = m_shadowSegmentation.clone();
}

void Sakbot::getSegmentationMask( cv::Mat &segmentation )
{
    segmentation = m_segmentation.clone();
}

void Sakbot::getBackground( cv::Mat &background )
{
    background = m_background.clone();
}

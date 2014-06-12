#ifndef SAKBOTUTILS_H
#define SAKBOTUTILS_H

#include <opencv/cv.h>
#include <opencv2/video/background_segm.hpp>

#include <QStringList>

//! This class implements a slightly modified version of the shadow
//! segmentation method described in the Sakbot system.
class Sakbot
{
public:
    Sakbot();

    //! Set the image to work on
    void setImage( cv::Mat current );
    //! Set the parameters to use for shadow segmentation
    void setShadowParams( double a, double b, int sat, int hue, int erode, int dilate );
    //! Set the parameters to use for object segmentation
    void setSegmentationParams(int erode, int dilate, double backgroundDiff);

    //! Initialize the background estimation
    void initBackground( QStringList *files, bool simple = false );
    //! Find the moving objects without their shadows
    void findSegmentation();

    //! Return the current shadow mask
    void getShadowMask( cv::Mat &shadow );
    //! Return the current segmentation
    void getSegmentationMask( cv::Mat &segmentation );
    //! Return the current image
    void getImage( cv::Mat &image );
    //! Return the current background
    void getBackground( cv::Mat &background );

private:
    //! Creates a grayscale image from the HSV V-channel
    void createGreyscale(cv::Mat src = cv::Mat(), cv::Mat dest = cv::Mat());
    //! Find the shadow segmentation
    void findShadows();
    //! Remove parts from the segmentation that barely differ from the background
    void finalizeSegmentation(bool shadow = false);

private:
    // background subtractor (background extraction and initial segmentation)
    cv::BackgroundSubtractorMOG2 m_bgSubtractor;
    cv::BackgroundSubtractorMOG2 m_bgSubtractorColor;

    // images
    cv::Mat m_image;
    cv::Mat m_imageHSV;

    // greyscale
    cv::Mat m_imageGrey;
    cv::Mat m_backgroundGrey;

    // HSV
    cv::Mat m_background;
    cv::Mat m_backgroundHSV;

    // masks
    cv::Mat m_segmentation;
    cv::Mat m_shadowSegmentation;
    cv::Mat m_shadowSegmentationFull;

    // params
    double m_aParam;
    double m_bParam;
    double m_satThresh;
    double m_hueThresh;
    int m_shadowErode;
    int m_shadowDilate;
    int m_segmentationErode;
    int m_segmentationDilate;
    double m_backgroundDiff;
};

#endif // SAKBOTUTILS_H

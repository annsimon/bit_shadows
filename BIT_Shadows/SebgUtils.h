#ifndef SEBGUTILS_H
#define SEBGUTILS_H

#include <QStringList>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/video/background_segm.hpp>

class SebgUtils
{
public:
    SebgUtils();

    //! Sets the current frame
    void setFrame(cv::Mat currentFrame);

    //! Initializes the background with (if possible) 30 frames
    void findInitialBackground(QStringList *originals);
    //! Segments the moving objects
    void findSegmentation();

    //! Returns the shadow mask
    void getShadowFrame(cv::Mat &shadowFrame);
    //! Returns the current frame
    void getFrame(cv::Mat &frame);
    //! Returns the contour frame of the object
    void getContourFrame(cv::Mat &contourFrame);

    //! Helpful for debbuging - all frames of the current scene
    void showSpecificFrame();

private:
    //! Creates a grayscale image
    void createWorkingFrame(cv::Mat src = cv::Mat(), cv::Mat dest = cv::Mat());
    //! Removes shadows from the initial segmentation
    void removeShadows();
    //! Shows the image in a opencv window
    void showPics(cv::Mat pic, QString name);

    //! Merges the foreground to the relevant outcut object
    cv::Mat mergeMatrix(cv::Mat cFrame);
    //! Creates gradient images of foreground and background
    void createGradient(cv::Mat src,  int modus);

private:
    // background subtractor for background extraction
    cv::BackgroundSubtractorMOG2 m_bgSubtractor;

    const static int MODUS_BG = 1;
    const static int MODUS_FG = 2;

    // current image
    cv::Mat m_currentFrame;
    // gray values
    cv::Mat m_workingFrame;

     // foreground
    cv::Mat m_currentForeground;
    cv::Mat m_workingFg;

    // image from the background subtractor
    cv::Mat m_currentBackground;
    cv::Mat m_currentShadow;

    // cutout from foreground and filled with background or current frame
    cv::Mat m_relevantBg;
    cv::Mat m_relevantFg;

    // gradient images
    cv::Mat m_currentGradBg;
    cv::Mat m_currentGradFg;

    // used for morphing
    cv::Mat m_currentDiffImage;
    cv::Mat m_currentBinImage; // just for debbuging
    cv::Mat m_currentMorphImage; // just for debbuging

};

#endif // SEBGUTILS_H

#ifndef SEGMETHOD2_H
#define SEGMETHOD2_H

#include <QStringList>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/video/background_segm.hpp>

//SEBG = Shadow Elimination Based on Gradients
class Sebg
{
public:
    Sebg();
    void run(QStringList *originals);

    //! Segments the moving objects
    void findSegmentation();
    //! Initializes the background with (if possible) 30 frames
    void findInitialBackground(QStringList *originals);
    //! Creates a grayscale image
    void createWorkingFrame(cv::Mat src = cv::Mat(), cv::Mat dest = cv::Mat());
    //! Removes shadows from the initial segmentation
    void removeShadows();

    //! Saves the original image with the contour of the segmented moving objects
    void saveResult(QString path);

    //! show the image
    void showPics(cv::Mat pic, QString name);

    //!
    cv::Mat mergeMatrix(cv::Mat cFrame);
    void createGradient(cv::Mat src,  int modus);

private:
    QStringList m_originals;
    QStringList m_segmentations;
    cv::Mat m_currentFrame;
    cv::Mat m_workingFrame;
    cv::Mat m_currentBackground;
    cv::Mat m_currentForeground;
    cv::Mat m_currentShadow;

    cv::Mat m_relevantBg;
    cv::Mat m_relevantFg;
    cv::Mat m_workingFg;

    cv::Mat m_currentGradBg;
    cv::Mat m_currentGradFg;
    cv::Mat m_currentDiffImage;
    cv::Mat m_currentBinImage; // just for debbuging
    cv::Mat m_currentMorphImage; // just for debbuging

    bool show;
    cv::BackgroundSubtractorMOG2 m_bgSubtractor;
};

#endif // SEGMETHOD2_H

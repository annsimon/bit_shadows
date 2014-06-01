#ifndef SAKBOT_H
#define SAKBOT_H

#include <QStringList>
#include <opencv2/video/background_segm.hpp>

class Sakbot
{
public:
    Sakbot();
    void run(QStringList *originals);

private:
    //! Segments the moving objects
    void findSegmentation();
    //! Initializes the background with (if possible) 30 frames
    void findInitialBackground(QStringList *originals);
    //! Creates a grayscale image from the HSV V-channel
    void createWorkingFrame(cv::Mat src = cv::Mat(), cv::Mat dest = cv::Mat());
    //! Removes shadows from the initial segmentation
    void removeShadows();

    //! Saves the original image with the contour of the segmented moving objects
    void saveResult(QString path);

private:
    QStringList m_originals;
    QStringList m_segmentations;
    cv::Mat m_currentFrame;
    cv::Mat m_workingFrame;
    cv::Mat m_currentBackground;
    cv::Mat m_currentForeground;
    cv::Mat m_currentShadow;
    cv::BackgroundSubtractorMOG2 m_bgSubtractor;
};

#endif // SAKBOT_H

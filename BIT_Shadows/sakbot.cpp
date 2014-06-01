#include "sakbot.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>

Sakbot::Sakbot()
{
}

void Sakbot::run(QStringList* originals)
{
    findInitialBackground(originals);
    for(int i = 0; i < originals->length(); i++)
    {
        m_currentFrame = cv::imread(originals->at(i).toStdString());
        if(m_currentFrame.empty())
            continue;
        createWorkingFrame();
        findSegmentation();
        saveResult(originals->at(i));
    }
}

void Sakbot::findSegmentation()
{
    // initial segmentation
    m_bgSubtractor(m_workingFrame,m_currentForeground);
    m_bgSubtractor.getBackgroundImage(m_currentBackground);

    // close holes
    cv::dilate(m_currentForeground,m_currentForeground,cv::Mat(),cv::Point(-1,-1),2);
    cv::erode(m_currentForeground,m_currentForeground,cv::Mat(),cv::Point(-1,-1),2);

    // get rid of small fragments
    cv::erode(m_currentForeground,m_currentForeground,cv::Mat(),cv::Point(-1,-1),3);
    cv::dilate(m_currentForeground,m_currentForeground,cv::Mat(),cv::Point(-1,-1),3);

    // get rid of the shadows
    removeShadows();
}

void Sakbot::removeShadows()
{
    // SHADOWS BE GONE
    // eventually
    // formula:
    // B = background image (HSV)
    // I = main image (HSV)
    // a = control parameter for lightsource strength (brighter the source => smaller a)
    // b = control parameter for noise sensitivity
    // if a < V(I)/V(B) < b continue
    // if S(I)-S(B) <= satThresh continue
    // if |H(I)-H(B)| <= hueThresh shadowpoint found
}

void Sakbot::saveResult(QString path)
{
    // get the path
    QStringList components = path.split('/');
    components.insert(components.length()-1,"Method1");
    path = components.join("/");

    // draw the segmentation's contours
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(m_currentForeground,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    cv::drawContours(m_workingFrame,contours,-1,cv::Scalar(0,0,255),1);

    // TODO: draw the shadow's contours
    //cv::findContours(m_currentShadow,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    //cv::drawContours(m_workingFrame,contours,-1,cv::Scalar(0,0,255),1);

    // save the image
    cv::imwrite(path.toStdString(), m_workingFrame);
}

void Sakbot::createWorkingFrame(cv::Mat src, cv::Mat dest)
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

    cv::cvtColor(m_currentFrame,m_workingFrame,CV_BGR2HSV);
    cv::mixChannels(m_workingFrame,m_workingFrame,mixVec);
}

void Sakbot::findInitialBackground(QStringList* originals)
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
        createWorkingFrame(curr, curr);
        m_bgSubtractor(curr,fg);
    }
}

#include "SebgUtils.h"

SebgUtils::SebgUtils()
{
}

void SebgUtils::findSegmentation()
{
    createWorkingFrame();

    // initial segmentation
    m_bgSubtractor(m_workingFrame,m_currentForeground);
    m_bgSubtractor.getBackgroundImage(m_currentBackground);

    cv::Mat kernel;
    int kernel_size = 1;
    int kernel_type = cv::MORPH_ELLIPSE;
    cv::Size kSize(2*kernel_size+1, 2*kernel_size+1);
    kernel = cv::getStructuringElement( kernel_type, kSize);


    erodeTimes = 2;

    // close hole / closing
    cv::dilate(m_currentForeground,m_currentForeground, kernel, cv::Point(-1,-1));
    cv::erode(m_currentForeground,m_currentForeground, kernel, cv::Point(-1,-1), erodeTimes);

    // get rid of small fragments  / opening
    cv::erode(m_currentForeground,m_currentForeground, kernel, cv::Point(-1,-1), erodeTimes);
    cv::dilate(m_currentForeground,m_currentForeground, kernel, cv::Point(-1,-1));

    cv::threshold(m_currentForeground, m_workingFg, 10, 255, CV_THRESH_BINARY );

    //m_currentForeground.copyTo(m_workingFg);
    // get rid of the shadows
    removeShadows();
}

void SebgUtils::removeShadows()
{
    m_relevantBg = mergeMatrix(m_currentBackground);
    m_relevantFg = mergeMatrix(m_workingFrame);

    createGradient(m_relevantBg, MODUS_BG);
    createGradient(m_relevantFg, MODUS_FG);

    // m_threshold is from user input
    cv::threshold(m_currentGradFg, m_currentGradFg, m_threshold, 255, CV_THRESH_BINARY );
    cv::threshold(m_currentGradBg, m_currentGradBg, m_threshold, 255, CV_THRESH_BINARY );
    cv::absdiff(m_currentGradFg, m_currentGradBg, m_currentDiffImage);

    cv::threshold(m_currentDiffImage, m_currentBinImage, m_threshold, 255, CV_THRESH_BINARY );


    morphImage();
}

void SebgUtils::morphImage()
{

    cv::Mat kernel;
     int kernel_size = 2;
     int kernel_type = cv::MORPH_ELLIPSE;
     cv::Size kSize(2*kernel_size+1, 2*kernel_size+1);
     kernel = getStructuringElement( kernel_type, kSize);

     cv::Mat morphBin;
     // get rid of small fragments
     cv::dilate(m_currentBinImage,morphBin, kernel, cv::Point(-1,-1),m_dilateObject); // 2
     cv::erode(morphBin,morphBin, kernel, cv::Point(-1,-1), m_erodeObject); // 3

     cv::absdiff(m_workingFg, morphBin, m_currentShadow);
     cv::erode(m_currentShadow,m_currentShadow, kernel, cv::Point(-1,-1), m_erodeShadow);
     cv::dilate(m_currentShadow,m_currentShadow, kernel, cv::Point(-1,-1), m_dilateShadow);

     morphBin.copyTo(m_currentMorphImage);
}

// merge the objects, which are relevant and moving in the scene
// foreground and the background
cv::Mat SebgUtils::mergeMatrix(cv::Mat cFrame)
{
    cv::Mat tmp;
    m_workingFg.copyTo(tmp);

    for(int i = 0;i < tmp.rows;i++)
    {
         for(int j = 0;j < tmp.cols;j++)
         {
            int   v = m_workingFg.data[m_workingFg.step[0]*i + tmp.step[1]* j + 0];

            if(v != 0 )
             {
                 tmp.data[tmp.step[0]*i + tmp.step[1]* j + 0] = cFrame.data[cFrame.step[0]*i + cFrame.step[1]* j + 0];
            }
         }
    }

    return tmp;
}

// gradient images for foreground and background
// just in the relevant background
void SebgUtils::createGradient(cv::Mat src, int modus)
{

    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;

    /// Generate grad_x and grad_y

    cv::Mat grad_x;
    cv::Mat grad_y;
    cv::Mat abs_grad_x;
    cv::Mat abs_grad_y;

    /// Gradient X
    cv::Sobel( src, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
    cv::convertScaleAbs( grad_x, abs_grad_x );

     /// Gradient Y
     cv::Sobel( src, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
     cv::convertScaleAbs( grad_y, abs_grad_y );

     /// Total Gradient (approximate)
     if(modus == MODUS_BG)
     {
         cv::addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, m_currentGradBg );
     }
     else if (modus == MODUS_FG)
     {
         cv::addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, m_currentGradFg );
     }
}



void SebgUtils::createWorkingFrame(cv::Mat src, cv::Mat dest)
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

void SebgUtils::findInitialBackground(QStringList* originals)
{
    cv::Mat curr;
    cv::Mat fg;

    int frames = (originals->length() < 30)? originals->length() : 30;
    // let the background subtractor learn for a few runs
    // to start with a decent background
    for(int j = 0; j < 4; j++)
    {
        for(int i = 0; i < frames; i++)
        {
            curr = cv::imread(originals->at(i).toStdString());
            if(curr.empty())
            continue;

            createWorkingFrame(curr, curr);
            m_bgSubtractor(curr,fg, false);
        }
    }
}


// SETTERS #############################################################
void SebgUtils::setFrame( cv::Mat currentFrame )
{
    m_currentFrame = currentFrame.clone();
}

// GETTERS #############################################################
void SebgUtils::getShadowFrame(cv::Mat &shadowFrame)
{
     shadowFrame = m_currentShadow.clone();
}

void SebgUtils::getFrame(cv::Mat &frame)
{
    frame = m_currentFrame.clone();
}

void SebgUtils::getContourFrame(cv::Mat &contourFrame)
{
    contourFrame = m_currentMorphImage.clone();
}

void SebgUtils::getBackground( cv::Mat &background )
{
    background = m_currentBackground.clone();
}

// DEBUG ################################################################
void SebgUtils::showPics(cv::Mat pic, QString name)
{
    cv::namedWindow(name.toStdString());
    cv::imshow(name.toStdString(), pic);
}

void SebgUtils::showSpecificFrame()
{
     showPics(m_currentFrame, "m_currentFrame");
     showPics(m_currentForeground, "m_currentForeground");
     showPics(m_currentBackground, "m_currentBackground");
     showPics(m_relevantBg, "m_relevantBg");
     showPics(m_relevantFg, "m_relevantFg");
     showPics(m_currentGradFg, "m_currentGradFg");
     showPics(m_currentGradBg, "m_currentGradBg");
     showPics(m_currentDiffImage, "m_currentDiffImage");
     showPics(m_currentShadow, "m_currentShadow");
     showPics(m_currentBinImage, "m_currentBinImage");
     showPics(m_workingFg, "m_workingFg");
     showPics(m_currentMorphImage, "m_currentMorphImage");
  // showPics(m_floodFrame, "m_floodFrame");
}

void SebgUtils::setShadowParams(int dilateObject , int erodeObject, int dilateShadow ,
                                int erodeShadow, int threshold ){
    m_dilateObject = dilateObject;
    m_erodeObject = erodeObject;
    m_dilateShadow = dilateShadow;
    m_erodeShadow = erodeShadow;
    m_threshold = threshold;
}

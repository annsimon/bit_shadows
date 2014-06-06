#include "sebg.h"


Sebg::Sebg()
{
}


void Sebg::showPics(cv::Mat pic, QString name){

    cv::namedWindow(name.toStdString());
    cv::imshow(name.toStdString(), pic);
}

void Sebg::run(QStringList* originals)
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

    //showPics(m_currentBackground, "m_currentBackground");
    //showPics(m_currentForeground, "m_currentForeground");

    //showPics(m_relevantBg, "m_relevantBg");
    //showPics(m_relevantFg, "m_relevantFg");
    //showPics(m_currentDiffImage, "m_currentDiffImage");
    //showPics(m_currentGradFg, "m_currentGradFg");
    //showPics(m_currentGradBg, "m_currentGradBg");
    //showPics(m_currentBinImage, "m_currentBinImage");

}

void Sebg::findSegmentation()
{
    // initial segmentation
    m_bgSubtractor(m_workingFrame,m_currentForeground, 0.002);
    m_bgSubtractor.getBackgroundImage(m_currentBackground);

    // close hole
 /*   cv::dilate(m_currentForeground,m_currentForeground,cv::Mat(),cv::Point(-1,-1),2);
    cv::erode(m_currentForeground,m_currentForeground,cv::Mat(),cv::Point(-1,-1),2);

    // get rid of small fragments
    cv::erode(m_currentForeground,m_currentForeground,cv::Mat(),cv::Point(-1,-1),3);
    cv::dilate(m_currentForeground,m_currentForeground,cv::Mat(),cv::Point(-1,-1),3);
*/


    cv::Mat kernel;

    int kernel_size = 1;
    cv::Point kPoint( kernel_size, kernel_size );

    int kernel_type = cv::MORPH_ELLIPSE;
    cv::Size kSize(2*kernel_size+1, 2*kernel_size+1);

    kernel = getStructuringElement( kernel_type, kSize, kPoint);

    /// Apply the erosion operation
    ///close
    cv::dilate(m_currentForeground, m_currentForeground, kernel );
    cv::erode(m_currentForeground, m_currentForeground, kernel );

    // 3x3
    cv::Point kPoint2( kernel_size-1, kernel_size-1 );
    cv::Size kSize2(kernel_size+1, kernel_size+1);

    kernel = getStructuringElement( kernel_type, kSize, kPoint);

    // open
    cv::erode(m_currentForeground, m_currentForeground, kernel );
    cv::dilate(m_currentForeground, m_currentForeground, kernel );

// closing


    // get rid of the shadows
    removeShadows();
}

cv::Mat Sebg::mergeMatrix(cv::Mat cFrame){
    cv::Mat tmp;
    cv::cvtColor(m_currentForeground, tmp, CV_GRAY2BGR);

    for(int i = 0;i < tmp.rows;i++){
         for(int j = 0;j < tmp.cols;j++){

             int   b = tmp.data[tmp.step[0]*i + tmp.step[1]* j + 0];
             int   g = tmp.data[tmp.step[0]*i + tmp.step[1]* j + 1];
             int   r =  tmp.data[tmp.step[0]*i + tmp.step[1]* j + 2];

             if(b != 0 || g != 0 || r != 0){
                 tmp.data[tmp.step[0]*i + tmp.step[1]* j + 0] = cFrame.data[cFrame.step[0]*i + cFrame.step[1]* j + 0] ;
                 tmp.data[tmp.step[0]*i + tmp.step[1]* j + 1] = cFrame.data[cFrame.step[0]*i + cFrame.step[1]* j + 1] ;
                 tmp.data[tmp.step[0]*i + tmp.step[1]* j + 2] = cFrame.data[cFrame.step[0]*i + cFrame.step[1]* j + 2] ;
             }
           }
       }

    return tmp;
}

void Sebg::createGradient(cv::Mat src, int modus){

    /*    cv::Mat g1(0, 0, -1, 1);
        cv::Mat g2 (0, -1, 0, -1);
        cv::Mat g3(0, 1, -1, 0);
        cv::Mat g4(-1, 0, 1, 0);
    */
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;

    /// Generate grad_x and grad_y

    cv::Mat grad_x, grad_y;
    cv::Mat abs_grad_x, abs_grad_y;

    /// Gradient X
    cv::Sobel( src, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
    cv::convertScaleAbs( grad_x, abs_grad_x );

     /// Gradient Y
     cv::Sobel( src, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
     cv::convertScaleAbs( grad_y, abs_grad_y );

     /// Total Gradient (approximate)
     if(modus == 1){// BG
         cv::addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, m_currentGradBg );
     }else if (modus ==2){ // FG
         cv::addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, m_currentGradFg );
     }

}

void Sebg::removeShadows()
{
    //TODO
    /*
     * gradientenbild von vorder- und hintergrund
     * binarisierung der bilder     *
     * differenzbilder der gradientenbilder
     *
     *connected component and morpgholgy
     *
     **/
    // foreground ablaufen, wo nicht schwarz, dann pixel aus hintergrund in rebg speichern

    cv::Mat tmp;

    m_relevantBg = mergeMatrix(m_currentBackground);
    m_relevantFg =  mergeMatrix(m_currentFrame);

    createGradient(m_relevantBg, 1);
    createGradient(m_relevantFg, 2);

    m_currentDiffImage = m_currentGradFg - m_currentGradBg;

    cv::cvtColor(m_currentDiffImage, tmp, CV_BGR2GRAY);
    cv::threshold(tmp, m_currentBinImage, 100, 255, CV_THRESH_BINARY );

    cv::Mat kernel;
    int kernel_size = 2;
    cv::Point kPoint( kernel_size, kernel_size );

    int kernel_type = cv::MORPH_ELLIPSE;
    cv::Size kSize(2*kernel_size+1, 2*kernel_size+1);

    kernel = getStructuringElement( kernel_type, kSize, kPoint);

    /// Apply the erosion operation
    ///close
    cv::dilate(m_currentBinImage, m_currentMorphImage, kernel );
    cv::erode(m_currentMorphImage, m_currentMorphImage, kernel );

    // 3x3
    cv::Point kPoint2( kernel_size-1, kernel_size-1 );
    cv::Size kSize2(kernel_size+1, kernel_size+1);

    kernel = getStructuringElement( kernel_type, kSize2, kPoint2);

    // open
 //   cv::erode(m_currentMorphImage, m_currentMorphImage, kernel );
 //   cv::dilate(m_currentMorphImage, m_currentMorphImage, kernel );

    showPics(m_currentMorphImage, "m_currentMorphImage");

    // close holes
 //   cv::dilate(m_currentBinImage ,m_currentMorphImage,cv::Mat(),cv::Point(-1,-1),5);
 //   cv::erode(m_currentMorphImage,m_currentMorphImage,cv::Mat(),cv::Point(-1,-1),5);

 //   // get rid of small fragments
 //   cv::erode(m_currentMorphImage,m_currentMorphImage,cv::Mat(),cv::Point(-1,-1),5);
 //   cv::dilate(m_currentMorphImage,m_currentMorphImage,cv::Mat(),cv::Point(-1,-1),5);
}


void Sebg::saveResult(QString path)
{
    cv::Mat tmp;
 //   cv::cvtColor(m_currentMorphImage, tmp, CV_BGR2GRAY);

    // get the path
    QStringList components = path.split('/');
    components.insert(components.length()-1,"Method2");
    path = components.join("/");

    // draw the segmentation's contours
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(m_currentMorphImage,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    cv::drawContours(m_workingFrame,contours,-1,cv::Scalar(0,0,255),1);

    // TODO: draw the shadow's contours
//    cv::findContours(m_currentShadow,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
//    cv::drawContours(m_workingFrame,contours,-1,cv::Scalar(0,0,255),1);

    // save the image
    cv::imwrite(path.toStdString(), m_workingFrame);
}

void Sebg::createWorkingFrame(cv::Mat src, cv::Mat dest)
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

void Sebg::findInitialBackground(QStringList* originals)
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
        m_bgSubtractor(curr,fg, 0.002);
    }
}

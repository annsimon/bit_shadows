#include "Utilities.h"

QStringList extractFrames(QString file, QString path){
    // extract the frames
    // save each frame as picture into m_originalDir and add the filename (e.g. img_1) to the combobox
    cv::Mat frame;
    cv::VideoCapture video(file.toStdString()); // open video file
    if(!video.isOpened()){ // check if we succeeded
        std::cout << "Video wurde nicht initialsiert!" << std::endl;
    }


    int frameCounter = 0;
    QString frameName = path + "/img_";
    QString frameType = ".bmp";
    QString framePath;
    QStringList frameList;

    while(video.grab()){
        frameCounter++;

        framePath = frameName + QString::number(frameCounter) + frameType;
        video >> frame; //extract a frame

        // Save the frame into a file
        if(!imwrite(framePath.toStdString(), frame)){
             std::cout << "Fehler beim Speichern des Frames: "<< frameCounter << std::endl;
             return frameList;
       }

        frameList.append(framePath);
    }

    return frameList;
}


#ifndef UTILITIES_H
#define UTILITIES_H

#include <QString>
#include <QStringList>
#include <iostream>

// opencv
#include <opencv/cv.h>
#include <opencv/highgui.h>

//! Extract the single frames from a video file and save them to the folder
//! specified by path.
QStringList extractFrames(QString file, QString path);


#endif // UTILITIES_H

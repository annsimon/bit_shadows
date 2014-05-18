#ifndef UTILITIES_H
#define UTILITIES_H

#include <QMainWindow>
#include <QDir>

// opencv
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

namespace Ui {
    class MainWindow;
}


QStringList extractFrames(QString file, QString path);


#endif // UTILITIES_H

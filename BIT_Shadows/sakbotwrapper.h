#ifndef SAKBOT_H
#define SAKBOT_H

#include <QStringList>

#include "parameterdialog.h"
#include "sakbot.h"

class SakbotWrapper
{
public:
    SakbotWrapper();
    ~SakbotWrapper();

    //! Start the method to create a segmentation of the moving objects.
    bool run(QStringList *originals);

private:
    //! Use user input and sample segmentations to determine parameters that are appropriate for the scene.
    bool findShadowParams();

    //! Saves the original image with the contour of the segmented moving objects.
    void saveResult(QString path);

private:
    // list of file paths for all frames
    QStringList *m_originals;

    Sakbot m_sakbot;

    // settings
    ParameterDialog m_paramDialog;
};

#endif // SAKBOT_H

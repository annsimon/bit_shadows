#ifndef SAKBOT_H
#define SAKBOT_H

#include <QStringList>

#include "parameterdialog.h"
#include "SakbotUtils.h"

class Sakbot
{
public:
    Sakbot();
    bool run(QStringList *originals);

private:
    //! Use user input and sample segmentations to determine parameters that are appropriate for the scene
    bool findShadowParams();

    //! Saves the original image with the contour of the segmented moving objects
    void saveResult(QString path);

private:
    QStringList *m_originals;

    SakbotUtils m_sakbot;

    // settings
    parameterDialog m_paramDialog;
};

#endif // SAKBOT_H

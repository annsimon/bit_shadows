#ifndef SEGMETHOD2_H
#define SEGMETHOD2_H

#include <QStringList>
#include "SebgUtils.h"
#include "sebgdialog.h"

//SEBG = Shadow Elimination Based on Gradients
class Sebg
{
public:
    Sebg();
    //! starts segmentation with SEBG
    bool run(QStringList *originals);
private:
    //! Saves the original image with the contour of the segmented moving objects
    void saveResult(QString path);

    //! Use user input and sample segmentations to determine parameters that are appropriate for the scene
    bool findShadowParams();

    QStringList *m_originals;
    SebgUtils m_sebg;

    // settings
   SebgDialog m_sebgDialog;
};

#endif // SEGMETHOD2_H

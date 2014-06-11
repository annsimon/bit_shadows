#ifndef SEGMETHOD2_H
#define SEGMETHOD2_H

#include <QStringList>
#include "SebgUtils.h"

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

    QStringList *m_originals;
    SebgUtils m_sebg;
    bool show;
};

#endif // SEGMETHOD2_H

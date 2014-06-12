#ifndef PARAMETERDIALOG_H
#define PARAMETERDIALOG_H

#include "sakbot.h"

#include <QDialog>

#include <opencv/cv.h>

namespace Ui {
class parameterDialog;
}

//! This class is used to query the user for parameters appropriate for the current scene.
class ParameterDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ParameterDialog(QWidget *parent = 0);
    ~ParameterDialog();

    //! Set the images the parameters can be tested on.
    void setImages( QStringList *list );

    //! Set the instance of the Sakbot class that will be used to create the segmentation.
    void setSakbot( Sakbot *sakbot );

private slots:
    // Test the parameters and display a preview.
    void testParams();
    
private:
    Ui::parameterDialog *ui;

    Sakbot *m_sakbot;

    QStringList *m_originals;
};

#endif // PARAMETERDIALOG_H

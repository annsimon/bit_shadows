#ifndef SEBGDIALOG_H
#define SEBGDIALOG_H

#include <QDialog>
#include "sebgutils.h"
#include <opencv/cv.h>

namespace Ui {
class SebgDialog;
}

class SebgDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SebgDialog(QWidget *parent = 0);
    ~SebgDialog();

    //! Set the images the parameters can be tested on.
    void setImages( QStringList *list );

    //! Set the instance of the SebgUtils class that will be used to create the segmentation.
    void setSebg( SebgUtils *sebg );
    
private slots:
    //! Test the parameters and display a preview.
    void testUserParams();

private:
    Ui::SebgDialog *ui;
    SebgUtils *m_sebg;
    QStringList *m_originals;
};

#endif // SEBGDIALOG_H

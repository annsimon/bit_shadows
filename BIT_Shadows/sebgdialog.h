#ifndef SEBGDIALOG_H
#define SEBGDIALOG_H

#include <QDialog>
#include "SebgUtils.h"
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

    void setImages( QStringList *list );

    void setSebg( SebgUtils *sebg );
    
private slots:
    void testUserParams();

private:
    Ui::SebgDialog *ui;
    SebgUtils *m_sebg;
    QStringList *m_originals;
};

#endif // SEBGDIALOG_H

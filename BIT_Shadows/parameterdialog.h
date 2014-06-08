#ifndef PARAMETERDIALOG_H
#define PARAMETERDIALOG_H

#include "SakbotUtils.h"

#include <QDialog>

#include <opencv/cv.h>


namespace Ui {
class parameterDialog;
}

class parameterDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit parameterDialog(QWidget *parent = 0);
    ~parameterDialog();

    void setImages( QStringList *list );

    void setSakbot( SakbotUtils *sakbot );

private slots:
    void testParams();
    
private:
    Ui::parameterDialog *ui;

    SakbotUtils *m_sakbot;

    QStringList *m_originals;
};

#endif // PARAMETERDIALOG_H

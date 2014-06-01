#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //! Opens file dialog and extracts frames of video to folder
    void extractImages();
    //! Displays the frame specified by the combobox and the segmentations available for it
    void displayImage();
    //! Runs the first segmentation method
    void executeMethod1();
    //! Runs the second segmentation method
    void executeMethod2();

    //! update Combobox
    bool updateCombobox();
    //! loads the original image to the center widget
    bool loadOriginalImage();
    //! loads the result of method 1 to the center widget
    bool loadMethod1Image();

private:
    //! Runs the sakbot-inspired method
    void executeSakbot();

private:
    Ui::MainWindow *ui;

    QDir m_originalDir;
    QDir m_method1Dir;
    QDir m_method2Dir;

    QStringList m_frameList;

    bool method1Done;
    bool method2Done;
};

#endif // MAINWINDOW_H

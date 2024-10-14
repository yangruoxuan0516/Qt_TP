#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QButtonGroup>
#include "drawgraphics.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
class DrawGraphics;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // 声明 setParametersFromSelection 函数
    void setParametersFromSelection(int color, int thickness, int style, int shape);
    void savePainting();
    void loadPainting();

private:
    Ui::MainWindow *ui;
    QButtonGroup *colorGroup;
    QButtonGroup *thicknessGroup;
    QButtonGroup *styleGroup;
    QButtonGroup *shapeGroup;
    DrawGraphics * dg;

protected:
    void closeEvent(QCloseEvent *event) override;  // Override closeEvent

};
#endif // MAINWINDOW_H

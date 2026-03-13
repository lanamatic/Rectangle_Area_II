#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QTimer>
#include <QComboBox>
#include <QLineEdit>


#include "algorithm_solution.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onStep();
    void onStepBack();
    void onAutoPlay();
    void onReset();
    void onSpeedChanged(int val);
    void onTimerTick();
    void onLoadInput();

private:
    Ui::MainWindow *ui;


    SweepLineAlgorithm algo;
    int currentStep;

    QWidget    *controlBar;
    QLineEdit  *txtInput;
    QPushButton *btnLoad, *btnStepBack, *btnStep, *btnAuto, *btnReset;
    QSlider    *speedSlider;
    QLabel     *lblSpeed, *lblDesc;
    QTimer     *autoTimer;
    bool        autoPlaying;

    std::vector<QColor> rectFillColors;
    std::vector<QColor> rectBorderColors;


    void setupControls();
    void loadRectangles(const std::vector<std::vector<int>>& rects);
    void updateDescription();
    std::vector<std::vector<int>> parseInput(const QString& text);


    QRectF canvasRect() const;
    double toScreenX(double wx) const;
    double toScreenY(double wy) const;
    QRectF toScreenRect(double x1, double y1, double x2, double y2) const;


    void drawGrid(QPainter &p);
    void drawActiveStrip(QPainter &p);
    void drawRectangles(QPainter &p);
    void drawSweepLine(QPainter &p);
    void drawCntPanel(QPainter &p);
    void drawLegend(QPainter &p);
};
#endif // MAINWINDOW_H

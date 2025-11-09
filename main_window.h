#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QRandomGenerator>
#include <QTimer>

#include "picture_generator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onInertialNoiseClicked();
    void onImpulseNoiseClicked();
    void onFrequencyChanged(int value);
    void updateNoise();

private:
    void setupUI();
    void generateInertialNoise(QImage &image);
    void generateImpulseNoise(QImage &image);
    void updateImageDisplays();
    
    PictureGenerator __pg_obj;

    QWidget *centralWidget;
    QLabel *imageLabel1, *imageLabel2, *imageLabel3;
    QPushButton *inertialButton, *impulseButton;
    QSlider *frequencySlider;
    QVBoxLayout *mainLayout;
    QHBoxLayout *imagesLayout, *controlsLayout;
    
    QTimer *updateTimer;
    int noiseType; // 0 - inertial, 1 - impulse
    int frequency; // Hz
    QRandomGenerator random;
};

#endif // MAINWINDOW_H
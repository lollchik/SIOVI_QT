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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QComboBox>
#include <QGroupBox>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <cmath>
#include <vector>
#include <algorithm>

#include "picture_generator.h"
#include "noise_generator.h"
#include "filter.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr){
        this->noiseType = 0;
        this->noiseLevel = 0.25;
        this->impulseNoiseType = 0;
        this->impulseIntensity = 0;
        setupUI();
    };  // Конструктор с параметром по умолчанию
    ~MainWindow() = default;

private slots:
// void onInertialNoiseClicked();
    void onDownloadNoiseClicked();
    void onAdditiveNoiseClicked();
    void onImpulseNoiseClicked();
    void onApplyNoiseClicked();
    void onAdditiveLevelChanged(int index);
    void onImpulseTypeChanged(int index);
    void onImpulseIntensityChanged(int index);
    void onImpulseLevelChanged(int index);
    // void onFrequencyChanged(int value);
    // void updateNoise();

private:

    QImage image;
    void setupUI();
    void generateInertialNoise(QImage &image, double eta);

    QImage generateAdditiveNoise(QImage image);
    QGroupBox* additiveGroup() const;
    QGroupBox* impulseGroup() const;
    // Переменные состояния
    int noiseType; // 0 - аддитивный, 1 - импульсный
    double noiseLevel; // 0.25, 0.50, 0.75
    int impulseNoiseType; // 0 - соль, 1 - перец, 2 - соль и перец
    int impulseIntensity; // 0 - точечный, 1 - строковый

    void generateImpulseNoise(QImage &image);
    void updateImageDisplays();
    
    PictureGenerator __pg_obj;
    MaskFilter __filter;

    QWidget *centralWidget;
    QLabel *imageLabel1;
    QLabel *imageLabel2;
    QLabel *imageLabel3;
    QPushButton *downloadButton;
    QPushButton *additiveNoiseButton;
    QPushButton *impulseNoiseButton;
    QPushButton *applyNoiseButton;
    QComboBox *additiveLevelCombo;
    QComboBox *impulseTypeCombo;
    QComboBox *impulseIntensityCombo;
    QComboBox *impulseLevelCombo;
    QGroupBox *additiveGroupBox;
    QGroupBox *impulseGroupBox;
    QVBoxLayout *mainLayout;
    QHBoxLayout *imagesLayout;
    QHBoxLayout *buttonsLayuout;
    QHBoxLayout *parametersLayout;
};

#endif // MAINWINDOW_H
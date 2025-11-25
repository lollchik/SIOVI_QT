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
#include <QDebug>
#include <cmath>
#include <vector>
#include <algorithm>

#include "picture_generator.h"
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
    void onImpulseNoiseClicked();
    // void onFrequencyChanged(int value);
    // void updateNoise();

private:
    void setupUI();
    void generateInertialNoise(QImage &image, double eta);

    void generateAdditiveNoise(QImage &image, double eta);
    void onAdditiveNoiseClicked();
    QGroupBox* additiveGroup() const;
    QGroupBox* impulseGroup() const;
    void onApplyNoiseClicked();
    void onAdditiveLevelChanged(int index);
    void onImpulseTypeChanged(int index);
    void onImpulseIntensityChanged(int index);
    void onImpulseLevelChanged(int index);
    double calculateImageEnergy(const QImage &image);
    double calculateMeanValue(const QImage &image);
    void generateImpulseNoise(QImage &image, double eta, int type, int intensity);
    void generateSaltNoisePoint(QImage &image, double eta);
    void generateSaltNoiseLine(QImage &image, double eta);
    void generatePepperNoisePoint(QImage &image, double eta);
    void generatePepperNoiseLine(QImage &image, double eta);
    void generateSaltAndPepperNoise(QImage &image, double eta, bool isLineNoise);
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
    QHBoxLayout *noiseTypeLayout;
    QHBoxLayout *parametersLayout;


    QTimer *updateTimer;
    int frequency; // Hz
    QRandomGenerator random;
};

#endif // MAINWINDOW_H
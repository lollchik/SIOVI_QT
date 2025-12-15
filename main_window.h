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

#include "enums.h"

#include "picture_generator.h"
#include "noise_generator.h"
#include "filter.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr){
        this->noiseType = 0;
        this->noiseLevel = 0.15;
        this->impulseNoiseType = 0;
        this->impulse_form = 0;
        setupUI();
    };  // Конструктор с параметром по умолчанию
    ~MainWindow() = default;

private slots:
// void onInertialNoiseClicked();
    void onDownloadNoiseClicked();
    void onAdditiveNoiseClicked();
    void onImpulseNoiseClicked();
    void onApplyNoiseClicked();
    void onFiltrTypeChanged(int index);
    void onAdditiveLevelChanged(int index);
    void onImpulseTypeChanged(int index);
    void onImpulseIntensityChanged(int index);
    void onImpulseLevelChanged(int index);
    // void onFrequencyChanged(int value);
    // void updateNoise();

private:

    void setupUI();
    void updateImageDisplays();

    QImage image;

    QGroupBox *additiveGroupBox;
    QGroupBox* additiveGroup() const;
    QGroupBox *impulseGroupBox;
    QGroupBox* impulseGroup() const;

    // Переменные состояния
    int noiseType = noise_type::additive; // 0 - аддитивный, 1 - импульсный
    double noiseLevel = noise_level::low; // 0.25, 0.50, 0.75
    int impulseNoiseType = inmpulse_noise_type::salt; // 0 - соль, 1 - перец, 2 - соль и перец
    int impulse_form = impulse_noise_form::point; // 0 - точечный, 1 - строковый
    int filtr_type  = filtr_type::mask_smoothing; // mask_smoothing, uniform_area_smoothing,  median_filtr,  morph_dilation,  morph_erosion

    
    PictureGenerator __pg_obj;
    Filters __filter;

    QWidget *centralWidget;
    QLabel *imageLabel1;
    QLabel *imageLabel2;
    QLabel *imageLabel3;
    QPushButton *downloadButton;
    QPushButton *additiveNoiseButton;
    QPushButton *impulseNoiseButton;
    QPushButton *applyButton;
    QComboBox *filtrCombo;
    QComboBox *additiveLevelCombo;
    QComboBox *impulseTypeCombo;
    QComboBox *impulse_formCombo;
    QComboBox *impulseLevelCombo;
    QVBoxLayout *mainLayout;
    QHBoxLayout *imagesLayout;
    QHBoxLayout *buttonsLayuout;
    QHBoxLayout *parametersLayout;
};

#endif // MAINWINDOW_H
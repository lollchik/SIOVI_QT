#include "main_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QRandomGenerator>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , noiseType(0)
    , frequency(10)
    , random(QRandomGenerator::global()->generate())
{
    setupUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Создание меток для изображений
    imageLabel1 = new QLabel(this);
    imageLabel2 = new QLabel(this);
    imageLabel3 = new QLabel(this);
    
    imageLabel1->setFixedSize(300, 200);
    imageLabel2->setFixedSize(300, 200);
    imageLabel3->setFixedSize(300, 200);
    
    // Создание кнопок
    inertialButton = new QPushButton("Инертный шум", this);
    impulseButton = new QPushButton("Импульсный шум", this);
    
    inertialButton->setFixedSize(150, 40);
    impulseButton->setFixedSize(150, 40);
    
    // Создание слайдера
    frequencySlider = new QSlider(Qt::Horizontal, this);
    frequencySlider->setRange(1, 60); // 1-60 Hz
    frequencySlider->setValue(frequency);
    frequencySlider->setFixedWidth(300);
    
    QLabel *sliderLabel = new QLabel("Частота шума: " + QString::number(frequency) + " Hz", this);
    
    // Компоновка
    imagesLayout = new QHBoxLayout();
    imagesLayout->addWidget(imageLabel1);
    imagesLayout->addWidget(imageLabel2);
    imagesLayout->addWidget(imageLabel3);
    
    controlsLayout = new QHBoxLayout();
    controlsLayout->addWidget(inertialButton);
    controlsLayout->addWidget(impulseButton);
    controlsLayout->setAlignment(Qt::AlignCenter);
    
    QVBoxLayout *sliderLayout = new QVBoxLayout();
    sliderLayout->addWidget(sliderLabel);
    sliderLayout->addWidget(frequencySlider);
    sliderLayout->setAlignment(Qt::AlignCenter);
    
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addLayout(imagesLayout);
    mainLayout->addLayout(controlsLayout);
    mainLayout->addLayout(sliderLayout);
    
    // Подключение сигналов
    connect(inertialButton, &QPushButton::clicked, this, &MainWindow::onInertialNoiseClicked);
    connect(impulseButton, &QPushButton::clicked, this, &MainWindow::onImpulseNoiseClicked);
    connect(frequencySlider, &QSlider::valueChanged, this, &MainWindow::onFrequencyChanged);
    
    // Обновляем отображение частоты при изменении слайдера
    connect(frequencySlider, &QSlider::valueChanged, [sliderLabel](int value) {
        sliderLabel->setText("Частота шума: " + QString::number(value) + " Hz");
    });
    
    setWindowTitle("Визуализатор шума");
    setMinimumSize(1000, 400);
    
    // Генерируем начальный шум
    updateNoise();
}

void MainWindow::onInertialNoiseClicked()
{
    noiseType = 0;
    inertialButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; }");
    impulseButton->setStyleSheet("QPushButton { background-color: none; }");
    updateNoise();
}

void MainWindow::onImpulseNoiseClicked()
{
    noiseType = 1;
    impulseButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; }");
    inertialButton->setStyleSheet("QPushButton { background-color: none; }");
    updateNoise();
}

void MainWindow::onFrequencyChanged(int value)
{
    frequency = value;
    updateNoise();
}

void MainWindow::updateNoise()
{
    updateImageDisplays();
}

void MainWindow::generateInertialNoise(QImage &image)
{
    int width = image.width();
    int height = image.height();
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int noise = random.bounded(256);
            int r = noise;
            int g = noise;
            int b = noise;
            
            if (random.bounded(100) < 30) {
                r = qMin(255, noise + random.bounded(50));
                g = qMin(255, noise + random.bounded(50));
                b = qMin(255, noise + random.bounded(50));
            }
            
            image.setPixelColor(x, y, QColor(r, g, b));
        }
    }
}

void MainWindow::generateImpulseNoise(QImage &image)
{
    int width = image.width();
    int height = image.height();

    image.fill(Qt::black);
    
    int numImpulses = width * height / 50; 
    
    for (int i = 0; i < numImpulses; ++i) {
        int x = random.bounded(width);
        int y = random.bounded(height);
        int size = random.bounded(1, 5);
        int brightness = random.bounded(200, 256);
        
        for (int dy = -size; dy <= size; ++dy) {
            for (int dx = -size; dx <= size; ++dx) {
                int nx = x + dx;
                int ny = y + dy;
                
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    if (random.bounded(100) < 80) { 
                        image.setPixelColor(nx, ny, QColor(brightness, brightness, brightness));
                    }
                }
            }
        }
    }
}

void MainWindow::updateImageDisplays()
{
    QImage image1(300, 200, QImage::Format_RGB32);
        __pg_obj.generateSineWave(image1);

    QImage image2(300, 200, QImage::Format_RGB32);

    QImage image3(300, 200, QImage::Format_RGB32);
        __pg_obj.generatePixelArt(image3);

    
    if (noiseType == 0) {
        // Инертный шум
        generateInertialNoise(image2);
    } else {
        generateImpulseNoise(image2);
    }
    
    imageLabel1->setPixmap(QPixmap::fromImage(image1));
    imageLabel2->setPixmap(QPixmap::fromImage(image2));
    imageLabel3->setPixmap(QPixmap::fromImage(image3));

}
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
            // Получаем текущий цвет пикселя
            QColor originalColor = image.pixelColor(x, y);
            int r = originalColor.red();
            int g = originalColor.green();
            int b = originalColor.blue();
            
            int noise = random.bounded(256);
            r = qMin(255, (int)(r + noise));
            g = qMin(255, (int)(g + noise));
            b = qMin(255, (int)(b + noise));
            
            image.setPixelColor(x, y, QColor(r, g, b));
        }
    }
}

void MainWindow::generateImpulseNoise(QImage &image)
{
    int width = image.width();
    int height = image.height();

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
                        QColor originalColor = image.pixelColor(nx, ny);
                        
                        int r = qMin(255, (originalColor.red() + brightness) / 2);
                        int g = qMin(255, (originalColor.green() + brightness) / 2);
                        int b = qMin(255, (originalColor.blue() + brightness) / 2);
                        
                        image.setPixelColor(nx, ny, QColor(r, g, b));
                    }
                }
            }
        }
    }
}

void MainWindow::updateImageDisplays()
{
    QImage image1(300, 200, QImage::Format_RGB32);
        __pg_obj.generateGeometricPattern(image1);

    QImage image2(300, 200, QImage::Format_RGB32);
    image2 = image1;
    QImage image3(300, 200, QImage::Format_RGB32);
    
    
    if (noiseType == 0) {
        // Инертный шум
        generateInertialNoise(image2);
    } else {
        generateImpulseNoise(image2);
    }
    
    imageLabel1->setPixmap(QPixmap::fromImage(image1));
    imageLabel2->setPixmap(QPixmap::fromImage(image2));
    image3 = __filter.apply(image2);
    imageLabel3->setPixmap(QPixmap::fromImage(image3));

}
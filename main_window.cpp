#include "main_window.h"




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
    
    imageLabel1->setAlignment(Qt::AlignCenter);
    imageLabel2->setAlignment(Qt::AlignCenter);
    imageLabel3->setAlignment(Qt::AlignCenter);
    
    imageLabel1->setText("Исходное\nизображение");
    imageLabel2->setText("Зашумленное\nизображение");
    imageLabel3->setText("Обработанное\nизображение");
    
    imageLabel1->setStyleSheet("QLabel { background-color: #f0f0f0; border: 1px solid #ccc; }");
    imageLabel2->setStyleSheet("QLabel { background-color: #f0f0f0; border: 1px solid #ccc; }");
    imageLabel3->setStyleSheet("QLabel { background-color: #f0f0f0; border: 1px solid #ccc; }");
    
    // Создание кнопок для выбора типа шума
    additiveNoiseButton = new QPushButton("Аддитивный шум", this);
    impulseNoiseButton = new QPushButton("Импульсный шум", this);
    
    additiveNoiseButton->setFixedSize(150, 40);
    impulseNoiseButton->setFixedSize(150, 40);
    
    // Группа для параметров аддитивного шума
    QGroupBox *additiveGroup = new QGroupBox("Параметры аддитивного шума", this);
    QVBoxLayout *additiveLayout = new QVBoxLayout(additiveGroup);
    
    QLabel *additiveLevelLabel = new QLabel("Уровень шума (η):", this);
    additiveLevelCombo = new QComboBox(this);
    additiveLevelCombo->addItems({"0.25", "0.50", "0.75"});
    additiveLevelCombo->setCurrentIndex(0);
    
    additiveLayout->addWidget(additiveLevelLabel);
    additiveLayout->addWidget(additiveLevelCombo);
    
    // Группа для параметров импульсного шума
    QGroupBox *impulseGroup = new QGroupBox("Параметры импульсного шума", this);
    QVBoxLayout *impulseLayout = new QVBoxLayout(impulseGroup);
    
    QLabel *impulseTypeLabel = new QLabel("Тип шума:", this);
    impulseTypeCombo = new QComboBox(this);
    impulseTypeCombo->addItems({"Соль", "Перец", "Соль и перец"});
    
    QLabel *impulseIntensityLabel = new QLabel("Интенсивность:", this);
    impulseIntensityCombo = new QComboBox(this);
    impulseIntensityCombo->addItems({"Точечный", "Строковый"});
    
    QLabel *impulseLevelLabel = new QLabel("Уровень шума (η):", this);
    impulseLevelCombo = new QComboBox(this);
    impulseLevelCombo->addItems({"0.25", "0.50", "0.75"});
    impulseLevelCombo->setCurrentIndex(0);
    
    impulseLayout->addWidget(impulseTypeLabel);
    impulseLayout->addWidget(impulseTypeCombo);
    impulseLayout->addWidget(impulseIntensityLabel);
    impulseLayout->addWidget(impulseIntensityCombo);
    impulseLayout->addWidget(impulseLevelLabel);
    impulseLayout->addWidget(impulseLevelCombo);
    
    // Кнопка применения шума
    applyNoiseButton = new QPushButton("Применить шум", this);
    applyNoiseButton->setFixedSize(200, 40);
    applyNoiseButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-weight: bold; }");
    
    // Компоновка
    imagesLayout = new QHBoxLayout();
    imagesLayout->addWidget(imageLabel1);
    imagesLayout->addWidget(imageLabel2);
    imagesLayout->addWidget(imageLabel3);
    
    noiseTypeLayout = new QHBoxLayout();
    noiseTypeLayout->addWidget(additiveNoiseButton);
    noiseTypeLayout->addWidget(impulseNoiseButton);
    noiseTypeLayout->setAlignment(Qt::AlignCenter);
    
    parametersLayout = new QHBoxLayout();
    parametersLayout->addWidget(additiveGroup);
    parametersLayout->addWidget(impulseGroup);
    
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addLayout(imagesLayout);
    mainLayout->addLayout(noiseTypeLayout);
    mainLayout->addLayout(parametersLayout);
    mainLayout->addWidget(applyNoiseButton, 0, Qt::AlignCenter);
    
    // Изначально скрываем параметры импульсного шума
    impulseGroup->setVisible(false);
    
    // Подключение сигналов
    connect(additiveNoiseButton, &QPushButton::clicked, this, &MainWindow::onAdditiveNoiseClicked);
    connect(impulseNoiseButton, &QPushButton::clicked, this, &MainWindow::onImpulseNoiseClicked);
    connect(applyNoiseButton, &QPushButton::clicked, this, &MainWindow::onApplyNoiseClicked);
    connect(additiveLevelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onAdditiveLevelChanged);
    connect(impulseTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onImpulseTypeChanged);
    connect(impulseIntensityCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onImpulseIntensityChanged);
    connect(impulseLevelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onImpulseLevelChanged);
    
    setWindowTitle("Генератор шумов для обработки изображений");
    setMinimumSize(1000, 600);
    
    // Генерируем начальное изображение
    updateImageDisplays();
}

void MainWindow::onAdditiveNoiseClicked()
{
    noiseType = 0;
    additiveNoiseButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; }");
    impulseNoiseButton->setStyleSheet("QPushButton { background-color: none; }");
    
    // Показываем только параметры аддитивного шума
    additiveGroup()->setVisible(true);
    impulseGroup()->setVisible(false);
}

void MainWindow::onImpulseNoiseClicked()
{
    noiseType = 1;
    impulseNoiseButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; }");
    additiveNoiseButton->setStyleSheet("QPushButton { background-color: none; }");
    
    // Показываем только параметры импульсного шума
    additiveGroup()->setVisible(false);
    impulseGroup()->setVisible(true);
}

void MainWindow::onApplyNoiseClicked()
{
    updateImageDisplays();
}

void MainWindow::onAdditiveLevelChanged(int index)
{
    noiseLevel = 0.25 + index * 0.25; // 0.25, 0.50, 0.75
}

void MainWindow::onImpulseTypeChanged(int index)
{
    impulseNoiseType = index;
}

void MainWindow::onImpulseIntensityChanged(int index)
{
    impulseIntensity = index;
}

void MainWindow::onImpulseLevelChanged(int index)
{
    noiseLevel = 0.25 + index * 0.25; // 0.25, 0.50, 0.75
}

// Вспомогательная функция для вычисления энергии изображения
double MainWindow::calculateImageEnergy(const QImage &image)
{
    double energy = 0.0;
    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            int value = qGray(image.pixel(x, y));
            energy += value * value;
        }
    }
    return energy;
}

// Вспомогательная функция для вычисления математического ожидания
double MainWindow::calculateMeanValue(const QImage &image)
{
    double sum = 0.0;
    int totalPixels = image.width() * image.height();
    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            sum += qGray(image.pixel(x, y));
        }
    }
    return sum / totalPixels;
}

void MainWindow::generateAdditiveNoise(QImage &image, double eta)
{
    int width = image.width();
    int height = image.height();
    int N = width;
    
    QRandomGenerator *rg = QRandomGenerator::global();
    
    // Создание шумового поля
    double **noiseField = new double*[height];
    double MO_sh = 0.0;
    
    for (int y = 0; y < height; y++) {
        noiseField[y] = new double[width];
        for (int x = 0; x < width; x++) {
            noiseField[y][x] = rg->bounded(256);
            MO_sh += noiseField[y][x];
        }
    }
    MO_sh /= (N * N);
    
    // Применение шума с компенсацией переполнения
    double nu = 0.0;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int originalValue = qGray(image.pixel(x, y));
            
            double pix = originalValue + eta * (noiseField[y][x] - MO_sh) + nu;
            
            if (pix >= 0 && pix <= 255) {
                int value = static_cast<int>(pix);
                image.setPixel(x, y, qRgb(value, value, value));
                nu = 0.0;
            }
            else if (pix > 255) {
                image.setPixel(x, y, qRgb(255, 255, 255));
                nu = pix - 255;
            }
            else {
                image.setPixel(x, y, qRgb(0, 0, 0));
                nu = -pix;
            }
        }
    }
    
    // Освобождение памяти
    for (int y = 0; y < height; y++) {
        delete[] noiseField[y];
    }
    delete[] noiseField;
}

void MainWindow::generateImpulseNoise(QImage &image, double eta, int type, int intensity)
{
    int width = image.width();
    int height = image.height();
    
    double B_ish = calculateImageEnergy(image);
    double targetNoiseEnergy = eta * B_ish;
    double MO_sh = calculateMeanValue(image);
    
    QRandomGenerator *rg = QRandomGenerator::global();
    double currentNoiseEnergy = 0.0;
    
    switch (type) {
    case 0: // Соль
        if (intensity == 0) { // Точечный
            generateSaltNoisePoint(image, eta);
        } else { // Строковый
            generateSaltNoiseLine(image, eta);
        }
        break;
        
    case 1: // Перец
        if (intensity == 0) { // Точечный
            generatePepperNoisePoint(image, eta);
        } else { // Строковый
            generatePepperNoiseLine(image, eta);
        }
        break;
        
    case 2: // Соль и перец
        generateSaltAndPepperNoise(image, eta, intensity == 1);
        break;
    }
}

void MainWindow::generateSaltNoisePoint(QImage &image, double eta)
{
    int width = image.width();
    int height = image.height();
    
    double B_ish = calculateImageEnergy(image);
    double targetNoiseEnergy = eta * B_ish;
    
    QRandomGenerator *rg = QRandomGenerator::global();
    double currentNoiseEnergy = 0.0;
    
    // Собираем все пиксели и перемешиваем
    std::vector<std::pair<int, int>> allPixels;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            allPixels.push_back({x, y});
        }
    }
    std::shuffle(allPixels.begin(), allPixels.end(), *rg);
    
    // Добавляем шумовые пиксели
    for (const auto& pixel : allPixels) {
        int x = pixel.first;
        int y = pixel.second;
        
        int originalValue = qGray(image.pixel(x, y));
        double pixelEnergy = pow(255 - originalValue, 2);
        
        if (currentNoiseEnergy + pixelEnergy <= targetNoiseEnergy) {
            image.setPixel(x, y, qRgb(255, 255, 255));
            currentNoiseEnergy += pixelEnergy;
        }
        
        if (currentNoiseEnergy >= targetNoiseEnergy) {
            break;
        }
    }
}

void MainWindow::generateSaltNoiseLine(QImage &image, double eta)
{
    int width = image.width();
    int height = image.height();
    
    double B_ish = calculateImageEnergy(image);
    double targetNoiseEnergy = eta * B_ish;
    
    QRandomGenerator *rg = QRandomGenerator::global();
    double currentNoiseEnergy = 0.0;
    
    while (currentNoiseEnergy < targetNoiseEnergy) {
        int startX = rg->bounded(width);
        int startY = rg->bounded(height);
        int length = rg->bounded(2, 6); // Длина 2-5 пикселей
        
        for (int i = 0; i < length && startX + i < width; i++) {
            int x = startX + i;
            int y = startY;
            
            if (y < height) {
                int originalValue = qGray(image.pixel(x, y));
                double pixelEnergy = pow(255 - originalValue, 2);
                
                if (currentNoiseEnergy + pixelEnergy <= targetNoiseEnergy) {
                    image.setPixel(x, y, qRgb(255, 255, 255));
                    currentNoiseEnergy += pixelEnergy;
                }
            }
        }
    }
}

void MainWindow::generatePepperNoisePoint(QImage &image, double eta)
{
    int width = image.width();
    int height = image.height();
    
    double B_ish = calculateImageEnergy(image);
    double targetNoiseEnergy = eta * B_ish;
    
    QRandomGenerator *rg = QRandomGenerator::global();
    double currentNoiseEnergy = 0.0;
    
    std::vector<std::pair<int, int>> allPixels;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            allPixels.push_back({x, y});
        }
    }
    std::shuffle(allPixels.begin(), allPixels.end(), *rg);
    
    for (const auto& pixel : allPixels) {
        int x = pixel.first;
        int y = pixel.second;
        
        int originalValue = qGray(image.pixel(x, y));
        double pixelEnergy = pow(originalValue, 2);
        
        if (currentNoiseEnergy + pixelEnergy <= targetNoiseEnergy) {
            image.setPixel(x, y, qRgb(0, 0, 0));
            currentNoiseEnergy += pixelEnergy;
        }
        
        if (currentNoiseEnergy >= targetNoiseEnergy) {
            break;
        }
    }
}

void MainWindow::generatePepperNoiseLine(QImage &image, double eta)
{
    int width = image.width();
    int height = image.height();
    
    double B_ish = calculateImageEnergy(image);
    double targetNoiseEnergy = eta * B_ish;
    
    QRandomGenerator *rg = QRandomGenerator::global();
    double currentNoiseEnergy = 0.0;
    
    while (currentNoiseEnergy < targetNoiseEnergy) {
        int startX = rg->bounded(width);
        int startY = rg->bounded(height);
        int length = rg->bounded(2, 6);
        
        for (int i = 0; i < length && startX + i < width; i++) {
            int x = startX + i;
            int y = startY;
            
            if (y < height) {
                int originalValue = qGray(image.pixel(x, y));
                double pixelEnergy = pow(originalValue, 2);
                
                if (currentNoiseEnergy + pixelEnergy <= targetNoiseEnergy) {
                    image.setPixel(x, y, qRgb(0, 0, 0));
                    currentNoiseEnergy += pixelEnergy;
                }
            }
        }
    }
}

void MainWindow::generateSaltAndPepperNoise(QImage &image, double eta, bool isLineNoise)
{
    int width = image.width();
    int height = image.height();
    
    double B_ish = calculateImageEnergy(image);
    double MO_sh = calculateMeanValue(image);
    double targetNoiseEnergy = eta * B_ish;
    
    QRandomGenerator *rg = QRandomGenerator::global();
    double currentNoiseEnergy = 0.0;
    
    if (!isLineNoise) {
        // Точечный шум
        std::vector<std::pair<int, int>> allPixels;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                allPixels.push_back({x, y});
            }
        }
        std::shuffle(allPixels.begin(), allPixels.end(), *rg);
        
        for (const auto& pixel : allPixels) {
            int x = pixel.first;
            int y = pixel.second;
            
            int originalValue = qGray(image.pixel(x, y));
            double pixelEnergy = pow(MO_sh, 2);
            
            if (currentNoiseEnergy + pixelEnergy <= targetNoiseEnergy) {
                if (originalValue < MO_sh) {
                    image.setPixel(x, y, qRgb(255, 255, 255)); // Соль
                } else {
                    image.setPixel(x, y, qRgb(0, 0, 0)); // Перец
                }
                currentNoiseEnergy += pixelEnergy;
            }
            
            if (currentNoiseEnergy >= targetNoiseEnergy) {
                break;
            }
        }
    } else {
        // Строковый шум
        while (currentNoiseEnergy < targetNoiseEnergy) {
            int startX = rg->bounded(width);
            int startY = rg->bounded(height);
            int length = rg->bounded(2, 6);
            
            int firstPixelValue = qGray(image.pixel(startX, startY));
            int lineBrightness = (firstPixelValue < MO_sh) ? 255 : 0;
            
            for (int i = 0; i < length && startX + i < width; i++) {
                int x = startX + i;
                int y = startY;
                
                if (y < height) {
                    double pixelEnergy = pow(MO_sh, 2);
                    
                    if (currentNoiseEnergy + pixelEnergy <= targetNoiseEnergy) {
                        image.setPixel(x, y, qRgb(lineBrightness, lineBrightness, lineBrightness));
                        currentNoiseEnergy += pixelEnergy;
                    }
                }
            }
        }
    }
}

void MainWindow::updateImageDisplays()
{
    // Генерация исходного изображения
    QImage image1(300, 200, QImage::Format_RGB32);
    __pg_obj.generateGeometricPattern(image1);

    QImage image2 = image1.copy();
    QImage image3(300, 200, QImage::Format_RGB32);
    
    // Применение выбранного шума
    if (noiseType == 0) {
        // Аддитивный шум
        generateAdditiveNoise(image2, noiseLevel);
    } else {
        // Импульсный шум
        generateImpulseNoise(image2, noiseLevel, impulseNoiseType, impulseIntensity);
    }
    
    // Отображение изображений
    imageLabel1->setPixmap(QPixmap::fromImage(image1).scaled(300, 200, Qt::KeepAspectRatio));
    imageLabel2->setPixmap(QPixmap::fromImage(image2).scaled(300, 200, Qt::KeepAspectRatio));
    
    // Применение фильтра и отображение результата
    image3 = __filter.apply(image2);
    imageLabel3->setPixmap(QPixmap::fromImage(image3).scaled(300, 200, Qt::KeepAspectRatio));
}

// Вспомогательные функции для доступа к группам
QGroupBox* MainWindow::additiveGroup() const
{
    return qobject_cast<QGroupBox*>(parametersLayout->itemAt(0)->widget());
}

QGroupBox* MainWindow::impulseGroup() const
{
    return qobject_cast<QGroupBox*>(parametersLayout->itemAt(1)->widget());
}
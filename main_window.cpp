#include "main_window.h"

#define low = 0.25
#define medium = 0.5
#define hight = 0.75



void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Создание меток для изображений
    imageLabel1 = new QLabel(this);
    imageLabel2 = new QLabel(this);
    imageLabel3 = new QLabel(this);
    
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
    downloadButton = new QPushButton("Загрузка изобрадения", this);
    additiveNoiseButton = new QPushButton("Аддитивный шум", this);
    impulseNoiseButton = new QPushButton("Импульсный шум", this);
    
    downloadButton->setFixedSize(150, 40);
    additiveNoiseButton->setFixedSize(150, 40);
    impulseNoiseButton->setFixedSize(150, 40);
    
    // Группа для параметров аддитивного шума
    QGroupBox *additiveGroup = new QGroupBox("Параметры аддитивного шума", this);
    QVBoxLayout *additiveLayout = new QVBoxLayout(additiveGroup);
    
    QLabel *additiveLevelLabel = new QLabel("Уровень шума (η):", this);
    additiveLevelCombo = new QComboBox(this);
    additiveLevelCombo->addItems({"0.15", "0.25", "0.50", "1.0"});
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
    impulseLevelCombo->addItems({"0.15", "0.25", "0.50", "1.0"});
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
    
    buttonsLayuout = new QHBoxLayout();
    buttonsLayuout->addWidget(downloadButton);
    buttonsLayuout->addWidget(additiveNoiseButton);
    buttonsLayuout->addWidget(impulseNoiseButton);

    buttonsLayuout->setAlignment(Qt::AlignCenter);
    
    parametersLayout = new QHBoxLayout();
    parametersLayout->addWidget(additiveGroup);
    parametersLayout->addWidget(impulseGroup);
    
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addLayout(imagesLayout);
    mainLayout->addLayout(buttonsLayuout);
    mainLayout->addLayout(parametersLayout);
    mainLayout->addWidget(applyNoiseButton, 0, Qt::AlignCenter);
    
    // Изначально скрываем параметры импульсного шума
    impulseGroup->setVisible(false);
    
    // Подключение сигналов
    connect(downloadButton, &QPushButton::clicked, this, &MainWindow::onDownloadNoiseClicked);
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


void MainWindow::onDownloadNoiseClicked()
{
          // Открываем диалог выбора файла
        QString filePath = QFileDialog::getOpenFileName(
            this,
            "Выберите изображение",
            QDir::homePath(),  // Начинаем с домашней директории
            "Изображения (*.png *.jpg *.jpeg *.bmp *.tiff *.gif);;Все файлы (*.*)"
        );
        
        // Если пользователь не выбрал файл (нажал Cancel)
        if (filePath.isEmpty()) {
            qDebug() << "Пользователь отменил выбор файла";
            return;
        }
        
        qDebug() << "Выбран файл:" << filePath;
        
        // Загружаем изображение
        if (image.load(filePath)) {
            qDebug() << "Изображение успешно загружено";
            qDebug() << "Размер:" << image.width() << "x" << image.height();
            qDebug() << "Формат:" << image.format();
        } else {
            QMessageBox::critical(this, "Ошибка", 
                "Не удалось загрузить изображение!\n"
                "Проверьте путь и формат файла.");
            qDebug() << "Ошибка загрузки изображения";
        }
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
    if(index == 0)
        noiseLevel = 0.15; // 0.15", "0.25", "0.5", "1.0
    else if(index == 1)
        noiseLevel = 0.25;
    else if(index == 2)
        noiseLevel = 0.5;
    else if(index == 3)
        noiseLevel = 1;
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
    if(index == 0)
        noiseLevel = 0.15; // 0.15", "0.25", "0.5", "1.0
    else if(index == 1)
        noiseLevel = 0.25;
    else if(index == 2)
        noiseLevel = 0.5;
    else if(index == 3)
        noiseLevel = 1;
}

void MainWindow::updateImageDisplays()
{
    if (image.isNull()) {
        return;
    }
    
    // Отображение исходного изображения
    imageLabel1->setPixmap(QPixmap::fromImage(image.convertToFormat(QImage::Format_Grayscale8)));//.scaled(300, 200, Qt::KeepAspectRatio));
    
    // Создание зашумленного изображения
    QImage noisyImage = image.copy();
    
    // Применение выбранного шума
    if (noiseType == 0) {
        // Аддитивный шум
        noisyImage = NoiseGenerator().generateAdditiveNoise(image, noiseLevel, 7897);
    } else {
        // Импульсный шум
        NoiseGenerator::ImpulseNoiseType type;
        switch (impulseNoiseType) {
        case 0: type = NoiseGenerator::ImpulseNoiseType::Salt; break;
        case 1: type = NoiseGenerator::ImpulseNoiseType::Pepper; break;
        case 2: type = NoiseGenerator::ImpulseNoiseType::SaltAndPepper; break;
        default: type = NoiseGenerator::ImpulseNoiseType::Salt; break;
        }
        
        NoiseGenerator::ImpulseNoiseIntensity intensity;
        switch (impulseIntensity) {
        case 0: intensity = NoiseGenerator::ImpulseNoiseIntensity::Point; break;
        case 1: intensity = NoiseGenerator::ImpulseNoiseIntensity::Line; break;
        default: intensity = NoiseGenerator::ImpulseNoiseIntensity::Point; break;
        }
        
        noisyImage = NoiseGenerator().generateImpulseNoise(image, noiseLevel, type, intensity, 7897);
    }
    
    // Отображение зашумленного изображения
    imageLabel2->setPixmap(QPixmap::fromImage(noisyImage));//.scaled(300, 200, Qt::KeepAspectRatio));
    
    // Применение фильтра и отображение результата
    // TODO: Добавить фильтрацию
    // QImage filtred_image = __filter.apply_uniform_area_smoothing(noisyImage); //по однорордным областям
    QImage filtred_image = __filter.apply_mask_smoothing(noisyImage); //по маске
    imageLabel3->setPixmap(QPixmap::fromImage(filtred_image));//.scaled(300, 200, Qt::KeepAspectRatio));
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
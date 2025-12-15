#include "main_window.h"

// #define low = 0.25
// #define medium = 0.5
// #define hight = 0.75



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
    additiveNoiseButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; }");
    impulseNoiseButton->setFixedSize(150, 40);

    // Группа для выбора типа фильтра
    QGroupBox *filtrGroup = new QGroupBox("Тип фильтра", this);
    QVBoxLayout *filtrLayout = new QVBoxLayout(filtrGroup);
    filtrCombo = new QComboBox(this);
    filtrCombo->addItems({  "сглаживание маской"
                                    , "сгаживание по ожнородным областям"
                                    , "медианный фильтр"
                                    , "выделение контуров"
                                    , "Морфология: дилитация"
                                    , "Морфология: эрозия"});
    filtrCombo->setCurrentIndex(0);
    
    filtrLayout->addWidget(filtrCombo);
    
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
    
    QLabel *impulse_formLabel = new QLabel("Интенсивность:", this);
    impulse_formCombo = new QComboBox(this);
    impulse_formCombo->addItems({"Точечный", "Строковый"});
    
    QLabel *impulseLevelLabel = new QLabel("Уровень шума (η):", this);
    impulseLevelCombo = new QComboBox(this);
    impulseLevelCombo->addItems({"0.15", "0.25", "0.50", "1.0"});
    impulseLevelCombo->setCurrentIndex(0);
    
    impulseLayout->addWidget(impulseTypeLabel);
    impulseLayout->addWidget(impulseTypeCombo);
    impulseLayout->addWidget(impulse_formLabel);
    impulseLayout->addWidget(impulse_formCombo);
    impulseLayout->addWidget(impulseLevelLabel);
    impulseLayout->addWidget(impulseLevelCombo);
    
    // Кнопка применения шума
    applyButton = new QPushButton("Применить шум и фильтрацию", this);
    applyButton->setFixedSize(400, 40);
    applyButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-weight: bold; }");
    
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
    parametersLayout->addWidget(filtrGroup);
    parametersLayout->addWidget(additiveGroup);
    parametersLayout->addWidget(impulseGroup);
    
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addLayout(imagesLayout);
    mainLayout->addLayout(buttonsLayuout);
    mainLayout->addLayout(parametersLayout);
    mainLayout->addWidget(applyButton, 0, Qt::AlignCenter);
    
    // Изначально скрываем параметры импульсного шума
    impulseGroup->setVisible(false);
    
    // Подключение сигналов
    connect(downloadButton, &QPushButton::clicked, this, &MainWindow::onDownloadNoiseClicked);
    connect(additiveNoiseButton, &QPushButton::clicked, this, &MainWindow::onAdditiveNoiseClicked);
    connect(impulseNoiseButton, &QPushButton::clicked, this, &MainWindow::onImpulseNoiseClicked);
    connect(applyButton, &QPushButton::clicked, this, &MainWindow::onApplyNoiseClicked);
    connect(filtrCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onFiltrTypeChanged);
    connect(additiveLevelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onAdditiveLevelChanged);
    connect(impulseTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onImpulseTypeChanged);
    connect(impulse_formCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onImpulseIntensityChanged);
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
    noiseLevel = 0.15; 
    noiseType = noise_type::additive;
    additiveNoiseButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; }");
    impulseNoiseButton->setStyleSheet("QPushButton { background-color: none; }");
    
    // Показываем только параметры аддитивного шума
    additiveGroup()->setVisible(true);
    impulseGroup()->setVisible(false);
}

void MainWindow::onImpulseNoiseClicked()
{
    noiseLevel = 0.15; 
    noiseType = noise_type::impulse;
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

void MainWindow::onFiltrTypeChanged(int index){
    filtr_type = index;
}

void MainWindow::onAdditiveLevelChanged(int index)
{
    // 0.15", "0.25", "0.5", "1.0
    switch(index){
        case noise_level::low:
            noiseLevel = 0.15; 
            break;        
        case noise_level::mediume:
            noiseLevel = 0.25; 
            break;
        case noise_level::half:
            noiseLevel = 0.5; 
            break;
        case noise_level::full:
            noiseLevel = 1; 
            break;
        default:
            noiseLevel = 0.15; 
            break;
    }
}

void MainWindow::onImpulseLevelChanged(int index)
{
    // 0.15", "0.25", "0.5", "1.0
    switch(index){
        case noise_level::low:
            noiseLevel = 0.15; 
            break;        
        case noise_level::mediume:
            noiseLevel = 0.25; 
            break;
        case noise_level::half:
            noiseLevel = 0.5; 
            break;
        case noise_level::full:
            noiseLevel = 1; 
            break;
        default:
            noiseLevel = 0.15; 
            break;
    }
}

void MainWindow::onImpulseTypeChanged(int index)
{
    impulseNoiseType = index;
}

void MainWindow::onImpulseIntensityChanged(int index)
{
    impulse_form = index;
}


void MainWindow::updateImageDisplays()
{
    qDebug() << "START НАЧАЛО РАБОТЫ С ИЗО";
    qDebug() << "Конфигурационный значения:";
    qDebug() << "noiseType - " << noiseType;
    qDebug() << "noiseLevel - " << noiseLevel;
    qDebug() << "impulseNoiseType - " << impulseNoiseType;
    qDebug() << "impulse_form - " << impulse_form;
    qDebug() << "filtr_type - " << filtr_type;
    qDebug() << "--------------------------------- ";

    if (image.isNull()) {
        return;
    }
    // __pg_obj.generateChessboard(image);
    // Отображение исходного изображения
    imageLabel1->setPixmap(QPixmap::fromImage(image.convertToFormat(QImage::Format_Grayscale8)));
    
    // Создание зашумленного изображения
    QImage noisyImage = image.copy();
    
    switch(noiseType){
        case noise_type::additive:
            noisyImage = NoiseGenerator().generateAdditiveNoise(image, noiseLevel, 7897);
            break;
        case noise_type::impulse:
            noisyImage = NoiseGenerator().generateImpulseNoise(image, noiseLevel,
                         static_cast<inmpulse_noise_type>(impulseNoiseType),
                          static_cast<impulse_noise_form>(impulse_form), 7897);
    }
    
    // Отображение зашумленного изображения
    imageLabel2->setPixmap(QPixmap::fromImage(noisyImage));
    
    // Применение фильтра и отображение результата
    QImage filtred_image;
    switch (filtr_type)
    {
    case filtr_type::mask_smoothing:
        qDebug() << "ШАГ финальный фитрация use: mask_smoothing";
        filtred_image = __filter.apply_mask_smoothing(noisyImage); //по маске
        break;
    case filtr_type::uniform_area_smoothing:
        qDebug() << "ШАГ финальный фитрация use: uniform_area_smoothing";
        filtred_image = __filter.apply_uniform_area_smoothing(noisyImage); 
        break;
    case filtr_type::median_filtr:
        qDebug() << "ШАГ финальный фитрация use: median_filtr";
        filtred_image = __filter.apply_median_filtr(noisyImage); 
        break;
    case filtr_type::sharpening_Filter:
        qDebug() << "ШАГ финальный фитрация use: sharpening_Filter";
        filtred_image = __filter.apply_sharpening_Filter(noisyImage); 
        break;
    case filtr_type::morph_dilation:
        qDebug() << "ШАГ финальный фитрация use: morph_dilation";
        filtred_image = __filter.dilation(noisyImage);
        break;
    case filtr_type::morph_erosion:
        qDebug() << "ШАГ финальный фитрация use: morph_erosion";
        filtred_image = __filter.erosion(noisyImage);
        break;
    default:
        qDebug() << "ШАГ финальный фитрация use: mask_smoothing";
        filtred_image = __filter.apply_mask_smoothing(noisyImage); //по маске    
        break;
    }
    imageLabel3->setPixmap(QPixmap::fromImage(filtred_image));//.scaled(300, 200, Qt::KeepAspectRatio));
    qDebug() << "--------------------------------- ";
    qDebug() << "END КОНЕЦ РАБОТЫ С ИЗО";
}

// Вспомогательные функции для доступа к группам
QGroupBox* MainWindow::additiveGroup() const
{
    return qobject_cast<QGroupBox*>(parametersLayout->itemAt(1)->widget());
}

QGroupBox* MainWindow::impulseGroup() const
{
    return qobject_cast<QGroupBox*>(parametersLayout->itemAt(2)->widget());
}
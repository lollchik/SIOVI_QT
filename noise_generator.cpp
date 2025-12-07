#include "noise_generator.h"

#include <QImage>
#include <QRandomGenerator>
#include <QtMath>
#include <QDate>
#include <QDebug>
#include <iostream>
#include <set>

#define RANGE 256
#define RANGE_STR_LOW 2
#define RANGE_STR_HIGHT 5

/**
 * @author Zhurckov G.S.
 * @brief Генерирует аддитивный шум на изображении с заданным уровнем энергии
 * @param inputImage Входное изображение
 * @param noiseLevel Уровень шума (0.25, 0.5, 0.75)
 * @param seed Начальное значение для ГПСЧ default(0 -> (new int))
 * @return Зашумленное изображение  (аддитивный шум)
 *
 * Алгоритм соответствует спецификации:
 * 1. Создается шумовое поле WxH со случайными значениями (wight x hight)
 * 2. Вычисляется математическое ожидание шумового поля
 * 3. Для каждого пикселя применяется формула с контролем диапазона [0, 255]
 * 4. Осуществляется перенос переполнения/недополнения (error diffusion)
 */
QImage NoiseGenerator::generateAdditiveNoise(QImage &inputImage, double noiseLevel, quint32 seed)
{    
    QImage image = inputImage.convertToFormat(QImage::Format_Grayscale8);
    
    int i_width = image.width();    // width
    int i_height = image.height();    // height
    const int N2 = i_width * i_height;
    
    //ГПСЧ init
    QRandomGenerator randGenerator;
    if (seed == 0)
    {   // используем мусор из памяти чтобы добится энтропии с использованием ГПСЧ
        randGenerator.seed(*(new int));
    }
    else
    {   // используем зхаданное значение для возможости воспроизведления 
        randGenerator.seed(seed);
    }

    //СОЗДАНИЕ ШУМОВОГО ПОЛЯ
    // Шумовое поле f_Ш(x,y)
    QVector<QVector<double>> noiseField(i_width, QVector<double>(i_height, 0.0));    
    for (int x = 0; x < i_width; ++x)
    {
        for (int y = 0; y < i_height; ++y)
        {
            noiseField[x][y] = randGenerator.bounded(RANGE); //rand 0 - 255
        }
    }
    qDebug() << "ШАГ 1 - Заполнили шумовое поле";

    //tech output
    // for(auto elem : noiseField)
    //     for(auto el : elem)
    //         std::cout<<el<<std::endl;
    
    // ВЫЧИСЛЕНИЕ МАТЕМАТИЧЕСКОГО ОЖИДАНИЯ ШУМОВОГО ПОЛЯ
    // MO_ш = (1/N²) * ΣΣ f_Ш(x,y)
    double mo_noise = 0.0; // MO_ш
    for (int x = 0; x < i_width; ++x)
    {
        for (int y = 0; y < i_height; ++y)
        {
            mo_noise += noiseField[x][y];
        }
    }
    mo_noise = mo_noise / N2;
    qDebug() << "ШАГ 2 - Математическое ожидание шумового поля (MO_ш):" << mo_noise;
    
    // ПРИМЕНЕНИЕ АДДИТИВНОГО ШУМА К ИЗОБРАЖЕНИЮ
    QImage noisyImage = image.copy();
    double v_err = 0.0; // ν_err накопленная ошибка
    // Временная переменная для расчета реальной энергии добавленного шума
    // double actualNoiseEnergy = 0.0;
    
    for (int x = 0; x < i_width; ++x)
    {
        for (int y = 0; y < i_height; ++y)
        {
            QRgb origColor = image.pixel(x, y);
            int original_brightness = qGray(origColor);
            // Значение пикселя с шумом
            // pix = f_исх(x,y) + η * (f_ш(x,y) - MO_ш) + ν
            double pix_value = original_brightness +
                              noiseLevel * (noiseField[x][y] - mo_noise) +
                              v_err;
            
            // ограничения диапазона [0, 255]
            int final_brightness;
            if (pix_value < 0)
            {
                // Случай pix < 0
                final_brightness = 0;
                v_err = -pix_value;
            }
            else if (pix_value > 255)
            {
                // Случай pix > 255
                final_brightness = 255;
                v_err = pix_value - 255;
            }
            else
            {
                // Случай 0 <= pix <= 255
                final_brightness = static_cast<int>(pix_value);
                v_err = 0.0;
            }
            //установка пикселя
            if (image.hasAlphaChannel())
            {
                noisyImage.setPixel(x, y, qRgb(final_brightness, final_brightness, final_brightness));
            }
            else
            {
                noisyImage.setPixel(x, y, qRgb(final_brightness, final_brightness, final_brightness));
            }
            
            // Вычисляем реальную добавленную энергию шума для этого пикселя
            // double addedNoise = final_brightness - original_brightness;
            // actualNoiseEnergy += abs(addedNoise) * abs(addedNoise);
        }
    }
    
    // // ВЫЧИСЛЕНИЕ ФАКТИЧЕСКОГО УРОВНЯ ШУМА
    // double actualNoiseLevel =  actualNoiseEnergy / energyOrig;
    // qDebug() << "Фактический уровень шума η (B_ш/B_исх):" << actualNoiseLevel;
    qDebug() << "Целевой уровень шума:" << noiseLevel;

    return noisyImage;
}

/**
 * @author Zhurckov G.S.
 * @brief Генерирует импульсный шум на изображении с заданным уровнем энергии
 * @param inputImage Входное изображение
 * @param noiseLevel Уровень шума (0.25, 0.5, 0.75)
 * @param type Тип импульсного шума (Salt, Pepper, SaltAndPepper)
 * @param intensity Интенсивность шума (Point, Line)
 * @param seed Начальное значение для ГПСЧ default(0 -> (new int))
 * @return Зашумленное изображение (импульсный шум)
 *
 * Алгоритм соответствует спецификации:
 * 1. Определяется тип импульсного шума и его интенсивность
 * 2. Вычисляется энергия исходного изображения
 * 3. Рассчитывается количество пикселей шума (KOL) в зависимости от типа
 * 4. Для шума "соль и перец" вычисляется математическое ожидание шумового поля
 * 5. Генерируются координаты шума и применяется шум к изображению
 * 6. Для строкового шума генерируется линия заданной длины
 */
QImage NoiseGenerator::generateImpulseNoise(QImage &inputImage, double noiseLevel, ImpulseNoiseType type, ImpulseNoiseIntensity intensity, quint32 seed)
{
    QImage image = inputImage.convertToFormat(QImage::Format_ARGB32);
    QImage image_n = image.copy();
    int i_width = image.width();
    int i_height = image.height();
    const int N2 = i_width * i_height;

    // ГПСЧ init
    QRandomGenerator randGenerator;
    if (seed == 0)
    {
        // используем мусор из памяти чтобы добится энтропии с использованием ГПСЧ
        randGenerator.seed(*(new int));
    }
    else
    {
        // используем заданное значение для возможости воспроизведния
        randGenerator.seed(seed);
    }

    if((type == ImpulseNoiseType::Salt) &&  (intensity == ImpulseNoiseIntensity::Point)){
        // Вычисляем B_text (сумма квадратов значений яркости исходного изображения)
        double B_ish = 0.0;
        for (int y = 0; y < i_height; y++) {
            for (int x = 0; x < i_width; x++) {
                int pixel_value = qGray(image.pixel(x, y));
                B_ish += pixel_value * pixel_value;
            }
        }
        
        double targetEta = noiseLevel; // input η value        
        double currentEta = 0.0; // curent η value
        int  addedNoisePixels = 0; //nums of modified pixels

        std::set<std::pair<int, int>> x_y_set{};
        while (currentEta <= targetEta) {
            // Генерируем случайные координаты
            int x = QRandomGenerator::global()->bounded(i_width);
            int y = QRandomGenerator::global()->bounded(i_height);
            if(x_y_set.find(std::make_pair(x,y)) != x_y_set.end()){
                continue;
            }
            else{
                x_y_set.insert(std::make_pair(x,y));
            }
            // Получаем текущее значение пикселя
            int originalValue = qGray(image.pixel(x, y));
            
            // Рассчитываем вклад этого пикселя в η
            double contribution = pow(255 - originalValue, 2);
            double newEta = currentEta + ( contribution / B_ish );
            
            image_n.setPixel(x, y, qRgb(255, 255, 255)); // set "salt" pixel
            addedNoisePixels++;
            currentEta = newEta;
                
                // qDebug() << "Добавлен шум в пиксель (" << x << "," << y 
                //          << "), добавлено:" << addedNoisePixels 
                //          << ", η:" << currentEta
                //          << ", need η:" << noiseLevel;
        }
        
        qDebug() << "Итоговые параметры шума:";
        qDebug() << "Добавлено пикселей:" << addedNoisePixels;
        qDebug() << "Общее η:" << currentEta;
        qDebug() << "Плотность шума:" << (double)addedNoisePixels / N2;
    }


    if((type == ImpulseNoiseType::Salt) &&  (intensity == ImpulseNoiseIntensity::Line)){
        // Вычисляем B_ish (сумма квадратов значений яркости исходного изображения)
        double B_ish = 0.0;
        for (int y = 0; y < i_height; y++) {
            for (int x = 0; x < i_width; x++) {
                int pixel_value = qGray(image.pixel(x, y));
                B_ish += pixel_value * pixel_value;
            }
        }
        
        // Целевое значение η (можно задать как параметр)
        double targetEta = noiseLevel; // или другое значение
        
        // Добавляем шум до достижения целевого η
        double currentEta = 0.0;
        std::set<std::pair<int, int>> x_y_set{};
        while (currentEta <= targetEta) {
            // Генерируем случайные координаты
            int x = QRandomGenerator::global()->bounded(i_width);
            int y = QRandomGenerator::global()->bounded(i_height);
            
            int line_size = QRandomGenerator::global()->bounded(RANGE_STR_LOW, RANGE_STR_HIGHT);
            
            // Получаем текущее значение пикселя
            if(x_y_set.find(std::make_pair(x,y)) == x_y_set.end()){
                x_y_set.insert(std::make_pair(x,y));
                for(int i = 0 ; i<line_size ; i++){
                    int originalValue = qGray(image.pixel(x+i, y));
                    // Рассчитываем вклад этого пикселя в η
                    double contribution = pow(255 - originalValue, 2);
                    double newEta = currentEta + ( contribution / B_ish );
                    // Устанавливаем пиксель в 255 (белый)
                    image_n.setPixel(x+i, y, qRgb(255, 255, 255));
                    currentEta = newEta;
                    // qDebug() << "Добавлен шум в пиксель (" << x << "," << y 
                    //          << ", η:" << currentEta
                    //          << ", need η:" << noiseLevel;
                }
                // qDebug() << "Добавлен шум в линию с начальной координатой (" << x << "," << y << ")"
                // << " и длинной "<<line_size; 
            }
        }
        
        qDebug() << "Итоговые параметры шума:";
        qDebug() << "Общее η:" << currentEta;
    }

    if((type == ImpulseNoiseType::Pepper) &&  (intensity == ImpulseNoiseIntensity::Point)){
        // Вычисляем B_ish (сумма квадратов значений яркости исходного изображения)
        double B_ish = 0.0;
        for (int y = 0; y < i_height; y++) {
            for (int x = 0; x < i_width; x++) {
                int pixel_value = qGray(image.pixel(x, y));
                B_ish += pixel_value * pixel_value;
            }
        }
        
        // Целевое значение η (можно задать как параметр)
        double targetEta = noiseLevel; // или другое значение
        
        // Добавляем шум до достижения целевого η
        double currentEta = 0.0;
        while (currentEta <= targetEta) {
            // Генерируем случайные координаты
            int x = QRandomGenerator::global()->bounded(i_width);
            int y = QRandomGenerator::global()->bounded(i_height);
            
            // Получаем текущее значение пикселя
            int originalValue = qGray(image.pixel(x, y));
            
            // Рассчитываем вклад этого пикселя в η
            double contribution = pow(originalValue, 2);
            double newEta = currentEta + ( contribution / B_ish );
            
            image_n.setPixel(x, y, qRgb(0, 0, 0));// set "pepper" pixel
            currentEta = newEta;
                
            // qDebug() << "Добавлен шум в пиксель (" << x << "," << y 
            //          << ", η:" << currentEta
            //          << ", need η:" << noiseLevel;
        }
        
        qDebug() << "Итоговые параметры шума:";
        qDebug() << "Общее η:" << currentEta;
    }


    if((type == ImpulseNoiseType::Pepper) &&  (intensity == ImpulseNoiseIntensity::Line)){
        // Вычисляем B_ish (сумма квадратов значений яркости исходного изображения)
        double B_ish = 0.0;
        for (int y = 0; y < i_height; y++) {
            for (int x = 0; x < i_width; x++) {
                int pixel_value = qGray(image.pixel(x, y));
                B_ish += pixel_value * pixel_value;
            }
        }
        
        // Целевое значение η (можно задать как параметр)
        double targetEta = noiseLevel; // или другое значение
        
        // Добавляем шум до достижения целевого η
        double currentEta = 0.0;
        std::set<std::pair<int, int>> x_y_set{};
        while (currentEta <= targetEta) {
            // Генерируем случайные координаты
            int x = QRandomGenerator::global()->bounded(i_width);
            int y = QRandomGenerator::global()->bounded(i_height);
            int line_size = QRandomGenerator::global()->bounded(RANGE_STR_LOW, RANGE_STR_HIGHT);
            // Получаем текущее значение пикселя
            if(x_y_set.find(std::make_pair(x,y)) == x_y_set.end()){
                x_y_set.insert(std::make_pair(x,y));
                for(int i = 0 ; i<line_size ; i++){
                    // Получаем текущее значение пикселя
                    int originalValue = qGray(image.pixel(x+i, y));
                    
                    // Рассчитываем вклад этого пикселя в η
                    double contribution = pow(originalValue, 2);
                    double newEta = currentEta + ( contribution / B_ish );
                    
                    image_n.setPixel(x+i, y, qRgb(0, 0, 0));// set "pepper" pixel
                    currentEta = newEta;
                    // qDebug() << "Добавлен шум в пиксель (" << x << "," << y 
                    //          << ", η:" << currentEta
                    //          << ", need η:" << noiseLevel;
                }
                // qDebug() << "Добавлен шум в линию с начальной координатой (" << x << "," << y << ")"
                // << " и длинной "<<line_size; 
            }
        }
        
        qDebug() << "Итоговые параметры шума:";
        qDebug() << "Общее η:" << currentEta;

    }
    // if((type == ImpulseNoiseType::SaltAndPepper) &&  (intensity == ImpulseNoiseIntensity::Point)){}
    // if((type == ImpulseNoiseType::SaltAndPepper) &&  (intensity == ImpulseNoiseIntensity::Line)){}
    
    return image_n;
}
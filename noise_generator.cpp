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
 *TODO доплнить документацию
 * Алгоритм соответствует спецификации: (переписан)
 */
QImage NoiseGenerator::generateAdditiveNoise(QImage &inputImage, double eta, quint32 seed)
{    
    QImage image = inputImage.convertToFormat(QImage::Format_Grayscale8);
    
    int width = image.width();    // iSize
    int height = image.height();   // jSize
    const int pixelCount = width * height; // N²

    // Инициализация ГПСЧ
    QRandomGenerator randGenerator;
    if (seed == 0)
    {
        randGenerator.seed(*(new int));
    }
    else
    {
        randGenerator.seed(seed);
    }

    // ШАГ 1: Создание шумового поля
    QVector<QVector<double>> noiseField(width, QVector<double>(height, 0.0));
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            noiseField[i][j] = randGenerator.bounded(255.0); // 0-255
        }
    }
    qDebug() << "ШАГ 1 - Заполнили шумовое поле";

    // ШАГ 2: Вычисление математического ожидания шумового поля
    // ШАГ 3: Вычисление суммы квадратов шума (V_sh)
    // ШАГ 4: Вычисление суммы квадратов исходного изображения (V_ish)
    double V_sh = 0.0;
    double mo_noise = 0.0; // MO_Noice
    double V_ish = 0.0;
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            V_sh += noiseField[i][j] * noiseField[i][j];
            mo_noise += noiseField[i][j];
            
            QRgb pixel = image.pixel(i, j);
            int brightness = qGray(pixel);
            V_ish += brightness * brightness;
        }
    }
    mo_noise = mo_noise / pixelCount;
    qDebug() << "ШАГ 2 - Математическое ожидание шумового поля (MO_ш):" << mo_noise;
    qDebug() << "ШАГ 3 - Сумма квадратов шума (V_sh):" << V_sh;
    qDebug() << "ШАГ 4 - Сумма квадратов исходного изображения (V_ish):" << V_ish;

    // ШАГ 5: Вычисление коэффициента Nu
    double Nu = V_ish / V_sh ; //тут вопрос а точно V_ish на V_sh а не на оборот 
    qDebug() << "ШАГ 5 - Коэффициент Nu (V_ish/V_sh):" << Nu;

    // ШАГ 6: Применение адаптивного аддитивного шума
    QImage noisyImage = image.copy();
    int v_err = 0;
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            QRgb origColor = image.pixel(i, j);
            int originalBrightness = qGray(origColor);
            
            // Формула из вашего Delphi кода:
            // pix = sImage[i,j] + eta * (Noice_Img[i,j] - MO_Noice) * (V_ish/V_sh)
            double noiseComponent = noiseField[i][j] - mo_noise;
            double pixValue = originalBrightness + eta * noiseComponent * Nu;
            // Ограничение диапазона и установка пикселя
            int finalBrightness;
            if (pixValue < 0)
            {
                finalBrightness = 0;
            }
            else if (pixValue > 255)
            {
                finalBrightness = 255;
            }
            else
            {
                finalBrightness = static_cast<int>(pixValue);
            }
            
            noisyImage.setPixel(i, j, qRgb(finalBrightness, finalBrightness, finalBrightness));
        }
    }
    
    qDebug() << "ШАГ 6 - Применен адаптивный аддитивный шум с eta =" << eta;
    qDebug() << "Соотношение энергий (B_исх/B_ш):" << Nu;

    return noisyImage;
}

/**
 * @author Zhurckov G.S.
 * @brief Генерирует импульсный шум на изображении с заданным уровнем энергии
 * @param inputImage Входное изображение
 * @param noiseLevel Уровень шума (0.25, 0.5, 0.75)
 * @param type Тип импульсного шума (salt, pepper, salt_and_pepper)
 * @param form Интенсивность шума (point, line)
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
QImage NoiseGenerator::generateImpulseNoise(QImage &inputImage, double noiseLevel, inmpulse_noise_type type, impulse_noise_form form, quint32 seed)
{
    QImage image = inputImage.convertToFormat(QImage::Format_Grayscale8);
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
    qDebug() << "Математическое ожидание шумового поля (MO_ш):" << mo_noise;

    if((type == inmpulse_noise_type::salt) &&  (form == impulse_noise_form::point)){
        // Вычисляем B_text (сумма квадратов значений яркости исходного изображения)
        double B_ish = 0.0;
        for (int y = 0; y < i_height; y++) {
            for (int x = 0; x < i_width; x++) {
                int pixel_value = qGray(image.pixel(x, y));
                B_ish += pixel_value * pixel_value;
            }
        }
        
        double targeeta = noiseLevel; // input η value        
        double curreneta = 0.0; // curent η value
        int  addedNoisePixels = 0; //nums of modified pixels

        std::set<std::pair<int, int>> x_y_set{};
        while (curreneta <= targeeta) {
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
            double newEta = curreneta + ( contribution / B_ish );
            
            image_n.setPixel(x, y, qRgb(255, 255, 255)); // set "salt" pixel
            addedNoisePixels++;
            curreneta = newEta;
                
                // qDebug() << "Добавлен шум в пиксель (" << x << "," << y 
                //          << "), добавлено:" << addedNoisePixels 
                //          << ", щас η:" << curreneta
                //          << ", need η:" << noiseLevel
                //          << "size added " << x_y_set.size();
        }
        
        qDebug() << "Итоговые параметры шума:";
        qDebug() << "Добавлено пикселей:" << addedNoisePixels;
        qDebug() << "Общее η:" << curreneta;
        qDebug() << "Плотность шума:" << (double)addedNoisePixels / N2;
    }


    if((type == inmpulse_noise_type::salt) &&  (form == impulse_noise_form::line)){
        // Вычисляем B_ish (сумма квадратов значений яркости исходного изображения)
        double B_ish = 0.0;
        for (int y = 0; y < i_height; y++) {
            for (int x = 0; x < i_width; x++) {
                int pixel_value = qGray(image.pixel(x, y));
                B_ish += pixel_value * pixel_value;
            }
        }
        
        // Целевое значение η 
        double targeeta = noiseLevel;
        
        // Добавляем шум до достижения целевого η
        double curreneta = 0.0;
        std::set<std::pair<int, int>> x_y_set{};
        while (curreneta <= targeeta) {
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
                    double newEta = curreneta + ( contribution / B_ish );
                    // Устанавливаем пиксель в 255 (белый)
                    image_n.setPixel(x+i, y, qRgb(255, 255, 255));
                    curreneta = newEta;
                    
                    x_y_set.insert(std::make_pair(x+i,y));
                    
                    // qDebug() << "Добавлен шум в пиксель (" << x << "," << y 
                    //          << ", η:" << curreneta
                    //          << ", need η:" << noiseLevel;
                }
                // qDebug() << "Добавлен шум в линию с начальной координатой (" << x << "," << y << ")"
                // << " и длинной "<<line_size; 
            }
        }
        
        qDebug() << "Итоговые параметры шума:";
        qDebug() << "Общее η:" << curreneta;
    }

    if((type == inmpulse_noise_type::pepper) &&  (form == impulse_noise_form::point)){
        // Вычисляем B_ish (сумма квадратов значений яркости исходного изображения)
        double B_ish = 0.0;
        for (int y = 0; y < i_height; y++) {
            for (int x = 0; x < i_width; x++) {
                int pixel_value = qGray(image.pixel(x, y));
                B_ish += pixel_value * pixel_value;
            }
        }
        
        // Целевое значение η 
        double targeeta = noiseLevel;
        
        // Добавляем шум до достижения целевого η
        double curreneta = 0.0;
        while (curreneta <= targeeta) {
            // Генерируем случайные координаты
            int x = QRandomGenerator::global()->bounded(i_width);
            int y = QRandomGenerator::global()->bounded(i_height);
            
            // Получаем текущее значение пикселя
            int originalValue = qGray(image.pixel(x, y));
            
            // Рассчитываем вклад этого пикселя в η
            double contribution = pow(originalValue, 2);
            double newEta = curreneta + ( contribution / B_ish );
            
            image_n.setPixel(x, y, qRgb(0, 0, 0));// set "pepper" pixel
            curreneta = newEta;
                
            // qDebug() << "Добавлен шум в пиксель (" << x << "," << y 
            //          << ", η:" << curreneta
            //          << ", need η:" << noiseLevel;
        }
        
        qDebug() << "Итоговые параметры шума:";
        qDebug() << "Общее η:" << curreneta;
    }


    if((type == inmpulse_noise_type::pepper) &&  (form == impulse_noise_form::line)){
        // Вычисляем B_ish (сумма квадратов значений яркости исходного изображения)
        double B_ish = 0.0;
        for (int y = 0; y < i_height; y++) {
            for (int x = 0; x < i_width; x++) {
                int pixel_value = qGray(image.pixel(x, y));
                B_ish += pixel_value * pixel_value;
            }
        }
        
        // Целевое значение η (можно задать как параметр)
        double targeeta = noiseLevel; // или другое значение
        
        // Добавляем шум до достижения целевого η
        double curreneta = 0.0;
        std::set<std::pair<int, int>> x_y_set{};
        while (curreneta <= targeeta) {
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
                    double newEta = curreneta + ( contribution / B_ish );
                    
                    image_n.setPixel(x+i, y, qRgb(0, 0, 0));// set "pepper" pixel
                    curreneta = newEta;
                    
                    x_y_set.insert(std::make_pair(x+i,y));

                    // qDebug() << "Добавлен шум в пиксель (" << x << "," << y 
                    //          << ", η:" << curreneta
                    //          << ", need η:" << noiseLevel;
                }
                // qDebug() << "Добавлен шум в линию с начальной координатой (" << x << "," << y << ")"
                // << " и длинной "<<line_size; 
            }
        }
        
        qDebug() << "Итоговые параметры шума:";
        qDebug() << "Общее η:" << curreneta;

    }
    if((type == inmpulse_noise_type::salt_and_pepper) &&  (form == impulse_noise_form::point)){
        // Вычисляем B_ish (сумма квадратов значений яркости исходного изображения)
        double B_ish = 0.0;
        for (int y = 0; y < i_height; y++) {
            for (int x = 0; x < i_width; x++) {
                int pixel_value = qGray(image.pixel(x, y));
                B_ish += pixel_value * pixel_value;
            }
        }
        
        // Целевое значение η (можно задать как параметр)
        double targeeta = noiseLevel; // или другое значение
        
        // Добавляем шум до достижения целевого η
        double curreneta = 0.0;
        std::set<std::pair<int, int>> x_y_set{};
        while (curreneta <= targeeta) {
            // Генерируем случайные координаты
            int x = QRandomGenerator::global()->bounded(i_width);
            int y = QRandomGenerator::global()->bounded(i_height);
            
            // Получаем текущее значение пикселя
            if(x_y_set.find(std::make_pair(x,y)) == x_y_set.end()){
                x_y_set.insert(std::make_pair(x,y));
                int originalValue = qGray(image.pixel(x, y));
                if(originalValue < mo_noise){
                    // Рассчитываем вклад этого пикселя в η
                    double contribution = pow(255 - originalValue, 2);
                    double newEta = curreneta + ( contribution / B_ish );
                    
                    image_n.setPixel(x, y, qRgb(255, 255, 255)); // set "salt" pixel
                    curreneta = newEta;
                }
                else{
                    // Рассчитываем вклад этого пикселя в η
                    double contribution = pow(originalValue, 2);
                    double newEta = curreneta + ( contribution / B_ish );
                    
                    image_n.setPixel(x, y, qRgb(0, 0, 0));// set "pepper" pixel
                    curreneta = newEta;
                }
            }   
            // qDebug() << "Добавлен шум в пиксель (" << x << "," << y 
            //          << ", η:" << curreneta
            //          << ", need η:" << noiseLevel;
        }
        
        qDebug() << "Итоговые параметры шума:";
        qDebug() << "Общее η:" << curreneta;
    }
    if((type == inmpulse_noise_type::salt_and_pepper) &&  (form == impulse_noise_form::line)){
        // Вычисляем B_ish (сумма квадратов значений яркости исходного изображения)
        double B_ish = 0.0;
        for (int y = 0; y < i_height; y++) {
            for (int x = 0; x < i_width; x++) {
                int pixel_value = qGray(image.pixel(x, y));
                B_ish += pixel_value * pixel_value;
            }
        }
        
        // Целевое значение η (можно задать как параметр)
        double targeeta = noiseLevel; // или другое значение
        
        // Добавляем шум до достижения целевого η
        double curreneta = 0.0;
        std::set<std::pair<int, int>> x_y_set{};
        while (curreneta <= targeeta) {
            // Генерируем случайные координаты
            int x = QRandomGenerator::global()->bounded(i_width);
            int y = QRandomGenerator::global()->bounded(i_height);
            int line_size = QRandomGenerator::global()->bounded(RANGE_STR_LOW, RANGE_STR_HIGHT);
            // Получаем текущее значение пикселя
            if(x_y_set.find(std::make_pair(x,y)) == x_y_set.end()){
                x_y_set.insert(std::make_pair(x,y));
                int originalValue = qGray(image.pixel(x, y));
                for(int i = 0 ; i<line_size ; i++){
                    if(originalValue < mo_noise){
                        // Рассчитываем вклад этого пикселя в η
                        double contribution = pow(255 - originalValue, 2);
                        double newEta = curreneta + ( contribution / B_ish );
                        
                        image_n.setPixel(x+i, y, qRgb(255, 255, 255)); // set "salt" pixel
                        curreneta = newEta;
                    }
                    else{
                        // Рассчитываем вклад этого пикселя в η
                        double contribution = pow(originalValue, 2);
                        double newEta = curreneta + ( contribution / B_ish );
                        
                        image_n.setPixel(x+i, y, qRgb(0, 0, 0));// set "pepper" pixel
                        curreneta = newEta;
                    }
                }
                // qDebug() << "Добавлен шум в линию с начальной координатой (" << x << "," << y << ")"
                // << " и длинной "<<line_size; 
            }
        }
        
        qDebug() << "Итоговые параметры шума:";
        qDebug() << "Общее η:" << curreneta;
    }
    
    return image_n;
}
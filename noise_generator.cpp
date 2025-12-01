#include "noise_generator.h"

#include <QImage>
#include <QRandomGenerator>
#include <QtMath>
#include <QDate>
#include <QDebug>
#include <iostream>

#define RANGE 256
/**
 * @author Zhurckov G.S.
 * @brief Генерирует аддитивный шум на изображении с заданным уровнем энергии
 * @param inputImage Входное изображение
 * @param noiseLevel Уровень шума (0.25, 0.5, 0.75)
 * @param seed Начальное значение для ГПСЧ default(0 -> (new int))
 * @return Зашумленное изображение  (аддитивный шум)
 *
 * Алгоритм соответствует спецификации:
 * 1. Создается шумовое поле 512x512 со случайными значениями
 * 2. Вычисляется математическое ожидание шумового поля
 * 3. Для каждого пикселя применяется формула с контролем диапазона [0, 255]
 * 4. Осуществляется перенос переполнения/недополнения (error diffusion)
 */
QImage NoiseGenerator::generateAdditiveNoise(QImage &inputImage, double noiseLevel, quint32 seed)
{    
    QImage image = inputImage.convertToFormat(QImage::Format_ARGB32);

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
    // Максимальный диапазон для равномерного распределения
    
    // Заполняем шумовое поле случайными значениями с равномерным распределением
    // Функция RANDOM(RANGE) возвращает значения в диапазоне [0, RANGE-1]
    for (int x = 0; x < i_width; ++x)
    {
        for (int y = 0; y < i_height; ++y)
        {
            noiseField[x][y] = randGenerator.bounded(RANGE);
        }
    }
    //tech output
    // for(auto elem : noiseField)
    //     for(auto el : elem)
    //         std::cout<<el<<std::endl;

    // ВЫЧИСЛЕНИЕ МАТЕМАТИЧЕСКОГО ОЖИДАНИЯ ШУМОВОГО ПОЛЯ 
    // MO_ш = (1/N²) * ΣΣ f_Ш(x,y)
    double moNoise = 0.0; // MO_ш
    for (int x = 0; x < i_width; ++x)
    {
        for (int y = 0; y < i_height; ++y)
        {
            moNoise += noiseField[x][y];
        }
    }
    moNoise /= N2; // Деление на N²

    qDebug() << "Математическое ожидание шумового поля (MO_ш):" << moNoise;

    // ВЫЧИСЛЕНИЕ ЭНЕРГИИ ИСХОДНОГО ИЗОБРАЖЕНИЯ (остановился на рефакторинге тут)
    // B_исх = ΣΣ [f_исх(x,y)]²
    double energyOrig = 0.0; // B_исх
    for (int x = 0; x < i_width; ++x)
    {
        for (int y = 0; y < i_height; ++y)
        {
            QColor color = image.pixelColor(x, y);
            // Используем яркость пикселя (можно использовать один из каналов или яркость)
            int brightness = qGray(color.rgb());
            energyOrig += brightness * brightness;
        }
    }

    qDebug() << "Энергия исходного изображения (B_исх):" << energyOrig;

    // РАСЧЕТ ЭНЕРГИИ ШУМА 
    // B_ш = η * B_исх
    double energyNoise = noiseLevel * energyOrig;
    qDebug() << "Целевая энергия шума (B_ш):" << energyNoise;

    // ПРИМЕНЕНИЕ АДДИТИВНОГО ШУМА К ИЗОБРАЖЕНИЮ 
    QImage noisyImage = image.copy();
    double cumulativeError = 0.0; // ν = err (накопленная ошибка)

    // Временная переменная для расчета реальной энергии добавленного шума
    double actualNoiseEnergy = 0.0;

    for (int x = 0; x < i_width; ++x)
    {
        for (int y = 0; y < i_height; ++y)
        {
            // Яркость исходного пикселя
            QColor origColor = image.pixelColor(x, y);
            int origBrightness = qGray(origColor.rgb());

            // Вычисляем значение пикселя с шумом согласно формуле:
            // pix = f_исх(x,y) + η * (f_ш(x,y) - MO_ш) + ν
            double pixValue = origBrightness +
                              noiseLevel * (noiseField[x][y] - moNoise) +
                              cumulativeError;

            // Применяем ограничение диапазона [0, 255]
            int finalBrightness;

            if (pixValue < 0)
            {
                // Случай pix < 0
                finalBrightness = 0;
                cumulativeError = -pixValue; // ν := -pix (положительная величина)
            }
            else if (pixValue > 255)
            {
                // Случай pix > 255
                finalBrightness = 255;
                cumulativeError = pixValue - 255; // ν := pix - 255 (положительная величина)
            }
            else
            {
                // Случай 0 <= pix <= 255
                finalBrightness = static_cast<int>(pixValue);
                cumulativeError = 0.0; // Сбрасываем ошибку, если значение в диапазоне
            }

            // Устанавливаем новый пиксель
            // Сохраняем альфа-канал и цветовую модель
            if (image.hasAlphaChannel())
            {
                noisyImage.setPixelColor(x, y, QColor(finalBrightness, finalBrightness, finalBrightness, origColor.alpha()));
            }
            else
            {
                noisyImage.setPixelColor(x, y, QColor(finalBrightness, finalBrightness, finalBrightness));
            }

            // Вычисляем реальную добавленную энергию шума для этого пикселя
            double addedNoise = finalBrightness - origBrightness;
            actualNoiseEnergy += addedNoise * addedNoise;
        }
    }

    // ВЫЧИСЛЕНИЕ ФАКТИЧЕСКОГО УРОВНЯ ШУМА
    double actualNoiseLevel = actualNoiseEnergy / energyOrig;
    qDebug() << "Фактический уровень шума η (B_ш/B_исх):" << actualNoiseLevel;
    qDebug() << "Целевой уровень шума:" << noiseLevel;
    qDebug() << "Разница:" << (actualNoiseLevel - noiseLevel);

    // ВАЛИДАЦИЯ РЕЗУЛЬТАТА
    if (qAbs(actualNoiseLevel - noiseLevel) > 0.01)
    {
        qWarning() << "Фактический уровень шума значительно отличается от целевого!"
                   << "Возможно, требуется коррекция алгоритма.";
    }

    return noisyImage;
}

// /**
//  * @brief Вспомогательная функция для генерации аддитивного шума с предустановками
//  * @param imagePath Путь к входному изображению
//  * @param noiseLevels Список уровней шума для генерации
//  * @param outputPrefix Префикс для имен выходных файлов
//  */
// void generateAdditiveNoiseExamples(const QString &imagePath,
//                                    const QVector<double> &noiseLevels = {0.25, 0.5, 0.75},
//                                    const QString &outputPrefix = "noisy_image")
// {
//     QImage inputImage(imagePath);
//     if (inputImage.isNull())
//     {
//         qWarning() << "Не удалось загрузить изображение:" << imagePath;
//         return;
//     }

//     // Приводим к размеру 512x512, если необходимо
//     if (inputImage.width() != 512 || inputImage.height() != 512)
//     {
//         inputImage = inputImage.scaled(512, 512, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//         qDebug() << "Изображение масштабировано до 512x512";
//     }

//     // Фиксированный seed для воспроизводимости результатов
//     const quint32 seed = 12345;

//     for (double noiseLevel : noiseLevels)
//     {
//         QImage noisyImage = generateAdditiveNoise(inputImage, noiseLevel, seed);

//         if (!noisyImage.isNull())
//         {
//             QString outputPath = QString("%1_η%2.png")
//                                      .arg(outputPrefix)
//                                      .arg(noiseLevel, 0, 'f', 2);

//             if (noisyImage.save(outputPath))
//             {
//                 qDebug() << "Сохранено изображение с шумом:" << outputPath
//                          << "η =" << noiseLevel;
//             }
//             else
//             {
//                 qWarning() << "Не удалось сохранить:" << outputPath;
//             }
//         }
//     }
// }

// /**
//  * @brief Демонстрационная функция с подробными комментариями
//  */
// void demonstrateAdditiveNoise()
// {
//     qDebug() << "=== ДЕМОНСТРАЦИЯ АДДИТИВНОГО ШУМА ===";
//     qDebug() << "Алгоритм работы:";
//     qDebug() << "1. Инициализация генератора случайных чисел (RANDOMIZE)";
//     qDebug() << "2. Создание шумового поля 512x512 со значениями 0-255";
//     qDebug() << "3. Вычисление MO_ш - среднего значения шумового поля";
//     qDebug() << "4. Расчет энергии исходного изображения B_исх";
//     qDebug() << "5. Определение целевой энергии шума B_ш = η * B_исх";
//     qDebug() << "6. Применение формулы: pix = f_исх + η*(f_ш - MO_ш) + ν";
//     qDebug() << "7. Ограничение результата диапазоном [0, 255] с переносом ошибки";
//     qDebug() << "8. Расчет фактического уровня шума для проверки точности";

//     // Пример использования
//     generateAdditiveNoiseExamples("input_image.png");
// }

// // Пример использования в основном коде:
// int main(int argc, char *argv[])
// {
//     QCoreApplication app(argc, argv);

//     // Загружаем изображение
//     QImage inputImage("path/to/your/image.png");

//     if (inputImage.isNull())
//     {
//         qDebug() << "Не удалось загрузить изображение";
//         return -1;
//     }

//     // Генерируем изображения с разным уровнем шума
//     QImage noisyImage25 = generateAdditiveNoise(inputImage, 0.25);
//     QImage noisyImage50 = generateAdditiveNoise(inputImage, 0.50);
//     QImage noisyImage75 = generateAdditiveNoise(inputImage, 0.75);

//     // Сохраняем результаты
//     noisyImage25.save("noisy_25.png");
//     noisyImage50.save("noisy_50.png");
//     noisyImage75.save("noisy_75.png");

//     // Демонстрация с подробным выводом
//     demonstrateAdditiveNoise();

//     return 0;
// }
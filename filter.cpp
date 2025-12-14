#include "filter.h"

QImage Filters::apply_median_filtr(QImage &inputImage)
{
    int maskSize = 3;
    // Проверка формата изображения
    if (inputImage.format() != QImage::Format_Grayscale8) {
        qWarning() << "Изображение должно быть в формате Format_Grayscale8";
        return QImage();
    }
    
    // Проверка размера маски
    if (maskSize % 2 == 0) {
        qWarning() << "Размер маски должен быть нечетным";
        return QImage();
    }
    
    if (maskSize < 3) {
        qWarning() << "Размер маски должен быть не менее 3";
        return QImage();
    }
    
    // Получаем размеры изображения
    int width = inputImage.width();
    int height = inputImage.height();
    int radius = maskSize / 2;
    
    // Создаем выходное изображение
    QImage outputImage(width, height, QImage::Format_Grayscale8);
    
    // Вектор для хранения значений из окна
    QVector<uchar> windowValues;
    windowValues.reserve(maskSize * maskSize);
    
    // Обрабатываем все пиксели, кроме краевых
    for (int y = radius; y < height - radius; ++y) {
        uchar *outputScanLine = outputImage.scanLine(y);
        
        for (int x = radius; x < width - radius; ++x) {
            // Очищаем вектор для нового окна
            windowValues.clear();
            
            // Собираем все значения из окна
            for (int dy = -radius; dy <= radius; ++dy) {
                const uchar *inputScanLine = inputImage.scanLine(y + dy);
                for (int dx = -radius; dx <= radius; ++dx) {
                    windowValues.append(inputScanLine[x + dx]);
                }
            }
            
            // Находим медианное значение
            std::nth_element(windowValues.begin(), 
                           windowValues.begin() + windowValues.size() / 2,
                           windowValues.end());
            
            // Медиана - средний элемент отсортированного массива
            uchar medianValue = windowValues[windowValues.size() / 2];
            
            // Записываем результат
            outputScanLine[x] = medianValue;
        }
    }
    
    return outputImage;
}

QImage Filters::apply_uniform_area_smoothing(QImage &inputImage)
{
    int windowSize = 5;
    int smallWindowSize = 3;

    // Проверяем формат изображения
    if (inputImage.format() != QImage::Format_Grayscale8)
    {
        qDebug() << "Изображение должно быть в формате Format_Grayscale8";
        return QImage();
    }

    if (windowSize % 2 == 0 || smallWindowSize % 2 == 0)
    {
        qDebug() << "Размеры окон должны быть нечетными";
        return QImage();
    }

    if (smallWindowSize >= windowSize)
    {
        qDebug() << "Размер малого окна должен быть меньше размера большого окна";
        return QImage();
    }

    int width = inputImage.width();
    int height = inputImage.height();

    // выходное изображение
    QImage outputImage(width, height, QImage::Format_Grayscale8);

    // радиусы для окон
    int bigRadius = windowSize / 2;
    int smallRadius = smallWindowSize / 2;

    // временные буферы для хранения значений яркости
    QVector<QVector<uchar>> pixelBuffer(height, QVector<uchar>(width, 0));

    // copy данные из QImage в буфер для быстрого доступа
    for (int y = 0; y < height; ++y)
    {
        const uchar *scanLine = inputImage.scanLine(y);
        for (int x = 0; x < width; ++x)
        {
            pixelBuffer[y][x] = scanLine[x];
        }
    }

    // проход по всем пикселям изображения (кроме краев)
    for (int y = bigRadius; y < height - bigRadius; ++y)
    {
        for (int x = bigRadius; x < width - bigRadius; ++x)
        {
            // 1. яркость центральной точки большого окна
            uchar centerBrightness = pixelBuffer[y][x];

            // 2. Координаты центров малых окон (3x3) в пределах большого окна (5x5)
            // Для окна 5x5 9 малых окон 3x3
            QVector<QPoint> smallWindowCenters;

            // все возможные центры малых окон в пределах большого окна
            for (int dy = -bigRadius + smallRadius; dy <= bigRadius - smallRadius; ++dy)
            {
                for (int dx = -bigRadius + smallRadius; dx <= bigRadius - smallRadius; ++dx)
                {
                    smallWindowCenters.append(QPoint(x + dx, y + dy));
                }
            }

            // 3. V(k) для каждого малого окна
            QVector<double> V_values(smallWindowCenters.size(), 0.0);
            double min_V = std::numeric_limits<double>::max();
            int min_V_index = 0;

            for (int k = 0; k < smallWindowCenters.size(); ++k)
            {
                QPoint center = smallWindowCenters[k];
                double V_k = 0.0;

                // абсолютные разности для всех точек малого окна
                for (int dy = -smallRadius; dy <= smallRadius; ++dy)
                {
                    for (int dx = -smallRadius; dx <= smallRadius; ++dx)
                    {
                        int px = center.x() + dx;
                        int py = center.y() + dy;

                        // яркость точки малого окна
                        uchar fk = pixelBuffer[py][px];

                        // абсолютную разность
                        V_k += std::abs(static_cast<double>(centerBrightness) - static_cast<double>(fk));
                    }
                }

                V_values[k] = V_k;

                // окно с минимальным V(k)
                if (V_k < min_V)
                {
                    min_V = V_k;
                    min_V_index = k;
                }
            }

            // 4. средняя яркость в наиболее однородном окне
            QPoint bestCenter = smallWindowCenters[min_V_index];
            double sum = 0.0;
            int count = 0;

            for (int dy = -smallRadius; dy <= smallRadius; ++dy)
            {
                for (int dx = -smallRadius; dx <= smallRadius; ++dx)
                {
                    int px = bestCenter.x() + dx;
                    int py = bestCenter.y() + dy;

                    sum += pixelBuffer[py][px];
                    count++;
                }
            }

            uchar newBrightness = static_cast<uchar>(std::round(sum / count));

            // 5. Записываем результат в выходное изображение
            outputImage.scanLine(y)[x] = newBrightness;
        }
    }

    // Копируем краевые пиксели без изменений
    for (int y = 0; y < height; ++y)
    {
        uchar *outputScanLine = outputImage.scanLine(y);
        const uchar *inputScanLine = inputImage.scanLine(y);

        for (int x = 0; x < width; ++x)
        {
            // Если пиксель на краю, копируем исходное значение
            if (y < bigRadius || y >= height - bigRadius ||
                x < bigRadius || x >= width - bigRadius)
            {
                outputScanLine[x] = inputScanLine[x];
            }
        }
    }

    return outputImage;
}

QImage Filters::apply_mask_smoothing(QImage &inputImage)
{

    // Проверяем формат изображения
    if (inputImage.format() != QImage::Format_Grayscale8)
    {
        qDebug() << "Изображение должно быть в формате Format_Grayscale8";
        return QImage();
    }
    int maskSize = 5;
    int width = inputImage.width();
    int height = inputImage.height();
    int radius = maskSize / 2;

    QImage outputImage(width, height, QImage::Format_Grayscale8);

    for (int y = radius; y < height - radius; ++y)
    {
        const uchar *scanLine = inputImage.scanLine(y);
        uchar *outputScanLine = outputImage.scanLine(y);

        for (int x = radius; x < width - radius; ++x)
        {
            double sum = 0;
            int count = 0;

            for (int dy = -radius; dy <= radius; ++dy)
            {
                const uchar *line = inputImage.scanLine(y + dy);
                for (int dx = -radius; dx <= radius; ++dx)
                {
                    sum += line[x + dx];
                    count++;
                }
            }

            outputScanLine[x] = static_cast<uchar>(std::round(sum / count));
        }
    }

    return outputImage;
}
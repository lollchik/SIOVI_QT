#include "filter.h"

QImage Filters::erosion(const QImage &inputImage, int maskSize,
                      StructuringElement elementType)
{
    if (inputImage.format() != QImage::Format_Grayscale8)
    {
        qWarning() << "Изображение должно быть в формате Format_Grayscale8";
        return QImage();
    }

    int width = inputImage.width();
    int height = inputImage.height();
    int radius = maskSize / 2;

    // Создаем структурный элемент
    QVector<QVector<bool>> mask = {        
        {true, true, true},
        {true, true, true},
        {true, true, true}};

    // Создаем выходное изображение
    QImage outputImage(width, height, QImage::Format_Grayscale8);
    outputImage.fill(255); // Изначально все фон (белый)

    //обработка ИЗО начиная с 1 элемента, являющимся не краевым
    for (int y = radius; y < height - radius; ++y)
    {
        const uchar *inputLine = inputImage.scanLine(y);
        uchar *outputLine = outputImage.scanLine(y);

        for (int x = radius; x < width - radius; ++x)
        {
            // Проверяем текущий пиксель
            if (inputLine[x]<=127)
            {
                // Если текущий пиксель - фон, результат тоже фон
                outputLine[x] = 0;
                continue;
            }

            // Проверяем окрестность
            bool is_not_background = true;

            for (int dy = -radius; dy <= radius; ++dy)
            {
                const uchar *neighborLine = inputImage.scanLine(y + dy);

                for (int dx = -radius; dx <= radius; ++dx)
                {
                    // если совпадает с положением 1 по маске
                    if (mask[dy + radius][dx + radius])
                    {
                        uchar neighborValue = neighborLine[x + dx];

                        // Если хотя бы один сосед - фон
                        if (neighborValue<=127)
                        {
                            is_not_background = false;
                            break;
                        }
                    }
                }
                if (!is_not_background)
                    break;
            }

            //  запись результатоy
            outputLine[x] = is_not_background ? 255 : 0;
        }
    }

    return outputImage;
}

QImage Filters::dilation(const QImage &inputImage, int maskSize, StructuringElement elementType)
{
    if (inputImage.format() != QImage::Format_Grayscale8)
    {
        qWarning() << "Изображение должно быть в формате Format_Grayscale8";
        return QImage();
    }

    int width = inputImage.width();
    int height = inputImage.height();
    int radius = maskSize / 2;

    // Создаем структурный элемент
    QVector<QVector<bool>> mask = QVector<QVector<bool>>{
        {true, true, true},
        {true, true, true},
        {true, true, true}};

    // Создаем выходное изображение
    QImage outputImage(width, height, QImage::Format_Grayscale8);
    outputImage.fill(0); // Изначально все фон (черный)

    // Обрабатываем внутренние пиксели
    for (int y = radius; y < height - radius; ++y)
    {
        const uchar *inputLine = inputImage.scanLine(y);
        uchar *outputLine = outputImage.scanLine(y);

        for (int x = radius; x < width - radius; ++x)
        {
            // Проверяем окрестность текущего пикселя
            bool is_not_background = false;

            for (int dy = -radius; dy <= radius; ++dy)
            {
                const uchar *neighborLine = inputImage.scanLine(y + dy);

                for (int dx = -radius; dx <= radius; ++dx)
                {
                    if (mask[dy + radius][dx + radius])
                    {
                        uchar neighborValue = neighborLine[x + dx];
                        // Если хотя бы один сосед - объект
                        if (neighborValue > 127)
                        {
                            is_not_background = true;
                            break;
                        }
                    }
                }
                if (is_not_background)
                    break;
            }

            outputLine[x] = is_not_background ? 255 : 0;
        }
    }

    return outputImage;
}

QImage Filters::apply_sharpening_Filter(const QImage& inputImage, double A, bool useLaplacian)
{
        if (inputImage.isNull()) {
        return QImage();
    }
    
    QImage grayImage = inputImage;
    if (inputImage.format() != QImage::Format_Grayscale8) {
        grayImage = inputImage.convertToFormat(QImage::Format_Grayscale8);
    }
    
    // Создаем выходное изображение в том же формате
    QImage outputImage(grayImage.size(), QImage::Format_Grayscale8);
    
    int width = grayImage.width();
    int height = grayImage.height();
    
    // Получаем доступ к данным изображения
    const uchar* srcData = grayImage.constBits();
    uchar* dstData = outputImage.bits();
    
    // Лапласиан маска
    int laplacianMask[3][3] = {
        {-1, -1, -1},
        {-1,  9, -1},
        {-1, -1, -1}
    };
    
    // Получаем размер строки (с учетом выравнивания)
    int srcBytesPerLine = grayImage.bytesPerLine();
    int dstBytesPerLine = outputImage.bytesPerLine();
    
    // обработка исключая краевые пиксели
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            int srcIndex = y * srcBytesPerLine + x;
            int dstIndex = y * dstBytesPerLine + x;
            
            int originalGray = static_cast<int>(srcData[srcIndex]);
            
            if (useLaplacian) {
                // Применение дискретного лапласиана
                int graySum = 0;
                
                for (int j = -1; j <= 1; ++j) {
                    for (int i = -1; i <= 1; ++i) {
                        int pixelX = x + i;
                        int pixelY = y + j;
                        int pixelIndex = pixelY * srcBytesPerLine + pixelX;
                        
                        int pixelValue = static_cast<int>(srcData[pixelIndex]);
                        int maskValue = laplacianMask[j + 1][i + 1];
                        
                        graySum += pixelValue * maskValue;
                    }
                }
                
                // Ограничиваем значения в диапазоне 0-255
                int newGray = std::min(std::max(graySum, 0), 255);
                dstData[dstIndex] = static_cast<uchar>(newGray);
            } else {
                // Нерезкое маскирование (unsharp masking)
                // Вычисляем среднее значение в окрестности 3x3
                int graySum = 0;
                
                for (int j = -1; j <= 1; ++j) {
                    for (int i = -1; i <= 1; ++i) {
                        int pixelX = x + i;
                        int pixelY = y + j;
                        int pixelIndex = pixelY * srcBytesPerLine + pixelX;
                        
                        graySum += static_cast<int>(srcData[pixelIndex]);
                    }
                }
                
                // Среднее значение (расфокусированное изображение)
                int avgGray = graySum / 9;
                
                // Применяем формулу нерезкого маскирования: f_s = A*f - f_avg
                int newGray = static_cast<int>(A * originalGray - avgGray);
                
                // Ограничиваем значения в диапазоне 0-255
                newGray = std::min(std::max(newGray, 0), 255);
                dstData[dstIndex] = static_cast<uchar>(newGray);
            }
        }
    }
    return outputImage;
}

QImage Filters::apply_median_filtr(QImage &inputImage)
{
    int maskSize = 3;
    // Проверка формата изображения
    if (inputImage.format() != QImage::Format_Grayscale8)
    {
        qWarning() << "Изображение должно быть в формате Format_Grayscale8";
        return QImage();
    }

    // Проверка размера маски
    if (maskSize % 2 == 0)
    {
        qWarning() << "Размер маски должен быть нечетным";
        return QImage();
    }

    if (maskSize < 3)
    {
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
    for (int y = radius; y < height - radius; ++y)
    {
        uchar *outputScanLine = outputImage.scanLine(y);

        for (int x = radius; x < width - radius; ++x)
        {
            // Очищаем вектор для нового окна
            windowValues.clear();

            // Собираем все значения из окна
            for (int dy = -radius; dy <= radius; ++dy)
            {
                const uchar *inputScanLine = inputImage.scanLine(y + dy);
                for (int dx = -radius; dx <= radius; ++dx)
                {
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
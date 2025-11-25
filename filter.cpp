#include "filter.h"

QImage MaskFilter::apply(QImage &inputImage) {
    // median
        int size = 9;
        QImage result = inputImage.copy();
        int width = inputImage.width();
        int height = inputImage.height();
        int radius = size / 2;
        
        for (int y = radius; y < height - radius; ++y) {
            for (int x = radius; x < width - radius; ++x) {
                
                QVector<int> reds, greens, blues;
                
                // Собираем значения из окрестности
                for (int dy = -radius; dy <= radius; ++dy) {
                    for (int dx = -radius; dx <= radius; ++dx) {
                        QColor pixel = inputImage.pixelColor(x + dx, y + dy);
                        reds.append(pixel.red());
                        greens.append(pixel.green());
                        blues.append(pixel.blue());
                    }
                }
                
                // Сортируем и берем медиану
                std::sort(reds.begin(), reds.end());
                std::sort(greens.begin(), greens.end());
                std::sort(blues.begin(), blues.end());
                
                int medianIndex = reds.size() / 2;
                int r = reds[medianIndex];
                int g = greens[medianIndex];
                int b = blues[medianIndex];
                
                result.setPixelColor(x, y, QColor(r, g, b));
            }
        }
        
        return result;


        //Sobel
    //  QImage gray = inputImage.convertToFormat(QImage::Format_Grayscale8);
    // QImage result = gray.copy();
    
    // QVector<QVector<double>> sobelX = {
    //     {-1, 0, 1},
    //     {-2, 0, 2},
    //     {-1, 0, 1}
    // };
    
    // QVector<QVector<double>> sobelY = {
    //     {-1, -2, -1},
    //     {0, 0, 0},
    //     {1, 2, 1}
    // };
    
    // int width = gray.width();
    // int height = gray.height();
    
    // for (int y = 1; y < height - 1; ++y) {
    //     for (int x = 1; x < width - 1; ++x) {
            
    //         double gx = 0.0, gy = 0.0;
            
    //         for (int ky = -1; ky <= 1; ++ky) {
    //             for (int kx = -1; kx <= 1; ++kx) {
    //                 int grayValue = qGray(gray.pixel(x + kx, y + ky));
    //                 gx += grayValue * sobelX[ky + 1][kx + 1];
    //                 gy += grayValue * sobelY[ky + 1][kx + 1];
    //             }
    //         }
            
    //         int magnitude = 255 - qBound(0, (int)sqrt(gx * gx + gy * gy), 255);
    //         result.setPixelColor(x, y, QColor(magnitude, magnitude, magnitude));
    //     }
    // }
    
    // return result;
    }
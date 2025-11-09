#include "picture_generator.h"

// 1. Шахматная доска
void PictureGenerator::generateChessboard(QImage &image)
{
    int width = image.width();
    int height = image.height();
    int cellSize = 20;
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            bool isBlack = ((x / cellSize) + (y / cellSize)) % 2 == 0;
            int color = isBlack ? 0 : 255;
            image.setPixelColor(x, y, QColor(color, color, color));
        }
    }
}

// 2. Градиент от черного к белому
void PictureGenerator::generateGradient(QImage &image)
{
    int width = image.width();
    int height = image.height();
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int intensity = (x * 255) / width;
            image.setPixelColor(x, y, QColor(intensity, intensity, intensity));
        }
    }
}

// 3. Вертикальные полосы
void PictureGenerator::generateVerticalStripes(QImage &image)
{
    int width = image.width();
    int height = image.height();
    int stripeWidth = 30;
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            bool isBlack = (x / stripeWidth) % 2 == 0;
            int color = isBlack ? 0 : 255;
            image.setPixelColor(x, y, QColor(color, color, color));
        }
    }
}

// 4. Горизонтальные полосы
void PictureGenerator::generateHorizontalStripes(QImage &image)
{
    int width = image.width();
    int height = image.height();
    int stripeHeight = 20;
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            bool isBlack = (y / stripeHeight) % 2 == 0;
            int color = isBlack ? 0 : 255;
            image.setPixelColor(x, y, QColor(color, color, color));
        }
    }
}

// 5. Круги (мишень)
void PictureGenerator::generateTarget(QImage &image)
{
    int width = image.width();
    int height = image.height();
    int centerX = width / 2;
    int centerY = height / 2;
    int maxRadius = qMin(width, height) / 2;
    
    image.fill(Qt::white);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int dx = x - centerX;
            int dy = y - centerY;
            double distance = sqrt(dx * dx + dy * dy);
            
            // Чередуем черные и белые круги
            bool isBlack = (static_cast<int>(distance) / 15) % 2 == 0;
            if (distance <= maxRadius) {
                int color = isBlack ? 0 : 255;
                image.setPixelColor(x, y, QColor(color, color, color));
            }
        }
    }
}

// 6. Синусоидальные волны
void PictureGenerator::generateSineWave(QImage &image)
{
    int width = image.width();
    int height = image.height();
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Синусоида с разными частотами по X и Y
            double valueX = sin(x * 0.05) * 0.5 + 0.5;
            double valueY = sin(y * 0.03) * 0.5 + 0.5;
            double combined = (valueX + valueY) / 2.0;
            
            int intensity = static_cast<int>(combined * 255);
            image.setPixelColor(x, y, QColor(intensity, intensity, intensity));
        }
    }
}

// 7. Диагональный градиент
void PictureGenerator::generateDiagonalGradient(QImage &image)
{
    int width = image.width();
    int height = image.height();
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int intensity = ((x + y) * 255) / (width + height);
            image.setPixelColor(x, y, QColor(intensity, intensity, intensity));
        }
    }
}

// 8. Пиксельный арт (квадратики)
void PictureGenerator::generatePixelArt(QImage &image)
{
    int width = image.width();
    int height = image.height();
    int pixelSize = 10;
    
    for (int blockY = 0; blockY < height / pixelSize; ++blockY) {
        for (int blockX = 0; blockX < width / pixelSize; ++blockX) {
            // Случайный цвет для каждого блока
            int color = (blockX + blockY) % 2 == 0 ? 0 : 255;
            
            for (int y = blockY * pixelSize; y < (blockY + 1) * pixelSize && y < height; ++y) {
                for (int x = blockX * pixelSize; x < (blockX + 1) * pixelSize && x < width; ++x) {
                    image.setPixelColor(x, y, QColor(color, color, color));
                }
            }
        }
    }
}

// 9. Радиальный градиент
void PictureGenerator::generateRadialGradient(QImage &image)
{
    int width = image.width();
    int height = image.height();
    int centerX = width / 2;
    int centerY = height / 2;
    double maxDistance = sqrt(centerX * centerX + centerY * centerY);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int dx = x - centerX;
            int dy = y - centerY;
            double distance = sqrt(dx * dx + dy * dy);
            int intensity = static_cast<int>((distance / maxDistance) * 255);
            image.setPixelColor(x, y, QColor(intensity, intensity, intensity));
        }
    }
}

// 10. Прямоугольник
void PictureGenerator::generateGeometricPattern(QImage &image)
{
    int width = image.width();
    int height = image.height();
    
    image.fill(Qt::white);
    
    for (int y = 50; y < 150; ++y) {
        for (int x = 50; x < 250; ++x) {
            if ((x >= 50 && x <= 250 && y >= 50 && y <= 150) || 
                (x >= 100 && x <= 200 && y >= 25 && y <= 175)) {
                image.setPixelColor(x, y, Qt::black);
            }
        }
    }
}
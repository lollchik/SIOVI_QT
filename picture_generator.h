#pragma once

#include <QImage>
#include <QtMath>

class PictureGenerator
{
private:
public:
    PictureGenerator(){};
    ~PictureGenerator(){};
    
    void generateChessboard(QImage &image);
    void generateGradient(QImage &image);
    void generateVerticalStripes(QImage &image);
    void generateHorizontalStripes(QImage &image);
    void generateTarget(QImage &image);
    void generateSineWave(QImage &image);
    void generateDiagonalGradient(QImage &image);
    void generatePixelArt(QImage &image);
    void generateRadialGradient(QImage &image);
    void generateGeometricPattern(QImage &image);
};


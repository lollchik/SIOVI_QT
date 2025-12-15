#pragma once
#include <QImage>
#include <QVector>
#include <QColor>
#include <cmath>
#include <QDebug>

enum StructuringElement
{
    SQUARE, // Квадратная матрица
    CROSS   // Крест
};

class Filters {
private:
    QVector<QVector<double>> kernel= {
        {0, -1, 0},
        {-1, 9, -1},
        {0, -1, 0}
    };
    int kernelSize;
    double divisor = 1.0;
    double offset= 0.0;

public:
    Filters(){};
    // Filters(QVector<QVector<double>> &mask , double div = 1.0, double off = 0.0) 
    //     : kernel(mask), divisor(div), offset(off) {
    //     kernelSize = kernel.size();
    // }

    QImage apply_mask_smoothing(QImage &inputImage);
    QImage apply_uniform_area_smoothing(QImage &inputImage);
    QImage apply_median_filtr(QImage &inputImage);
    QImage dilation(const QImage &inputImage, int maskSize = 3,
                         StructuringElement elementType = SQUARE);
    QImage erosion(const QImage &inputImage, int maskSize = 3,
                      StructuringElement elementType = SQUARE);
    QImage apply_sharpening_Filter(const QImage& inputImage, double A = 1.5, bool useLaplacian = false);
    
};
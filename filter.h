#pragma once
#include <QImage>
#include <QVector>
#include <QColor>
#include <cmath>

class MaskFilter {
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
    MaskFilter(){};
    // MaskFilter(QVector<QVector<double>> &mask , double div = 1.0, double off = 0.0) 
    //     : kernel(mask), divisor(div), offset(off) {
    //     kernelSize = kernel.size();
    // }

    QImage apply(QImage &inputImage);
};
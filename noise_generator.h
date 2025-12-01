#pragma once
#include <QImage>
#include <QRandomGenerator>

class NoiseGenerator
{
private:
public:
    QImage generateAdditiveNoise(QImage& inputImage, double noiseLevel, quint32 seed = 0);
    void generateInertialNoise(QImage &image);
    // NoiseGenerator(/* args */);
    // ~NoiseGenerator();
};
#pragma once
#include <QImage>
#include <QRandomGenerator>

class NoiseGenerator
{
private:
public:
    void generateImpulseNoise(QImage &image);
    void generateInertialNoise(QImage &image);
    NoiseGenerator(/* args */);
    ~NoiseGenerator();
};
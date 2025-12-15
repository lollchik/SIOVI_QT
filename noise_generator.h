#pragma once
#include <QImage>
#include <QRandomGenerator>
#include "enums.h"

class NoiseGenerator
{
public:
    enum class ImpulseNoiseIntensity {
        Point,
        Line
    };
    
private:
    
public:
    QImage generateAdditiveNoise(QImage& inputImage, double noiseLevel, quint32 seed = 0);
    QImage generateImpulseNoise(QImage& inputImage, double noiseLevel, inmpulse_noise_type type, impulse_noise_form form, quint32 seed = 0);
    // NoiseGenerator(/* args */);
    // ~NoiseGenerator();
};
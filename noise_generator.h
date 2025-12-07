#pragma once
#include <QImage>
#include <QRandomGenerator>

class NoiseGenerator
{
public:
    enum class ImpulseNoiseType {
        Salt,
        Pepper,
        SaltAndPepper
    };
    
    enum class ImpulseNoiseIntensity {
        Point,
        Line
    };
    
private:
    
public:
    QImage generateAdditiveNoise(QImage& inputImage, double noiseLevel, quint32 seed = 0);
    QImage generateImpulseNoise(QImage& inputImage, double noiseLevel, ImpulseNoiseType type, ImpulseNoiseIntensity intensity, quint32 seed = 0);
    // NoiseGenerator(/* args */);
    // ~NoiseGenerator();
};
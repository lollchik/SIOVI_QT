#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <cstring>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

class FFT2D {
public:
    static const int N = 512;
    
    struct Image {
        float data[N][N];
        
        Image() {
            memset(data, 0, sizeof(data));
        }
        
        bool loadFromFile(const std::string& filename) {
            int width, height, channels;
            unsigned char* imgData = stbi_load(filename.c_str(), &width, &height, &channels, 0);
            
            if (!imgData) {
                std::cerr << "Ошибка загрузки: " << filename << std::endl;
                return false;
            }
            
            std::cout << "Загружено: " << width << "x" << height << ", каналов: " << channels << std::endl;
            
            // Конвертируем в оттенки серого и масштабируем
            std::vector<float> grayData(width * height);
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int idx = y * width + x;
                    unsigned char r = imgData[idx * channels];
                    unsigned char g = channels > 1 ? imgData[idx * channels + 1] : r;
                    unsigned char b = channels > 2 ? imgData[idx * channels + 2] : r;
                    
                    // Формула для преобразования в оттенки серого
                    grayData[idx] = (0.299f * r + 0.587f * g + 0.114f * b) / 255.0f;
                }
            }
            
            // Масштабируем до нужного размера
            if (width != N || height != N) {
                std::cout << "Масштабирую до " << N << "x" << N << std::endl;
                
                // Билинейная интерполяция
                for (int y = 0; y < N; y++) {
                    float fy = (float)y / N * (height - 1);
                    int y0 = (int)fy;
                    int y1 = std::min(y0 + 1, height - 1);
                    float ay = fy - y0;
                    
                    for (int x = 0; x < N; x++) {
                        float fx = (float)x / N * (width - 1);
                        int x0 = (int)fx;
                        int x1 = std::min(x0 + 1, width - 1);
                        float ax = fx - x0;
                        
                        float v00 = grayData[y0 * width + x0];
                        float v10 = grayData[y1 * width + x0];
                        float v01 = grayData[y0 * width + x1];
                        float v11 = grayData[y1 * width + x1];
                        
                        data[y][x] = (1 - ax) * (1 - ay) * v00 +
                                     ax * (1 - ay) * v01 +
                                     (1 - ax) * ay * v10 +
                                     ax * ay * v11;
                    }
                }
            } else {
                // Прямая загрузка если размер совпадает
                for (int y = 0; y < N; y++) {
                    for (int x = 0; x < N; x++) {
                        data[y][x] = grayData[y * width + x];
                    }
                }
            }
            
            stbi_image_free(imgData);
            return true;
        }
        
        bool saveToPNG(const std::string& filename) const {
            std::vector<unsigned char> pixelData(N * N);
            
            // Находим диапазон значений для нормализации
            float minVal = data[0][0];
            float maxVal = data[0][0];
            
            for (int y = 0; y < N; y++) {
                for (int x = 0; x < N; x++) {
                    minVal = std::min(minVal, data[y][x]);
                    maxVal = std::max(maxVal, data[y][x]);
                }
            }
            
            std::cout << "  Диапазон: " << minVal << " - " << maxVal << std::endl;
            
            // Нормализация к 0-255
            if (maxVal - minVal < 0.0001f) {
                // Если все значения одинаковы, сохраняем как есть
                for (int i = 0; i < N * N; i++) {
                    pixelData[i] = 128;
                }
            } else {
                for (int y = 0; y < N; y++) {
                    for (int x = 0; x < N; x++) {
                        // Линейная нормализация к [0, 1], затем к [0, 255]
                        float normalized = (data[y][x] - minVal) / (maxVal - minVal);
                        normalized = std::max(0.0f, std::min(1.0f, normalized));
                        pixelData[y * N + x] = static_cast<unsigned char>(normalized * 255);
                    }
                }
            }
            
            return stbi_write_png(filename.c_str(), N, N, 1, pixelData.data(), N) != 0;
        }
        
        void createTestImage() {
            std::cout << "Создаю тестовое изображение..." << std::endl;
            
            // Четкое черно-белое изображение
            for (int y = 0; y < N; y++) {
                for (int x = 0; x < N; x++) {
                    // Белый фон
                    float value = 1.0f;
                    
                    // Черный квадрат в центре
                    if (x > 150 && x < 350 && y > 150 && y < 350) {
                        value = 0.0f;
                    }
                    
                    // Белый круг
                    float dx = x - N/2;
                    float dy = y - N/3;
                    if (dx*dx + dy*dy < 60*60) {
                        value = 1.0f;
                    }
                    
                    // Черная горизонтальная линия
                    if (abs(y - 400) < 2) {
                        value = 0.0f;
                    }
                    
                    // Черная вертикальная линия
                    if (abs(x - 100) < 2) {
                        value = 0.0f;
                    }
                    
                    data[y][x] = value;
                }
            }
        }
        
        void printStats() const {
            float minVal = data[0][0];
            float maxVal = data[0][0];
            double sum = 0;
            
            for (int y = 0; y < N; y++) {
                for (int x = 0; x < N; x++) {
                    minVal = std::min(minVal, data[y][x]);
                    maxVal = std::max(maxVal, data[y][x]);
                    sum += data[y][x];
                }
            }
            
            std::cout << "  Диапазон: " << minVal << " - " << maxVal 
                      << ", Среднее: " << sum/(N*N) << std::endl;
        }
    };
    
    // Основная функция фильтрации
    static Image applyFilter(const Image& input, int filterType, int param) {
        std::cout << "Применение " << (filterType == 0 ? "высокочастотного" : "низкочастотного") 
                  << " фильтра с параметром " << param << std::endl;
        
        // Шаг 1: Умножение на (-1)^(x+y)
        Image multiplied;
        for (int y = 0; y < N; y++) {
            for (int x = 0; x < N; x++) {
                float sign = ((x + y) % 2 == 0) ? 1.0f : -1.0f;
                multiplied.data[y][x] = input.data[y][x] * sign;
            }
        }
        
        // Шаг 2: Прямое БПФ
        std::vector<std::complex<float>> spectrum(N * N);
        forwardDFT(multiplied, spectrum);
        
        // Шаг 3: Применение фильтра
        if (filterType == 0) {
            // Высокочастотная фильтрация: обнуляем центральное окно
            int halfSize = param / 2;
            int centerX = N / 2;
            int centerY = N / 2;
            
            std::cout << "Обнуление центра " << param << "x" << param << std::endl;
            
            for (int dy = -halfSize; dy <= halfSize; dy++) {
                for (int dx = -halfSize; dx <= halfSize; dx++) {
                    int x = (centerX + dx + N) % N;
                    int y = (centerY + dy + N) % N;
                    spectrum[y * N + x] = std::complex<float>(0.0f, 0.0f);
                }
            }
        } else {
            // Низкочастотная фильтрация: обнуляем периметр
            std::cout << "Обнуление периметра шириной " << param << " пикселей" << std::endl;
            
            for (int y = 0; y < N; y++) {
                for (int x = 0; x < N; x++) {
                    if (x < param || x >= N - param || y < param || y >= N - param) {
                        spectrum[y * N + x] = std::complex<float>(0.0f, 0.0f);
                    }
                }
            }
        }
        
        // Шаг 4: Обратное БПФ
        Image processed;
        inverseDFT(spectrum, processed);
        
        // Шаг 5 и 6: Умножение на (-1)^(x+y) и нормализация
        Image result;
        
        // Сначала умножаем на (-1)^(x+y)
        for (int y = 0; y < N; y++) {
            for (int x = 0; x < N; x++) {
                float sign = ((x + y) % 2 == 0) ? 1.0f : -1.0f;
                float val = processed.data[y][x] * sign;
                result.data[y][x] = val;
            }
        }
        
        // Нормализация для улучшения контраста
        normalizeResult(result);
        
        return result;
    }
    
private:
    // Рекурсивный алгоритм БПФ
    static void fft(std::vector<std::complex<float>>& a, bool invert) {
        int n = (int)a.size();
        if (n == 1) return;

        std::vector<std::complex<float>> a0(n/2), a1(n/2);
        for (int i = 0, j = 0; i < n; i += 2, ++j) {
            a0[j] = a[i];
            a1[j] = a[i+1];
        }
        
        fft(a0, invert);
        fft(a1, invert);

        float ang = 2 * M_PI / n * (invert ? 1.0f : -1.0f);
        std::complex<float> w(1.0f, 0.0f);
        std::complex<float> wn(std::cos(ang), std::sin(ang));
        
        for (int i = 0; i < n/2; ++i) {
            std::complex<float> temp = w * a1[i];
            a[i] = a0[i] + temp;
            a[i + n/2] = a0[i] - temp;
            w = w * wn;
            
            // Для обратного преобразования делим на 2
            if (invert) {
                a[i] = a[i] * 0.5f;
                a[i + n/2] = a[i + n/2] * 0.5f;
            }
        }
    }
    
    // Прямое 2D БПФ
    static void forwardDFT(const Image& img, std::vector<std::complex<float>>& spectrum) {
        // БПФ по строкам
        std::vector<std::complex<float>> row(N);
        
        for (int y = 0; y < N; y++) {
            for (int x = 0; x < N; x++) {
                row[x] = std::complex<float>(img.data[y][x], 0.0f);
            }
            fft(row, false);
            
            for (int x = 0; x < N; x++) {
                spectrum[y * N + x] = row[x];
            }
        }
        
        // БПФ по столбцам
        std::vector<std::complex<float>> col(N);
        
        for (int x = 0; x < N; x++) {
            for (int y = 0; y < N; y++) {
                col[y] = spectrum[y * N + x];
            }
            fft(col, false);
            
            for (int y = 0; y < N; y++) {
                spectrum[y * N + x] = col[y];
            }
        }
    }
    
    // Обратное 2D БПФ
    static void inverseDFT(const std::vector<std::complex<float>>& spectrum, Image& img) {
        // Обратное БПФ по строкам
        std::vector<std::complex<float>> row(N);
        
        for (int y = 0; y < N; y++) {
            for (int x = 0; x < N; x++) {
                row[x] = spectrum[y * N + x];
            }
            fft(row, true);
            
            for (int x = 0; x < N; x++) {
                img.data[y][x] = row[x].real();
            }
        }
        
        // Обратное БПФ по столбцам
        std::vector<std::complex<float>> col(N);
        
        for (int x = 0; x < N; x++) {
            for (int y = 0; y < N; y++) {
                col[y] = std::complex<float>(img.data[y][x], 0.0f);
            }
            fft(col, true);
            
            for (int y = 0; y < N; y++) {
                img.data[y][x] = col[y].real();
            }
        }
    }
    
    // Нормализация результата
    static void normalizeResult(Image& img) {
        float minVal = img.data[0][0];
        float maxVal = img.data[0][0];
        
        for (int y = 0; y < N; y++) {
            for (int x = 0; x < N; x++) {
                minVal = std::min(minVal, img.data[y][x]);
                maxVal = std::max(maxVal, img.data[y][x]);
            }
        }
        
        float range = maxVal - minVal;
        if (range < 0.0001f) {
            // Все значения одинаковы
            float avg = (minVal + maxVal) / 2.0f;
            for (int y = 0; y < N; y++) {
                for (int x = 0; x < N; x++) {
                    img.data[y][x] = avg;
                }
            }
        } else {
            // Нормализуем к [0, 1]
            for (int y = 0; y < N; y++) {
                for (int x = 0; x < N; x++) {
                    img.data[y][x] = (img.data[y][x] - minVal) / range;
                }
            }
        }
    }
};

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   ФИЛЬТРАЦИЯ ИЗОБРАЖЕНИЙ БПФ" << std::endl;
    std::cout << "   Размер: 512x512 пикселей" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    FFT2D::Image inputImage;
    
    // Выбор режима
    std::cout << "1. Загрузить изображение из файла" << std::endl;
    std::cout << "2. Использовать тестовое изображение" << std::endl;
    std::cout << "Выберите (1 или 2): ";
    
    int choice;
    std::cin >> choice;
    
    if (choice == 1) {
        std::string filename;
        std::cout << "Введите имя файла (PNG, JPG, BMP): ";
        std::cin >> filename;
        
        if (!inputImage.loadFromFile(filename)) {
            std::cout << "Не удалось загрузить файл. Создаю тестовое изображение." << std::endl;
            inputImage.createTestImage();
        }
    } else {
        inputImage.createTestImage();
    }
    
    // Сохраняем исходное изображение
    inputImage.saveToPNG("original.png");
    std::cout << "\nИсходное изображение сохранено в original.png" << std::endl;
    inputImage.printStats();
    
    // Основной цикл фильтрации
    while (true) {
        std::cout << "\n=== ВЫБОР ФИЛЬТРА ===" << std::endl;
        std::cout << "1. ВЫСОКОЧАСТОТНЫЙ фильтр (обнулить центр спектра)" << std::endl;
        std::cout << "   Удаляет низкие частоты, подчеркивает контуры" << std::endl;
        std::cout << "   Рекомендуемый параметр: 32-128" << std::endl;
        
        std::cout << "\n2. НИЗКОЧАСТОТНЫЙ фильтр (обнулить края спектра)" << std::endl;
        std::cout << "   Удаляет высокие частоты, сглаживает изображение" << std::endl;
        std::cout << "   Рекомендуемый параметр: 8-32" << std::endl;
        
        std::cout << "\n3. Выход" << std::endl;
        std::cout << "Выберите: ";
        
        int filterChoice;
        std::cin >> filterChoice;
        
        if (filterChoice == 3) break;
        if (filterChoice < 1 || filterChoice > 2) continue;
        
        int filterType = filterChoice - 1;
        
        // Ввод параметра
        int param;
        if (filterType == 0) {
            std::cout << "Введите размер окна для обнуления (например, 64): ";
        } else {
            std::cout << "Введите ширину полосы для обнуления (например, 16): ";
        }
        std::cin >> param;
        
        // Корректировка параметра
        if (filterType == 0) {
            param = std::max(2, std::min(param, FFT2D::N - 2));
            if (param % 2 != 0) param++;
        } else {
            param = std::max(1, std::min(param, FFT2D::N / 2));
        }
        
        // Применение фильтра
        FFT2D::Image result = FFT2D::applyFilter(inputImage, filterType, param);
        
        // Сохранение результата
        std::string outputFile;
        if (filterType == 0) {
            outputFile = "highpass_" + std::to_string(param) + ".png";
        } else {
            outputFile = "lowpass_" + std::to_string(param) + ".png";
        }
        
        if (result.saveToPNG(outputFile)) {
            std::cout << "✓ Результат сохранен в " << outputFile << std::endl;
            std::cout << "Статистика результата:" << std::endl;
            result.printStats();
            
            // Описание эффекта
            std::cout << "\nЭффект фильтрации: ";
            if (filterType == 0) {
                std::cout << "выделены края и контуры" << std::endl;
            } else {
                std::cout << "изображение сглажено" << std::endl;
            }
        } else {
            std::cout << "Ошибка сохранения результата!" << std::endl;
        }
        
        std::cout << "\nПродолжить? (y/n): ";
        char continueChoice;
        std::cin >> continueChoice;
        if (continueChoice != 'y' && continueChoice != 'Y') break;
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Созданные файлы:" << std::endl;
    std::cout << "- original.png - исходное изображение" << std::endl;
    std::cout << "- highpass_XX.png - высокочастотная фильтрация" << std::endl;
    std::cout << "- lowpass_XX.png - низкочастотная фильтрация" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    return 0;
}
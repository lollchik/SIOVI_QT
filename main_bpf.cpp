#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <png.h>
//g++ ../main_bpf.cpp -o bpf_1 -lpng
using namespace std;

// Тип для комплексных чисел
using Complex = complex<double>;
using Matrix = vector<vector<Complex>>;

const double PI = 3.14159265358979323846;

// ================================
// 1. PNG ЧТЕНИЕ/ЗАПИСЬ
// ================================

/**
 * Чтение PNG изображения в матрицу комплексных чисел
 * Изображение конвертируется в градации серого
 */
Matrix readPNGToMatrix(const string& filename) {
    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        throw runtime_error("Не удалось открыть файл: " + filename);
    }
    
    // Проверка сигнатуры PNG
    png_byte header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        fclose(fp);
        throw runtime_error("Файл не является PNG: " + filename);
    }
    
    // Инициализация структур libpng
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fclose(fp);
        throw runtime_error("Ошибка создания png_struct");
    }
    
    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(fp);
        throw runtime_error("Ошибка создания png_info");
    }
    
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        throw runtime_error("Ошибка во время чтения PNG");
    }
    
    png_init_io(png, fp);
    png_set_sig_bytes(png, 8);
    png_read_info(png, info);
    
    int width = png_get_image_width(png, info);
    int height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);
    
    // Конвертируем в 8-bit grayscale если нужно
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png);
    }
    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
        png_set_rgb_to_gray_fixed(png, 1, -1, -1);
    }
    if (bit_depth == 16) {
        png_set_strip_16(png);
    }
    if (color_type & PNG_COLOR_MASK_ALPHA) {
        png_set_strip_alpha(png);
    }
    
    png_read_update_info(png, info);
    
    // Чтение строк изображения
    png_bytep* row_pointers = new png_bytep[height];
    for (int y = 0; y < height; y++) {
        row_pointers[y] = new png_byte[png_get_rowbytes(png, info)];
    }
    
    png_read_image(png, row_pointers);
    fclose(fp);
    
    // Создаем матрицу (обрезаем до 512x512 если нужно)
    int N = min(width, height);
    N = min(N, 512); // Ограничиваем размер для БПФ
    
    Matrix image(N, vector<Complex>(N));
    
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < N; x++) {
            double pixel = row_pointers[y][x] / 255.0; // Нормализуем к [0, 1]
            image[x][y] = Complex(pixel, 0);
        }
    }
    
    // Освобождаем память
    for (int y = 0; y < height; y++) {
        delete[] row_pointers[y];
    }
    delete[] row_pointers;
    png_destroy_read_struct(&png, &info, NULL);
    
    cout << "Загружено изображение " << filename 
         << " размером " << N << "x" << N << endl;
    
    return image;
}

/**
 * Сохранение матрицы в PNG файл
 */
void saveMatrixToPNG(const Matrix& matrix, const string& filename) {
    int N = matrix.size();
    
    FILE* fp = fopen(filename.c_str(), "wb");
    if (!fp) {
        throw runtime_error("Не удалось создать файл: " + filename);
    }
    
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fclose(fp);
        throw runtime_error("Ошибка создания png_struct для записи");
    }
    
    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, NULL);
        fclose(fp);
        throw runtime_error("Ошибка создания png_info для записи");
    }
    
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        throw runtime_error("Ошибка во время записи PNG");
    }
    
    png_init_io(png, fp);
    
    // Настройка заголовка PNG
    png_set_IHDR(
        png, info,
        N, N, 8, PNG_COLOR_TYPE_GRAY,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    
    png_write_info(png, info);
    
    // Подготовка данных для записи
    png_bytep* row_pointers = new png_bytep[N];
    for (int y = 0; y < N; y++) {
        row_pointers[y] = new png_byte[N];
        for (int x = 0; x < N; x++) {
            // Нормализуем и обрезаем значения
            double value = real(matrix[x][y]);
            value = max(0.0, min(1.0, value)); // Обрезаем к [0, 1]
            row_pointers[y][x] = static_cast<png_byte>(value * 255);
        }
    }
    
    png_write_image(png, row_pointers);
    png_write_end(png, NULL);
    
    // Освобождаем память
    for (int y = 0; y < N; y++) {
        delete[] row_pointers[y];
    }
    delete[] row_pointers;
    
    png_destroy_write_struct(&png, &info);
    fclose(fp);
    
    cout << "Сохранено изображение: " << filename << endl;
}

// ================================
// 2. ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ (остаются без изменений)
// ================================

void applyShiftMatrix(Matrix& image) {
    int N = image.size();
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            if ((x + y) % 2 == 1) {
                image[x][y] *= -1.0;
            }
        }
    }
}

void printImageStats(const Matrix& img, const string& name) {
    double minVal = 1e9, maxVal = -1e9, sum = 0;
    int N = img.size();
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double val = abs(img[i][j]);
            minVal = min(minVal, val);
            maxVal = max(maxVal, val);
            sum += val;
        }
    }
    
    cout << name << ": min=" << minVal << ", max=" << maxVal 
         << ", avg=" << sum/(N*N) << endl;
}

// ================================
// 3. БПФ ФУНКЦИИ (остаются без изменений)
// ================================

vector<Complex> fft(const vector<Complex>& input) {
    int N = input.size();
    
    if (N == 1) {
        return input;
    }
    
    // if ((N & (N - 1)) != 0) {
    //     throw runtime_error("Размер должен быть степенью двойки");
    // }
    
    vector<Complex> even(N/2), odd(N/2);
    for (int i = 0; i < N/2; i++) {
        even[i] = input[2*i];
        odd[i] = input[2*i + 1];
    }
    
    vector<Complex> evenTransformed = fft(even);
    vector<Complex> oddTransformed = fft(odd);
    
    vector<Complex> output(N);
    for (int k = 0; k < N/2; k++) {
        Complex t = polar(1.0, -2.0 * PI * k / N) * oddTransformed[k];
        output[k] = evenTransformed[k] + t;
        output[k + N/2] = evenTransformed[k] - t;
    }
    
    return output;
}

vector<Complex> ifft(const vector<Complex>& input) {
    int N = input.size();
    
    if (N == 1) {
        return input;
    }
    
    vector<Complex> even(N/2), odd(N/2);
    for (int i = 0; i < N/2; i++) {
        even[i] = input[2*i];
        odd[i] = input[2*i + 1];
    }
    
    vector<Complex> evenTransformed = ifft(even);
    vector<Complex> oddTransformed = ifft(odd);
    
    vector<Complex> output(N);
    for (int k = 0; k < N/2; k++) {
        Complex t = polar(1.0, 2.0 * PI * k / N) * oddTransformed[k];
        output[k] = evenTransformed[k] + t;
        output[k + N/2] = evenTransformed[k] - t;
    }
    
    for (int i = 0; i < N; i++) {
        output[i] /= 2.0;
    }
    
    return output;
}

Matrix fft2d(const Matrix& input) {
    int N = input.size();
    Matrix output(N, vector<Complex>(N));
    
    for (int i = 0; i < N; i++) {
        vector<Complex> row(N);
        for (int j = 0; j < N; j++) {
            row[j] = input[i][j];
        }
        vector<Complex> rowTransformed = fft(row);
        for (int j = 0; j < N; j++) {
            output[i][j] = rowTransformed[j];
        }
    }
    
    for (int j = 0; j < N; j++) {
        vector<Complex> col(N);
        for (int i = 0; i < N; i++) {
            col[i] = output[i][j];
        }
        vector<Complex> colTransformed = fft(col);
        for (int i = 0; i < N; i++) {
            output[i][j] = colTransformed[i];
        }
    }
    
    return output;
}

Matrix ifft2d(const Matrix& input) {
    int N = input.size();
    Matrix output(N, vector<Complex>(N));
    
    for (int i = 0; i < N; i++) {
        vector<Complex> row(N);
        for (int j = 0; j < N; j++) {
            row[j] = input[i][j];
        }
        vector<Complex> rowTransformed = ifft(row);
        for (int j = 0; j < N; j++) {
            output[i][j] = rowTransformed[j];
        }
    }
    
    for (int j = 0; j < N; j++) {
        vector<Complex> col(N);
        for (int i = 0; i < N; i++) {
            col[i] = output[i][j];
        }
        vector<Complex> colTransformed = ifft(col);
        for (int i = 0; i < N; i++) {
            output[i][j] = colTransformed[i];
        }
    }
    
    return output;
}

// ================================
// 4. ФИЛЬТРЫ В ЧАСТОТНОЙ ОБЛАСТИ
// ================================

void applyHighPassFilter(Matrix& spectrum, int windowSize) {
    int N = spectrum.size();
    int center = N / 2;
    int halfWindow = windowSize / 2;
    
    for (int i = center - halfWindow; i < center + halfWindow; i++) {
        for (int j = center - halfWindow; j < center + halfWindow; j++) {
            if (i >= 0 && i < N && j >= 0 && j < N) {
                spectrum[i][j] = 0.0;
            }
        }
    }
}

void applyLowPassFilter(Matrix& spectrum, int borderWidth) {
    int N = spectrum.size();
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i < borderWidth || i >= N - borderWidth ||
                j < borderWidth || j >= N - borderWidth) {
                spectrum[i][j] = 0.0;
            }
        }
    }
}

// ================================
// 5. ОСНОВНАЯ ПРОЦЕДУРА ФИЛЬТРАЦИИ
// ================================

Matrix filterImage(const Matrix& input, int filterType, int filterParam) {
    int N = input.size();
    Matrix result = input;
    
    cout << "\n=== Начало обработки изображения ===" << endl;
    printImageStats(input, "Исходное изображение");
    
    // Шаг 1: Умножение на (-1)^(x+y)
    cout << "1. Умножение на (-1)^(x+y)..." << endl;
    applyShiftMatrix(result);
    
    // Шаг 2: Прямое 2D БПФ
    cout << "2. Вычисление прямого 2D БПФ..." << endl;
    Matrix spectrum = fft2d(result);
    printImageStats(spectrum, "Спектр (после БПФ)");
    
    // Шаг 3: Применение фильтра
    cout << "3. Применение фильтра..." << endl;
    if (filterType == 0) {
        cout << "   Высокочастотный фильтр, размер окна: " << filterParam << endl;
        applyHighPassFilter(spectrum, filterParam);
    } else {
        cout << "   Низкочастотный фильтр, ширина полосы: " << filterParam << endl;
        applyLowPassFilter(spectrum, filterParam);
    }
    printImageStats(spectrum, "Спектр (после фильтра)");
    
    // Шаг 4: Обратное 2D БПФ
    cout << "4. Вычисление обратного 2D БПФ..." << endl;
    result = ifft2d(spectrum);
    
    // Шаг 5: Выделение вещественной части
    cout << "5. Выделение вещественной части..." << endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            result[i][j] = Complex(real(result[i][j]), 0);
        }
    }
    
    // Шаг 6: Умножение на (-1)^(x+y) (обратное преобразование)
    cout << "6. Умножение на (-1)^(x+y)..." << endl;
    applyShiftMatrix(result);
    
    printImageStats(result, "Результат");
    cout << "=== Обработка завершена ===\n" << endl;
    
    return result;
}

// ================================
// 6. ГЛАВНАЯ ФУНКЦИЯ
// ================================

int main(int argc, char* argv[]) {
    try {
        // Параметры по умолчанию
        string inputFilename = "../test_cases/morphological_test.png";
        string outputHighPass = "output_highpass.png";
        string outputLowPass = "output_lowpass.png";
        
        // Можно изменить параметры через аргументы командной строки
        if (argc > 1) inputFilename = argv[1];
        if (argc > 2) outputHighPass = argv[2];
        if (argc > 3) outputLowPass = argv[3];
        
        cout << "=== 2D БПФ обработка изображений ===" << endl;
        cout << "Входной файл: " << inputFilename << endl;
        cout << "Выходные файлы: " << outputHighPass << ", " << outputLowPass << endl;
        
        // Загрузка изображения
        cout << "\nЗагрузка изображения..." << endl;
        Matrix image = readPNGToMatrix(inputFilename);
        
        // Проверка размера (должен быть 512x512)
        if (image.size() != 512) {
            cout << "Предупреждение: изображение будет обрезано/обработано как 512x512" << endl;
        }
        
        // Пример 1: Высокочастотная фильтрация (обостряет края)
        cout << "\n======================================" << endl;
        cout << "ПРИМЕР 1: ВЫСОКОЧАСТОТНАЯ ФИЛЬТРАЦИЯ" << endl;
        cout << "======================================" << endl;
        int windowSize = 100; // Размер окна для обнуления в центре
        
        Matrix highPassResult = filterImage(image, 0, windowSize);
        
        // Сохранение результата
        cout << "Сохранение высокочастотного фильтра..." << endl;
        saveMatrixToPNG(highPassResult, outputHighPass);
        
        // Пример 2: Низкочастотная фильтрация (размытие)
        cout << "\n=====================================" << endl;
        cout << "ПРИМЕР 2: НИЗКОЧАСТОТНАЯ ФИЛЬТРАЦИЯ" << endl;
        cout << "=====================================" << endl;
        int borderWidth = 50; // Ширина полосы по краям
        
        Matrix lowPassResult = filterImage(image, 1, borderWidth);
        
        // Сохранение результата
        cout << "Сохранение низкочастотного фильтра..." << endl;
        saveMatrixToPNG(lowPassResult, outputLowPass);
        
        // Информация о результате
        cout << "\n=== РЕЗУЛЬТАТЫ ===" << endl;
        cout << "1. Высокочастотная фильтрация сохранена в: " << outputHighPass << endl;
        cout << "   - Удаляет низкие частоты (центр спектра)" << endl;
        cout << "   - Подчеркивает края и детали" << endl;
        cout << "   - Параметр: windowSize = " << windowSize << endl;
        
        cout << "\n2. Низкочастотная фильтрация сохранена в: " << outputLowPass << endl;
        cout << "   - Удаляет высокие частоты (края спектра)" << endl;
        cout << "   - Размывает изображение, убирает шум" << endl;
        cout << "   - Параметр: borderWidth = " << borderWidth << endl;
        
        cout << "\n=== ТЕХНИЧЕСКАЯ ИНФОРМАЦИЯ ===" << endl;
        cout << "Размер обработки: 512x512 пикселей" << endl;
        cout << "Использован алгоритм БПФ Кули-Тьюки" << endl;
        cout << "Все вычисления с двойной точностью" << endl;
        
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        cerr << "\nИспользование: " << endl;
        cerr << "  " << argv[0] << " [input.png] [output_highpass.png] [output_lowpass.png]" << endl;
        cerr << "\nПример: " << endl;
        cerr << "  " << argv[0] << " image.png result_high.png result_low.png" << endl;
        return 1;
    }
    
    return 0;
}
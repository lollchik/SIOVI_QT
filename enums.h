#pragma once
#include <map>

enum noise_type{
    additive, 
    impulse
};

// 0.15, 0.25, 0.5, 1.0
enum noise_level{
    low,
    mediume,
    half,
    full
};

enum inmpulse_noise_type{
    salt,
    pepper,
    salt_and_pepper
};

enum impulse_noise_form{
    point,
    line
};

enum filtr_type{
    mask_smoothing,
    uniform_area_smoothing, 
    median_filtr, 
    sharpening_Filter, 
    morph_dilation, 
    morph_erosion
};
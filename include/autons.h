#ifndef _AUTONS_H_
#define _AUTONS_H_
#include "liblvgl/lvgl.h"

struct Auton {
    const char* name;
    const char* description;
    void (*run)();
    // nullptr shows steamhappy, keep images 110x110 or smaller
    const lv_image_dsc_t* image = nullptr;
};

extern const Auton autons[];
extern const int autonCount;

#endif

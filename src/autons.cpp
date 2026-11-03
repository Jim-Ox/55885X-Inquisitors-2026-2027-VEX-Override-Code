#include "liblvgl/widgets/image/lv_image.h"
#include "main.h"
#include "robotConfigs.h"
#include "lift.h"
#include "selector.h"
#include "autons.h"

LV_IMAGE_DECLARE(steamhappy);

// placeholder autons, nothing written yet
void leftAuton() {}

void rightAuton() {}

void skillsAuton() {}

const Auton autons[] = {
    {"Left", "placeholder, starts on the left side", leftAuton},
    {"Right", "placeholder, starts on the right side", rightAuton},
    {"Skills", "60 second skills run", skillsAuton},
    {"None", "hi hi hi hi hi", nullptr, &steamhappy},
};

const int autonCount = sizeof(autons) / sizeof(autons[0]);
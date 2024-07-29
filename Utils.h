#ifndef UTILS_H
#define UTILS_H

#include <libdragon.h>
#include "PVector.h"

// Constants
const float TWO_PI = 2 * M_PI;
const float DEADZONE = 0.5f; // Define the deadzone threshold
const color_t WHITE = (color_t){255, 255, 255, 255};
const color_t BLACK = (color_t){0, 0, 0, 255};
const color_t RED = (color_t){255, 0, 0, 255};
const color_t DARK_RED = (color_t){128, 0, 0, 255};
const color_t GREEN = (color_t){0, 255, 0, 255};
const color_t DARK_GREEN = (color_t){0, 128, 0, 255};
const color_t BLUE = (color_t){0, 0, 255, 255};

PVector constrainDistance(PVector pos, PVector anchor, float constraint);
float simplifyAngle(float angle);
float relativeAngleDiff(float angle, float anchor);
float constrainAngle(float angle, float anchor, float constraint);
float apply_deadzone(float value);

#endif // UTILS_H
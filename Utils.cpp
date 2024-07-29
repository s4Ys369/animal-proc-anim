#include <libdragon.h>
#include "Utils.h"

// Utility function definitions

PVector constrainDistance(PVector pos, PVector anchor, float constraint) {
    return PVector::add(anchor, PVector::sub(pos, anchor).setMag(constraint));
}

float simplifyAngle(float angle) {
    while (angle >= TWO_PI) {
        angle -= TWO_PI;
    }
    while (angle < 0) {
        angle += TWO_PI;
    }
    return angle;
}

float relativeAngleDiff(float angle, float anchor) {
    angle = simplifyAngle(angle + M_PI - anchor);
    anchor = M_PI;
    return anchor - angle;
}

float constrainAngle(float angle, float anchor, float constraint) {
    if (std::abs(relativeAngleDiff(angle, anchor)) <= constraint) {
        return simplifyAngle(angle);
    }

    if (relativeAngleDiff(angle, anchor) > constraint) {
        return simplifyAngle(anchor - constraint);
    }

    return simplifyAngle(anchor + constraint);
}

// Function to apply deadzone to a joystick axis input
float apply_deadzone(float value) {
  if (std::abs(value) < DEADZONE) {
      return 0.0f; // Within deadzone, treat as zero
  } else {
    // Remap the value outside the deadzone
    if (value > 0) {
        return (value - DEADZONE) / (1.0f - DEADZONE);
    } else {
      return (value + DEADZONE) / (1.0f - DEADZONE);
    }
  }
}

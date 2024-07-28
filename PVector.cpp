#include <libdragon.h>
#include "PVector.h"

PVector::PVector(float x, float y) : x(x), y(y) {}

void PVector::add(const PVector& v) {
    x += v.x;
    y += v.y;
}

PVector PVector::add(const PVector& v1, const PVector& v2) {
    return PVector(v1.x + v2.x, v1.y + v2.y);
}

PVector PVector::sub(const PVector& v1, const PVector& v2) {
    return PVector(v1.x - v2.x, v1.y - v2.y);
}

float PVector::heading() const {
    return std::atan2(y, x);
}

PVector PVector::fromAngle(float angle) {
    return PVector(std::cos(angle), std::sin(angle));
}

// Calculate magnitude
float PVector::magnitude() const {
    return std::sqrt(x * x + y * y);
}

    // Normalize the vector
void PVector::normalize() {
    float mag = magnitude();
    if (mag != 0) {
        x /= mag;
        y /= mag;
    }
}

    // Set the magnitude of the vector
PVector& PVector::setMag(float newMag) {
        normalize();
        x *= newMag;
        y *= newMag;
        return *this;
}

PVector PVector::copy() const {
    return PVector(x, y);
}

// Function to scale a point outward from the center
PVector PVector::scale(const PVector& center, const PVector& point, float scale) {
    float dx = point.x - center.x;
    float dy = point.y - center.y;
    return PVector(center.x + dx * scale, center.y + dy * scale);
}
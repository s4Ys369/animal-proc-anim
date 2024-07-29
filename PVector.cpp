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

PVector PVector::translate(PVector p, float dx, float dy) {
    p.x += dx;
    p.y += dy;
    return p;
}

PVector PVector::rotate(PVector p, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);
    float xnew = p.x * c - p.y * s;
    float ynew = p.x * s + p.y * c;
    p.x = xnew;
    p.y = ynew;
    return p;
}

// Function to get the X and Y position with a transformation applied
PVector PVector::transform(const PVector& point, float angle, float width) {
    PVector transformed;
    transformed.x = point.x + cosf(angle) * width;
    transformed.y = point.y + sinf(angle) * width;
    return transformed;
}

float PVector::epsilon_test(const PVector& A, const PVector& B, const PVector& C) {
  return (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
}

bool PVector::point_in_triangle(const PVector& P, const PVector& A, const PVector& B, const PVector& C) {
  float ax = C.x - B.x, ay = C.y - B.y;
  float bx = A.x - C.x, by = A.y - C.y;
  float cx = B.x - A.x, cy = B.y - A.y;
  float apx = P.x - A.x, apy = P.y - A.y;
  float bpx = P.x - B.x, bpy = P.y - B.y;
  float cpx = P.x - C.x, cpy = P.y - C.y;

  float aCROSSbp = ax * bpy - ay * bpx;
  float cCROSSap = cx * apy - cy * apx;
  float bCROSScp = bx * cpy - by * cpx;

  return ((aCROSSbp >= 0) && (bCROSScp >= 0) && (cCROSSap >= 0));
}
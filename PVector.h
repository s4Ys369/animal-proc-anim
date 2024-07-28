#ifndef PVECTOR_H
#define PVECTOR_H

#include <libdragon.h>

class PVector {
public:
    float x, y;

    PVector(float x = 0, float y = 0);

    void add(const PVector& v);
    static PVector add(const PVector& v1, const PVector& v2);
    static PVector sub(const PVector& v1, const PVector& v2);
    float heading() const;
    static PVector fromAngle(float angle);
    float magnitude() const;
    void normalize();
    PVector& setMag(float newMag);
    PVector copy() const;
};

#endif // PVECTOR_H
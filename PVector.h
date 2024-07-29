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
    static PVector scale(const PVector& center, const PVector& point, float scale);
    static PVector translate(PVector p, float dx, float dy);
    static PVector rotate(PVector p, float angle);
    static PVector transform(const PVector& point, float angle, float width);
    static float epsilon_test(const PVector& A, const PVector& B, const PVector& C);
    static bool point_in_triangle(const PVector& P, const PVector& A, const PVector& B, const PVector& C);
};

#endif // PVECTOR_H
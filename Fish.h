#ifndef FISH_H
#define FISH_H

#include <libdragon.h>
#include <vector>
#include "Chain.h"
#include "PVector.h"

class Fish {
public:
    Chain spine;
    color_t bodyColor;
    color_t finColor;
    std::vector<float> bodyWidth;

    Fish(PVector origin);
    void resolve(float mouseX, float mouseY);
    void display();

private:
    float getPosX(int i, float angleOffset, float lengthOffset);
    float getPosY(int i, float angleOffset, float lengthOffset);
    void draw_ellipse(float cx, float cy, float rx, float ry);
    void get_ellipse_points(float cx, float cy, float rx, float ry, int segments, std::vector<PVector>& points);
    void draw_pectoral_fin(float posX, float posY, float rotation, float width, float height);
};

#endif // FISH_H
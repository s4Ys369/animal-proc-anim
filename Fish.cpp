#include <libdragon.h>
#include "Chain.h"
#include "Fish.h"
#include "Utils.h"


Fish::Fish(PVector origin)
  : spine(origin, 12, 12, M_PI / 8),
    bodyColor(BLUE),
    finColor(RED),
    bodyWidth{3.2f, 4.0f, 4.2f, 3.8f, 3.4f, 3.2f, 1.8f, 1.0f,0,0} {}

void Fish::resolve(float mouseX, float mouseY) {
  PVector headPos = spine.joints[0];
  PVector targetPos = headPos;

  // Apply deadzone to the joystick inputs
  float adjustedX = apply_deadzone(mouseX);
  float adjustedY = apply_deadzone(mouseY);

  // Normalize the joystick input to get the direction
  PVector direction(adjustedX, -adjustedY);
  direction.normalize();

  // Determine the target position based on the direction and a fixed magnitude
  float movementMag = 3.5;
  targetPos = PVector::add(headPos, direction.setMag(movementMag));

  spine.resolve(targetPos);
  //debugf("X %.1f\nY %.1f\n", targetPos.x, targetPos.y);
}

float Fish::getPosX(int i, float angleOffset, float lengthOffset) {
  return spine.joints[i].x + cosf(spine.angles[i] + angleOffset) * (bodyWidth[i] + lengthOffset);
}

float Fish::getPosY(int i, float angleOffset, float lengthOffset) {
  return spine.joints[i].y + sinf(spine.angles[i] + angleOffset) * (bodyWidth[i] + lengthOffset);
}

void Fish::draw_ellipse(float cx, float cy, float rx, float ry) {
  const int segments = 14;
  float theta = 2 * M_PI / float(segments);
  float cos_theta = cosf(theta);
  float sin_theta = sinf(theta);

  float x = rx;
  float y = 0;

  for (int i = 0; i < segments; ++i) {
    float next_x = cos_theta * x - sin_theta * y;
    float next_y = sin_theta * x + cos_theta * y;

    float v1[] = { cx, cy };
    float v2[] = { cx + x, cy + y };
    float v3[] = { cx + next_x, cy + next_y };

    rdpq_triangle(&TRIFMT_FILL, v1, v2, v3);

    x = next_x;
    y = next_y;
  }
}

// Function get points around an ellipse
void Fish::get_ellipse_points(float cx, float cy, float rx, float ry, int segments, std::vector<PVector>& points) {
  points.clear(); // Clear the vector to store new points
  for (int i = 0; i <= segments; ++i) {
    float theta = 2.0f * M_PI * float(i) / float(segments);
    float x = rx * cosf(theta);
    float y = ry * sinf(theta);
    points.push_back({cx + x, cy + y});
  }
}

void Fish::draw_fin(float posX, float posY, float rotation, float width, float height) {
  // Apply transformations
  PVector origin = spine.joints[6];
  origin = PVector::translate(origin, posX, posY);
  origin = PVector::rotate(origin, rotation);

  // Draw the ellipse at the transformed position
  draw_ellipse(origin.x, origin.y, width, height);
}

// Function to compute tail width
float Fish::get_tail_width(int i, float headToTail) {
  if (i < 12) {
    return 0.05f * headToTail * (i - 8) * (i - 8);
  }
  return fmaxf(-0.05f, fminf(0.05f, headToTail * 6));
}

// Function to draw a curve using triangle fan
void Fish::draw_curve(const std::vector<PVector>& points) {
  const int segments = 14; // Adjust segments as needed

  if (points.size() < 2) return; // Not enough points to draw a curve

  float theta = 2 * M_PI / float(segments);
  float cos_theta = cosf(theta);
  float sin_theta = sinf(theta);

  for (size_t i = 0; i < points.size() - 1; ++i) {
    PVector start = points[i];
    PVector end = points[i + 1];

    float dx = end.x - start.x;
    float dy = end.y - start.y;
    float length = sqrtf(dx * dx + dy * dy);
    float angle = atan2f(dy, dx);

    float x = length / 2; // Initial x offset
    float y = 0; // Initial y offset

    for (int j = 0; j < segments; ++j) {
      float next_x = cos_theta * x - sin_theta * y;
      float next_y = sin_theta * x + cos_theta * y;

      PVector v1 = { start.x, start.y };
      PVector v2 = { start.x + x * cosf(angle), start.y + x * sinf(angle) };
      PVector v3 = { end.x + next_x * cosf(angle), end.y + next_y * sinf(angle) };

      rdpq_triangle(&TRIFMT_FILL, &v1.x, &v2.x, &v3.x);

      x = next_x;
      y = next_y;
    }
  }
}

// Function to draw a line segment of certain thickness using two triangles
void Fish::draw_line(float x1, float y1, float x2, float y2, float thickness) {
    // Calculate direction vector of the line
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = sqrtf(dx * dx + dy * dy);

    // Normalize the direction vector
    if (length != 0) {
      dx /= length;
      dy /= length;
    }

    // Calculate the perpendicular vector for the thickness
    float perp_x = -dy * thickness / 2;
    float perp_y = dx * thickness / 2;

    // Define vertices of the trapezoid
    float x1_left = x1 + perp_x;
    float y1_left = y1 + perp_y;
    float x1_right = x1 - perp_x;
    float y1_right = y1 - perp_y;
    float x2_left = x2 + perp_x;
    float y2_left = y2 + perp_y;
    float x2_right = x2 - perp_x;
    float y2_right = y2 - perp_y;

    // Define vertices for two triangles
    float v1[] = { x1_left, y1_left };
    float v2[] = { x1_right, y1_right };
    float v3[] = { x2_left, y2_left };
    float v4[] = { x2_right, y2_right };

    // Draw two triangles to form the trapezoid
    rdpq_triangle(&TRIFMT_FILL, v1, v2, v3); // First triangle
    rdpq_triangle(&TRIFMT_FILL, v2, v4, v3); // Second triangle
}

// Function to draw a Bézier curve using line segments
void Fish::draw_bezier_curve(const PVector& p0, const PVector& p1, const PVector& p2, const PVector& p3, int segments) {
  std::vector<PVector> curvePoints;

  // Compute Bézier curve points
  for (int i = 0; i <= segments; ++i) {
    float t = float(i) / float(segments);
    float u = 1 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    PVector p = { uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x,
                  uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y };

    curvePoints.push_back(p);
  }

  // Draw the Bézier curve as a series of connected line segments
  for (size_t i = 0; i < curvePoints.size() - 1; ++i) {
    PVector p1 = curvePoints[i];
    PVector p2 = curvePoints[i + 1];

    draw_line(p1.x, p1.y, p2.x, p2.y , 1.0f);
  }
}

void Fish::draw_tail(const std::vector<PVector>& j, const std::vector<float>& a, float headToTail) {
    std::vector<PVector> bottomPoints;
    std::vector<PVector> topPoints;

    // "Bottom" of the fish
    for (int i = 8; i < 12; ++i) {
        float tailWidth = get_tail_width(i, headToTail);
        PVector transformedPoint = PVector::transform(j[i], a[i] - M_PI / 2, tailWidth);
        bottomPoints.push_back(transformedPoint);
    }

    // "Top" of the fish
    for (int i = 11; i >= 8; --i) {
        float tailWidth = get_tail_width(i, headToTail);
        PVector transformedPoint = PVector::transform(j[i], a[i] + M_PI / 2, tailWidth);
        topPoints.push_back(transformedPoint);
    }

    // Draw the "bottom" and "top" of the tail as a single closed shape
    bottomPoints.insert(bottomPoints.end(), topPoints.begin(), topPoints.end());
    draw_curve(bottomPoints); // Draw the shape
}

void Fish::draw_body() {
  const int segments = 10; // Number of segments to approximate the curve

  // Prepare a list of points
  std::vector<PVector> points;

  // Right half of the fish
  for (int i = 0; i < segments; ++i) {
    float x = getPosX(i, M_PI/2, 0);
    float y = getPosY(i, M_PI/2, 0);
    points.push_back({x, y});
  }

  // Bottom of the fish
  points.push_back({getPosX(9, M_PI, 0), getPosY(9, M_PI, 0)});

    // Left half of the fish
  for (int i = 9; i >= 0; --i) {
    float x = getPosX(i, -M_PI/2, 0);
    float y = getPosY(i, -M_PI/2, 0);
    points.push_back({x, y});
  }

  // Top of the head (completes the loop)
  points.push_back({getPosX(0, -M_PI/6, 0), getPosY(0, -M_PI/6, 0)});
  points.push_back({getPosX(0, 0, 4), getPosY(0, 0, 4)});
  points.push_back({getPosX(0, M_PI/6, 0), getPosY(0, M_PI/6, 0)});

  // Some overlap needed because curveVertex requires extra vertices that are not rendered
  points.push_back({getPosX(0, M_PI/2, 0), getPosY(0, M_PI/2, 0)});
  points.push_back({getPosX(1, M_PI/2, 0), getPosY(1, M_PI/2, 0)});
  points.push_back({getPosX(2, M_PI/2, 0), getPosY(2, M_PI/2, 0)});

  // Draw the shape using triangles
  // Assumes a triangle strip or fan method
  for (size_t i = 0; i < points.size() - 2; ++i) {
    // cast PVector to float[] for `rdpq_triangle`
    float v1[] = { points[i].x, points[i].y };
    float v2[] = { points[i + 1].x, points[i + 1].y };
    float v3[] = { points[i + 2].x, points[i + 2].y };
    rdpq_triangle(&TRIFMT_FILL, v1, v2, v3);
  }
}

void Fish::display() {

  rdpq_set_mode_fill(finColor);

  // Alternate labels for shorter lines of code
  std::vector<PVector> j = spine.joints;
  std::vector<float> a = spine.angles;

  // Relative angle differences are used in some hacky computation for the dorsal fin
  float headToMid1 = relativeAngleDiff(a[0], a[6]);
  float headToMid2 = relativeAngleDiff(a[0], a[7]);

  // For the caudal fin, we need to compute the relative angle difference from the head to the tail, but given
  // a joint count of 12 and angle constraint of PI/8, the maximum difference between head and tail is 11PI/8,
  // which is >PI. This complicates the relative angle calculation (flips the sign when curving too tightly).
  // A quick workaround is to compute the angle difference from the head to the middle of the fish, and then
  // from the middle of the fish to the tail.
  float headToTail = headToMid1 + relativeAngleDiff(a[6], a[11]);

  // === START PECTORAL FINS ===

  // Drawing the right fin
  float xPosRight_pec = getPosX(3, M_PI / 3, 0);
  float yPosRight_pec = getPosY(3, M_PI / 3, 0);
  float angleRight_pec = a[2] - M_PI / 4;
  draw_fin(xPosRight_pec, yPosRight_pec, angleRight_pec, 10.0f, 4.0f);

  // Drawing the left fin
  float xPosLeft_pec = getPosX(3, -M_PI / 3, 0);
  float yPosLeft_pec = getPosY(3, -M_PI / 3, 0);
  float angleLeft_pec = a[2] + M_PI / 4;
  draw_fin(xPosLeft_pec, yPosLeft_pec, angleLeft_pec, 10.0f, 4.0f);
  // === END PECTORAL FINS ===

  // === START VENTRAL FINS ===

  // Drawing the right fin
  float xPosRight_ven = getPosX(7, M_PI / 2, 0);
  float yPosRight_ven = getPosY(7, M_PI / 2, 0);
  float angleRight_ven = a[6] - M_PI / 4;
  draw_fin(xPosRight_ven, yPosRight_ven, angleRight_ven, 7.0f, 2.0f);

  // Drawing the left fin
  float xPosLeft_ven = getPosX(7, -M_PI / 2, 0);
  float yPosLeft_ven = getPosY(7, -M_PI / 2, 0);
  float angleLeft_ven = a[6] + M_PI / 4;
  draw_fin(xPosLeft_ven, yPosLeft_ven, angleLeft_ven, 7.0f, 2.0f);

  // === END VENTRAL FINS ===

  // === START CAUDAL FINS ===

  // Draw the tail
  //draw_tail(j, a, headToTail);

  // === END CAUDAL FINS ===

  rdpq_set_mode_fill(bodyColor);

  // === START BODY ===
  
  // Draw the body
  draw_body();

  // === END BODY ===

  rdpq_set_mode_fill(finColor);

  // === START DORSAL FIN ===

  // Draw Bezier curve for dorsal fin
  draw_bezier_curve(j[4], j[5], j[6], j[7], 16); // Right

  //Transform the points for the left side
  PVector j5 = {j[5].x + cosf(a[5] + M_PI/2) * headToMid2 * 16,
                j[5].y + sinf(a[5] + M_PI/2) * headToMid2 * 16,};
  PVector j6 = {j[6].x + cosf(a[6] + M_PI/2) * headToMid2 * 16,
                j[6].y + sinf(a[6] + M_PI/2) * headToMid2 * 16,};

  draw_bezier_curve(j[7], j6, j5, j[4], 16); // Left

  // === END DORSAL FIN ===

  // === START EYES ===
  
  // Draw eye outline
  rdpq_set_prim_color(DARK_GREEN);
  draw_ellipse(getPosX(0, M_PI / 2, -1), getPosY(0, M_PI / 2, -1), 1.5, 1.5);
  draw_ellipse(getPosX(0, -M_PI / 2, -1), getPosY(0, -M_PI / 2, -1), 1.5, 1.5);

  // Draw eyes
  rdpq_set_prim_color(GREEN);
  draw_ellipse(getPosX(0, M_PI / 2, -1), getPosY(0, M_PI / 2, -1), 1, 1);
  draw_ellipse(getPosX(0, -M_PI / 2, -1), getPosY(0, -M_PI / 2, -1), 1, 1);

  // === END EYES ===
}


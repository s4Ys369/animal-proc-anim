#include <libdragon.h>
#include "Chain.h"
#include "Fish.h"
#include "Utils.h"


Fish::Fish(PVector origin)
    : bodyColor(BLUE),
      finColor(RED),
      bodyWidth{4, 7, 7.2f, 7.1f, 5, 4.5, 4.2, 4, 3, 2, 1.5f, 1},
      spine(origin, 12, 12, M_PI / 8) {}


const float DEADZONE = 0.5f; // Define the deadzone threshold

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

void Fish::draw_pectoral_fin(float posX, float posY, float rotation, float width, float height) {
    //glTranslatef(posX, posY, 0.0f);
    //glRotatef(rotation * 180.0f / M_PI, 0.0f, 0.0f, 1.0f); // Convert radians to degrees
    draw_ellipse(0.0f, 0.0f, width / 2.0f, height / 2.0f);
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
  draw_pectoral_fin(xPosRight_pec, yPosRight_pec, angleRight_pec, 10.0f, 4.0f);

  // Drawing the left fin
  float xPosLeft_pec = getPosX(3, -M_PI / 3, 0);
  float yPosLeft_pec = getPosY(3, -M_PI / 3, 0);
  float angleLeft_pec = a[2] + M_PI / 4;
  draw_pectoral_fin(xPosLeft_pec, yPosLeft_pec, angleLeft_pec, 10.0f, 4.0f);
  // === END PECTORAL FINS ===

  // === START VENTRAL FINS ===

  // === END VENTRAL FINS ===

  // === START CAUDAL FINS ===
  beginShape();
  // "Bottom" of the fish
  for (int i = 8; i < 12; i++) {
    float tailWidth = 1.5 * headToTail * (i - 8) * (i - 8);
    curveVertex(j[i].x + cosf(a[i] - PI/2) * tailWidth, j[i].y + sinf(a[i] - PI/2) * tailWidth);
  }

  // "Top" of the fish
  for (int i = 11; i >= 8; i--) {
    float tailWidth = fmaxf(-13, fminf(13, headToTail * 6));
    curveVertex(j[i].x + cosf(a[i] + PI/2) * tailWidth, j[i].y + sinf(a[i] + PI/2) * tailWidth);
  }
  endShape(CLOSE);
  // === END CAUDAL FINS ===

  fill(bodyColor);

  // === START BODY ===
  beginShape();

  // Right half of the fish
  for (int i = 0; i < 10; i++) {
    curveVertex(getPosX(i, PI/2, 0), getPosY(i, PI/2, 0));
  }

  // Bottom of the fish
  curveVertex(getPosX(9, PI, 0), getPosY(9, PI, 0));

  // Left half of the fish
  for (int i = 9; i >= 0; i--) {
    curveVertex(getPosX(i, -PI/2, 0), getPosY(i, -PI/2, 0));
  }


  // Top of the head (completes the loop)
  curveVertex(getPosX(0, -PI/6, 0), getPosY(0, -PI/6, 0));
  curveVertex(getPosX(0, 0, 4), getPosY(0, 0, 4));
  curveVertex(getPosX(0, PI/6, 0), getPosY(0, PI/6, 0));

  // Some overlap needed because curveVertex requires extra vertices that are not rendered
  curveVertex(getPosX(0, PI/2, 0), getPosY(0, PI/2, 0));
  curveVertex(getPosX(1, PI/2, 0), getPosY(1, PI/2, 0));
  curveVertex(getPosX(2, PI/2, 0), getPosY(2, PI/2, 0));

  endShape(CLOSE);
  // === END BODY ===

  fill(finColor);

  // === START DORSAL FIN ===
  beginShape();
  vertex(j.get(4).x, j.get(4).y);
  bezierVertex(j.get(5).x, j.get(5).y, j.get(6).x, j.get(6).y, j.get(7).x, j.get(7).y);
  bezierVertex(j.get(6).x + cos(a.get(6) + PI/2) * headToMid2 * 16, j.get(6).y + sin(a.get(6) + PI/2) * headToMid2 * 16, j.get(5).x + cos(a.get(5) + PI/2) * headToMid1 * 16, j.get(5).y + sin(a.get(5) + PI/2) * headToMid1 * 16, j.get(4).x, j.get(4).y);
  endShape();
  // === END DORSAL FIN ===

  // === START EYES ===
  fill(255);
  ellipse(getPosX(0, PI/2, -18), getPosY(0, PI/2, -18), 24, 24);
  ellipse(getPosX(0, -PI/2, -18), getPosY(0, -PI/2, -18), 24, 24);
  // === END EYES ===
}


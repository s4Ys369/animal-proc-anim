#include <libdragon.h>
#include "Chain.h"
#include "Snake.h"
#include "Utils.h"

// Wiggly lil dude
Snake::Snake(PVector origin)
    : spine(origin, 12, 12, M_PI / 8),
      bodyWidth{3, 3, 4, 4, 4, 4, 3, 3, 3, 2, 2, 1} {}
      

void Snake::resolve(float mouseX, float mouseY) {

  PVector headPos = spine.joints[0];
  PVector targetPos = headPos;

  // Normalize the joystick input to get the direction
  PVector direction(mouseX, -mouseY);
  direction.normalize();

  // Determine the target position based on the direction and a fixed magnitude
  float movementMag = 4.0;
  targetPos = PVector::add(headPos, direction.setMag(movementMag));

  spine.resolve(targetPos);
  //debugf("X %.1f\nY %.1f\n", targetPos.x, targetPos.y);
}

float Snake::getBodyWidth(int i) {
  return bodyWidth[i];
}

float Snake::getPosX(int i, float angleOffset, float lengthOffset) {
  return spine.joints[i].x + cosf(spine.angles[i] + angleOffset) * (bodyWidth[i] + lengthOffset);
}

float Snake::getPosY(int i, float angleOffset, float lengthOffset) {
  return spine.joints[i].y + sinf(spine.angles[i] + angleOffset) * (bodyWidth[i] + lengthOffset);
}

void Snake::draw_ellipse(float cx, float cy, float rx, float ry) {
  const int segments = 10;
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
void Snake::get_ellipse_points(float cx, float cy, float rx, float ry, int segments, std::vector<PVector>& points) {
  points.clear(); // Clear the vector to store new points
  for (int i = 0; i <= segments; ++i) {
      float theta = 2.0f * M_PI * float(i) / float(segments);
      float x = rx * cosf(theta);
      float y = ry * sinf(theta);
      points.push_back({cx + x, cy + y});
  }
}


void Snake::draw_snake_shape() {
  size_t vertex_count = 0;
  size_t max_vertices = spine.joints.size() * 2 + 6; // Adjust this as needed
  float (*vertices)[2] = (float (*)[2])malloc(max_vertices * 2 * sizeof(float));
    
  if (!vertices) {
    debugf("No vertices!");
    return;
  }

  // Right half of the snake
  for (size_t i = 0; i < 12; i++) {
    vertices[vertex_count][0] = getPosX(i, M_PI / 2, 0);
    vertices[vertex_count][1] = getPosY(i, M_PI / 2, 0);
    vertex_count++;
  }

  // Top of the head (completes the loop)
  vertices[vertex_count][0] = getPosX(11, M_PI, 0);
  vertices[vertex_count][1] = getPosY(11, M_PI, 0);
  vertex_count++;

  // Left half of the snake
  for (int i = 11; i >= 0; --i) {
    vertices[vertex_count][0] = getPosX(i, -M_PI / 2, 0);
    vertices[vertex_count][1] = getPosY(i, -M_PI / 2, 0);
    vertex_count++;
  }

  // Add vertices to complete the loop
  vertices[vertex_count][0] = getPosX(0, -M_PI / 6, 0);
  vertices[vertex_count][1] = getPosY(0, -M_PI / 6, 0);
  vertex_count++;
  vertices[vertex_count][0] = getPosX(0, 0, 0);
  vertices[vertex_count][1] = getPosY(0, 0, 0);
  vertex_count++;
  vertices[vertex_count][0] = getPosX(0, M_PI / 6, 0);
  vertices[vertex_count][1] = getPosY(0, M_PI / 6, 0);
  vertex_count++;

  // Set color
  rdpq_set_prim_color(RED);

  // Draw edges
  for (size_t i = 0; i < vertex_count - 2; ++i) {
    rdpq_triangle(&TRIFMT_FILL, vertices[i], vertices[i + 1], vertices[i + 2]);
  }

  std::vector<PVector> previous_points;
  std::vector<PVector> current_points;

  // Draw joints
  for (size_t i = 0; i < spine.joints.size(); ++i) {
    draw_ellipse(getPosX(i, 0, 0), getPosY(i, 0, 0), getBodyWidth(i), getBodyWidth(i));
    get_ellipse_points(getPosX(i, 0, 0), getPosY(i, 0, 0), getBodyWidth(i), getBodyWidth(i), 10, current_points);

    if (!previous_points.empty()) {
      // Create triangles between previous_points and current_points
      for (int j = 0; j < 10; ++j) {
        float v1[] = { previous_points[j].x, previous_points[j].y };
        float v2[] = { previous_points[j + 1].x, previous_points[j + 1].y };
        float v3[] = { current_points[j].x, current_points[j].y };
        float v4[] = { current_points[j + 1].x, current_points[j + 1].y };

        // Draw two triangles to form a quad between the points
        rdpq_triangle(&TRIFMT_FILL, v1, v2, v3);
        rdpq_triangle(&TRIFMT_FILL, v2, v4, v3);
      }
    }

    previous_points = current_points; // Save current points for the next iteration
  }

  // Draw eyes
  rdpq_set_prim_color(GREEN);
  draw_ellipse(getPosX(0, M_PI / 2, -1), getPosY(0, M_PI / 2, -1), 1, 1);
  draw_ellipse(getPosX(0, -M_PI / 2, -1), getPosY(0, -M_PI / 2, -1), 1, 1);

  // Free vertices after drawing
  free(vertices);
}

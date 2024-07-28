#include <libdragon.h>
#include "Chain.h"
#include "Snake.h"
#include "Utils.h"

// Global variables
//Fish* fish;
Snake* snake;
//Lizard* lizard;
int animal;
float mouseX = 0;
float mouseY = 0;
surface_t disp;

// Initialize the environment
void setup() {
  // Initialize libdragon
  debug_init_isviewer();
  debug_init_usblog();
    
  dfs_init(DFS_DEFAULT_LOCATION);

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS_DEDITHER);
  disp = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height());

  rdpq_init();
  rdpq_debug_start();

  joypad_init();

  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  // Initialize animals
  snake = new Snake(PVector(160, 120)); // Centered on a 320x240 screen
  //fish = new Fish(PVector(160, 120));  
  //lizard = new Lizard(PVector(160, 120));

  animal = 0;
}

// Main rendering function
void draw() {

  switch (animal) {
    case 0:
      snake->resolve(mouseX, mouseY);
      snake->draw_snake_shape();
      break;
    case 1:
      //fish->resolve();
      //fish->display();
      break;
    case 2:
      //lizard->resolve();
      //lizard->display();
      break;
  }
}

// Handle mouse press to switch animals
void mousePressed() {
  if (++animal > 2) {
    animal = 0;
  }
}

// Main function with rendering loop
int main() {
  setup();

  while (1) {
    rdpq_attach(display_get(), &disp);
    rdpq_clear(WHITE);
    rdpq_clear_z(0xFFFC);
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);

    // Check for mouse press event and handle it
    joypad_poll();
    joypad_inputs_t input = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    joypad_buttons_t keysDown = joypad_get_buttons_held(JOYPAD_PORT_1);

    mouseX = (float)input.stick_x;
    mouseY = (float)input.stick_y;

    if (keys.a) {
      mousePressed();
    }

    draw();
    if(keysDown.b){
      snake->spine.display();
    }

    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 20, 220, "FPS: %.2f", display_get_fps());

    rdpq_detach_show();
  }

  return 0;
}

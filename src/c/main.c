#include <pebble.h>
#include "elements.h"

static Window *s_main_window;


static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  init_layers(window_layer);
}

static void main_window_unload(Window *window) {
  destroy_layers();
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  activate_seconds();
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_set_background_color(s_main_window, GColorBlack);

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Make sure the time is displayed from the start
  update_time();

  // Register tap handler
  accel_tap_service_subscribe(tap_handler);
  
  // Register for Bluetooth connection updates
  connection_service_subscribe((ConnectionHandlers) {
      .pebble_app_connection_handler = bluetooth_callback
  });
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
// Used as a countdown
static int show_seconds;

static void activate_seconds();
static void deactivate_seconds();

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // Write the current hours and minutes into a buffer
  static char s_time_buffer[10];
  static char s_date_buffer[11];

  if (show_seconds > 0) {
    // Write the current hours and minutes into a buffer
    strftime(s_time_buffer, sizeof(s_time_buffer),
             clock_is_24h_style() ? "%T" : "%I:%M:%S", tick_time);
    
    show_seconds--;
    // Stop updating every second
    if (show_seconds == 0) {
      deactivate_seconds();
    }
  } else {  
    strftime(s_time_buffer, sizeof(s_time_buffer),
             clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_time_buffer);
  
  
  // ...and the date layer
  strftime(s_date_buffer, sizeof(s_date_buffer), "%F", tick_time);
  // Display date on the date layer
  text_layer_set_text(s_date_layer, s_date_buffer);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer for time with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 45));
  
  // Create the TextLayer for date with specific bounds
  s_date_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(98, 92), bounds.size.w, 25));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Not very DRY...
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);


  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
}

static void main_window_unload(Window *window) {
  // Destroy time
  text_layer_destroy(s_time_layer);
  // Destroy date
  text_layer_destroy(s_date_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void activate_seconds() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Activating seconds");
  show_seconds = 5;
  
  // Change some properties for time layer with seconds
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS));
  update_time();
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deactivate_seconds() {
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  update_time();
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
  show_seconds = 0;

  // Make sure the time is displayed from the start
  update_time();

  // Register tap handler
  accel_tap_service_subscribe(tap_handler);
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

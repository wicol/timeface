#include "elements.h"

// Private vars
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static GRect window_bounds;
// Used as a countdown
static int show_seconds;

void init_layers(Layer *window_layer) {
    window_bounds = layer_get_bounds(window_layer);
    
    // Create the TextLayer for time with specific bounds
    s_time_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(50, 44), window_bounds.size.w, 45));
    // Set time layer characteristics
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    // Add it as a child layer to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
    
    // Create the TextLayer for date with specific bounds
    s_date_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(90, 84), window_bounds.size.w, 45));
    // Set date layer characteristics
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, GColorWhite);
    text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
    // Add it as a child layer to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
}

void destroy_layers() {
    // Destroy time
    text_layer_destroy(s_time_layer);
    // Destroy date
    text_layer_destroy(s_date_layer);
}

void update_time() {
    // Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    
    // Write the current time into buffers
    static char s_time_buffer[10];
    static char s_date_buffer[15];
    
    if (show_seconds > 0) {
        // Put hours, minutes and seconds into the buffer
        strftime(s_time_buffer, sizeof(s_time_buffer),
                 clock_is_24h_style() ? "%T" : "%I:%M:%S", tick_time);
        
        show_seconds--;
        // If time is up: stop updating every second
        if (show_seconds == 0) {
          deactivate_seconds();
        }
    } else {
        // Put just hours and minutes into the buffer
        strftime(s_time_buffer, sizeof(s_time_buffer),
                 clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
    }
    // Display time on the time TextLayer
    text_layer_set_text(s_time_layer, s_time_buffer);
    
    // Put the date - and week number - into the date buffer
    strftime(s_date_buffer, sizeof(s_date_buffer), "%F\n%V", tick_time);
    // Display date on the date TextLayer
    text_layer_set_text(s_date_layer, s_date_buffer);
}

void activate_seconds() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Activating seconds");
  show_seconds = 5;
  
  // Change some properties for time layer with seconds
  // Smaller font so it'll all fit the screen
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS));
  // Move layer down a bit to compensate for the smaller font
  layer_set_frame(
      text_layer_get_layer(s_time_layer),
      GRect(0, PBL_IF_ROUND_ELSE(55, 49), window_bounds.size.w, 45)
  );
  update_time();
  // Update every second
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

void deactivate_seconds() {
  // Update every minute
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  // Reset font and layer position
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  layer_set_frame(text_layer_get_layer(s_time_layer), GRect(0, PBL_IF_ROUND_ELSE(50, 44), window_bounds.size.w, 45));
  update_time();
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
#include <pebble.h>

void init_layers(Layer *window_layer);
void destroy_layers();
void activate_seconds();
void deactivate_seconds();
void update_time();

void tick_handler(struct tm *tick_time, TimeUnits units_changed);
void bluetooth_callback(bool connected);
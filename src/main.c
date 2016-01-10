#include <pebble.h>

static Window *s_main_window;
static Layer *s_time_layer;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time(Layer *layer, GContext *ctx) {
  time_t temp = time(NULL); 
  struct tm *t = localtime(&temp);

  GRect bounds = GRect(12, 12, 156, 156);
  graphics_context_set_stroke_width(ctx, 2);
  graphics_draw_circle(ctx, gpoint_from_polar(bounds, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(t->tm_hour*30)), 6);
  graphics_draw_circle(ctx, gpoint_from_polar(bounds, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(t->tm_min*6)), 3);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_main_window));
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_MEGAWATCH_BACKGROUND);
  s_background_layer = bitmap_layer_create(bounds);

  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));  
    
  s_time_layer = layer_create(bounds);
  layer_set_update_proc(s_time_layer, update_time);
  layer_add_child(window_layer, s_time_layer);
}

static void main_window_unload(Window *window) {
  layer_destroy(s_time_layer);
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
}


static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_background_color(s_main_window, GColorWhite);  
  window_stack_push(s_main_window, true);
    
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
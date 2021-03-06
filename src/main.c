#include <pebble.h>
#include "main_window.h"
#include "resources.h"

extern Window *main_window;
extern TextLayer *text_layer;
extern TextLayer *time_text;
extern TextLayer *top_date_text;
extern TextLayer *bottom_date_text;
extern TextLayer *bluetooth_text;
extern TextLayer *battery_text;
extern TextLayer *top_additional_info_text;
extern TextLayer *bottom_additional_info_text;

extern BitmapLayer *bluetooth_icon_layer;
extern BitmapLayer *battery_icon_layer;
extern BitmapLayer *unread_message_layer;

extern GBitmap *bluetooth_icon;
extern GBitmap *battery_icon;
extern GBitmap *unread_message_icon;

struct {
	int language;
	int window_color;
	int time_text_size;
	int date_format;
	int show_battery_text;
	int show_bluetooth_text;
	int vibe_hourly_vibe;
	int vibe_bluetooth_state_change;
	int night_mode_enabled;
	int night_mode_started;
	int night_mode_finished;
	int night_mode_display_invert;
	int night_mode_update_info;
	int night_mode_vibe_on_event;
	int night_mode_vibe_hourly_vibe;
	int data_updates_frequency;
	int date_style;
	int show_last_disconnect_time;
	int blink_colon;
	int night_mode_blink_colon;
} settings;

struct {
	int current_window_color;
	int is_bluetooth_connected;
	int vibes_allowed;
	int is_night_now;
	int is_charging;
	int is_plugged;
	int battery_percents;
	int hourly_vibes_allowed;
	int is_state_changed;
	int is_unread_messages;
} flags;

char top_additional_info_buffer    [27];
char bottom_additional_info_buffer [27];
char time_text_buffer              [6];
char date_text_buffer              [26];
char battery_text_buffer           [5];
char bluetooth_text_buffer         [6];

AppTimer* is_receiving_data;
AppTimer* battery_animation_timer;

void update_time(struct tm* current_time, TimeUnits units_changed);
void update_date(struct tm* current_time, TimeUnits units_changed);

void update_bluetooth_connection(bool is_connected);
void update_battery_state(BatteryChargeState battery_state);
inline void update_additional_info(void);

inline void read_persist_settings(void);

inline void initialization(void);

inline void is_night(void);
inline void subscribe_to_time_update_service(void);

void update_battery_text(void);
void update_battery_icon(void);
void update_unread_message_icon(void);
inline void update_bar(void);

void update_bluetooth_icon(void);
void update_bluetooth_text(void);

void update_bar(void);

tm* get_system_time();

int main(void);


tm* get_system_time(){
	static time_t now;
	now = time(NULL);
	return localtime(&now);
}

void request_data_error(){
	gbitmap_destroy(bluetooth_icon); 
	bluetooth_icon = gbitmap_create_with_resource(updating_icons[flags.current_window_color][1]); 
	bitmap_layer_set_bitmap(bluetooth_icon_layer, bluetooth_icon);
	layer_mark_dirty((Layer *)bluetooth_icon_layer);
}

void request_data_from_phone(){
	if ( (!flags.is_night_now || settings.night_mode_update_info) && (flags.is_bluetooth_connected) ){
		is_receiving_data = app_timer_register(RECEIVING_LATENCY_TIME, request_data_error, 0);
	
		gbitmap_destroy(bluetooth_icon); 
		bluetooth_icon = gbitmap_create_with_resource(updating_icons[flags.current_window_color][0]); 
		bitmap_layer_set_bitmap(bluetooth_icon_layer, bluetooth_icon);
		layer_mark_dirty((Layer *)bluetooth_icon_layer);
	
    	DictionaryIterator *iter;
    	app_message_outbox_begin(&iter);
    	dict_write_uint8(iter, ASK_DATA_FROM_PHONE_INFO, 0);
    	app_message_outbox_send();
	}
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {	
 	Tuple *language_tuple = dict_find(iterator, LANGUAGE_INFO);
	Tuple *window_color_tuple = dict_find(iterator, WINDOW_COLOR_INFO);
	Tuple *time_text_size_tuple = dict_find(iterator, TIME_TEXT_SIZE_INFO);
	Tuple *date_format_tuple = dict_find(iterator, DATE_FORMAT_INFO);
	Tuple *date_style_tuple = dict_find(iterator, DATE_STYLE_INFO);
	Tuple *show_battery_text_tuple = dict_find(iterator, SHOW_BATTERY_TEXT_INFO);
	Tuple *show_bluetooth_text_tuple = dict_find(iterator, SHOW_BLUETOOTH_TEXT_INFO);
	Tuple *vibe_hourly_vibe_tuple = dict_find(iterator, VIBE_HOURLY_VIBE_INFO);
	Tuple *vibe_bluetooth_state_change_tuple = dict_find(iterator, VIBE_BLUETOOTH_STATE_CHANGE_INFO);
	Tuple *night_mode_enabled_tuple = dict_find(iterator, NIGHT_MODE_ENABLED_INFO);
	Tuple *night_mode_started_tuple = dict_find(iterator, NIGHT_MODE_STARTED_INFO);
	Tuple *night_mode_finished_tuple = dict_find(iterator, NIGTH_MODE_FINISHED_INFO);
	Tuple *night_mode_vibe_hourly_vibe_tuple = dict_find(iterator, NIGHT_MODE_HOURLY_VIBE_INFO);
	Tuple *top_additional_string_text_tuple = dict_find(iterator, TOP_ADDITIONAL_STRING_TEXT_INFO);
	Tuple *bottom_additional_string_text_tuple = dict_find(iterator, BOTTOM_ADDITIONAL_STRING_TEXT_INFO);
	Tuple *night_mode_display_invert_tuple = dict_find(iterator, NIGTH_MODE_DISPLAY_INVERT_INFO);
	Tuple *night_mode_update_info_tuple = dict_find(iterator, NIGHT_MODE_UPDATE_INFO_INFO);
	Tuple *night_mode_vibe_on_event_tuple= dict_find(iterator, NIGTH_MODE_VIBE_ON_EVENT_INFO);
	Tuple *data_updates_frequency_tuple = dict_find(iterator, DATA_UPDATE_FREQUENCY_INFO);
	Tuple *show_last_disconnect_time_tuple = dict_find(iterator, SHOW_LAST_DISCONNECT_TIME_INFO);
	Tuple *blink_colon_tuple = dict_find(iterator, BLINK_COLON_INFO);
	Tuple *night_mode_blink_colon_tuple = dict_find(iterator, NIGHT_MODE_BLINK_COLON_INFO);

	
	flags.vibes_allowed = 0;
	flags.is_state_changed = STATE_WAS_CHANGED;
	
	if (top_additional_string_text_tuple){
		strcpy(top_additional_info_buffer, top_additional_string_text_tuple->value->cstring);
		persist_write_string(TOP_ADDITIONAL_STRING_TEXT_KEY, top_additional_info_buffer);
		text_layer_set_text(top_additional_info_text, top_additional_info_buffer);
		//APP_LOG(APP_LOG_LEVEL_INFO, "top additional info received!");
	}
	
	if (bottom_additional_string_text_tuple){
		strcpy(bottom_additional_info_buffer, bottom_additional_string_text_tuple->value->cstring);
		persist_write_string(BOTTOM_ADDITIONAL_STRING_TEXT_KEY, bottom_additional_info_buffer);
		text_layer_set_text(bottom_additional_info_text, bottom_additional_info_buffer);
		//APP_LOG(APP_LOG_LEVEL_INFO, "bottom additional info received!");
	}
	
	if (data_updates_frequency_tuple){
		persist_write_int(DATA_UPDATES_FREQUENCY_KEY, (int)data_updates_frequency_tuple->value->int32);
		settings.data_updates_frequency = (int)data_updates_frequency_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "data update frequency settings received!");
	}
	
	if (night_mode_blink_colon_tuple){
		persist_write_int(NIGHT_MODE_BLINK_COLON_KEY, (int)night_mode_blink_colon_tuple->value->int32);
		settings.night_mode_blink_colon = (int)night_mode_blink_colon_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "night blinking settings received!");
	}
	
	if (show_last_disconnect_time_tuple){
		persist_write_int(SHOW_LAST_DISCONNECT_TIME_KEY, (int)show_last_disconnect_time_tuple->value->int32);
		settings.show_last_disconnect_time = (int)show_last_disconnect_time_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "data update frequency settings received!");
	}
	
	if (night_mode_vibe_hourly_vibe_tuple){
		persist_write_int(NIGHT_MODE_VIBE_HOURLY_VIBE_KEY, (int)night_mode_vibe_hourly_vibe_tuple->value->int32);
		settings.night_mode_vibe_hourly_vibe = (int)night_mode_vibe_hourly_vibe_tuple->value->int32;
	}
	
	if (night_mode_vibe_on_event_tuple){
		persist_write_int(NIGHT_MODE_VIBE_ON_EVENT_KEY, (int)night_mode_vibe_on_event_tuple->value->int32);
		settings.night_mode_vibe_on_event = (int)night_mode_vibe_on_event_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "night mode vibrations settings received!");
	}
	
	if (night_mode_update_info_tuple){
		persist_write_int(NIGHT_MODE_UPDATE_INFO_KEY, (int)night_mode_update_info_tuple->value->int32);
		settings.night_mode_update_info = (int)night_mode_update_info_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "night mode update info settings received!");
	}
	
	if (night_mode_display_invert_tuple){
		persist_write_int(NIGHT_MODE_DISPLAY_INVERT_KEY, (int)night_mode_display_invert_tuple->value->int32);
		settings.night_mode_display_invert = (int)night_mode_display_invert_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "night mode display invert settings received!");
	}
	
	if (night_mode_finished_tuple){
		persist_write_int(NIGHT_MODE_FINISHED_KEY, (int)night_mode_finished_tuple->value->int32);
		settings.night_mode_finished = (int)night_mode_finished_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "night mode finished settings received!");
	}
	
	if (night_mode_started_tuple){
		persist_write_int(NIGHT_MODE_STARTED_KEY, (int)night_mode_started_tuple->value->int32);
		settings.night_mode_started = (int)night_mode_started_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "night mode started settings received!");
	}
	
	if (night_mode_enabled_tuple){
		persist_write_int(NIGHT_MODE_ENABLED_KEY, (int)night_mode_enabled_tuple->value->int32);
		settings.night_mode_enabled = (int)night_mode_enabled_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "night mode enabled settings received!");
	}
	
	if (vibe_bluetooth_state_change_tuple){
		persist_write_int(VIBE_BLUETOOTH_STATE_CHANGE_KEY, (int)vibe_bluetooth_state_change_tuple->value->int32);
		settings.vibe_bluetooth_state_change = (int)vibe_bluetooth_state_change_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "bluetooth vibration settings received!");
	}
	
	if (vibe_hourly_vibe_tuple){
		persist_write_int(VIBE_HOURLY_VIBE_KEY, (int)vibe_hourly_vibe_tuple->value->int32);
		settings.vibe_hourly_vibe = (int)vibe_hourly_vibe_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "hourly vibration settings received!");
	}
	
	if (show_bluetooth_text_tuple){
		persist_write_int(SHOW_BLUETOOTH_TEXT_KEY, (int)show_bluetooth_text_tuple->value->int32);
		settings.show_bluetooth_text = (int)show_bluetooth_text_tuple->value->int32;
		update_bluetooth_text();
		//APP_LOG(APP_LOG_LEVEL_INFO, "bluetooth text settings received!");
	}
	
	if (show_battery_text_tuple){
		persist_write_int(SHOW_BATTERY_TEXT_KEY, (int)show_battery_text_tuple->value->int32);
		settings.show_battery_text = (int)show_battery_text_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "battery text settings received!");
	}
	
	if (date_format_tuple){
		persist_write_int(DATE_FORMAT_KEY, (int)date_format_tuple->value->int32);
		settings.date_format = (int)date_format_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "date format settings received!");
	}
	
	if (date_style_tuple){
		persist_write_int(DATE_STYLE_KEY, (int)date_style_tuple->value->int32);
		settings.date_style = (int)date_style_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "date style settings received!");
	}
	
	if (time_text_size_tuple){
		persist_write_int(TIME_TEXT_SIZE_KEY, (int)time_text_size_tuple->value->int32);
		settings.time_text_size = (int)time_text_size_tuple->value->int32;
		
		//APP_LOG(APP_LOG_LEVEL_INFO, "text size settings received!");
	}
	
	if (window_color_tuple){
		persist_write_int(WINDOW_COLOR_KEY, (int)window_color_tuple->value->int32);
		settings.window_color = (int)window_color_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "window color settings received!");
	}
	
	if (language_tuple){
		persist_write_int(LANGUAGE_KEY, (int)language_tuple->value->int32);
		settings.language = (int)language_tuple->value->int32;
		update_date(get_system_time(), SECOND_UNIT);
		//APP_LOG(APP_LOG_LEVEL_INFO, "language settings received!");
	}
	
	if (blink_colon_tuple){
		persist_write_int(BLINK_COLON_KEY, (int)blink_colon_tuple->value->int32);
		settings.blink_colon = (int)blink_colon_tuple->value->int32;
		//APP_LOG(APP_LOG_LEVEL_INFO, "blink colon settings received!");
	}
	
	app_timer_cancel(is_receiving_data);
	if (settings.data_updates_frequency != NEVER_REQUEST_DATA){
		app_timer_register(MILLS_IN_HOUR / settings.data_updates_frequency, request_data_from_phone, 0);
	}
	
	flags.vibes_allowed = 1;
	subscribe_to_time_update_service();
	
	flags.is_unread_messages = 0;
	update_unread_message_icon();
}

inline void update_bar(){
	update_battery_icon();
	update_battery_text();
	
	update_bluetooth_icon();
	update_bluetooth_text();
}

inline void is_night(){
	if (!settings.night_mode_enabled){
		flags.is_night_now = 0;
		return;
	}
	
	tm *current_time = get_system_time();
	int current_time_minutes = (current_time -> tm_hour) * 60 + (current_time -> tm_min);
	
	if (settings.night_mode_started > settings.night_mode_finished){
		if ( (current_time_minutes >= settings.night_mode_started) || (current_time_minutes < settings.night_mode_finished) ){
			flags.is_night_now = 1;
			return;
		}
	}
	
	if (settings.night_mode_started < settings.night_mode_finished){
		if ( (current_time_minutes >= settings.night_mode_started) && (current_time_minutes < settings.night_mode_finished) ){
			flags.is_night_now = 1;
			return;
		}
	}
	
	flags.is_night_now = 0;
	
}

inline void read_persist_settings(){
	if (persist_exists(LANGUAGE_KEY)){
		settings.language = persist_read_int(LANGUAGE_KEY);
	} else {
		settings.language = ENGLISH_LANGUAGE;
	}
	
	if (persist_exists(NIGHT_MODE_BLINK_COLON_KEY)){
		settings.night_mode_blink_colon = persist_read_int(NIGHT_MODE_BLINK_COLON_KEY);
	} else {
		settings.night_mode_blink_colon = 0;
	}
	
	if (persist_exists(BLINK_COLON_KEY)){
		settings.blink_colon = persist_read_int(BLINK_COLON_KEY);
	} else {
		settings.blink_colon = 0;
	}
	
	if (persist_exists(WINDOW_COLOR_KEY)){
		settings.window_color = persist_read_int(WINDOW_COLOR_KEY);
	} else {
		settings.window_color = NORMAL_COLOR;
	}
	
	if (persist_exists(TIME_TEXT_SIZE_KEY)){
		settings.time_text_size = persist_read_int(TIME_TEXT_SIZE_KEY);
	} else {
		settings.time_text_size = SMALL_TIME_TEXT;
	}
	
	if (persist_exists(DATE_FORMAT_KEY)){
		settings.date_format = persist_read_int(DATE_FORMAT_KEY);
	} else {
		settings.date_format = DD_MMMM_DATE_FORMAT;
	}
	
	if (persist_exists(DATE_STYLE_KEY)){
		settings.date_style = persist_read_int(DATE_STYLE_KEY);
	} else {
		settings.date_style = WEEKDAY_BELOW_DATE_STYLE;
	}
	
	if (persist_exists(SHOW_LAST_DISCONNECT_TIME_KEY)){
		settings.show_last_disconnect_time = persist_read_int(SHOW_LAST_DISCONNECT_TIME_KEY);
	} else {
		settings.show_last_disconnect_time = 0;
	}
	
	if (persist_exists(SHOW_BATTERY_TEXT_KEY)){
		settings.show_battery_text = persist_read_int(SHOW_BATTERY_TEXT_KEY);
	} else {
		settings.show_battery_text = 1;
	}
	
	if (persist_exists(SHOW_BLUETOOTH_TEXT_KEY)){
		settings.show_bluetooth_text = persist_read_int(SHOW_BLUETOOTH_TEXT_KEY);
	} else {
		settings.show_bluetooth_text = 1;
	}
	
	if (persist_exists(VIBE_HOURLY_VIBE_KEY)){
		settings.vibe_hourly_vibe = persist_read_int(VIBE_HOURLY_VIBE_KEY);
	} else {
		settings.vibe_hourly_vibe = 1;
	}
	
	if (persist_exists(VIBE_BLUETOOTH_STATE_CHANGE_KEY)){
		settings.vibe_bluetooth_state_change = persist_read_int(VIBE_BLUETOOTH_STATE_CHANGE_KEY);
	} else {
		settings.vibe_bluetooth_state_change = 1;
	}
	
	if (persist_exists(NIGHT_MODE_ENABLED_KEY)){
		settings.night_mode_enabled = persist_read_int(NIGHT_MODE_ENABLED_KEY);
	} else {
		settings.night_mode_enabled = 1;
	}
		
	if (persist_exists(NIGHT_MODE_STARTED_KEY)){
		settings.night_mode_started = persist_read_int(NIGHT_MODE_STARTED_KEY);
	} else {
		settings.night_mode_started = 1;
	}
		
	if (persist_exists(NIGHT_MODE_FINISHED_KEY)){
		settings.night_mode_finished = persist_read_int(NIGHT_MODE_FINISHED_KEY);
	} else {
		settings.night_mode_finished = 1;
	}
	
	if (persist_exists(NIGHT_MODE_DISPLAY_INVERT_KEY)){
		settings.night_mode_display_invert = persist_read_int(NIGHT_MODE_DISPLAY_INVERT_KEY);
	} else {
		settings.night_mode_display_invert = 1;
	}
	
	if (persist_exists(NIGHT_MODE_UPDATE_INFO_KEY)){
		settings.night_mode_update_info = persist_read_int(NIGHT_MODE_UPDATE_INFO_KEY);
	} else {
		settings.night_mode_update_info = 1;
		persist_write_int(NIGHT_MODE_UPDATE_INFO_KEY, 1);
	}
		
	if (persist_exists(NIGHT_MODE_VIBE_ON_EVENT_KEY)){
		settings.night_mode_vibe_on_event = persist_read_int(NIGHT_MODE_VIBE_ON_EVENT_KEY);
	} else {
		settings.night_mode_vibe_on_event = 1;
	}
	
	if (persist_exists(NIGHT_MODE_VIBE_HOURLY_VIBE_KEY)){
		settings.night_mode_vibe_hourly_vibe = persist_read_int(NIGHT_MODE_VIBE_HOURLY_VIBE_KEY);
	} else {
		settings.night_mode_vibe_hourly_vibe = 1;
	}
	
	if (persist_exists(DATA_UPDATES_FREQUENCY_KEY)){
		settings.data_updates_frequency = persist_read_int(DATA_UPDATES_FREQUENCY_KEY);
	} else {
		settings.data_updates_frequency = 1;
	}
	

	if (persist_exists(TOP_ADDITIONAL_STRING_TEXT_KEY)){
		persist_read_string(TOP_ADDITIONAL_STRING_TEXT_KEY, top_additional_info_buffer, sizeof(top_additional_info_buffer));
	} else {
		strcpy(top_additional_info_buffer, "Smartface");
	}
		
	if (persist_exists(BOTTOM_ADDITIONAL_STRING_TEXT_KEY)){
		persist_read_string(BOTTOM_ADDITIONAL_STRING_TEXT_KEY, bottom_additional_info_buffer, sizeof(bottom_additional_info_buffer));
	} else {
		strcpy(bottom_additional_info_buffer, "GrakovNe");
	}
}

inline void update_time_routine(struct tm* current_time){
	is_night();
	
	if (!current_time-> tm_min){
		if ((flags.hourly_vibes_allowed) && (flags.vibes_allowed) && (settings.vibe_hourly_vibe) &&  (!flags.is_night_now || settings.night_mode_vibe_hourly_vibe) ){
			vibes_double_pulse();
			flags.hourly_vibes_allowed = 0;
		}
	} 
	
	else {
		flags.hourly_vibes_allowed = 1;
	}
			
	if ( flags.is_night_now && (settings.night_mode_display_invert) ){
		flags.current_window_color = !settings.window_color;
	} else {
		flags.current_window_color = settings.window_color;	
	}
	
	if (flags.is_state_changed == flags.is_night_now){
		return;
	}
	
	set_window_color(flags.current_window_color);
	update_bar();
	subscribe_to_time_update_service();
	flags.is_state_changed = flags.is_night_now;
}

void update_time_minutes(struct tm* current_time, TimeUnits units_changed){
	snprintf(time_text_buffer, sizeof(time_text_buffer), "%02d:%02d", current_time->tm_hour, current_time->tm_min);
	text_layer_set_text(time_text, time_text_buffer);
	
	if (!current_time -> tm_hour){
		update_date(current_time, SECOND_UNIT);
	}
	
	update_time_routine(current_time);
}

void update_time_seconds(struct tm* current_time, TimeUnits units_changed){
	static char delimiter[] = {':', ' '};
	
	snprintf(time_text_buffer, sizeof(time_text_buffer), "%02d%c%02d", current_time->tm_hour, delimiter[current_time -> tm_sec % 2], current_time->tm_min);
	text_layer_set_text(time_text, time_text_buffer);
	
	if (!current_time -> tm_hour){
		update_date(current_time, SECOND_UNIT);
	}
	
	if ( (!current_time -> tm_sec) || (flags.is_state_changed == STATE_WAS_CHANGED) ){
		update_time_routine(current_time);
	}
}

void update_date(struct tm* current_time, TimeUnits units_changed){
	static TextLayer * weekday_text_current;
	static TextLayer * date_text_current;
	
	switch (settings.date_style){
		case WEEKDAY_BELOW_DATE_STYLE:
		weekday_text_current = top_date_text;
		date_text_current = bottom_date_text;
	break;
		
		case WEEKDAY_ABOVE_DATE_STYLE:
		date_text_current = top_date_text;
		weekday_text_current = bottom_date_text;
	break;
		
	}
	
	switch(settings.date_format){
		case DD_MM_YYYY_DATE_FORMAT:
			snprintf(date_text_buffer, sizeof(date_text_buffer), "%02d.%02d.%d", current_time->tm_mday, current_time->tm_mon + 1, current_time->tm_year + 1900);
		break;
		
		case DD_MMMM_DATE_FORMAT:
			snprintf(date_text_buffer, sizeof(date_text_buffer), "%02d %s", current_time->tm_mday, month_names[settings.language][current_time->tm_mon]);
		break;
		
		case MM_DD_YYYY_DATE_FORMAT:
			snprintf(date_text_buffer, sizeof(date_text_buffer), "%02d/%02d/%04d", 1 + current_time->tm_mon, current_time->tm_mday, 1900 + current_time->tm_year);
		break;
	}
	
	text_layer_set_text(weekday_text_current, weekday_names[settings.language][current_time->tm_wday]);
	text_layer_set_text(date_text_current, date_text_buffer);
}

void update_bluetooth_text(){
	if (settings.show_bluetooth_text){
		if (settings.show_last_disconnect_time && !flags.is_bluetooth_connected){
			tm * current_time = get_system_time();
			snprintf(bluetooth_text_buffer, sizeof(bluetooth_text_buffer), "%02d:%02d", current_time -> tm_hour, current_time -> tm_min);
			text_layer_set_text(bluetooth_text, bluetooth_text_buffer);
		}
		
		else {
			text_layer_set_text(bluetooth_text, bluetooth_states_names[settings.language][flags.is_bluetooth_connected]);
		}
		
	} 
	
	else {
		text_layer_set_text(bluetooth_text, EMPTY_STRING);
	}
}

void update_bluetooth_icon(){
	gbitmap_destroy(bluetooth_icon); 
	bluetooth_icon = gbitmap_create_with_resource(bluetooth_icons[flags.current_window_color][flags.is_bluetooth_connected]); 
	bitmap_layer_set_bitmap(bluetooth_icon_layer, bluetooth_icon);
	layer_mark_dirty((Layer *)bluetooth_icon_layer);
}

void animate_battery_on_charge(){
	static int current_frame_number = 120;
	
	if (current_frame_number > 10){
		current_frame_number = flags.battery_percents / 10;
	}
	
	gbitmap_destroy(battery_icon); 
	battery_icon = gbitmap_create_with_resource(battery_icons[flags.current_window_color][current_frame_number]); 
	bitmap_layer_set_bitmap(battery_icon_layer, battery_icon);
	layer_mark_dirty((Layer *)battery_icon_layer);
	
	current_frame_number ++;
	
	if (flags.is_charging && (flags.battery_percents < 100)){
		app_timer_cancel(battery_animation_timer);
		battery_animation_timer = app_timer_register(BATTERY_ANIMATION_DELAY_MS, animate_battery_on_charge, 0);
		return;
	}	
	
	update_battery_state(battery_state_service_peek());
}

void update_battery_text(){
	if (settings.show_battery_text){
		snprintf(battery_text_buffer, sizeof(battery_text_buffer), "%d%%", flags.battery_percents);
		text_layer_set_text(battery_text, battery_text_buffer);
	} else {
		text_layer_set_text(battery_text, EMPTY_STRING);
	}
}

void update_battery_icon(){
	if ( flags.is_charging && (flags.battery_percents < 100) ){
		animate_battery_on_charge();
		return;
	}
	
	gbitmap_destroy(battery_icon); 
	if ( (flags.is_plugged) && (flags.battery_percents == 100)) {
		battery_icon = gbitmap_create_with_resource(battery_icons[flags.current_window_color][11]);
	} else {
		battery_icon = gbitmap_create_with_resource(battery_icons[flags.current_window_color][flags.battery_percents / 10]); 
	}
	
	bitmap_layer_set_bitmap(battery_icon_layer, battery_icon);
	layer_mark_dirty((Layer *)battery_icon_layer);
}

void update_unread_message_icon(){
	if (!flags.is_unread_messages){
		gbitmap_destroy(unread_message_icon);
		bitmap_layer_set_bitmap(unread_message_layer, unread_message_icon);
		layer_mark_dirty((Layer *)unread_message_layer);
		return;
	}
	
	unread_message_icon = gbitmap_create_with_resource(unread_message_icons[flags.current_window_color]);
	bitmap_layer_set_bitmap(unread_message_layer, unread_message_icon);
	layer_mark_dirty((Layer *)unread_message_layer);
}

void update_battery_state(BatteryChargeState battery_state){
	flags.is_charging = battery_state.is_charging;
	flags.battery_percents = battery_state.charge_percent;
	flags.is_plugged = battery_state.is_plugged;
	
	update_battery_text();
	update_battery_icon();
}

void update_bluetooth_connection(bool is_connected){
	flags.is_bluetooth_connected = is_connected;
	
	if ((flags.vibes_allowed) && settings.vibe_bluetooth_state_change && (!flags.is_night_now || settings.night_mode_vibe_on_event)){
		vibes_long_pulse();
	}
	
	if (!flags.is_bluetooth_connected){
		app_timer_cancel(is_receiving_data);
	}
		
	update_bluetooth_icon();
	update_bluetooth_text();
}

inline void update_additional_info(){
	text_layer_set_text(top_additional_info_text, top_additional_info_buffer);
	text_layer_set_text(bottom_additional_info_text, bottom_additional_info_buffer);
}

inline void subscribe_to_time_update_service(){
	if ( (settings.blink_colon) && (!flags.is_night_now || settings.night_mode_blink_colon) ) {
		tick_timer_service_subscribe(SECOND_UNIT, &update_time_seconds);
	}
	else {
		tick_timer_service_subscribe(MINUTE_UNIT, &update_time_minutes);
	}
}

void initialization(void) {
	flags.is_state_changed = STATE_WAS_CHANGED;
	
	/*Reading settings*/
	read_persist_settings();
	
	/*Creating main window*/
	create_window(settings.time_text_size, settings.window_color);
	
	/*Updating time and date*/
	tm* current_time = get_system_time();
    update_time_minutes(current_time, SECOND_UNIT);
	update_date(current_time, SECOND_UNIT);
	
	/*Showing main window*/
	window_stack_push(main_window, false);
	
	/*Showing window and updating info*/
	update_bluetooth_connection(connection_service_peek_pebble_app_connection());
	update_battery_state(battery_state_service_peek());
	update_additional_info();
	
	/*Subscribing for a timers and events*/
	
	subscribe_to_time_update_service();
	bluetooth_connection_service_subscribe(update_bluetooth_connection);
	battery_state_service_subscribe(update_battery_state);
		
	app_timer_register(3000, request_data_from_phone, 0);
	
	/*open connection with a phone*/
	app_message_register_inbox_received(inbox_received_callback);
	//app_message_register_outbox_failed(outbox_failed_callback);
	
	app_message_open(256, 64);
	flags.vibes_allowed = 1;
	
	/*debug part*/
	flags.is_unread_messages = 1;
	update_unread_message_icon();
}

int main(void) {
	initialization();
	app_event_loop();
	destroy_window();
}
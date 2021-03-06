#include <pebble.h>

#define MILLS_IN_HOUR               3600000

#define RECEIVING_LATENCY_TIME             30000

#define SECOND_SCREEN_EMPTY 0 

#define ENGLISH_LANGUAGE 0
#define RUSSIAN_LANGUAGE 1

#define DD_MM_YYYY_DATE_FORMAT 0
#define DD_MMMM_DATE_FORMAT 1
#define MM_DD_YYYY_DATE_FORMAT 2

#define WEEKDAY_ABOVE_DATE_STYLE 0
#define WEEKDAY_BELOW_DATE_STYLE 1

#define SMALL_TIME_TEXT 0
#define BIG_TIME_TEXT 1

#define NORMAL_COLOR 0
#define INVERTED_COLOR 1

#define EMPTY_STRING ""

#define BATTERY_ANIMATION_DELAY_MS 800

#define LANGUAGE_KEY                       0
#define WINDOW_COLOR_KEY                   1
#define TIME_TEXT_SIZE_KEY                 2
#define DATE_FORMAT_KEY                    3
#define SHOW_BATTERY_TEXT_KEY              4
#define SHOW_BLUETOOTH_TEXT_KEY            5
#define SHOW_TOP_ADDITIONAL_INFO_KEY       6
#define SHOW_BOTTOM_ADDITIONAL_INFO_KEY    7
#define VIBE_HOURLY_VIBE_KEY               8
#define VIBE_BLUETOOTH_STATE_CHANGE_KEY    9
#define NIGHT_MODE_ENABLED_KEY             10
#define NIGHT_MODE_STARTED_KEY             11
#define NIGHT_MODE_FINISHED_KEY            12
#define TOP_ADDITIONAL_STRING_TEXT_KEY     13
#define BOTTOM_ADDITIONAL_STRING_TEXT_KEY  14
#define NIGHT_MODE_DISPLAY_INVERT_KEY      15
#define NIGHT_MODE_UPDATE_INFO_KEY         16
#define NIGHT_MODE_VIBE_ON_EVENT_KEY       17
#define DATA_UPDATES_FREQUENCY_KEY         18
#define NIGHT_MODE_VIBE_HOURLY_VIBE_KEY    19
#define DATE_STYLE_KEY                     20
#define SHOW_LAST_DISCONNECT_TIME_KEY      21
#define BLINK_COLON_KEY                    22
#define NIGHT_MODE_BLINK_COLON_KEY         23

#define STATE_WAS_CHANGED                  30
#define NEVER_REQUEST_DATA                 30

static const uint32_t battery_icons[2][12] = {
	{
		RESOURCE_ID_BATTERY_0_NORMAL, 
		RESOURCE_ID_BATTERY_10_NORMAL, 
		RESOURCE_ID_BATTERY_20_NORMAL, 
		RESOURCE_ID_BATTERY_30_NORMAL, 
		RESOURCE_ID_BATTERY_40_NORMAL, 
		RESOURCE_ID_BATTERY_50_NORMAL, 
		RESOURCE_ID_BATTERY_60_NORMAL, 
		RESOURCE_ID_BATTERY_70_NORMAL, 
		RESOURCE_ID_BATTERY_80_NORMAL, 
		RESOURCE_ID_BATTERY_90_NORMAL,
		RESOURCE_ID_BATTERY_100_NORMAL,
		RESOURCE_ID_BATTERY_FULL_NORMAL
	},
	{
		RESOURCE_ID_BATTERY_0_INVERTED, 
		RESOURCE_ID_BATTERY_10_INVERTED, 
		RESOURCE_ID_BATTERY_20_INVERTED, 
		RESOURCE_ID_BATTERY_30_INVERTED, 
		RESOURCE_ID_BATTERY_40_INVERTED, 
		RESOURCE_ID_BATTERY_50_INVERTED, 
		RESOURCE_ID_BATTERY_60_INVERTED, 
		RESOURCE_ID_BATTERY_70_INVERTED, 
		RESOURCE_ID_BATTERY_80_INVERTED, 
		RESOURCE_ID_BATTERY_90_INVERTED,
		RESOURCE_ID_BATTERY_100_INVERTED,
		RESOURCE_ID_BATTERY_FULL_INVERTED
	}
};

static const uint32_t updating_icons[2][2] = {
	{
		RESOURCE_ID_UPDATING_NOW_NORMAL,
		RESOURCE_ID_UPDATING_ERR_NORMAL
	},
	{
		RESOURCE_ID_UPDATING_NOW_INVERTED,
		RESOURCE_ID_UPDATING_ERR_INVERTED
	}
};

static const uint32_t bluetooth_icons[2][2] = {
	{
		RESOURCE_ID_BLUETOOTH_NO_NORMAL,
		RESOURCE_ID_BLUETOOTH_YES_NORMAL
	},
	{
		RESOURCE_ID_BLUETOOTH_NO_INVERTED,
		RESOURCE_ID_BLUETOOTH_YES_INVERTED
	}
};

static const uint32_t unread_message_icons[2] = {
	RESOURCE_ID_UNREAD_MESSAGE_ICON_NORMAL,
	RESOURCE_ID_UNREAD_MESSAGE_ICON_INVERTED
};

static const char weekday_names[3][7][24] = {
	{
		"SUNDAY",
		"MONDAY",
		"TUESDAY",
		"WEDNESDAY",
		"THURSDAY",
		"FRIDAY",
		"SATURDAY"
	},
	{
		"ВОСКРЕСЕНЬЕ",
		"ПОНЕДЕЛЬНИК",
		"ВТОРНИК",
		"СРЕДА",
		"ЧЕТВЕРГ",
		"ПЯТНИЦА",
		"СУББОТА"
	},	
	{ 
		"SONNTAG", 
		"MONTAG", 
		"DIENSTAG", 
		"MITTWOCH", 
		"DONNERSTAG", 
		"FREITAG", 
		"SAMSTAG" 
	}
};

static const char month_names[3][12][24] = {
	{
		"JANUARY",
		"FEBRUARY",
		"MARCH",
		"APRIL",
		"MAY",
		"JUNE",
		"JULE",
		"AUGUST",
		"SEPTEMBER",
		"OCTOBER",
		"NOVEMBER",
		"DECEMBER"
	},
	{
		"ЯНВАРЯ",
		"ФЕВРАЛЯ",
		"МАРТА",
		"АПРЕЛЯ",
		"МАЯ",
		"ИЮНЯ",
		"ИЮЛЯ",
		"АВГУСТА",
		"СЕНТЯБРЯ",
		"ОКТЯБРЯ",
		"НОЯБРЯ",
		"ДЕКАБРЯ"
	},
		
	{ 
		"JANUAR", 
		"FEBRUAR", 
		"MÄRZ", 
		"APRIL", 
		"MAI", 
		"JUNI", 
		"JULI", 
		"AUGUST", 
		"SEPTEMBER", 
		"OKTOBER", 
		"NOVEMBER", 
		"DEZEMBER" 
	}
};

static const char offline_text_names[3][16] = {
	"OFFLINE",
	"HET CETИ",
	"OFFLINE"
};

static const char bluetooth_states_names[3][2][7] = {
	{
		"LOST",
		"OK"
	},
	{
		"HET",
		"OK"
	},
	{
		"NEIN",
		"OK"
	}
};

enum {
	LANGUAGE_INFO                         = 0,
	WINDOW_COLOR_INFO                     = 1,
	TIME_TEXT_SIZE_INFO                   = 2,
	DATE_FORMAT_INFO                      = 3,
	SHOW_BATTERY_TEXT_INFO                = 4,
	SHOW_BLUETOOTH_TEXT_INFO              = 5,
	VIBE_HOURLY_VIBE_INFO                 = 6,
	VIBE_BLUETOOTH_STATE_CHANGE_INFO      = 7,
	NIGHT_MODE_ENABLED_INFO               = 8,
	NIGHT_MODE_STARTED_INFO               = 9,
	NIGTH_MODE_FINISHED_INFO              = 10,
	TOP_ADDITIONAL_STRING_TEXT_INFO       = 11,
	BOTTOM_ADDITIONAL_STRING_TEXT_INFO    = 12,
	NIGTH_MODE_DISPLAY_INVERT_INFO        = 13,
	NIGHT_MODE_UPDATE_INFO_INFO           = 14,
	NIGTH_MODE_VIBE_ON_EVENT_INFO         = 15,
	DATA_UPDATE_FREQUENCY_INFO			  = 16,
	NIGHT_MODE_HOURLY_VIBE_INFO           = 17,
	DATE_STYLE_INFO                       = 18,
	SHOW_LAST_DISCONNECT_TIME_INFO        = 19,
	BLINK_COLON_INFO                      = 20,
	NIGHT_MODE_BLINK_COLON_INFO           = 21,
	ASK_DATA_FROM_PHONE_INFO              = 99,
};


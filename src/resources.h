#include <pebble.h>

#define ENGLISH_LANGUAGE 0
#define RUSSIAN_LANGUAGE 1

#define DD_MM_YYYY_DATE_FORMAT 0
#define DD_MMMM_DATE_FORMAT 1

#define SMALL_TIME_TEXT 0
#define BIG_TIME_TEXT 1

#define NORMAL_COLOR 0
#define INVERTED_COLOR 1

#define EMPTY_STRING ""

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

static const char weekday_names[2][7][24] = {
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
		"BOCKPECEHЬE",
		"ПOHEДEЛЬHИK",
		"BTOPHИK",
		"CPEДA",
		"ЧETBЕPГ",
		"ПЯTHИЦA",
		"CYББOTA"
	}
};

static const char month_names[2][12][24] = {
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
		
	}
};

static const char offline_text_names[2][16] = {
	"OFFLINE",
	"HET CETИ"
};

static const char bluetooth_states_names[2][2][7] = {
	{
		"LOST",
		"OK"
	},
	{
		"HET",
		"OK"
	}
};

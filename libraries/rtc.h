#ifndef RTC_H_
#define RTC_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32g0xx.h"

//--------------------------------------------------------------------------------------------------------------------------------

#define RTC_INTERRUPT_LEVEL 3
#define RTC_SECONDS_IN_WEEK 604800
#define RTC_SECONDS_IN_DAY 86400
#define RTC_SECONDS_IN_HOUR 3600
#define RTC_SECONDS_IN_MINUTE 60

//--------------------------------------------------------------------------------------------------------------------------------

#define RTC_MONDAY 1
#define RTC_TUESDAY 2
#define RTC_WEDNESDAY 3
#define RTC_THURSDAY 4
#define RTC_FRIDAY 5
#define RTC_SATURDAY 6
#define RTC_SUNDAY 7

//--------------------------------------------------------------------------------------------------------------------------------

typedef enum {
  ALARM_A = 0,
  ALARM_B = 1
} ALARM_e;

//--------------------------------------------------------------------------------------------------------------------------------

typedef struct {
  uint8_t year;
  uint8_t month;
  uint8_t month_day;
  uint8_t week_day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint32_t ms;
} RTC_Datetime_t;

typedef struct {
  bool week;
  bool day_mask;
  uint8_t day;
  bool hour_mask;
  uint8_t hour;
  bool minute_mask;
  uint8_t minute;
  bool second_mask;
  uint8_t second;
} RTC_Alarm_t;

//--------------------------------------------------------------------------------------------------------------------------------

void RTC_Init(void);

RTC_Datetime_t RTC_UnixToDatetime(uint32_t timestamp);
RTC_Datetime_t RTC_TimestampToDatetime(uint64_t timestamp);
char *RTC_WeekDayString(void);
uint32_t RTC_DatetimeToUnix(RTC_Datetime_t *date);

RTC_Alarm_t RTC_DaystampToAlarm(uint32_t stamp);
RTC_Alarm_t RTC_WeekstampToAlarm(uint32_t stamp);
uint32_t RTC_AlarmToDaystamp(RTC_Alarm_t *alarm);
uint32_t RTC_AlarmToWeekstamp(RTC_Alarm_t *alarm);

bool RTC_DatetimeSyncIsCorrect(RTC_Datetime_t *date, int8_t time_zone);
void RTC_SetDatetime(RTC_Datetime_t *datetime);
void RTC_SetTimestamp(uint32_t timestamp);
void RTC_Reset(void);

RTC_Datetime_t RTC_Datetime(void);
uint32_t RTC_Timestamp(void);
uint64_t RTC_TimestampMs(void);
uint32_t RTC_Daystamp(void);
uint32_t RTC_Weekstamp(void);

RTC_Alarm_t RTC_Alarm(ALARM_e alarm);
RTC_Alarm_t RTC_Alarm_A(void);
uint32_t RTC_Daystamp_A(void);
RTC_Alarm_t RTC_Alarm_B(void);
uint32_t RTC_Daystamp_B(void);

void RTC_Alarm_Enable(ALARM_e alarm, RTC_Alarm_t *set);
void RTC_Alarm_A_Enable(RTC_Alarm_t *set);
void RTC_Daystamp_A_Enable(uint32_t stamp);
void RTC_Weekstamp_A_Enable(uint32_t stamp);
void RTC_Interval_A_Enable(uint32_t interval);
void RTC_Alarm_B_Enable(RTC_Alarm_t *set);
void RTC_Daystamp_B_Enable(uint32_t stamp);
void RTC_Weekstamp_B_Enable(uint32_t stamp);
void RTC_Interval_B_Enable(uint32_t interval);

void RTC_AlarmA_Disable(void);
void RTC_AlarmB_Disable(void);
void RTC_WakeupTimer_Enable(uint32_t sec);
void RTC_WakeupTimer_Disable(void);

bool RTC_Check_Daystamp(uint32_t stamp_alarm, uint32_t offset_min_sec, uint32_t offset_max_sec);
bool RTC_Check_Weekstamp(uint32_t stamp_alarm, uint32_t offset_min_sec, uint32_t offset_max_sec);
bool RTC_Check_A(uint32_t offset_min_sec, uint32_t offset_max_sec);
bool RTC_Check_B(uint32_t offset_min_sec, uint32_t offset_max_sec);

bool RTC_Event_A(void);
bool RTC_Event_B(void);
bool RTC_Event_WakeupTimer(void);

void RTC_Force_A(void);
void RTC_Force_B(void);
void RTC_Force_WakeupTimer(void);

//--------------------------------------------------------------------------------------------------------------------------------

extern const char *rtc_weak_day_string[];
extern bool rtc_ready;

//--------------------------------------------------------------------------------------------------------------------------------
#endif

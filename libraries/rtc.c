#include "rtc.h"

//--------------------------------------------------------------------------------------------------------------------- DEFINE

#define RTC_LSE_FREQUENCY 32768
#define PREDIV_S 255
#define PREDIV_A 127
#define RTC_LEAP_YEAR(year) ((((year) % 4 == 0) && ((year) % 100 != 0)) || ((year) % 400 == 0))
#define RTC_DAYS_IN_YEAR(x) RTC_LEAP_YEAR(x) ? 366 : 365
#define RTC_OFFSET_YEAR 1970

//--------------------------------------------------------------------------------------------------------------------- VAR

volatile bool _alarm_flag[2] = { false, false };
volatile bool _wakeup_timer_flag = false;

const uint8_t RTC_DAYS_IN_MONTH[2][12] = {
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },	// Not leap year
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }	// Leap year
};

const uint32_t alarm_mask[2] = { RTC_CR_ALRAE, RTC_CR_ALRBE };
const char *rtc_weak_day_string[8] = { "\0", "Mon\0", "Tue\0", "Wed\0", "Thu\0", "Fri\0", "Sat\0", "Sun\0" };
// const char *rtc_weak_day_string[8] = { "\0", "Monday\0", "Tuesday\0", "Wednesday\0", "Thursday\0", "Friday\0", "Saturday\0", "Sunday\0" };
bool rtc_ready;

//--------------------------------------------------------------------------------------------------------------------- INIT

void RTC_Init(void)
{
	RCC->APBENR1 |= RCC_APBENR1_PWREN;
	RCC->APBENR1 |= RCC_APBENR1_RTCAPBEN;
	RCC->BDCR |= RCC_BDCR_BDRST;
	RCC->BDCR &= ~RCC_BDCR_BDRST;
	PWR->CR1 |= PWR_CR1_DBP;
	RCC->BDCR |= RCC_BDCR_LSEON;
	while(!(RCC->BDCR & RCC_BDCR_LSERDY)) __DSB();
	RCC->BDCR |= RCC_BDCR_RTCSEL_0;
	RCC->BDCR |= RCC_BDCR_RTCEN;
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
	RTC->CR |= (RTC_CR_TSIE | RTC_CR_WUTIE | RTC_CR_ALRBIE | RTC_CR_ALRAIE);
	RTC->WPR = 0xFF;
	NVIC_ClearPendingIRQ(RTC_TAMP_IRQn);
	NVIC_EnableIRQ(RTC_TAMP_IRQn);
	NVIC_SetPriority(RTC_TAMP_IRQn, RTC_INTERRUPT_LEVEL);
}

//--------------------------------------------------------------------------------------------------------------------- Convert

RTC_Datetime_t RTC_UnixToDatetime(uint32_t timestamp)
{
  RTC_Datetime_t date_result;

  uint16_t year;
  uint8_t seconds, minutes, hours, month, month_day, week_day;

  seconds = timestamp % 60;
  timestamp /= 60;
  minutes = timestamp % 60;
  timestamp /= 60;
  hours = timestamp % 24;
  timestamp /= 24;
  week_day = (timestamp + 3) % 7 + 1;

  year = RTC_OFFSET_YEAR;
  while(1) {
    if(RTC_LEAP_YEAR(year)) {
      if(timestamp >= 366) timestamp -= 366;
      else break;
    } else if(timestamp >= 365) timestamp -= 365;
    else break;
    year++;
  }

  for(month = 0; month < 12; month++) {
    if(timestamp >= (uint32_t)RTC_DAYS_IN_MONTH[RTC_LEAP_YEAR(year)][month]) timestamp -=
        RTC_DAYS_IN_MONTH[RTC_LEAP_YEAR(year)][month];
    else break;
  }

  month++;
  month_day = timestamp + 1;

  if(year < 2000) year = 2000;
  date_result.year = year % 100;
  date_result.month = month;
  date_result.month_day = month_day;
  date_result.week_day = week_day;
  date_result.hour = hours;
  date_result.minute = minutes;
  date_result.second = seconds;
  date_result.ms = 0;

  return date_result;
}

RTC_Datetime_t RTC_TimestampToDatetime(uint64_t timestamp)
{
  RTC_Datetime_t datetime = RTC_UnixToDatetime(timestamp / 1000);
  datetime.ms = timestamp & 1000;
  return datetime;
}

static uint8_t _RTC_WeekDay(RTC_Datetime_t *datatime)
{
  uint32_t timestamp = RTC_DatetimeToUnix(datatime);
  datatime->week_day = (timestamp / RTC_SECONDS_IN_DAY + 3) % 7 + 1;
  return datatime->week_day;
}

char *RTC_WeekDayString(void)
{
  return (char*)rtc_weak_day_string[RTC_Datetime().week_day];
}

bool RTC_DatetimeSyncIsCorrect(RTC_Datetime_t *date, int8_t time_zone)
{
  bool update = true;
  if(rtc_ready) {
    uint32_t now = RTC_Timestamp();
    uint32_t new = RTC_DatetimeToUnix(date) - ((time_zone * 15) * 60);
    uint32_t diff = (uint32_t)abs((int64_t)now - new);
    if(diff > 3600) update = false;
  }
  return update &&
    date->year >= 22 &&
    date->second >= 0 && date->second < 60 &&
    date->hour >= 0 && date->hour <= 23 &&
    date->month > 0 && date->month <= 12 &&
    date->month_day > 0 && date->month_day <= 31;
}

uint32_t RTC_DatetimeToUnix(RTC_Datetime_t *date)
{
  uint32_t days = 0, seconds = 0;
  uint16_t year = (uint16_t)(date->year + 2000);

  if(year < RTC_OFFSET_YEAR) return 0;
  for(uint16_t i = RTC_OFFSET_YEAR; i < year; i++) days += RTC_DAYS_IN_YEAR(i);
  for(uint16_t i = 1; i < date->month; i++) days += RTC_DAYS_IN_MONTH[RTC_LEAP_YEAR(year)][i - 1];

  days += date->month_day - 1;
  seconds = days * RTC_SECONDS_IN_DAY;
  seconds += date->hour * RTC_SECONDS_IN_HOUR;
  seconds += date->minute * RTC_SECONDS_IN_MINUTE;
  seconds += date->second;
  return seconds;
}

//--------------------------------------------------------------------------------------------------------------------- CONV-ALARM

RTC_Alarm_t RTC_DaystampToAlarm(uint32_t stamp)
{
  RTC_Alarm_t alarm;

  alarm.day = 0;
  alarm.day_mask = 1;

  stamp %= RTC_SECONDS_IN_DAY;
  alarm.hour = stamp / RTC_SECONDS_IN_HOUR;
  alarm.hour_mask = 0;

  stamp %= RTC_SECONDS_IN_HOUR;
  alarm.minute = stamp / RTC_SECONDS_IN_MINUTE;
  alarm.minute_mask = 0;

  stamp %= RTC_SECONDS_IN_MINUTE;
  alarm.second = stamp;
  alarm.second_mask = 0;

  return alarm;
}

RTC_Alarm_t RTC_WeekstampToAlarm(uint32_t stamp)
{
  RTC_Alarm_t alarm;
  alarm.week = true;

  stamp %= RTC_SECONDS_IN_WEEK;
  alarm.day = (stamp / RTC_SECONDS_IN_DAY) + 1;
  alarm.day_mask = 0;

  stamp %= RTC_SECONDS_IN_DAY;
  alarm.hour = stamp / RTC_SECONDS_IN_HOUR;
  alarm.hour_mask = 0;

  stamp %= RTC_SECONDS_IN_HOUR;
  alarm.minute = stamp / RTC_SECONDS_IN_MINUTE;
  alarm.minute_mask = 0;

  stamp %= RTC_SECONDS_IN_MINUTE;
  alarm.second = stamp;
  alarm.second_mask = 0;

  return alarm;
}

uint32_t RTC_AlarmToDaystamp(RTC_Alarm_t *alarm)
{
  return (alarm->hour * RTC_SECONDS_IN_HOUR) + (alarm->minute * RTC_SECONDS_IN_MINUTE) + alarm->second;
}

uint32_t RTC_AlarmToWeekstamp(RTC_Alarm_t *alarm)
{
  uint32_t weekstamp = RTC_AlarmToDaystamp(alarm);
  if(!alarm->day_mask) weekstamp += (alarm->day - 1) * RTC_SECONDS_IN_DAY;
  return weekstamp;
}

//--------------------------------------------------------------------------------------------------------------------- SET

static uint32_t _RTC_DateRegister(RTC_Datetime_t *date)
{
  if(date->year >= 22) rtc_ready = true;

  uint32_t dr = 0;
  dr |= (date->year / 10) << RTC_DR_YT_Pos;
  dr |= (date->year % 10) << RTC_DR_YU_Pos;
  dr |= (date->week_day) << RTC_DR_WDU_Pos;
  dr |= (date->month / 10) << RTC_DR_MT_Pos;
  dr |= (date->month % 10) << RTC_DR_MU_Pos;
  dr |= (date->month_day / 10) << RTC_DR_DT_Pos;
  dr |= (date->month_day % 10) << RTC_DR_DU_Pos;
  return dr;
}

static uint32_t _RTC_TimeRegister(RTC_Datetime_t *date)
{
  uint32_t tr = 0;
  tr |= (date->hour / 10) << RTC_TR_HT_Pos;
  tr |= (date->hour % 10) << RTC_TR_HU_Pos;
  tr |= (date->minute / 10) << RTC_TR_MNT_Pos;
  tr |= (date->minute % 10) << RTC_TR_MNU_Pos;
  tr |= (date->second / 10) << RTC_TR_ST_Pos;
  tr |= (date->second % 10) << RTC_TR_SU_Pos;
  return tr;
}

void RTC_SetDatetime(RTC_Datetime_t *datetime)
{
  _RTC_WeekDay(datetime);
	uint32_t tr = _RTC_TimeRegister(datetime);
	uint32_t dr = _RTC_DateRegister(datetime);

	RTC->WPR = 0xCA;
	RTC->WPR = 0x53; // Write access ON
	RTC->ICSR |= RTC_ICSR_INIT; // Init Start
	while(!(RTC->ICSR & RTC_ICSR_INITF)) __DSB();

	RTC->TR = tr;
	RTC->DR = dr;
	RTC->ICSR &= ~RTC_ICSR_INIT; // Init Stop
	while((RTC->ICSR & RTC_ICSR_RSF) == 0);

	RTC->WPR = 0xFF; // Write access OFF
}

void RTC_SetTimestamp(uint32_t timestamp)
{
  RTC_Datetime_t date = RTC_UnixToDatetime(timestamp);
  RTC_SetDatetime(&date);
}

void RTC_Reset(void)
{
  RTC_Datetime_t date = { .year = 20, .month = 1, .month_day = 1, .hour = 0, .minute = 0, .second = 0 };
  RTC_SetDatetime(&date);
  rtc_ready = false;
}

//--------------------------------------------------------------------------------------------------------------------- GET

RTC_Datetime_t RTC_Datetime(void)
{
  RTC_Datetime_t datetime;

  RTC->ICSR &= ~RTC_ICSR_RSF;
  while(!(RTC->ICSR & RTC_ICSR_RSF)) __DSB();

  uint32_t subsecund = RTC->SSR;
  uint32_t time = RTC->TR;
  uint32_t date = RTC->DR;

  datetime.year = ((date & RTC_DR_YT) >> RTC_DR_YT_Pos) * 10;
  datetime.year += ((date & RTC_DR_YU) >> RTC_DR_YU_Pos);
  if(datetime.year >= 22) rtc_ready = true;

  datetime.week_day = ((date & RTC_DR_WDU) >> RTC_DR_WDU_Pos);

  datetime.month = ((date & RTC_DR_MT) >> RTC_DR_MT_Pos) * 10;
  datetime.month += ((date & RTC_DR_MU) >> RTC_DR_MU_Pos);

  datetime.month_day = ((date & RTC_DR_DT) >> RTC_DR_DT_Pos) * 10;
  datetime.month_day += ((date & RTC_DR_DU) >> RTC_DR_DU_Pos);

  datetime.hour = ((time & RTC_TR_HT) >> RTC_TR_HT_Pos) * 10;
  datetime.hour += ((time & RTC_TR_HU) >> RTC_TR_HU_Pos);

  datetime.minute = ((time & RTC_TR_MNT) >> RTC_TR_MNT_Pos) * 10;
  datetime.minute += ((time & RTC_TR_MNU) >> RTC_TR_MNU_Pos);

  datetime.second = ((time & RTC_TR_ST) >> RTC_TR_ST_Pos) * 10;
  datetime.second += ((time & RTC_TR_SU) >> RTC_TR_SU_Pos);

  datetime.ms = ((PREDIV_S - subsecund) * ((PREDIV_A + 1) * 10000) / (RTC_LSE_FREQUENCY)) / 10;

  return datetime;
}

uint32_t RTC_Timestamp(void)
{
  RTC_Datetime_t date = RTC_Datetime();
  return RTC_DatetimeToUnix(&date);
}

uint64_t RTC_TimestampMs(void)
{
  RTC_Datetime_t date = RTC_Datetime();
  return (1000 * (uint64_t)RTC_DatetimeToUnix(&date)) + date.ms;
}

uint32_t RTC_Daystamp(void)
{
  return RTC_Timestamp() % RTC_SECONDS_IN_DAY;
}

uint32_t RTC_Weekstamp(void)
{
  return (3 * RTC_SECONDS_IN_DAY + RTC_Timestamp()) % RTC_SECONDS_IN_WEEK;
}

//--------------------------------------------------------------------------------------------------------------------- ALARM-GET

RTC_Alarm_t RTC_Alarm(ALARM_e alarm)
{
  RTC_Alarm_t out;

  uint32_t reg = *((uint32_t *)(RTC_BASE + 0x40 + (alarm * 8)));

  if(reg & RTC_ALRMAR_MSK4) out.day_mask = true;
  else out.day_mask = false;
  if(reg & RTC_ALRMAR_MSK3) out.hour_mask = true;
  else out.hour_mask = false;
  if(reg & RTC_ALRMAR_MSK2) out.minute_mask = true;
  else out.minute_mask = false;
  if(reg & RTC_ALRMAR_MSK1) out.second_mask = true;
  else out.second_mask = false;

  out.day = (((reg >> RTC_ALRMAR_DT_Pos) & 0x03) * 10) + ((reg >> RTC_ALRMAR_DU_Pos) & 0x0F);
  out.hour = (((reg >> RTC_ALRMAR_HT_Pos) & 0x03) * 10) + ((reg >> RTC_ALRMAR_HU_Pos) & 0x0F);
  out.minute = (((reg >> RTC_ALRMAR_MNT_Pos) & 0x07) * 10) + ((reg >> RTC_ALRMAR_MNU_Pos) & 0x0F);
  out.second = (((reg >> RTC_ALRMAR_ST_Pos) & 0x07) * 10) + ((reg >> RTC_ALRMAR_SU_Pos) & 0x0F);

  return out;
}

RTC_Alarm_t RTC_Alarm_A(void)
{
  return RTC_Alarm(ALARM_A);
}

uint32_t RTC_Daystamp_A(void)
{
  RTC_Alarm_t alarm = RTC_Alarm_A();
  return RTC_AlarmToDaystamp(&alarm);
}

RTC_Alarm_t RTC_Alarm_B(void)
{
  return RTC_Alarm(ALARM_B);
}

uint32_t RTC_Daystamp_B(void)
{
  RTC_Alarm_t alarm = RTC_Alarm_B();
  return RTC_AlarmToDaystamp(&alarm);
}

//--------------------------------------------------------------------------------------------------------------------- ALARM-ON-OFF

static bool _RTC_Alarm_IsEnable(ALARM_e alarm)
{
  if(RTC->CR & alarm_mask[alarm]) return true;
  else return false;
}

void RTC_Alarm_Enable(ALARM_e alarm, RTC_Alarm_t *set)
{
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53; // Write access ON

  RTC->CR &= ~alarm_mask[alarm];
  while(!(RTC->ICSR & RTC_ICSR_ALRAWF)) __DSB();

  *((uint32_t *)(RTC_BASE + 0x40 + (alarm * 8))) =
     (set->week         << RTC_ALRMAR_WDSEL_Pos) |
     (set->day_mask     << RTC_ALRMAR_MSK4_Pos)  |
     (set->hour_mask    << RTC_ALRMAR_MSK3_Pos)  |
     (set->minute_mask  << RTC_ALRMAR_MSK2_Pos)  |
     (set->second_mask  << RTC_ALRMAR_MSK1_Pos)  |
    ((set->day / 10)    << RTC_ALRMAR_DT_Pos)    |
    ((set->day % 10)    << RTC_ALRMAR_DU_Pos)    |
    ((set->hour / 10)   << RTC_ALRMAR_HT_Pos)    |
    ((set->hour % 10)   << RTC_ALRMAR_HU_Pos)    |
    ((set->minute / 10) << RTC_ALRMAR_MNT_Pos)   |
    ((set->minute % 10) << RTC_ALRMAR_MNU_Pos)   |
    ((set->second / 10) << RTC_ALRMAR_ST_Pos)    |
    ((set->second % 10) << RTC_ALRMAR_SU_Pos)    ;

  RTC->CR |= alarm_mask[alarm]; // Enable alarm
  RTC->WPR = 0xFF; // Write access OFF
}

void RTC_Alarm_A_Enable(RTC_Alarm_t *set)
{
  RTC_Alarm_Enable(ALARM_A, set);
}

void RTC_Daystamp_A_Enable(uint32_t stamp)
{
  RTC_Alarm_t alarm = RTC_DaystampToAlarm(stamp);
  RTC_Alarm_A_Enable(&alarm);
}

void RTC_Weekstamp_A_Enable(uint32_t stamp)
{
  RTC_Alarm_t alarm = RTC_WeekstampToAlarm(stamp);
  RTC_Alarm_A_Enable(&alarm);
}

void RTC_Interval_A_Enable(uint32_t interval)
{
  uint32_t stamp = RTC_Timestamp() + interval;
  RTC_Daystamp_A_Enable(stamp);
}

void RTC_Alarm_B_Enable(RTC_Alarm_t *set)
{
  RTC_Alarm_Enable(ALARM_B, set);
}

void RTC_Daystamp_B_Enable(uint32_t stamp)
{
  RTC_Alarm_t alarm = RTC_DaystampToAlarm(stamp);
  RTC_Alarm_B_Enable(&alarm);
}

void RTC_Interval_B_Enable(uint32_t interval)
{
  uint32_t stamp = RTC_Timestamp() + interval;
  RTC_Daystamp_B_Enable(stamp);
}

void RTC_Weekstamp_B_Enable(uint32_t stamp)
{
  RTC_Alarm_t alarm = RTC_WeekstampToAlarm(stamp);
  RTC_Alarm_B_Enable(&alarm);
}

static void _RTC_Alarm_Disable(ALARM_e alarm)
{
  RTC->CR &= ~alarm_mask[alarm];
}

void RTC_Alarm_A_Disable(void)
{
  _RTC_Alarm_Disable(ALARM_A);
}

void RTC_Alarm_B_Disable(void)
{
  _RTC_Alarm_Disable(ALARM_B);
}

void RTC_WakeupTimer_Enable(uint32_t sec)
{
  _wakeup_timer_flag = false;

  RTC->WPR = 0xCA;
  RTC->WPR = 0x53; // Write access ON

  RTC->CR &= ~RTC_CR_WUTE;
  while(!(RTC->ICSR & RTC_ICSR_WUTWF));

  RTC->CR |= RTC_CR_WUCKSEL_2;
  RTC->CR &= ~RTC_CR_WUCKSEL_1; // set ck_spre as source
  RTC->WUTR = sec - 1;

  RTC->CR |= RTC_CR_WUTE; // Enable wakeup timer
  RTC->WPR = 0xFF; // Write access OFF
}

void RTC_WakeupTimer_Disable(void)
{
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;
  RTC->CR &= ~RTC_CR_WUTE;
  RTC->WPR = 0xFF;
}

//--------------------------------------------------------------------------------------------------------------------- ALARM-CHECK

static bool _RTC_Check_Base(int32_t stamp, int32_t stamp_min, int32_t stamp_max, int32_t stamp_alarm,
    uint32_t stamp_reload)
{
  if(stamp_min < 0) {
    stamp_min += stamp_reload;
    if((stamp_alarm > stamp_min || stamp_alarm < stamp_max)) return true;
  } else if(stamp_max >= stamp_reload) {
    stamp_max -= RTC_SECONDS_IN_DAY;
    if((stamp_alarm > stamp_min || stamp_alarm < stamp_max)) return true;
  } else {
    if((stamp_alarm > stamp_min) && (stamp_alarm < stamp_max)) return true;
  }
  return false;
}

bool RTC_Check_Daystamp(uint32_t stamp_alarm, uint32_t offset_min_sec, uint32_t offset_max_sec)
{
  int32_t daystamp = (int32_t)RTC_Daystamp();
  int32_t daystamp_min = daystamp - offset_min_sec;
  int32_t daystamp_max = daystamp + offset_max_sec;
  return _RTC_Check_Base(daystamp, daystamp_min, daystamp_max, (int32_t)stamp_alarm, RTC_SECONDS_IN_DAY);
}

bool RTC_Check_Weekstamp(uint32_t stamp_alarm, uint32_t offset_min_sec, uint32_t offset_max_sec)
{
  int32_t weekstamp = (int32_t)RTC_Weekstamp();
  int32_t weekstamp_min = weekstamp - offset_min_sec;
  int32_t weekstamp_max = weekstamp + offset_max_sec;
  return _RTC_Check_Base(weekstamp, weekstamp_min, weekstamp_max, (int32_t)stamp_alarm, RTC_SECONDS_IN_WEEK);
}

/**
 * @brief: Function which checks if alarm has occurred or will be active in given range
 * @param alarm: Type of alarm (Alarm A or Alarm B)
 * @param offset_min_sec: Seconds for hysteresis before
 * @param offset_max_sec: Seconds for hysteresis after
 * @retval bool: alarm will/was/is active in given range (now +- offset)
 */
static bool _RTC_Alarm_Check(ALARM_e alarm, uint32_t offset_min_sec, uint32_t offset_max_sec)
{
  RTC_Alarm_t alarm_ab;
  if(!_RTC_Alarm_IsEnable(alarm)) return false;
  switch(alarm) {
    case ALARM_A: alarm_ab = RTC_Alarm_A(); break;
    case ALARM_B: alarm_ab = RTC_Alarm_B(); break;
  }
  if(alarm_ab.day_mask) return RTC_Check_Daystamp(RTC_AlarmToDaystamp(&alarm_ab), offset_min_sec, offset_max_sec);
  return RTC_Check_Weekstamp(RTC_AlarmToWeekstamp(&alarm_ab), offset_min_sec, offset_max_sec);
}

bool RTC_Check_A(uint32_t offset_min_sec, uint32_t offset_max_sec)
{
  return _RTC_Alarm_Check(ALARM_A, offset_min_sec, offset_max_sec);
}

bool RTC_Check_B(uint32_t offset_min_sec, uint32_t offset_max_sec)
{
  return _RTC_Alarm_Check(ALARM_B, offset_min_sec, offset_max_sec);
}

//--------------------------------------------------------------------------------------------------------------------- ALARM-INT

bool RTC_Event_A(void)
{
  if(_alarm_flag[ALARM_A]) {
    _alarm_flag[ALARM_A] = false;
    return true;
  } else return false;
}

bool RTC_Event_B(void)
{
  if(_alarm_flag[ALARM_B]) {
    _alarm_flag[ALARM_B] = false;
    return true;
  } else return false;
}

bool RTC_Event_WakeupTimer(void)
{
  if(_wakeup_timer_flag) {
    _wakeup_timer_flag = false;
    return true;
  }
  else return false;
}

void RTC_Force_A(void) { _alarm_flag[ALARM_A] = true; }
void RTC_Force_B(void) { _alarm_flag[ALARM_B] = true; }
void RTC_Force_WakeupTimer(void) { _wakeup_timer_flag = true; }

void RTC_STAMP_IRQHandler(void)
{
	if(RTC->SR & RTC_SR_ALRAF) {
	  RTC->SCR |= RTC_SCR_CALRAF;
	  _alarm_flag[ALARM_A] = true;
	}

	if(RTC->SR & RTC_SR_ALRBF) {
	  RTC->SCR |= RTC_SCR_CALRBF;
	  _alarm_flag[ALARM_B] = true;
	}

	if(RTC->SR & RTC_SR_WUTF) {
	  RTC->SCR |= RTC_SCR_CWUTF;
	  _wakeup_timer_flag = true;
	}

	NVIC_ClearPendingIRQ(RTC_TAMP_IRQn);
}

//---------------------------------------------------------------------------------------------------------------------

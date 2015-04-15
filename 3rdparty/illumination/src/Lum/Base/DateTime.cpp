/*
  This source is part of the Illumination library
  Copyright (C) 2004  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <Lum/Base/DateTime.h>

#include <Lum/Private/Config.h>

#include <Lum/Base/String.h>

#include <cassert>
#include <cstddef>
#include <cstring>

#if defined(__WIN32__) || defined(WIN32)
  #include <Lum/OS/Win32/OSAPI.h>
#else
  #include <unistd.h>
#endif

#include <Lum/Private/Config.h>

#if defined(HAVE_SYS_TIME_H)
  #include <sys/time.h>
#endif

#if defined(__WIN32__) && !defined(__MINGW32__)
struct timezone {
   int tz_minuteswest; /* of Greenwich */
   int tz_dsttime;     /* type of dst correction to apply */
};
#endif

#if !defined(HAVE_GETTIMEOFDAY)
#if defined(HAVE_GETSYSTEMTIMEASFILETIME)
void gettimeofday(struct timeval* tv, struct timezone* /*tz*/)
{
  FILETIME now;

  GetSystemTimeAsFileTime(&now);

  ULARGE_INTEGER _100ns;
  _100ns.LowPart = now.dwLowDateTime;
  _100ns.HighPart = now.dwHighDateTime;
  _100ns.QuadPart -= 116444736000000000LL;

  tv->tv_usec=(long)((_100ns.QuadPart / 10LL) % 1000000LL );
  tv->tv_sec= (long)(_100ns.QuadPart/10000000LL);
}
#else
#error no implementation for gettimeofday available!
#endif
#endif

namespace Lum {
  namespace Base {

    static bool my_gmtime(time_t time, struct tm* res)
    {
#if defined(HAVE_GMTIME_R)
      return gmtime_r(&time,res)!=NULL;
#elif defined(HAVE_GMTIME_S)
      return gmtime_s(res,&time)==0;
#else
      struct tm *tmp;

      tmp=gmtime(&time);

      if (tmp==NULL) {
        return false;
      }

      memcpy(res,tmp,sizeof(struct tm)); // Flawfinder: ignore
      return true;
#endif
    }

    static bool my_localtime(time_t time, struct tm* res)
    {
#if defined(HAVE_LOCALTIME_R)
      return localtime_r(&time,res)!=NULL;
#elif defined(HAVE_LOCALTIME_S)
      return localtime_s(res,&time)==0;
#else
      struct tm *tmp;

      tmp=localtime(&time);

      if (tmp==NULL) {
        return false;
      }

      memcpy(res,tmp,sizeof(struct tm));  // Flawfinder: ignore
      return true;
#endif
    }

    std::wstring TMToWString(struct tm* tm, const char* format)
    {
      char   buffer[1024];  // Flawfinder: ignore
      size_t length;

      length=strftime(buffer,1024,format,tm);

      buffer[length]='\0';

      return StringToWString(buffer);
    }
/*
    static bool my_asctime(const struct tm* tm, std::wstring& value)
    {
#if defined(HAVE_ASCTIME_R)
      char       tmp[26];
      const char *buffer;

      buffer=asctime_r(tm,tmp);
#else
      const char *buffer;

      buffer=asctime(tm);
#endif
      if (buffer==NULL) {
        return false;
      }

      value=StringToWString(buffer);

      value.erase(24);

      return true;
    }
*/
    static void assignTMToDateTime(DateTime& dateTime, struct tm* structTM)
    {
      dateTime.second=structTM->tm_sec;
      dateTime.minute=structTM->tm_min;
      dateTime.hour=structTM->tm_hour;
      dateTime.dayOfMonth=structTM->tm_mday;
      dateTime.month=structTM->tm_mon+1;
      dateTime.year=structTM->tm_year+1900;
      dateTime.dayOfWeek=structTM->tm_wday;
      dateTime.dayOfYear=structTM->tm_yday;
    }

    /**
      Initializes instance to the current local time.
    */
    Time::Time()
    {
      SetToNow();
    }

    /**
      Initializes instance to \p hour and \p second.
    */
    Time::Time(unsigned char hour, unsigned char minute, unsigned char second)
    : hour(hour),minute(minute),second(second)
    {
      // no code
    }

    /**
      Sets the current time.

      \p hour must be >=0 and <=23
      \p second must be >=0 and <=59
    */
    void Time::Set(unsigned char hour, unsigned char minute, unsigned char second)
    {
      assert(hour<=23 && minute<=59 && second<=59);

      this->hour=hour;
      this->minute=minute;
      this->second=second;
    }

    void Time::SetHour(unsigned char hour)
    {
      assert(hour<=23);

      this->hour=hour;
    }

    void Time::SetMinute(unsigned char minute)
    {
      assert(minute<=59);

      this->minute=minute;
    }

    void Time::SetSecond(unsigned char second)
    {
      assert(second<=59);

      this->second=second;
    }

    void Time::IncHour()
    {
      if (hour==23) {
        hour=0;
      }
      else {
        hour++;
      }
    }

    void Time::DecHour()
    {
      if (hour==0) {
        hour=23;
      }
      else {
        hour--;
      }
    }

    void Time::IncMinute()
    {
      if (minute==59) {
        minute=0;
        IncHour();
      }
      else {
        minute++;
      }
    }

    void Time::DecMinute()
    {
      if (minute==0) {
        minute=59;
        DecHour();
      }
      else {
        minute--;
      }
    }

    void Time::IncSecond()
    {
     if (second==59) {
        second=0;
        IncMinute();
      }
      else {
        second++;
      }
    }

    void Time::DecSecond()
    {
      if (second==0) {
        second=59;
        DecMinute();
      }
      else {
        second--;
      }
    }

    /**
    Set the date to the date given in 'date'. 'date' has to follow the ISO8601 format,
    which is defined as 'YYYY-MM-DD' where 'YYYY' stand for the four digit year,
    'MM' stands for the two digit month (1..12) and 'DD' stand for the two digit day
    (1..31).
    */
    bool Time::SetISO8601(const std::wstring& date)
    {
      unsigned char h,m,s;

      if (date.length()!=8) {
        return false;
      }

      std::wstring tmp(date);

      if (tmp[2]!=L':') {
        return false;
      }
      tmp.erase(2,1);

      if (tmp[4]!=L':') {
        return false;
      }
      tmp.erase(4,1);

      for (size_t x=0; x<tmp.length(); x++) {
        if (tmp[x]<L'0' || tmp[x]>L'9') {
          return false;
        }
      }

      h=(tmp[0]-L'0')*10+(tmp[1]-L'0');
      m=(tmp[2]-L'0')*10+(tmp[3]-L'0');
      s=(tmp[4]-L'0')*10+(tmp[5]-L'0');

      if (h>23 || m>59 || s>59) {
        return false;
      }

      hour=h;
      minute=m;
      second=s;

      return true;
    }


    /**
      Initializes instance to the current local time.
    */
    void Time::SetToNow()
    {
      DateTime time;

      SystemTime::GetCurrentLocalTime(time);

      hour=time.hour;
      minute=time.minute;
      second=time.second;
    }

    unsigned char Time::GetHour() const
    {
      return hour;
    }

    unsigned char Time::GetMinute() const
    {
      return minute;
    }

    unsigned char Time::GetSecond() const
    {
      return second;
    }

    std::wstring Time::GetISO8601() const
    {
      std::wstring res;

      res.reserve(8);

      res.append(1,(wchar_t)(L'0'+(hour /   10) % 10));
      res.append(1,(wchar_t)(L'0'+(hour /    1) % 10));
      res.append(L":");
      res.append(1,(wchar_t)(L'0'+(minute / 10) % 10));
      res.append(1,(wchar_t)(L'0'+(minute /  1) % 10));
      res.append(L":");
      res.append(1,(wchar_t)(L'0'+(second / 10) % 10));
      res.append(1,(wchar_t)(L'0'+(second /  1) % 10));

      return res;
    }

    std::wstring Time::GetLocaleTime(TimeFormat format) const
    {
      struct tm tm;

      tm.tm_sec=second;
      tm.tm_min=minute;
      tm.tm_hour=hour;
      tm.tm_mday=0;
      tm.tm_mon=0;
      tm.tm_year=0;
      tm.tm_wday=0;
      tm.tm_yday=0;
      tm.tm_isdst=-1;

      switch (format) {
      case timeFormatDefault:
        return TMToWString(&tm,"%X");
      case timeFormatHM:
        return TMToWString(&tm,"%H:%M");
      case timeFormatHMS:
        return TMToWString(&tm,"%H:%M:%S");
      }

      assert(false);
    }

    bool operator<(const Time& a, const Time& b)
    {
      if (a.hour==b.hour) {
        if (a.minute==b.minute) {
          return a.second<b.second;
        }
        else {
          return a.minute<b.minute;
        }
      }
      else {
        return a.hour<b.hour;
      }
    }

    bool operator<=(const Time& a, const Time& b)
    {
      if (a.hour==b.hour) {
        if (a.minute==b.minute) {
          return a.second<=b.second;
        }
        else {
          return a.minute<b.minute;
        }
      }
      else {
        return a.hour<b.hour;
      }
    }

    bool operator==(const Time& a, const Time& b)
    {
      return a.hour==b.hour && a.minute==b.minute && a.second==b.second;
    }

    bool operator!=(const Time& a, const Time& b)
    {
      return a.hour!=b.hour || a.minute!=b.minute || a.second!=b.second;
    }

    bool operator>=(const Time& a, const Time& b)
    {
      if (a.hour==b.hour) {
        if (a.minute==b.minute) {
          return a.second>=b.second;
        }
        else {
          return a.minute>b.minute;
        }
      }
      else {
        return a.hour>b.hour;
      }
    }

    bool operator>(const Time& a, const Time& b)
    {
      if (a.hour==b.hour) {
        if (a.minute==b.minute) {
          return a.second>b.second;
        }
        else {
          return a.minute>b.minute;
        }
      }
      else {
        return a.hour>b.hour;
      }
    }

    struct SystemTime::SystemTimePIMPL
    {
      timeval time;
    };

    SystemTime::SystemTime()
    : pimpl(new SystemTimePIMPL())
    {
      gettimeofday(&pimpl->time,NULL);
    }

    SystemTime::SystemTime(const SystemTime& systemTime)
    : pimpl(new SystemTimePIMPL())
    {
      *pimpl=*systemTime.pimpl;
    }

    SystemTime::SystemTime(time_t seconds, long microseconds)
    : pimpl(new SystemTimePIMPL())
    {
      SetTime(seconds,microseconds);
    }

#if defined(__WIN32__) || defined(WIN32)
    SystemTime::SystemTime(const FILETIME& time)
    : pimpl(new SystemTimePIMPL())
    {
      SetTime(time);
    }
#endif

    SystemTime::~SystemTime()
    {
      delete pimpl;
    }

    SystemTime& SystemTime::operator=(const SystemTime& systemTime)
    {
      // Works also with self-assignment!
      *pimpl=*systemTime.pimpl;

      return *this;
    }

    time_t SystemTime::GetTime() const
    {
      return pimpl->time.tv_sec;
    }

    long SystemTime::GetMicroseconds() const
    {
      return pimpl->time.tv_usec;
    }

    void SystemTime::GetUTCDateTime(DateTime& dateTime) const throw (DateOutOfRange)
    {
      struct tm utc;
      time_t    tmp;

      tmp=pimpl->time.tv_sec;
      if (!my_gmtime(tmp,&utc)) {
        throw DateOutOfRange();
      }

      assignTMToDateTime(dateTime,&utc);
      dateTime.microsecond=pimpl->time.tv_usec;
    }

    void SystemTime::GetLocalDateTime(DateTime& dateTime) const throw (DateOutOfRange)
    {
      struct tm local;
      time_t    tmp;

      tmp=pimpl->time.tv_sec;
      if (!my_localtime(tmp,&local)) {
        throw DateOutOfRange();
      }

      assignTMToDateTime(dateTime,&local);
      dateTime.microsecond=pimpl->time.tv_usec;
    }

    int SystemTime::GetTimezone() const  throw (DateOutOfRange)
    {
      time_t    u,l,tmp;
      struct tm utc,local;

      tmp=pimpl->time.tv_sec; // workaround non-const call
      if (!my_gmtime(tmp,&utc)) {
        throw DateOutOfRange();
      }

      tmp=pimpl->time.tv_sec; // workaround non-const call
      if (!my_localtime(tmp,&local)) {
        throw DateOutOfRange();
      }

      u=mktime(&utc);

      if (u==(time_t)-1) {
        throw DateOutOfRange();
      }

      l=mktime(&local);

      if (l==(time_t)-1) {
        throw DateOutOfRange();
      }

      return (int)(u-l);
    }

    bool SystemTime::IsDSTActive() const throw (DateOutOfRange)
    {
      struct tm local1,local2;
      time_t    tmp,time1,time2;

      tmp=pimpl->time.tv_sec;
      if (!my_localtime(tmp,&local1)) {
        throw DateOutOfRange();
      }

      tmp=pimpl->time.tv_sec;
      if (!my_localtime(tmp,&local2)) {
        throw DateOutOfRange();
      }

      local1.tm_isdst=-1;
      time1=mktime(&local1);

      local2.tm_isdst=0;
      time2=mktime(&local2);

      return time1!=time2;
    }

    int SystemTime::GetDST() const throw (DateOutOfRange)
    {
      struct tm local1,local2;
      time_t    tmp,time1,time2;

      tmp=pimpl->time.tv_sec;
      if (!my_localtime(tmp,&local1)) {
        throw DateOutOfRange();
      }

      tmp=pimpl->time.tv_sec;
      if (!my_localtime(tmp,&local2)) {
        throw DateOutOfRange();
      }

      local1.tm_isdst=-1;
      time1=mktime(&local1);

      local2.tm_isdst=0;
      time2=mktime(&local2);

      return (int)(time1-time2);
    }

    int SystemTime::GetDifferenceToUTC() const throw (DateOutOfRange)
    {
      time_t    tmp,u,l;
      struct tm utc,local;

      tmp=pimpl->time.tv_sec;
      if (!my_gmtime(tmp,&utc)) {
        throw DateOutOfRange();
      }

      tmp=pimpl->time.tv_sec;
      if (!my_localtime(tmp,&local)) {
        throw DateOutOfRange();
      }

      utc.tm_isdst=-1;
      u=mktime(&utc);

      if (u==(time_t)-1) {
        throw DateOutOfRange();
      }

      local.tm_isdst=-1;
      l=mktime(&local);

      if (l==(time_t)-1) {
        throw DateOutOfRange();
      }

      return (int)(u-l);
    }

    int SystemTime::GetDaysOfCurrentMonth() const throw (DateOutOfRange)
    {
      time_t    tmp,l;
      struct tm local;
      int       month,day;

      tmp=pimpl->time.tv_sec;
      if (!my_localtime(tmp,&local)) {
        throw DateOutOfRange();
      }

      month=local.tm_mon;
      day=28; /* Do not need to start with day 1 */

      local.tm_mday=day;
      l=mktime(&local);

      if (l==(time_t)-1) {
        throw DateOutOfRange();
      }

      if (!my_localtime(l,&local)) {
        throw DateOutOfRange();
      }

      while (local.tm_mon==month) {
        day++;
        local.tm_mday=day;

        l=mktime(&local);

        if (l==(time_t)-1) {
          throw DateOutOfRange();
        }

        if (!my_localtime(l,&local)) {
          throw DateOutOfRange();
        }
      }

      return day-1;
    }

    bool SystemTime::IsToday() const
    {
      DateTime x,now;

      GetCurrentLocalTime(now);
      GetLocalDateTime(x);

      return x.year==now.year && x.month==now.month && x.dayOfMonth==now.dayOfMonth;
    }

    std::wstring SystemTime::GetUTCISO8601Time() const throw (DateOutOfRange)
    {
      DateTime     time;
      std::wstring res;

      res.reserve(8);

      GetUTCDateTime(time);

      res.append(1,(wchar_t)(L'0'+(time.hour /   10) % 10));
      res.append(1,(wchar_t)(L'0'+(time.hour /    1) % 10));
      res.append(L":");
      res.append(1,(wchar_t)(L'0'+(time.minute / 10) % 10));
      res.append(1,(wchar_t)(L'0'+(time.minute /  1) % 10));
      res.append(L":");
      res.append(1,(wchar_t)(L'0'+(time.second / 10) % 10));
      res.append(1,(wchar_t)(L'0'+(time.second /  1) % 10));

      return res;
    }

    std::wstring SystemTime::GetUTCISO8601DateTime() const throw (DateOutOfRange)
    {
      DateTime     time;
      std::wstring res;

      res.reserve(10+1+8+1);

      GetUTCDateTime(time);

      // Date
      res.append(1,(wchar_t)(L'0'+(time.year / 1000) % 10));
      res.append(1,(wchar_t)(L'0'+(time.year /  100) % 10));
      res.append(1,(wchar_t)(L'0'+(time.year /   10) % 10));
      res.append(1,(wchar_t)(L'0'+(time.year /    1) % 10));
      res.append(L"-");
      res.append(1,(wchar_t)(L'0'+(time.month / 10) % 10));
      res.append(1,(wchar_t)(L'0'+(time.month /  1) % 10));
      res.append(L"-");
      res.append(1,(wchar_t)(L'0'+(time.dayOfMonth / 10) % 10));
      res.append(1,(wchar_t)(L'0'+(time.dayOfMonth /  1) % 10));

      res.append(1,'T');

      // Time
      res.append(1,(wchar_t)(L'0'+(time.hour /   10) % 10));
      res.append(1,(wchar_t)(L'0'+(time.hour /    1) % 10));
      res.append(L":");
      res.append(1,(wchar_t)(L'0'+(time.minute / 10) % 10));
      res.append(1,(wchar_t)(L'0'+(time.minute /  1) % 10));
      res.append(L":");
      res.append(1,(wchar_t)(L'0'+(time.second / 10) % 10));
      res.append(1,(wchar_t)(L'0'+(time.second /  1) % 10));

      res.append(1,'Z');

      return res;
    }

    std::wstring SystemTime::GetLocalISO8601Time() const throw (DateOutOfRange)
    {
      DateTime     time;
      std::wstring res;

      res.reserve(8);

      GetLocalDateTime(time);

      res.append(1,(wchar_t)(L'0'+(time.hour /   10) % 10));
      res.append(1,(wchar_t)(L'0'+(time.hour /    1) % 10));
      res.append(L":");
      res.append(1,(wchar_t)(L'0'+(time.minute / 10) % 10));
      res.append(1,(wchar_t)(L'0'+(time.minute /  1) % 10));
      res.append(L":");
      res.append(1,(wchar_t)(L'0'+(time.second / 10) % 10));
      res.append(1,(wchar_t)(L'0'+(time.second /  1) % 10));

      return res;
    }

    std::wstring SystemTime::GetLocalLocaleDate() const throw (DateOutOfRange)
    {
      time_t    tmp;
      struct tm local;

      tmp=pimpl->time.tv_sec;
      if (!my_localtime(tmp,&local)) {
        throw DateOutOfRange();
      }

      return TMToWString(&local,"%x");
    }

    std::wstring SystemTime::GetLocalLocaleTime() const throw (DateOutOfRange)
    {
      time_t    tmp;
      struct tm local;

      tmp=pimpl->time.tv_sec;
      if (!my_localtime(tmp,&local)) {
        throw DateOutOfRange();
      }

      return TMToWString(&local,"%X");
    }

    std::wstring SystemTime::GetLocalLocaleDateTime() const throw (DateOutOfRange)
    {
      time_t    tmp;
      struct tm local;

      tmp=pimpl->time.tv_sec;
      if (!my_localtime(tmp,&local)) {
        throw DateOutOfRange();
      }

      return TMToWString(&local,"%c");
    }

    /**
      Since it is unclear how to output locale aware UTC times, this function
      in fact uses asctime internally for conversion.
    */
    std::wstring SystemTime::GetUTCLocaleDateTime() const throw (DateOutOfRange)
    {
      time_t    tmp;
      struct tm utc;

      tmp=pimpl->time.tv_sec;
      if (!my_gmtime(tmp,&utc)) {
        throw DateOutOfRange();
      }

      return TMToWString(&utc,"%c");
    }

    bool SystemTime::IsValid() const
    {
      return (pimpl->time.tv_sec!=0 || pimpl->time.tv_usec!=0);
    }

    void SystemTime::Invalidate()
    {
      pimpl->time.tv_sec=0;
      pimpl->time.tv_usec=0;
    }

    void SystemTime::SetToNow()
    {
      gettimeofday(&pimpl->time,NULL);
    }

    void SystemTime::SetTime(time_t seconds, long microseconds)
    {
      pimpl->time.tv_sec=seconds;
      pimpl->time.tv_usec=microseconds;
    }

#if defined(__WIN32__) || defined(WIN32)
    void SystemTime::SetTime(const FILETIME& time)
    {
      ULARGE_INTEGER _100ns;
      _100ns.LowPart=time.dwLowDateTime;
      _100ns.HighPart=time.dwHighDateTime;
      _100ns.QuadPart-=116444736000000000LL;

      pimpl->time.tv_usec=(long)((_100ns.QuadPart / 10LL) % 1000000LL );
      pimpl->time.tv_sec= (long)(_100ns.QuadPart/10000000LL);
    }
#endif

    /**
      Sets the time to the given time in UTC.
    */
    void SystemTime::SetUTCDateTime(int hour, int minute, int second, int microsecond,
                                    int day, int month, int year) throw (DateOutOfRange)
    {
      struct tm tmp;
      time_t    newTime;

      memset(&tmp,0,sizeof(struct tm));

      tmp.tm_sec=second;
      tmp.tm_min=minute;
      tmp.tm_hour=hour;
      tmp.tm_mday=day;
      tmp.tm_mon=month-1;
      tmp.tm_year=year-1900;
      tmp.tm_isdst=-1;

      newTime=mktime(&tmp);

      if (newTime==(time_t)-1) {
        throw DateOutOfRange();
      }

      pimpl->time.tv_sec=newTime;
      pimpl->time.tv_usec=microsecond;

      pimpl->time.tv_sec-=GetDifferenceToUTC();
      // This may not be completely true, if the UTC time and the temporary calculated
      // localtime have differnt DST settings. We should try to convert with and without
      // DST, reconvert back and see which of the both values is correct.
    }

    /**
      Sets the time to the given time in current local time.
    */
    void SystemTime::SetLocalDateTime(int hour, int minute, int second, int microsecond,
                                      int day, int month, int year) throw (DateOutOfRange)
    {
      struct tm tmp;
      time_t    newTime;

      memset(&tmp,0,sizeof(struct tm));

      tmp.tm_sec=second;
      tmp.tm_min=minute;
      tmp.tm_hour=hour;
      tmp.tm_mday=day;
      tmp.tm_mon=month-1;
      tmp.tm_year=year-1900;
      tmp.tm_isdst=-1;

      newTime=mktime(&tmp);

      if (newTime==(time_t)-1) {
        throw DateOutOfRange();
      }

      pimpl->time.tv_sec=newTime;
      pimpl->time.tv_usec=microsecond;
    }

    /**
      Adds the given time interval \p interval to the currently hold time.
    */
    void SystemTime::Add(const SystemTime& interval)
    {
      pimpl->time.tv_sec+=interval.pimpl->time.tv_sec+
                          (pimpl->time.tv_usec + interval.pimpl->time.tv_usec) / 1000000;
      pimpl->time.tv_usec=(pimpl->time.tv_usec+interval.pimpl->time.tv_usec) % 1000000;
    }

    void SystemTime::Add(time_t seconds, long microseconds)
    {
      pimpl->time.tv_sec+=seconds+(pimpl->time.tv_usec + microseconds) / 1000000;
      pimpl->time.tv_usec=(pimpl->time.tv_usec+microseconds) % 1000000;
    }

    /**
      Subtracts the given time interval \p interval from the currently hold time.
    */
    void SystemTime::Sub(const SystemTime& interval)
    {
      if (pimpl->time.tv_sec<interval.pimpl->time.tv_sec ||
          (pimpl->time.tv_sec==interval.pimpl->time.tv_sec &&
           pimpl->time.tv_usec<interval.pimpl->time.tv_usec)) {
        pimpl->time.tv_sec=0;
        pimpl->time.tv_usec=0;
        return;
      }


      if (pimpl->time.tv_usec<interval.pimpl->time.tv_usec) {
        pimpl->time.tv_usec=pimpl->time.tv_usec+1000000-interval.pimpl->time.tv_usec;
        pimpl->time.tv_sec--;
      }
      else {
        pimpl->time.tv_usec=pimpl->time.tv_usec-interval.pimpl->time.tv_usec;
      }

      pimpl->time.tv_sec-=interval.pimpl->time.tv_sec;
    }

    /**
      Returns the current time in UTC.
    */
    void SystemTime::GetCurrentUTCTime(DateTime& dateTime) throw (DateOutOfRange)
    {
      timeval   currentTime;
      struct tm utc;

      gettimeofday(&currentTime,NULL);

      if (!my_gmtime(currentTime.tv_sec,&utc)) {
        throw DateOutOfRange();
      }

      assignTMToDateTime(dateTime,&utc);
      dateTime.microsecond=currentTime.tv_usec;
    }

    /**
      Returns the current time in local time.
    */
    void SystemTime::GetCurrentLocalTime(DateTime& dateTime) throw (DateOutOfRange)
    {
      timeval   currentTime;
      struct tm local;

      gettimeofday(&currentTime,NULL);

      if (!my_localtime(currentTime.tv_sec,&local)) {
        throw DateOutOfRange();
      }

      assignTMToDateTime(dateTime,&local);
      dateTime.microsecond=currentTime.tv_usec;
    }

    void SystemTime::Sleep(unsigned long seconds)
    {
#if defined(HAVE_SLEEPEX)
      SleepEx(seconds*1000,false);
#else
      sleep(seconds);
#endif
    }


    bool operator<(const SystemTime& a, const SystemTime& b)
    {
      if (a.pimpl->time.tv_sec==b.pimpl->time.tv_sec) {
        return a.pimpl->time.tv_usec<b.pimpl->time.tv_usec;
      }
      else {
        return a.pimpl->time.tv_sec<b.pimpl->time.tv_sec;
      }
    }

    bool operator<=(const SystemTime& a, const SystemTime& b)
    {
      if (a.pimpl->time.tv_sec==b.pimpl->time.tv_sec) {
        return a.pimpl->time.tv_usec<=b.pimpl->time.tv_usec;
      }
      else {
        return a.pimpl->time.tv_sec<=b.pimpl->time.tv_sec;
      }
    }

    bool operator==(const SystemTime& a, const SystemTime& b)
    {
      return a.pimpl->time.tv_sec==b.pimpl->time.tv_sec &&
             a.pimpl->time.tv_usec==b.pimpl->time.tv_usec;
    }

    bool operator!=(const SystemTime& a, const SystemTime& b)
    {
      return a.pimpl->time.tv_sec!=b.pimpl->time.tv_sec ||
             a.pimpl->time.tv_usec!=b.pimpl->time.tv_usec;
    }

    bool operator>=(const SystemTime& a, const SystemTime& b)
    {
      if (a.pimpl->time.tv_sec==b.pimpl->time.tv_sec) {
        return a.pimpl->time.tv_usec>=b.pimpl->time.tv_usec;
      }
      else {
        return a.pimpl->time.tv_sec>=b.pimpl->time.tv_sec;
      }
    }

    bool operator>(const SystemTime& a, const SystemTime& b)
    {
      if (a.pimpl->time.tv_sec==b.pimpl->time.tv_sec) {
        return a.pimpl->time.tv_usec>b.pimpl->time.tv_usec;
      }
      else {
        return a.pimpl->time.tv_sec>b.pimpl->time.tv_sec;
      }
    }

    long operator-(const SystemTime& a, const SystemTime& b)
    {
      return a.pimpl->time.tv_sec-b.pimpl->time.tv_sec;
    }

    Timer::Timer()
    : time(0,0),start(0,0)
    {
    }

    time_t Timer::GetTime() const
    {
      if (!start.IsValid()) {
        return time.GetTime();
      }
      else {
        SystemTime tmp,current;

        current.SetToNow();

        current.Sub(start);

        tmp=time;
        tmp.Add(current);

        return tmp.GetTime();
      }
    }

    long Timer::GetMicroseconds() const
    {
      if (!start.IsValid()) {
        return time.GetMicroseconds();
      }
      else {
        SystemTime tmp,current;

        current.SetToNow();

        current.Sub(start);

        tmp=time;
        tmp.Add(current);

        return tmp.GetMicroseconds();
      }
    }

    bool Timer::IsStarted() const
    {
      return start.IsValid();
    }

    bool Timer::IsPausing() const
    {
      return time.IsValid() && !start.IsValid();
    }

    bool Timer::IsStoped() const
    {
      return !time.IsValid() && !start.IsValid();
    }

    void Timer::Start()
    {
      time.Invalidate();

      start.SetToNow();
    }

    void Timer::Pause()
    {
      SystemTime current;

      current.SetToNow();

      current.Sub(start);

      time.Add(current);

      start.Invalidate();
    }

    void Timer::Resume()
    {
      start.SetToNow();
    }

    void Timer::Reset()
    {
      time.Invalidate();
      start.Invalidate();
    }


    bool Calendar::IsLeapYear(int year)
    {
      if (year % 400==0) {
        return true;
      }

      return (year % 4==0) && !(year % 100==0);
    }

    int CalculateWeekDay(int dayOfMonth, int month, int year)
    {
      int a,y,m;

      a=(14-month) / 12;
      y=year-a;
      m=month+12*a-2;

      return (dayOfMonth+y+y/4-y/100+y/400+(31*m)/12)%7;
    }

    int CalculateDaysOfMonth(int month, int year)
    {
      switch (month) {
      case 1:
        return 31;
      case 2:
        if (Calendar::IsLeapYear(year)) {
          return 29;
        }
        else {
          return 28;
        }
      case 3:
        return 31;
      case 4:
        return 30;
      case 5:
        return 31;
      case 6:
        return 30;
      case 7:
        return 31;
      case 8:
        return 31;
      case 9:
        return 30;
      case 10:
        return 31;
      case 11:
        return 30;
      case 12:
        return 31;
      }

      return -1;
    }

    int CalculateDayOfYear(int dayOfMonth, int month, int year)
    {
      int days,x;

      days=0;
      for (x=1; x<month; x++) {
        days+=CalculateDaysOfMonth(x,year);
      }
      days+=dayOfMonth;

      return days;
    }

    bool Calendar::IsValid(int dayOfMonth, int month, int year)
    {
      if (month<=0 || month>12 || dayOfMonth<=0) {
        return false;
      }

      switch (month) {
      case 1:
        if (dayOfMonth>31) {
          return false;
        }
        break;
      case 2:
        if (Calendar::IsLeapYear(year)) {
          if (dayOfMonth>29) {
            return false;
          }
        }
        else {
          if (dayOfMonth>28) {
            return false;
          }
        }
        break;
      case 3:
        if (dayOfMonth>31) {
          return false;
        }
        break;
      case 4:
        if (dayOfMonth>30) {
          return false;
        }
        break;
      case 5:
        if (dayOfMonth>31) {
          return false;
        }
        break;
      case 6:
        if (dayOfMonth>30) {
          return false;
        }
        break;
      case 7:
        if (dayOfMonth>31) {
          return false;
        }
        break;
      case 8:
        if (dayOfMonth>31) {
          return false;
        }
        break;
      case 9:
        if (dayOfMonth>30) {
          return false;
        }
        break;
      case 10:
        if (dayOfMonth>31) {
          return false;
        }
        break;
      case 11:
        if (dayOfMonth>30) {
          return false;
        }
        break;
      case 12:
        if (dayOfMonth>31) {
          return false;
        }
        break;
      }

      return true;
    }

    Calendar::Calendar()
    {
      SetToCurrent();
    }

    Calendar::Calendar(const Calendar& other)
    {
      dayOfWeek=other.dayOfWeek;
      dayOfMonth=other.dayOfMonth;
      dayOfYear=other.dayOfYear;
      month=other.month;
      year=other.year;
    }

    void Calendar::Normalize()
    {
      if (month>12) {
        year+=(month-1)/12;
        month=(month-1)%12+1;
      }
      else if (month<=0) {
        year-=(-month+1)/12+1;
        month+=((-month+1)/12+1)*12;
      }

      while (dayOfMonth<=0) {
        if (month==1) {
          dayOfMonth+=CalculateDaysOfMonth(12,year-1);
          year--;
          month=12;
        }
        else {
          dayOfMonth+=CalculateDaysOfMonth(month-1,year);
          month--;
        }
      }

      int dom=CalculateDaysOfMonth(month,year);
      while (dayOfMonth>dom) {
        dayOfMonth-=dom;
        if (month==12) {
          year++;
          month=1;
        }
        else {
          month++;
        }
        dom=CalculateDaysOfMonth(month,year);
      }

      dayOfWeek=CalculateWeekDay(dayOfMonth,month,year);
      dayOfYear=CalculateDayOfYear(dayOfMonth,month,year);
    }

    bool operator<(const Calendar& a, const Calendar& b)
    {
      if (a.year!=b.year) {
        return a.year<b.year;
      }

      if (a.month!=b.month) {
        return a.month<b.month;
      }

      return a.dayOfMonth<b.dayOfMonth;
    }

    bool operator<=(const Calendar& a, const Calendar& b)
    {
      if (a.year!=b.year) {
        return a.year<b.year;
      }

      if (a.month!=b.month) {
        return a.month<b.month;
      }

      return a.dayOfMonth<=b.dayOfMonth;
    }

    bool operator>(const Calendar& a, const Calendar& b)
    {
      if (a.year!=b.year) {
        return a.year>b.year;
      }

      if (a.month!=b.month) {
        return a.month>b.month;
      }

      return a.dayOfMonth>b.dayOfMonth;
    }

    bool operator>=(const Calendar& a, const Calendar& b)
    {
      if (a.year!=b.year) {
        return a.year>b.year;
      }

      if (a.month!=b.month) {
        return a.month>b.month;
      }

      return a.dayOfMonth>=b.dayOfMonth;
    }

    bool operator==(const Calendar& a, const Calendar& b)
    {
      return a.dayOfMonth==b.dayOfMonth && a.month==b.month && a.year==b.year;
    }

    bool operator!=(const Calendar& a, const Calendar& b)
    {
      return a.dayOfMonth!=b.dayOfMonth || a.month!=b.month || a.year!=b.year;
    }

    /**
     * Subtract b from a and return the number of days between them.
     */
    long operator-(const Calendar& a, const Calendar& b)
    {
      if (a==b) {
        return 0;
      }

      if (b>a) {
        return -(operator-(b,a));
      }

      // It is now save to assume that a>b...

      long result=0;

      Calendar cb(b);

      while (cb.year<a.year) {
        result+=cb.GetDaysOfCurrentYear()-cb.GetDayOfYear()+1;
        cb.AddDays(cb.GetDaysOfCurrentYear()-cb.GetDayOfYear()+1);
      }

      result+=a.GetDayOfYear()-cb.GetDayOfYear();

      return result;
    }

    void operator++(Calendar& a)
    {
      a.AddDays(1);
    }

    /**
     * Set object to the current date (local time).
     */
    void Calendar::SetToCurrent()
    {
      DateTime current;

      SystemTime::GetCurrentLocalTime(current);

      SetDate(current.dayOfMonth,current.month,current.year);
    }

    /**
     * Sets the calendar object to the date represented by
     * the parameters dayOfMonth, month and year.
     */
    void Calendar::SetDate(int dayOfMonth, int month, int year)
    {
      assert(IsValid(dayOfMonth,month,year));

      this->dayOfMonth=dayOfMonth;
      this->month=month;
      this->year=year;
      dayOfWeek=CalculateWeekDay(dayOfMonth,month,year);
      dayOfYear=CalculateDayOfYear(dayOfMonth,month,year);
    }

    int Calendar::GetDayOfWeek() const
    {
      return dayOfWeek;
    }

    int Calendar::GetDayOfMonth() const
    {
      return dayOfMonth;
    }

    int Calendar::GetDayOfYear() const
    {
      return dayOfYear;
    }

    int Calendar::GetMonth() const
    {
      return month;
    }

    int Calendar::GetYear() const
    {
      return year;
    }

    /**
     * Returns the number of days the current month has.
     */
    int Calendar::GetDaysOfCurrentMonth() const
    {
      return CalculateDaysOfMonth(month,year);
    }

    int Calendar::GetDaysOfCurrentYear() const
    {
      if (IsLeapYear()) {
        return 366;
      }
      else {
        return 365;
      }
    }

    std::wstring Calendar::GetISO8601() const
    {
      std::wstring res;

      res.reserve(10);

      res.append(1,(wchar_t)(L'0'+(year / 1000) % 10));
      res.append(1,(wchar_t)(L'0'+(year /  100) % 10));
      res.append(1,(wchar_t)(L'0'+(year /   10) % 10));
      res.append(1,(wchar_t)(L'0'+(year /    1) % 10));
      res.append(L"-");
      res.append(1,(wchar_t)(L'0'+(month /  10) % 10));
      res.append(1,(wchar_t)(L'0'+(month /   1) % 10));
      res.append(L"-");
      res.append(1,(wchar_t)(L'0'+(dayOfMonth / 10) % 10));
      res.append(1,(wchar_t)(L'0'+(dayOfMonth /  1) % 10));

      return res;
    }

    bool Calendar::IsLeapYear() const
    {
      return IsLeapYear(year);
    }

    std::wstring Calendar::GetLocaleDate() const
    {
      struct tm tm;

      tm.tm_sec=0;
      tm.tm_min=0;
      tm.tm_hour=0;
      tm.tm_mday=dayOfMonth;
      tm.tm_mon=month-1;
      tm.tm_year=year-1900;
      tm.tm_wday=0;
      tm.tm_yday=0;
      tm.tm_isdst=-1;

      return TMToWString(&tm,"%x");
    }

    /**
     *Move the year by \p years.
     */
    void Calendar::AddYears(int years)
    {
      year+=years;
      Normalize();
    }

    /**
     * Move the date by months. If the resulting date is invalid, because
     * the resulting month has lesser days that the date hold before, these days
     * will be added wraped around to the next month.
     */
    void Calendar::AddMonths(int months)
    {
      month+=months;
      Normalize();
    }

    /**
     * Move the date by days.
     */
    void Calendar::AddDays(int days)
    {
      dayOfMonth+=days;
      Normalize();
    }

    /**
     * Set the date to the easter sunday of the current year.
     */
    void Calendar::GotoEasterSunday()
    {
      int golden,solar,lunar,pfm,dom,tmp,easter;

      /* the Golden number */
      golden=(year % 19) + 1;

      /* the "Dominical number" - finding a Sunday */
      dom=(year + (year / 4) - (year / 100) + (year / 400)) % 7;
      if (dom<0) {
        dom+=7;
      }

      /* the solar and lunar corrections */
      solar=(year-1600) / 100 - (year-1600) / 400;
      lunar=(((year-1400) / 100) * 8) / 25;

      /* uncorrected date of the Paschal full moon */
      pfm=(3 - (11*golden) + solar - lunar) % 30;
      if (pfm<0) {
        pfm+=30;
      }

      /* corrected date of the Paschal full moon - days after 21st March */
      if ((pfm==29) || ((pfm==28) && (golden>11))) {
        pfm--;
      }

      tmp=(4-pfm-dom) % 7;
      if (tmp<0) {
        tmp+=7;
      }

      /* Easter as the number of days after 21st March */
      easter=pfm+tmp+1;

      if (easter<11) {
        SetDate(easter+21,3,year);
      }
      else {
        SetDate(easter-10,4,year);
      }
    }

    /**
     * Set the date to the date given in 'date'. 'date' has to follow the ISO8601 format,
     * which is defined as 'YYYY-MM-DD' where 'YYYY' stand for the four digit year,
     * 'MM' stands for the two digit month (1..12) and 'DD' stand for the two digit day
     * (1..31).
     */
    bool Calendar::SetISO8601(const std::wstring& date)
    {
      int d,m,y;

      if (date.length()!=10) {
        return false;
      }

      std::wstring tmp(date);

      if (tmp[7]!=L'-') {
        return false;
      }
      tmp.erase(7,1);

      if (tmp[4]!=L'-') {
        return false;
      }
      tmp.erase(4,1);

      for (size_t x=0; x<tmp.length(); x++) {
        if (tmp[x]<L'0' || tmp[x]>L'9') {
          return false;
        }
      }

      y=(tmp[0]-L'0')*1000+(tmp[1]-L'0')*100+(tmp[2]-L'0')*10+(tmp[3]-L'0');
      m=(tmp[4]-L'0')*10+(tmp[5]-L'0');
      d=(tmp[6]-L'0')*10+(tmp[7]-L'0');

      if (!IsValid(d,m,y)) {
        return false;
      }

      SetDate(d,m,y);

      return true;
    }

    std::wstring Calendar::GetWeekDayName(size_t day)
    {
      struct tm t;
      char   buffer[1024]; // Flawfinder: ignore

      t.tm_sec=0;
      t.tm_min=0;
      t.tm_hour=0;
      t.tm_mday=0;
      t.tm_mon=0;
      t.tm_year=70;
      t.tm_wday=1;
      t.tm_yday=0;

      t.tm_wday=day;
      strftime(buffer,1024,"%A",&t);

      return StringToWString(buffer);
    }

    std::wstring Calendar::GetWeekDayNameShort(size_t day)
    {
      struct tm t;
      char   buffer[1024]; // Flawfinder: ignore

      t.tm_sec=0;
      t.tm_min=0;
      t.tm_hour=0;
      t.tm_mday=0;
      t.tm_mon=0;
      t.tm_year=70;
      t.tm_wday=1;
      t.tm_yday=0;

      t.tm_wday=day;
      strftime(buffer,1024,"%a",&t);

      return StringToWString(buffer);
    }

    std::wstring Calendar::GetMonthName(size_t month)
    {
      struct tm t;
      char   buffer[1024]; // Flawfinder: ignore

      t.tm_sec=0;
      t.tm_min=0;
      t.tm_hour=0;
      t.tm_mday=0;
      t.tm_mon=month-1;
      t.tm_year=70;
      t.tm_wday=1;
      t.tm_yday=0;

      strftime(buffer,1024,"%B",&t);

      return StringToWString(buffer);
    }

    std::wstring Calendar::GetMonthNameShort(size_t month)
    {
      struct tm t;
      char   buffer[1024]; // Flawfinder: ignore

      t.tm_sec=0;
      t.tm_min=0;
      t.tm_hour=0;
      t.tm_mday=0;
      t.tm_mon=month-1;
      t.tm_year=70;
      t.tm_wday=1;
      t.tm_yday=0;

      strftime(buffer,1024,"%b",&t);

      return StringToWString(buffer);
    }
  }
}

#ifndef LUM_BASE_DATETIME_H
#define LUM_BASE_DATETIME_H

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

#if defined(__WIN32__) || defined(WIN32)
  #include <Lum/OS/Win32/OSAPI.h>
#endif

#include <ctime>
#include <string>

#include <Lum/Private/ImportExport.h>

namespace Lum {
  namespace Base {

    class LUMAPI DateError
    {
    };

    class LUMAPI DateOutOfRange : public DateError
    {
    };

    enum TimeFormat {
      timeFormatDefault,
      timeFormatHM,
      timeFormatHMS
    };

    /**
      Simple class for storing time without tim zone information.
    */
    class LUMAPI Time
    {
    private:
      unsigned char hour;
      unsigned char minute;
      unsigned char second;

    public:
      Time();
      Time(unsigned char hour, unsigned char minute, unsigned char second=0);

      void Set(unsigned char hour, unsigned char minute, unsigned char second=0);
      void SetHour(unsigned char hour);
      void SetMinute(unsigned char minute);
      void SetSecond(unsigned char second);

      void IncHour();
      void DecHour();
      void IncMinute();
      void DecMinute();
      void IncSecond();
      void DecSecond();

      bool SetISO8601(const std::wstring& date);

      void SetToNow();

      unsigned char GetHour() const;
      unsigned char GetMinute() const;
      unsigned char GetSecond() const;

      std::wstring GetISO8601() const;
      std::wstring GetLocaleTime(TimeFormat format=timeFormatHMS) const;

      friend LUMAPI bool operator<(const Time& a, const Time& b);
      friend LUMAPI bool operator<=(const Time& a, const Time& b);
      friend LUMAPI bool operator==(const Time& a, const Time& b);
      friend LUMAPI bool operator!=(const Time& a, const Time& b);
      friend LUMAPI bool operator>=(const Time& a, const Time& b);
      friend LUMAPI bool operator>(const Time& a, const Time& b);
    };

    extern LUMAPI bool operator<(const Time& a, const Time& b);
    extern LUMAPI bool operator<=(const Time& a, const Time& b);
    extern LUMAPI bool operator==(const Time& a, const Time& b);
    extern LUMAPI bool operator!=(const Time& a, const Time& b);
    extern LUMAPI bool operator>=(const Time& a, const Time& b);
    extern LUMAPI bool operator>(const Time& a, const Time& b);

    /**
      Helper class for class SystemTime
    */
    class LUMAPI DateTime
    {
    public:
      long microsecond;
      int  second;
      int  minute;
      int  hour;
      int  dayOfMonth;
      int  month;
      int  year;
      int  dayOfWeek;
      int  dayOfYear;
    };

    /**
      Abstracts access to the OS clock.

      Time that holds date and time information and also information about the current
      time zone, daylight saving time and difference to universal time zone.
    */
    class LUMAPI SystemTime
    {
    private:
      struct SystemTimePIMPL;

      SystemTimePIMPL *pimpl;

    public:
      SystemTime();
      SystemTime(const SystemTime& systemTime);
      SystemTime(time_t seconds, long microseconds=0);
#if defined(__WIN32__) || defined(WIN32)
      SystemTime(const FILETIME& time);
#endif
      virtual ~SystemTime();

      SystemTime& operator=(const SystemTime& systemTime);

      time_t GetTime() const;
      long GetMicroseconds() const;
      void GetUTCDateTime(DateTime& dateTime) const throw (DateOutOfRange);
      void GetLocalDateTime(DateTime& dateTime) const throw (DateOutOfRange);

      int GetTimezone() const throw (DateOutOfRange);
      bool IsDSTActive() const throw (DateOutOfRange);
      int GetDST() const throw (DateOutOfRange);
      int GetDifferenceToUTC() const throw (DateOutOfRange);
      int GetDaysOfCurrentMonth() const throw (DateOutOfRange);
      bool IsToday() const;

      std::wstring GetUTCISO8601Time() const throw (DateOutOfRange);
      std::wstring GetUTCISO8601DateTime() const throw (DateOutOfRange);

      std::wstring GetLocalISO8601Time() const throw (DateOutOfRange);
      std::wstring GetLocalLocaleDate() const throw (DateOutOfRange);
      std::wstring GetLocalLocaleTime() const throw (DateOutOfRange);
      std::wstring GetLocalLocaleDateTime() const throw (DateOutOfRange);
      std::wstring GetUTCLocaleDateTime() const throw (DateOutOfRange);

      bool IsValid() const;

      void Invalidate();
      void SetTime(time_t seconds, long microseconds=0);
#if defined(__WIN32__) || defined(WIN32)
      void SetTime(const FILETIME& time);
#endif
      void SetToNow();
      void SetUTCDateTime(int hour, int minute, int second, int microsecond,
                            int day, int month, int year) throw (DateOutOfRange);
      void SetLocalDateTime(int hour, int minute, int second, int microsecond,
                            int day, int month, int year) throw (DateOutOfRange);

      void Add(const SystemTime& interval);
      void Add(time_t seconds, long microseconds=0);
      void Sub(const SystemTime& interval);

      static void GetCurrentUTCTime(DateTime& dateTime) throw (DateOutOfRange);
      static void GetCurrentLocalTime(DateTime& dateTime) throw (DateOutOfRange);

      static void Sleep(unsigned long seconds);

      friend LUMAPI bool operator<(const SystemTime& a, const SystemTime& b);
      friend LUMAPI bool operator<=(const SystemTime& a, const SystemTime& b);
      friend LUMAPI bool operator==(const SystemTime& a, const SystemTime& b);
      friend LUMAPI bool operator!=(const SystemTime& a, const SystemTime& b);
      friend LUMAPI bool operator>=(const SystemTime& a, const SystemTime& b);
      friend LUMAPI bool operator>(const SystemTime& a, const SystemTime& b);

      friend LUMAPI time_t operator-(const SystemTime& a, const SystemTime& b);
    };

    extern LUMAPI bool operator<(const SystemTime& a, const SystemTime& b);
    extern LUMAPI bool operator<=(const SystemTime& a, const SystemTime& b);
    extern LUMAPI bool operator==(const SystemTime& a, const SystemTime& b);
    extern LUMAPI bool operator!=(const SystemTime& a, const SystemTime& b);
    extern LUMAPI bool operator>=(const SystemTime& a, const SystemTime& b);
    extern LUMAPI bool operator>(const SystemTime& a, const SystemTime& b);

    extern LUMAPI time_t operator-(const SystemTime& a, const SystemTime& b);

    /**
      simple mid resolution timer based on SystemTime.
    */
    class LUMAPI Timer
    {
    private:
      SystemTime time;
      SystemTime start;

    public:
      Timer();

      time_t GetTime() const;
      long GetMicroseconds() const;

      bool IsStarted() const;
      bool IsPausing() const;
      bool IsStoped() const;

      void Start();
      void Pause();
      void Resume();
      void Reset();
    };

    /**
      Calendar class, holding information about the current day
      in relation to the year, week of year and similar.
    */
    class LUMAPI Calendar
    {
    private:
      int dayOfWeek;  //! Day in the week, starting with 0, aka sunday
      int dayOfMonth; //! Day of current month, starting with 1
      int dayOfYear;  //! Day in the current year, starting with 1
      int month;      //! Month of current year, starting with 1
      int year;       //! The current year

    private:
      void Normalize();

    public:
      static bool IsLeapYear(int year);
      static bool IsValid(int dayOfMonth, int month, int year);

      Calendar();
      Calendar(const Calendar& other);

      // Setter
      void SetToCurrent();
      void SetDate(int dayOfMonth, int month, int year);
      void AddYears(int years);
      void AddMonths(int months);
      void AddDays(int days);
      void GotoEasterSunday();
      bool SetISO8601(const std::wstring& date);

      // Getter
      int GetDayOfWeek() const;
      int GetDayOfMonth() const;
      int GetDayOfYear() const;
      int GetMonth() const;
      int GetYear() const;
      int GetDaysOfCurrentMonth() const;
      int GetDaysOfCurrentYear() const;

      bool IsLeapYear() const;

      std::wstring GetISO8601() const;
      std::wstring GetLocaleDate() const;

      static std::wstring GetWeekDayName(size_t day);
      static std::wstring GetWeekDayNameShort(size_t day);
      static std::wstring GetMonthName(size_t month);
      static std::wstring GetMonthNameShort(size_t month);

      friend LUMAPI bool operator<(const Calendar& a, const Calendar& b);
      friend LUMAPI bool operator<=(const Calendar& a, const Calendar& b);
      friend LUMAPI bool operator>(const Calendar& a, const Calendar& b);
      friend LUMAPI bool operator>=(const Calendar& a, const Calendar& b);
      friend LUMAPI bool operator==(const Calendar& a, const Calendar& b);
      friend LUMAPI bool operator!=(const Calendar& a, const Calendar& b);
      friend LUMAPI long operator-(const Calendar& a, const Calendar& b);
      friend LUMAPI void operator++(Calendar& a);
    };

    extern LUMAPI bool operator<(const Calendar& a, const Calendar& b);
    extern LUMAPI bool operator<=(const Calendar& a, const Calendar& b);
    extern LUMAPI bool operator>(const Calendar& a, const Calendar& b);
    extern LUMAPI bool operator>=(const Calendar& a, const Calendar& b);
    extern LUMAPI bool operator==(const Calendar& a, const Calendar& b);
    extern LUMAPI bool operator!=(const Calendar& a, const Calendar& b);
    extern LUMAPI long operator-(const Calendar& a, const Calendar& b);
    extern LUMAPI void operator++(Calendar& a);
  }
}

#endif

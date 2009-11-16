/*
  libpuTools - Basic types/algorithms/containers

  $Id$

  Copyright (C) 2006 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
  NORWAY
  email: diana@met.no

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
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

// miTime.h : Declarations for miTime class
// �A, 1997
// LBS May 28 1997, comparison functions added

#ifndef __dnmi_miTime__
#define __dnmi_miTime__

#include <time.h>
#include <iostream>

#include "miString.h"
#include "miDate.h"
#include "miClock.h"

namespace miutil{

class miTime {
  miDate Date;
  miClock Clock;

public:
  miTime() {} // produces 'undef' state
  miTime(int y, int m, int d, int h, int min =0, int s =0) :
    Date(y,m,d),
    Clock(h,min,s) {}
  miTime(const miDate& d, const miClock& c) :
    Date(d),
    Clock(c) {}
  miTime(const time_t&);
  miTime(const char* s)
  { setTime(s); }
  miTime(const miString& s)
  { setTime(s); }

  bool undef() const
  { return (Date.undef() || Clock.undef()); }

  void setTime(int y, int m, int d, int h, int min =0, int s =0)
  { Date.setDate(y,m,d); Clock.setClock(h,min,s); }
  void setTime(const miDate& d, const miClock& c)
  { Date=d; Clock=c; }
  void setTime(const miString&);

  static bool isValid(int, int, int, int, int =0, int =0);
  static bool isValid(const miString&);

  miDate date() const
  { return Date; }
  miClock clock() const
  { return Clock; }

  int year() const
  { return Date.year(); }
  int month() const
  { return Date.month(); }
  int day() const
  { return Date.day(); }
  int dayOfYear() const
  { return Date.dayOfYear(); }
  int dayOfWeek() const
  { return Date.dayOfWeek(); }

  int hour() const
  { return Clock.hour(); }
  int min() const
  { return Clock.min(); }
  int sec() const
  { return Clock.sec(); }

  int weekNo() const
  { return Date.weekNo(); }

  miString isoTime(miString delim=" ") const; // usefull delim="T"
  miString isoDate() const
  { return Date.isoDate(); }
  miString isoClock() const
  { return Clock.isoClock(); }

  miString isoTime(bool withmin,  bool withsec) const;
  miString isoClock(bool withmin, bool withsec) const
  { return Clock.isoClock(withmin, withsec); }



  friend bool operator==(const miTime& lhs, const miTime& rhs)
  { return (lhs.Date==rhs.Date) && (lhs.Clock==rhs.Clock); }
  friend bool operator!=(const miTime& lhs, const miTime& rhs)
  { return (lhs.Date!=rhs.Date) || (lhs.Clock!=rhs.Clock); }

  friend bool operator>(const miTime& lhs, const miTime& rhs)
  { return (lhs.Date>rhs.Date) ||
      ((lhs.Date==rhs.Date) && (lhs.Clock>rhs.Clock)); }
  friend bool operator<(const miTime& lhs, const miTime& rhs)
  { return (lhs.Date<rhs.Date) ||
      ((lhs.Date==rhs.Date) && (lhs.Clock<rhs.Clock)); }

  friend bool operator>=(const miTime& lhs, const miTime& rhs)
  { return (lhs.Date>rhs.Date) ||
      ((lhs.Date==rhs.Date) && (lhs.Clock>=rhs.Clock)); }
  friend bool operator<=(const miTime& lhs, const miTime& rhs)
  { return (lhs.Date<rhs.Date) ||
      ((lhs.Date==rhs.Date) && (lhs.Clock<=rhs.Clock)); }

  void addDay(int =1);  // add days
  void addHour(int =1); // add hours
  void addMin(int =1);  // add minutes
  void addSec(int =1);  // add seconds

  static int hourDiff(const miTime&, const miTime&);
  static int minDiff(const miTime&, const miTime&);
  static int secDiff(const miTime&, const miTime&);

  static miTime nowTime()
  { return miTime(miDate::today(),miClock::oclock()); }

  friend std::ostream& operator<<(std::ostream& output, const miTime& t)
  { return output << t.isoTime(); }

  int dst()     const;    // daylight saving time (added by JS/2001)
  int timezone(miString); // hours from UTC

  /// return formatted output (see man date)

  // old version kept for compability
  miString format(miString,const miDate::lang) const;

  // new version - use no/en/de etc for language
  miString format(miString, miString="") const;

  void setDefaultLanguage(const miString l) { Date.setDefaultLanguage(l);}

};

}
#endif

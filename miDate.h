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

/* miDate.h

   Definition of miDate -- a class for manipulating Gregorian dates.
   Leap years, 400-year cycles, the `Year 2000 compliance' and so on,
   are dealt with by using the Julian day number (jdn) as the internal
   representation of the date, making this class able to perform any
   of its operations safely on dates ranging from about 5000000 BCE to
   5000000 CE or thereabouts.

   The standard C library (through time.h) is used in some functions.
   This means that in programs using these functions are `safe' (eg.
   for the future Y2038 dilemma) only to the extent that libc is; this
   safety is implementation-dependent.

   The default value of a date is `undef', and there is a function
   undef() to check for this state.

   Author: LBS. DNMI/FoU 1997, 1998, 1999
   Part of the puTools kit. */

#ifndef __dnmi_miDate__
#define __dnmi_miDate__

#include <iostream>
#include <iomanip>

#include "miString.h"

namespace miutil{

class miDate {
private:
  int Year;
  int Month;
  int Day;
  long jdn; // Julian day number

  miDate& jdntodate(long);

  int intWeekday() const
    { return (((jdn+1)%7)+7)%7; }

  static miString defaultLanguage;

public:
  enum lang {
    English,
    Norwegian
  };

  enum days {
    Sunday=0,
    Monday=1,
    Tuesday=2,
    Wednesday=3,
    Thursday=4,
    Friday=5,
    Saturday=6
  };

  miDate(int y =0, int m =0, int d =0)
    { setDate(y,m,d); }
  miDate(const char* s)
    { setDate(s); }
  miDate(const miString& s)
    { setDate(s); }

  bool undef() const
    { return jdn==0; }

  void setDate(int, int, int);
  void setDate(const miString&);


  //IS THIS THREADSAFE
  static bool isValid(int, int, int);
  static bool isValid(const miString&);

  int year() const
    { return Year; }
  int month() const
    { return Month; }
  int day() const
    { return Day; }

  int dayOfYear() const;
  int dayOfWeek() const
  { return intWeekday(); }
  int daysInMonth() const;
  int daysInYear() const;

  long julianDay() const
    { return jdn; }

  int weekNo() const;

  miDate easterSundayThisYear() const;

  friend int operator==(const miDate& lhs, const miDate& rhs)
    { return (lhs.jdn==rhs.jdn); }
  friend int operator!=(const miDate& lhs, const miDate& rhs)
    { return (lhs.jdn!=rhs.jdn); }
  friend int operator>(const miDate& lhs, const miDate& rhs)
    { return (lhs.jdn>rhs.jdn); }
  friend int operator>=(const miDate& lhs, const miDate& rhs)
    { return (lhs.jdn>=rhs.jdn); }
  friend int operator<(const miDate& lhs, const miDate& rhs)
    { return (lhs.jdn<rhs.jdn); }
  friend int operator<=(const miDate& lhs, const miDate& rhs)
    { return (lhs.jdn<=rhs.jdn); }

  friend long operator-(const miDate& lhs, const miDate& rhs)
    { return (lhs.jdn-rhs.jdn); }

  miDate& operator++()
    { return *this=jdntodate(++jdn); }
  miDate& operator++(int) // postfix
    { return *this=jdntodate(++jdn); }
  miDate& operator--()
    { return *this=jdntodate(--jdn); }
  miDate& operator--(int) // postfix
    { return *this=jdntodate(--jdn); }

  void addDay(const long =1);

  miString isoDate() const;

  // old versions kept for compability
  miString weekday(        const lang ) const;
  miString shortweekday(   const lang ) const;
  miString monthname(      const lang ) const;
  miString shortmonthname( const lang ) const;
  miString format(miString,const lang ) const;

  // new versions send in string lang as en/no/de etc.
  miString weekday(         miString="" ) const;
  miString shortweekday(    miString="" ) const;
  miString monthname(       miString="" ) const;
  miString shortmonthname(  miString="" ) const;
  miString format(miString, miString="" ) const;


  void setDefaultLanguage(const miString l=""){ defaultLanguage=l; }

  static miDate today(); // return system date

  friend std::ostream& operator<<(std::ostream& output, const miDate& d)
    { return output << d.isoDate(); }

};

}
#endif

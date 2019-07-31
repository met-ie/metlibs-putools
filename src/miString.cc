/*
  libpuTools - Basic types/algorithms/containers

  Copyright (C) 2006-2019 met.no

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

#define METLIBS_SUPPRESS_DEPRECATED
#include "miString.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <iomanip>

using namespace puAlgo;

namespace miutil {

std::string from_latin1_to_utf8(const std::string& latin1)
{
  std::string utf8;
  utf8.reserve(latin1.size());
  for (char ch : latin1) {
    unsigned char uch = static_cast<unsigned char>(ch);
    if ((uch & 0x80) != 0) {
      unsigned char uch1 = (0xc0 | (uch >> 6)); /* first byte, simplified since our range is only 8-bits */
      utf8 += static_cast<char>(uch1);

      uch = (0x80 | (uch & 0x3f));
    }
    utf8 += static_cast<char>(uch);
  }
  return utf8;
}

std::string from_number(const int i, const int width, const char fill)
{
    std::ostringstream ost;
    if (width>0) {
#if defined(__WIN32__) || defined(__WIN64__)
        ost << std::setw(width) << std::setfill(static_cast<char>(fill)) << i;
#else
        ost << std::setw(width) << std::setfill(fill) << i;
#endif
    } else {
        ost << i;
    }
    return ost.str();
}

std::string from_number(const double d, const int prec)
{
    std::ostringstream ost;
    if (prec!=-1)
        ost.precision(prec);
    ost << d;
    return ost.str();
}

std::string from_number(const float d, const int prec)
{
    std::ostringstream ost;
    if (prec!=-1)
        ost.precision(prec);
    ost << d;
    return ost.str();
}

namespace {
bool trim_limits(const std::string& text, bool left, bool right, const char* wspace, size_t& begin, size_t& end)
{
  const size_t len = text.length();
  begin = 0;
  end = len;
  if (left) {
    begin = text.find_first_not_of(wspace);
    if (begin == std::string::npos)
      begin = len;
  }
  if (right) {
    const size_t e = text.find_last_not_of(wspace);
    if (e != std::string::npos)
      end = e + 1;
  }
  return begin != 0 || end != len;
}

bool trim_will_change(const std::string& text, bool left, bool right, const char* wspace)
{
  size_t begin, end;
  return trim_limits(text, left, right, wspace, begin, end);
}

std::string do_trim(const std::string& text, size_t begin, size_t end)
{
  return std::string(text.begin() + begin, text.begin() + end);
}

bool trim_changed(std::string& text, bool left, bool right, const char* wspace)
{
  size_t begin, end;
  if (!trim_limits(text, left, right, wspace, begin, end))
    return false;

  text = do_trim(text, begin, end);
  return true;
}
} // namespace

void trim(std::string& text, bool left, bool right, const char* wspace)
{
  trim_changed(text, left, right, wspace);
}

void trim_remove_empty(std::vector<std::string>& vec)
{
  const bool left = true, right = true;
  const char* wspace = whitespaces;

  const auto first_changed = std::find_if(vec.begin(), vec.end(), [=](const std::string& s) { return s.empty() || trim_will_change(s, left, right, wspace); });
  if (first_changed == vec.end())
    return;

  std::vector<std::string> cleaned(vec.begin(), first_changed);
  cleaned.reserve(vec.size());
  for (auto it = first_changed; it != vec.end(); ++it) {
    if (it->empty())
      continue;
    size_t begin, end;
    if (trim_limits(*it, left, right, wspace, begin, end)) {
      if (begin != end)
        cleaned.push_back(do_trim(*it, begin, end));
    } else {
      cleaned.push_back(*it);
    }
  }
  vec = std::move(cleaned);
}

std::vector<std::string> split(const std::string& text, int nos, const char* separator_chars, const bool clean)
{
    std::vector<std::string> vec;
    if (text.empty())
        return vec;

    int splitnumber = 0;
    size_t len = text.length();
    size_t start=(clean? text.find_first_not_of(separator_chars, 0): 0);
    while (start != std::string::npos && start<len) {
        size_t stop = text.find_first_of(separator_chars, start);
        if (stop == std::string::npos || stop > len)
            stop=len;
        vec.push_back(text.substr(start, stop-start));
        
        if(nos)
            if(++splitnumber >= nos) {
                stop++;
                if(stop < len )
                    vec.push_back(text.substr(stop,len-stop));
                break;
            }
        start=(clean? text.find_first_not_of(separator_chars, stop+1): stop+1);
    }
    
    if (clean)
        trim_remove_empty(vec);

    return vec;
}

std::vector<std::string> split_protected(const std::string& text,
                                         const char lb, // left border
                                         const char rb, // right border
                                         const char* separator_chars,
                                         const bool clean)
{
    std::vector<std::string> vec;
    if (text.empty())
        return vec;

    const size_t len = text.length();
    size_t start = (clean ? text.find_first_not_of(separator_chars, 0): 0);

    while (start != std::string::npos && start<len) {
        size_t stop = text.find_first_of(separator_chars, start);
        size_t tmp = start;
        bool isok = false;
        while (not isok) {
            const size_t lbp = text.find(lb, tmp);
            if (lbp != std::string::npos && lbp < stop) {
                const size_t rbp = text.find(rb, lbp+1);
                if (rbp == std::string::npos)
                    return vec;
                tmp = rbp+1;
                if (rbp != std::string::npos && rbp > stop)
                    stop = text.find_first_of(separator_chars, tmp);
            } else {
                isok = true;
            }
        }
        if (stop == std::string::npos || stop>len)
            stop=len;

        vec.push_back(text.substr(start, stop-start));
        start = (clean ? text.find_first_not_of(separator_chars, stop+1): stop+1);
    }
    
    if (clean)
        trim_remove_empty(vec);

    return vec;
}

void remove(std::string& text, const char c)
{
    if (text.empty())
        return;

    for (size_t pos=text.find(c); pos!=std::string::npos; pos=text.find(c))
        text.erase(pos,1);
}


void replace(std::string& text, const char thys, const char that)
{
    const size_t len = text.size();
    for (size_t i=0; i<len; i++)
        if (text[i]==thys)
            text[i]=that;
}

void replace(std::string& text, const std::string& thys, const std::string& that)
{
    if (text.empty())
        return;

    const size_t len1=thys.length(), len2=that.length();
    for (size_t pos=text.find(thys, 0); pos!=std::string::npos; pos=text.find(thys, pos)) {
        text.replace(pos, len1, that);
        pos += len2;
    }
}

bool is_number(const std::string& text)
{
    if (text.empty())
        return false;
    
    bool fl=false;
    std::string::const_iterator i1, i2;
    
    for (i1=text.begin(); i1!=text.end() && isspace(*i1); i1++);
    
    if(i1==text.end())
        return false;
    
    for (i2=text.end()-1; isspace(*i2); i2--);
    
    if(i2!=text.end())
        i2++;
    
    if(*i1=='-' || *i1=='+'){
        i1++;
        
        if(*i1=='e' || *i1=='E')
            return false;
        // only a - or + is not accepted
        if(i1==i2)
            return false;
    }
    
    if(*i1=='e' || *i1=='E')
        return false;
    
    for (; i1!=i2; i1++) {
        if (!isdigit(*i1)) {
            if (*i1=='.') {
                if (fl)
                    return false;
                else
                    fl=true;
            } else
                break;
        }
    }
    
    if(*i1=='e' || *i1=='E'){
        i1++;
        
        // we dont accept a number ending in e or E.
        if(i1==i2)
            return false;
    }
    
    if(*i1=='-' || *i1=='+'){
        i1++;
        
        if(i1==i2)
            return false;
    }
    
    for (; i1!=i2; i1++)
        if (!isdigit(*i1))
            return false;
    
    return true;
}

bool is_int(const std::string& text)
{
    if (text.empty())
        return false;
    
    std::string::const_iterator i1, i2;
    
    for (i1=text.begin(); i1!=text.end() && isspace(*i1); i1++);
    
    if(i1==text.end())
        return false;
    
    for (i2=text.end()-1; isspace(*i2); i2--);
    
    if(i2!=text.end())
        i2++;
    
    if(*i1=='-' || *i1=='+'){
        i1++;
        
        // only a - or + is not accepted
        if(i1==i2)
            return false;
    }
    
    for (; i1!=i2; i1++){
        if (!isdigit(*i1)){
            return false;
        }
    }
    
    return true;
}

int to_int(const std::string& text, const int undefined)
{
    if (text.empty() or not is_number(text))
        return undefined;
    return std::atoi(text.c_str());
}

long to_long(const std::string& text, const long undefined)
{
    if (text.empty() or not is_number(text))
        return undefined;
    return std::atol(text.c_str());
}

float to_float(const std::string& text, const float undefined)
{
#if 1
    float ret = undefined;
    char test = 0;
    std::istringstream iss(text);
    iss.imbue(std::locale::classic());
    iss >> ret >> test;
    if (not iss.eof())
      return undefined;
    return ret;
#else
    if (text.empty() or not is_number(text))
        return undefined;
    return std::atof(text.c_str());
#endif
}

double to_double(const std::string& text, const double undefined)
{
#if 1
    double ret = undefined;
    char test = 0;
    std::istringstream iss(text);
    iss.imbue(std::locale::classic());
    iss >> ret >> test;
    if (not iss.eof())
      return undefined;
    return ret;
#else
    if (text.empty() or not is_number(text))
        return undefined;
    return std::atof(text.c_str());
#endif
}

std::string to_lower(const std::string& text)
{
  return boost::algorithm::to_lower_copy(text, std::locale::classic());
}

std::string to_lower_latin1(const std::string& text)
{
  std::string t(text);
  for (size_t i=0; i<t.size(); ++i) {
    unsigned char c = tolower(t[i]);
    if ((c>=192 && c<=214) || (c>=216 && c<=222))
      c+=32;
    t[i] = c;
  }
  return t;
}

std::string to_upper(const std::string& text)
{
  return boost::algorithm::to_upper_copy(text, std::locale::classic());
}

std::string to_upper_latin1(const std::string& text)
{
  std::string t(text);
  for (size_t i=0; i<t.size(); ++i) {
    unsigned char c = toupper(t[i]);
    if ((c>=224 && c<=246) || (c>=248 && c<=254))
      c-=32;
    t[i] = c;
  }
  return t;
}

std::string append(const std::string& a, const std::string& separator, const std::string& b)
{
  if (b.empty())
    return a;
  if (a.empty())
    return b;
  return a + separator + b;
}

void appendTo(std::string& a, const std::string& separator, const std::string& b)
{
  if (not b.empty()) {
    if (a.empty())
      a = b;
    else
      a += separator + b;
  }
}

extern const char whitespaces[] =" \r\t\n";

// ########################################################################

miString::miString(const char* s)
{
  *this = std::string(s ? s : "");
}

miString::miString(const int i, const int width, const char fill)
    : std::string(from_number(i, width, fill))
{
}

miString::miString(const double d, const int prec)
    : std::string(from_number(d, prec))
{
}

miString::miString(const float f, const int prec)
    : std::string(from_number(f, prec))
{
}

int
miString::toInt( const int undefined ) const
{
  if(empty())
    return undefined;
  if(!isNumber())
    return undefined;
  return atoi(c_str());
}

long
miString::toLong( const long undefined ) const
{
  if(empty())
    return undefined;
  if(!isNumber())
    return undefined;
  return atol(c_str());
}

float
miString::toFloat( const float undefined ) const
{
  if(empty())
    return undefined;
  if(!isNumber())
    return undefined;
  return atof(c_str());
}

double miString::toDouble( const double undefined ) const
{
  if(empty())
    return undefined;
  if(!isNumber())
    return undefined;
  return atof(c_str());
}

void
miString::trim(bool l, bool r, const miString wspace)
{
    miutil::trim(*this, l, r, wspace.c_str());
}

void
miString::remove(const char c)
{
    miutil::remove(*this, c);
}

miString
miString::replace(const char c1, const char c2) const
{
    miString r(*this);
    miutil::replace(r, c1, c2);
    return r;
}

void
miString::replace(const miString& s1, const miString& s2)
{
    miutil::replace(*this, s1, s2);
}

std::vector<miString>
miString::split(const miString s, const bool clean) const
{
  return split(0, s, clean);
}

std::vector<miString>
miString::split(const char c, const bool clean) const
{
  return split(0, c, clean);
}


std::vector<miString>
miString::split(int nos, const char c, const bool clean) const
{
  char cs[2];
  cs[0]=c;
  cs[1]='\0';
  miString s(cs);
  return split(nos, cs, clean);
}

std::vector<miString>
miString::split(int nos, const miString sep, const bool clean) const
{
    const std::vector<std::string> s = miutil::split(*this, nos, sep.c_str(), clean);
    return std::vector<miString>(s.begin(), s.end());
}

// split with protected entities (i.e. () "" etc...



std::vector<miString>
miString::split(const char lb, // left border
    const char rb, // right border
    const miString s,
    const bool clean) const
{
  std::vector<miString> vec;
  if (!empty()) {

    size_t len=length();

    size_t start=(clean ? find_first_not_of(s,0): 0);

    while (start != std::string::npos && start<len) {
      size_t stop;

      stop=find_first_of(s,start);

      size_t  tmp= start;
      bool isok=false;
      while ( !isok ){
        size_t lbp = find(lb,tmp);
        if (lbp != std::string::npos && lbp < stop) {
          size_t rbp = find(rb,lbp+1);
          if ( rbp == npos ) {
            return vec;
          }
          tmp=rbp+1;
          if (rbp != std::string::npos && rbp > stop) {
            stop = find_first_of(s,tmp);
          }
        } else isok=true;
      }

      if (stop == std::string::npos || stop>len)
        stop=len;

      vec.push_back(substr(start,stop-start));

      start=(clean? find_first_not_of(s,stop+1): stop+1);
    }


    if (clean)
      for (size_t i=0; i<vec.size(); i++) {
        vec[i].trim();
        if (!(vec[i].exists()))
          vec.erase(vec.begin()+i--);
      }
  }
  return vec;
}




/**
 * Does this miString represent a number? Leading and trailing space
 * is allowed. A number consists of an optional + or - followed
 * by a decimal number. A decimal number consists of a non empty
 * sequence of digits and possibly a decimal point (.). An optional exponent
 * (e, E) followed by an optional + or -, followed by a non empty sequence
 * of decimal digits.
 *
 * ex. of numbers.
 *       .23  0.23 +0.23 -0.23 -0.23e-10
 *
 * ex. These strings are not considered as numbers.
 *     0.23e
 *     0.23 e+23 it is not allowed with a space before e.
 *     e+23
 *
 * BUGS:
 *   The decimalpoint shold be decided from the active local setting.
 *
 * NOTE:
 *   scanf doesn't accept as numbers strings that begins with a decimal
 *   point. Is this a bug in Linux libc or is it conforming to the C standard?
 *
 *       Ex. The string a=".23" give wrong result with sscanf.
 *           sscanf(a, "%f", &f)
 *
 * \return true if this string is a number false otherwise.
 * \see atof(3), atoi(3), strtod(3), scanf(3), printf(3)
 */
bool miString::isNumber() const
{
    return miutil::is_number(*this);
}

/**
 * Does this miString represent an int? Leading and trailing space
 * is allowed. A number consists of an optional + or - followed
 * by a non empty sequence of digits.
 *
 * ex. of ints.
 *       0 -1 +125
 *
 * \return true if this string is an int false otherwise.
 */
bool miString::isInt() const
{
    return miutil::is_int(*this);
}

miString
miString::upcase(int start, int len) const
{
  if( start < 0 ) start = 0;
  if( len   < 0 ) len   = 0;

  miString s(*this);
  int c, n=s.length(),stop=start+len;

  if( start > n ) return s;
  if( !len      ) stop = n;
  if( stop > n  ) stop = n;

  for (int i=start; i<stop; i++) {
    c=toupper(s[i]);
    if ((c>=224 && c<=246) || (c>=248 && c<=254))
      c-=32;
    s[i] = c;
  }
  return s;
}

miString
miString::downcase(int starti, int leni) const
{
  const size_t start = (starti > 0) ? starti : 0;
  const size_t len   = (leni   > 0) ? leni   : 0;

  miString s(*this);
  size_t c, n=s.length(),stop=start+len;

  if( start > n ) return s;
  if( !len      ) stop = n;
  if( stop > n  ) stop = n;


  for (size_t i=start; i<stop; i++) {
    c=tolower(s[i]);
    if ((c>=192 && c<=214) || (c>=215 && c<=222))
      c+=32;
    s[i] = c;
  }
  return s;
}

} // namespace miutil


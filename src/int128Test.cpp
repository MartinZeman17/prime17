///
/// @file   int128.cpp
/// @brief  Test int128_t and uint128_t types.
///
/// Copyright (C) 2018 Kim Walisch, <kim.walisch@gmail.com>
///
/// This file is distributed under the BSD License. See the COPYING
/// file in the top level directory.
///
#include "int128_t.hpp"
using namespace primecount;

#if defined(HAVE_INT128_T)

#include <cstdlib>
#include <sstream>
#include <string>
#include <iostream>

using std::endl;
using std::cout;


void check(bool OK)
{
  cout << "   " << (OK ? "OK" : "ERROR") << "\n";
  if (!OK)
    exit(1);
}

#endif

int int128Test()
{
  static_assert(prt::numeric_limits<int8_t>::max() == std::numeric_limits<int8_t>::max(), 
                "prt::numeric_limits<int8_t>::max() is broken");

  static_assert(prt::numeric_limits<uint64_t>::max() == std::numeric_limits<uint64_t>::max(), 
                "prt::numeric_limits<uint64_t>::max() is broken");

  static_assert(prt::is_integral<uint64_t>::value, 
                "prt::is_integral<uint64_t> != true");

  static_assert(!prt::is_integral<double>::value, 
                "prt::is_integral<double> != false");

  static_assert(prt::is_signed<int64_t>::value, 
                "prt::is_signed<int64_t> != true");

  static_assert(!prt::is_signed<uint64_t>::value, 
                "prt::is_signed<uint64_t> != false");

  static_assert(!prt::is_unsigned<int64_t>::value, 
                "prt::is_unsigned<int64_t> != false");

  static_assert(prt::is_unsigned<uint64_t>::value, 
                "prt::is_unsigned<uint64_t> != true");

#if defined(HAVE_INT128_T)

  static_assert(prt::numeric_limits<uint128_t>::max() == ~((uint128_t) 0), 
                "prt::numeric_limits<uint128_t>::max() is broken");

  static_assert(prt::is_integral<int128_t>::value, 
                "prt::is_integral<int128_t> != true");

  static_assert(prt::is_integral<uint128_t>::value, 
                "prt::is_integral<uint128_t> != true");

  static_assert(prt::is_signed<int128_t>::value, 
                "prt::is_signed<int128_t> != true");

  static_assert(!prt::is_signed<uint128_t>::value, 
                "prt::is_signed<uint128_t> != false");

  static_assert(!prt::is_unsigned<int128_t>::value, 
                "prt::is_unsigned<int128_t> != false");

  static_assert(prt::is_unsigned<uint128_t>::value, 
                "prt::is_unsigned<uint128_t> != true");

  {
    std::ostringstream s;
    s << (((int128_t) 1 ) << 100);
    cout << "2^100 = " << s.str();
    check(s.str() == "1267650600228229401496703205376");
  }

  {
    std::ostringstream s;
    s << prt::numeric_limits<int128_t>::min() + 1;
    cout << "-2^127+1 = " << s.str();
    check(s.str() == "-170141183460469231731687303715884105727");
  }

  {
    std::ostringstream s;
    s << prt::numeric_limits<int128_t>::max();
    cout << "2^127-1 = " << s.str();
    check(s.str() == "170141183460469231731687303715884105727");
  }

  {
    std::ostringstream s;
    s << prt::numeric_limits<uint128_t>::max();
    cout << "2^128-1 = " << s.str();
    check(s.str() == "340282366920938463463374607431768211455");
  }

#endif

  cout << endl;
  cout << "All tests passed successfully!" << endl;

  uint128_t aa = prt::numeric_limits<uint128_t>::max();
  uint128_t bb = 3;
  uint128_t cc = aa/bb;
  {
    std::ostringstream s;
    s << cc;
    cout << "cc = " << s.str();
    // check(s.str() == "340282366920938463463374607431768211455");
  }
 
  return 0;
} 
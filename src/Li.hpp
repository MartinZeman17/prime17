#ifndef LI_HPP
#define LI_HPP

#include "int128_t.hpp"
using namespace primecount;


namespace Li{
    long double li(const long double x);

    long double Li(const long double x);

    const long double li2 = 1.045163780117492784844588889194613136522615578151L;

    long double NoPrimesInt(const unsigned long long Beg, const unsigned long long End);

    long double NoPrimesInt(const uint128_t Beg, const uint128_t End);
}
#endif
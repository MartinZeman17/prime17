#include "Li.hpp"

#include <math.h>

// #include "UInt128tompz.hpp"


//https://en.wikipedia.org/wiki/Logarithmic_integral_function#Series_representation
long double Li::li(const long double x)
{
    if (x <= 1)
        return 0;

    long double gamma = 0.57721566490153286060651209008240243104215933593992L;
    long double sum = 0;
    long double inner_sum = 0;
    long double factorial = 1;
    long double p = -1;
    long double q = 0;
    long double power2 = 1;
    long double logx = log(x);
    int k = 0;

    for (int n = 1; true; n++)
    {
        p *= -logx;
        factorial *= n;
        q = factorial * power2;
        power2 *= 2;

        for (; k <= (n - 1) / 2; k++)
        inner_sum += 1.0L / (2 * k + 1);

        auto old_sum = sum;
        sum += (p / q) * inner_sum;

        // Not converging anymore
        if (abs(sum - old_sum) < std::numeric_limits<long double>::epsilon())
        break;
    }
    long double ret = gamma + log(logx) + sqrt(x) * sum;
    return ret;
}

long double Li::Li(const long double x)
{

const long double li2 = 1.045163780117492784844588889194613136522615578151L;
if (x <= li2)
    return 0;
else
    return li(x) - li2;
}

long double Li::NoPrimesInt(const long double Beg, const long double End)
{
    return li(End) - li(Beg);
}


long double Li::NoPrimesInt(const unsigned long long Beg, const unsigned long long End)
{
    return li(End) - li(Beg);
}

long double Li::NoPrimesInt(const uint128_t Beg, const uint128_t End){

    // //compute prime count estimation
    long double B=0;
    long double E=0;

    B = (long double) Beg;
    E = (long double) End;
    return li(E) - li(B);   
}

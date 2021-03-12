
#include "BitRange.hpp"
#include "Li.hpp"

#include <iostream>


using std::endl;
using std::cout;


void BitRangeTest()
{
    const BitRange::NoPrimesIntFuncT functToCall = &Li::NoPrimesInt;

    // all bits indexed from 0 to 63
    int BitCond1 =3;
    int BitCond2 =5;
    int BitEnd = 15;

    cout << "BitCond1: " << BitCond1 << endl << "BitCond2: "  << BitCond2 << endl << "BitEnd " << BitEnd << endl;
    long double NoOfPrimesInIntervalLi = BitRange::BitRange2BitCond(BitCond1, BitCond2, BitEnd, functToCall);
    cout << "Li (expected number of primes) for BitRange: " << NoOfPrimesInIntervalLi << endl;
}


  /*
    for(int i = 1;i<=65;i++){
        long double s,e;
        s = pow(2,i);
        e = powl(2,i+1);
        long double pi = li(e) - li(s);
        cout << i << " " << pi << endl;
    }
    return 0;
    */

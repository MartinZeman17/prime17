#ifndef __BITRANGE_HPP
#define __BITRANGE_HPP

/*
Usage:
    NoPrimesIntFuncT functToCall = &NoPrimesInt;
    long double NoOfPrimesInBitRange =BitRange2BitCond(0,1, 3, functToCall);
*/
namespace BitRange{
    typedef long double (*NoPrimesIntFuncT)(const unsigned long long Start, const unsigned long long End);

    /*
    Iterates through range of numbers smaller than 2^BitEnd.
    All generated numbers have exactly two bits (BitCond1, BitCond2) set to 1.
    For each number calls function of type NoPrimesIntFuncT and summs all results up.

    BitEnd = 0..63
    BitCond1,BitCond2 can be equal or diffrent
    BitCond1,2<=BitEnd
    Eg. 1, 4,7 => XXX1XX1X
    */
    long double BitRange2BitCond(const int BitCond1, const int BitCond2, const int BitEnd, NoPrimesIntFuncT funct);

}

#endif
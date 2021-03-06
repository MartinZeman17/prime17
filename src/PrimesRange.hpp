#ifndef PRIMESRANGE_HPP
#define PRIMESRANGE_HPP

#include <iostream>

const std::string StorageDir = "PrimesStorage"; 

using std::string;

class PrimesRange{

    private:
    unsigned long long p_Begin=0;
    unsigned long long p_End=0;
    unsigned long long p_MinPrime=0;
    unsigned long long p_MaxPrime=0;
    unsigned long long p_PrimesCount=0;
    uint32_t * p_PrimesArray32=nullptr;
    uint64_t * p_PrimesArray64=nullptr;

    public:
    PrimesRange(unsigned long long Begin, unsigned long long End);
    
    // object copy disabled (compiler should complain)
    // copy constructor not implemented 
    PrimesRange(const PrimesRange&) = delete;
    
    // copy assignment not implemented 
    PrimesRange& operator=(const PrimesRange&) = delete;

    ~PrimesRange();

    const unsigned long long & Begin = p_Begin;
    const unsigned long long & End = p_End;
    const unsigned long long & MinPrime = p_MinPrime;
    const unsigned long long & MaxPrime = p_MaxPrime;
    const unsigned long long & PrimesCount = p_PrimesCount;
    

    // ToDo: May be there is a better sollution to implement read-only pointer
    //       Address to pointer is not allowed in C++
    //       Further study required 
    // Dirty: implementation in a header
    inline uint32_t * PrimesArray32() const {return p_PrimesArray32;}
    inline uint64_t * PrimesArray64() const {return p_PrimesArray64;}

};

#endif

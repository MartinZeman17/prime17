#ifndef PRIMESRANGE_HPP
#define PRIMESRANGE_HPP

#include <iostream>

const std::string StorageDir = "PrimesStorage"; 

using std::string;

class PrimesRange{

    private:
    unsigned long long pBegin_=0;
    unsigned long long pEnd_=0;
    unsigned long long p_MinPrime=0;
    unsigned long long pMaxPrime_=0;
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

    const unsigned long long & Begin = pBegin_;
    const unsigned long long & End = pEnd_;
    const unsigned long long & MinPrime = p_MinPrime;
    const unsigned long long & MaxPrime = pMaxPrime_;
    const unsigned long long & PrimesCount = p_PrimesCount;
    

    inline uint32_t * PrimesArray32() const {return p_PrimesArray32;}
    inline uint64_t * PrimesArray64() const {return p_PrimesArray64;}

};

#endif

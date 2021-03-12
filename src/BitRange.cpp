#include "BitRange.hpp"

#include <stdint.h>



long double BitRange::BitRange2BitCond(const int BitCond1, const int BitCond2, const int BitEnd, const NoPrimesIntFuncT funct)
{
  long double ret = 0;
  int BC1, BC2;

  if (BitCond1 <= BitCond2)
  {
    BC1 = BitCond1;
    BC2 = BitCond2;
  }
  else
  {
    BC1 = BitCond2;
    BC2 = BitCond1;
  }

  // const 1 has to be cast to uint64 bit first
  unsigned long long maskBC2 = static_cast<uint64_t>(1) << BC2;
  unsigned long maskBC1 = static_cast<uint64_t>(1) << BC1;
  unsigned long long maskEnd = maskBC1;
  if (maskEnd) maskEnd--;

  unsigned long long MasterEnd = static_cast<uint64_t>(1) << (BitEnd - BC2);
  for (unsigned long long Master = 0; Master < MasterEnd; Master++)
  {
    unsigned long long MasterOffset = Master << (BC2 + 1);
    MasterOffset = MasterOffset | maskBC2;
    
    unsigned long long Slave =0;
    int SlaveBits = (BC2 - BC1 - 1);
    unsigned long long SlaveEnd = 0;
    if (SlaveBits>0) SlaveEnd = static_cast<uint64_t>(1) << SlaveBits;
    do {
      unsigned long long SlaveOffset = Slave << (BC1 +1);
      unsigned long long Beg = MasterOffset | SlaveOffset | maskBC1;
      unsigned long long End = Beg | maskEnd;

      ret += funct(Beg, End + 1);

/*
      std::bitset<64> b(Beg);
      std::bitset<64> e(End);
      cout << b << " " << Beg << endl;
      cout << e << " " << End << endl;
      cout << endl;
*/

      Slave++;
    } while(Slave < SlaveEnd);
  }
  return ret;
}



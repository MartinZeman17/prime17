#include "BitRange.hpp"
// #include "Li.hpp"

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
  uint64_t maskBC2 = static_cast<uint64_t>(1) << BC2;
  uint64_t maskBC1 = static_cast<uint64_t>(1) << BC1;
  uint64_t maskEnd = maskBC1;
  if (maskEnd) maskEnd--;

  uint64_t MasterEnd = static_cast<uint64_t>(1) << (BitEnd - BC2);
  for (uint64_t Master = 0; Master < MasterEnd; Master++)
  {
    uint64_t MasterOffset = Master << (BC2 + 1);
    MasterOffset = MasterOffset | maskBC2;
    
    uint64_t Slave =0;
    int SlaveBits = (BC2 - BC1 - 1);
    uint64_t SlaveEnd = 0;
    if (SlaveBits>0) SlaveEnd = static_cast<uint64_t>(1) << SlaveBits;
    do {
      uint64_t SlaveOffset = Slave << (BC1 +1);
      uint64_t Beg = MasterOffset | SlaveOffset | maskBC1;
      uint64_t End = Beg | maskEnd;

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



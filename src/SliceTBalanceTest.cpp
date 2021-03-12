#include "SliceTBalance.hpp"

void SliceTBalanceTest(){
    SliceTBalance STB;

    // STB.CompIntervalJoin(26, true);

    // STB.CompIntervalJoin(26, true);

    for (unsigned int i = 1; i<=36; i++){
        STB.CompIntervalJoin(i, true);    
    }

}

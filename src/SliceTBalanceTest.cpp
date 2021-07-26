#include "SliceTBalance.hpp"
#include "Log.hpp"

void SliceTBalanceCorrection(){
    SliceTBalance STB;
    for (unsigned int i = 20; i<=35; i++){
        Log::out() << "\nPower: " << i << "\n";
        STB.CompCorrectionLi(i);
    }
}

void SliceTBalanceTest(){
    SliceTBalance STB;

    // STB.CompIntervalJoin(26, true);

    // STB.CompIntervalJoin(26, true);

    for (unsigned int i = 1; i<=36; i++){
        STB.CompIntervalJoin(i, true);    
    }

}

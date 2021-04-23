#include "Binomials.hpp"
#include "Log.hpp"

#include <gmp.h>

void Binomials::ComputeBinomials(){
    mpz_t mpz_res;
    mpz_init(mpz_res);

    for (unsigned int n=0; n<=65; n++){
        for (unsigned int k=0; k<=n; k++){
            mpz_bin_uiui(mpz_res, n, k);
            Log::out() <<  "insert into binomials (bin_n, bin_k, bin_val) values (" << n << ", " << k << ", " << mpz_res << ");\n";
         }
    }
}
#include "PrimeTest.hpp"

#include <gmp.h>
#include <stdint.h>
#include <stdint.h>
#include <stdlib.h> 
#include <iostream>

#include "utils_mpz.hpp"

using std::endl;
using std::cout;


void PrimeTest::LoadPrimes32(){
    P = PrimesRangeService::GetPrimesRange(0,UINT32_MAX);
    PrimesCount32 = P->PrimesCount;
    Primes32 = P->PrimesArray32();
}

void PrimeTest::LoadPrimes64(){
    P = PrimesRangeService::GetPrimesRange( (unsigned long long) UINT32_MAX + 1, (unsigned long long) UINT32_MAX + UINT32_MAX / 2  );
    PrimesCount64 = P->PrimesCount;
    Primes64 = P->PrimesArray64();
}

void PrimeTest::Constructor(unsigned int iBits, bool bLoadPrimes32, bool bLoadPrimes64){

    /* Allocate the static variables used in Miller's test. */
    mpz_init(mpzB);  /* Never needs more than one limb */
    //iBits=mp_bits_per_limb*(1 + mpz_size(mpzN));
    if(iBits < 512)iBits=512;
    mpz_init2(mpzNm1, iBits);
    mpz_init2(mpzOne, iBits);
    mpz_set_si(mpzOne, 1); //ok
    mpz_init2(mpzd, iBits);
    mpz_init2(mpzRem, 2*iBits);  /* must contain products */

    ulDmax=0;  /* tracks global max of Lucas-Selfridge |D| */

    mpz_init2(mpzN, iBits);


    mpz_init2(mpz_X, iBits);

    //mpz variables for DivByPrime
    mpz_init2(tmp, iBits);
    mpz_init2(mpz_Prime64, iBits);  // to store ull under windows environment

    DHist1.assign(C_HistItems+1,0);
    DHist10.assign(C_HistItems+1,0);
    DHist100.assign(C_HistItems+1,0);
    DHist1000.assign(C_HistItems+1,0);

    MaxD=0;
    mpz_init2(NumberWithMaxD, iBits);
    mpz_set_ui(NumberWithMaxD, 0); //ok

    if (bLoadPrimes32){
        LoadPrimes32();
    }

    if (bLoadPrimes64){
        LoadPrimes64();
    }
    
}

PrimeTest::PrimeTest(unsigned int iBits, bool bLoadPrimes32, bool bLoadPrimes64){
    Constructor(iBits, bLoadPrimes32, bLoadPrimes64);
}

PrimeTest::PrimeTest(unsigned int iBits){
    Constructor(iBits, false, false);
}

PrimeTest::~PrimeTest(){
    mpz_clear(mpzB);
    mpz_clear(mpzNm1);
    mpz_clear(mpzd);
    mpz_clear(mpzRem);
    mpz_clear(mpzOne);
    mpz_clear(mpzN);

    mpz_clear(mpz_X);

    mpz_clear(tmp);
    mpz_clear(mpz_Prime64);
}


// bool PrimeTest::iMillerRabinBase2(const uint128_t & N){
//     long iB=2;
//     // long iComp2;
//     uint32_t s; //stores bit position
//     uint32_t j; //stores bit position
//     uint128_t Nm1;


//     // const uint128_t C_Bit127 = (((int128_t) 1 ) << 127);
//     // cout << (prt::numeric_limits<uint128_t>::max() & C_Bit127) << endl;
    
// // mpz_t F;
// // mpz_t S;
// // mpz_init(F);
// // mpz_init(S);

// // mpz_set_ui(F,3);
// // mpz_set_ui(S, 99);
// // cout << mpz_divisible_p(F,S) << endl;
// // cout << mpz_divisible_p(S,F) << endl;


// // mpz_set_ui(S, 98);
// // cout << mpz_divisible_p(F,S) << endl;
// // cout << mpz_divisible_p(S,F) << endl;


// // mpz_divisible_p(mpzB, mpzN)) 

// mpz_t mpzN;
// mpz_init(mpzN);
// // Utils::UInt128tompz(N, mpzN);
// Utils::mpz_set_ul128(mpzN, N);


//     /* First take care of all N < 3 and all even N. */
//     if (N<2) return false;
//     if (N==2) return true;
//     if (N % 2 == 0) return false;
    

//     /* Check for valid input. Miller's test requires B > 1, and N must not
//     divide B exactly. Choose B=2 and B<--B+1 if these problems arise.
//     This is technically a deviation from the pure Miller's test, but
//     avoids the necessity of handling an error return of -1. */

// if(iB < 2)iB=2;
// mpz_set_si(mpzB, iB);  //ok
// // if(mpz_divisible_p(mpzB, mpzN))mpz_add_ui(mpzB, mpzB, 1);

//     /* Now compute d and s, where d is odd and N - 1 = (2^s)*d. */

// mpz_sub_ui(mpzNm1, mpzN, 1); //ok
// s=(uint32_t) mpz_scan1(mpzNm1, 0);   //returns first true bit index. 
// mpz_tdiv_q_2exp(mpzd, mpzNm1, s); 

//     Nm1=N-1;
//     const uint128_t C_Bit1= (int128_t) 1;
//     uint128_t sPower = 1;
//     uint128_t Nshift=Nm1;
//     for (s=0; (Nshift & C_Bit1)==0; s++ ){
//         Nshift >>= 1;
//         sPower <<= 1; 
//     }

//     uint128_t d = Nm1 / sPower;

//  /* Now proceed with the Miller's algorithm. First, if B^d is
//     congruent to 1 mod N, N is a strong probable prime to base B. */

//     mpz_powm(mpzRem, mpzB, mpzd, mpzN);
//     if(mpz_cmp_si(mpzRem, 1)==0) return(true); //ok

//     if(mpz_cmp(mpzRem, mpzNm1)==0) return(true);  /* j=0 case */
//     for(j=1; j < s; j++)
//     {
//         mpz_mul(mpzRem, mpzRem, mpzRem);
//         mpz_mod(mpzRem, mpzRem, mpzN);
//         if(mpz_cmp(mpzRem, mpzNm1)==0) return(true);
//         if(mpz_cmp(mpzRem, mpzOne)==0) return(false);
//         }


//      return false;
// }


bool PrimeTest::iMillerRabinBase2(const mpz_t & mpzN){
    /* Test N for primality using the Miller's strong probable prime
    test with base B. See Gary Miller's famous paper ("Riemann's
    hypothesis and tests for primality," Journal of Computation and
    System Science, 1976, Volume 13, pp 300-317).

    Returns 1 if N is a prime or a base-B strong probable prime.
    Returns 0 if N is definitely not a prime (composite or < 2).

    NOTE 1: Some will not regard this as a "pure" Miller's test with
    base B, since certain adjustments are made, prior to applying the
    algorithm, in order to work around invalid input values and
    improve performance:

    1) N < 3 and even N are screened out first.
    2) Multiples of the small primes (to qMax=# of binary digits in N)
        are returned as composite.
    3) Values of B < 2 are replaced by B=2.
    4) If N divides B exactly, B is replaced by B+1.

    If such adjustments are not made, a third return value (e.g., -1)
    must be allowed, indicating invalid input and an indeterminate result,
    and complicating the calling source code.

    NOTE 2: Not all authorities agree exactly on the conditions for
    Miller's test. Some allow B < 2 (Rosen, "Elementary number theory and
    its applications," 3rd ed., 1993, pp. 195-200), although there are good
    reasons to exclude such values. On the other hand, some require
    1 < B < N (Ribenboim, "The new book of prime number records,"
    3rd ed., 1996, pp. 113-116, 143-146). As far as I know, no one
    allows N to divide B, as this produces "false negatives"; e.g.,
    N=3 and B=6 fails Miller's test, thus indicating N=3 as composite.
    In practice, there appears to be no good reason to use any B < 2,
    and in fact its value is almost always chosen to be a small
    (positive) prime number. Furthermore, in my opinion, refusing to
    first screen out even values of N and N < 3 gratuitously adds
    unnecessary complication to the test.
    */

    long iB=2;
    //mpz_t mpzB, mpzNm1, mpzd, mpzRem, mpzOne; //moved to a class
    long iComp2;
    //long iBits;
    uint32_t s; //stores bit position
    uint32_t j; //stores bit position
    //long q;
    //long digits;
    //unsigned long qMax;
    //unsigned long ulDmax;  /* tracks global max of Lucas-Selfridge |D| */

    // mpz_out_str(stdout, 10, mpzN);
    // cout << endl;

    /* First take care of all N < 3 and all even N. */
    iComp2=mpz_cmp_si(mpzN, 2); //ok
    if(iComp2 < 0) return(false);        /* N < 2 is by convention not prime */
    if(iComp2==0) return(true);         /* N=2 is prime */
    if(mpz_even_p(mpzN)) return(false);  /* Even N > 2 is composite */


    /* Check for valid input. Miller's test requires B > 1, and N must not
    divide B exactly. Choose B=2 and B<--B+1 if these problems arise.
    This is technically a deviation from the pure Miller's test, but
    avoids the necessity of handling an error return of -1. */

    if(iB < 2)iB=2;
    mpz_set_si(mpzB, iB);  //ok
    if(mpz_divisible_p(mpzB, mpzN))mpz_add_ui(mpzB, mpzB, 1); //ok

    /* Now compute d and s, where d is odd and N - 1 = (2^s)*d. */

    mpz_sub_ui(mpzNm1, mpzN, 1); //ok
    s=(uint32_t) mpz_scan1(mpzNm1, 0);   //returns first true bit index. 
    mpz_tdiv_q_2exp(mpzd, mpzNm1, s);

    /* Now proceed with the Miller's algorithm. First, if B^d is
    congruent to 1 mod N, N is a strong probable prime to base B. */

    mpz_powm(mpzRem, mpzB, mpzd, mpzN);
    if(mpz_cmp_si(mpzRem, 1)==0) return(true); //ok

    /* Now calculate B^((2^j)*d), for j=0,1,...,s-1 by successive
    squaring. If any of these is congruent to -1 mod N, N is a
    sprp base B. Start with j=0 and B^d, already computed.
    Miller's test uses repeated modular squaring in place of repeated
    modular exponentiation for speed (squaring is an order of
    magnitude faster). */

    if(mpz_cmp(mpzRem, mpzNm1)==0) return(true);  /* j=0 case */
    for(j=1; j < s; j++)
    {
        mpz_mul(mpzRem, mpzRem, mpzRem);
        mpz_mod(mpzRem, mpzRem, mpzN);
        if(mpz_cmp(mpzRem, mpzNm1)==0) return(true);
        if(mpz_cmp(mpzRem, mpzOne)==0) return(false);
        }

    //mpz_clears(mpzB, mpzNm1, mpzd, mpzRem, mpzSqrt, mpzOne);  

    return(false);
}

bool PrimeTest::iMillerRabinBase2(const unsigned long long & X){
    utils_mpz:: mpz_set_ull(mpz_X, X);
    return iMillerRabinBase2(mpz_X);
}

bool PrimeTest::iMillerRabinBase2(const uint128_t & X){
    utils_mpz:: mpz_set_ul128(mpz_X, X);
    return iMillerRabinBase2(mpz_X);
}


/*
static int
mod_eq_m1 (mpz_srcptr x, mpz_srcptr m)
{
  mp_size_t ms;
  mp_srcptr mp, xp;
  ms = SIZ (m);
  if (SIZ (x) != ms)
    return 0;
  ASSERT (ms > 0);
  mp = PTR (m);
  xp = PTR (x);
  ASSERT ((mp[0] - 1) == (mp[0] ^ 1)); // n is odd
  if ((*xp ^ CNST_LIMB(1) ^ *mp) != CNST_LIMB(0)) // xp[0] != mp[0] - 1 
    return 0;
  else
    {
      int cmp;
      --ms;
      ++xp;
      ++mp;
      MPN_CMP (cmp, xp, mp, ms);
      return cmp == 0;
    }
}

static int
millerrabin (mpz_srcptr n, mpz_ptr x, mpz_ptr y,
	     mpz_srcptr q, unsigned long int k)
{
  unsigned long int i;

  mpz_powm (y, x, q, n);

  if (mpz_cmp_ui (y, 1L) == 0 || mod_eq_m1 (y, n))
    return 1;

  for (i = 1; i < k; i++)
    {
        (y, y, 2L, n);
      if (mod_eq_m1 (y, n))
	return 1;
    //    y == 1 means that the previous y was a non-trivial square root
	//  of 1 (mod n). y == 0 means that n is a power of the base.
	//  In either case, n is not prime. 
      if (mpz_cmp_ui (y, 1L) <= 0)
	return 0;
    }
  return 0;
}

*/

/**********************************************************************/
bool PrimeTest::iLucasSelfridge(const mpz_t & mpzN){
    /* Test mpzN for primality using Lucas's test with Selfridge's parameters.
    Returns 1 if mpzN is prime or a Lucas-Selfridge pseudoprime. Returns
    0 if mpzN is definitely composite. Note that a Lucas-Selfridge test
    typically requires three to seven times as many bit operations as a
    single Miller's test. The frequency of Lucas-Selfridge pseudoprimes
    appears to be roughly four times that of base-2 strong pseudoprimes;
    the Baillie-PSW test is based on the hope (verified by the author,
    May, 2005, for all N < 10^13; and by Martin Fuller, January, 2007,
    for all N < 10^15) that the two tests have no common pseudoprimes. */

    int iComp2, iP, iJ, iSign;
    long lD, lQ;
    unsigned long lDabs;
    unsigned long ulMaxBits, ulNbits, ul, ulGCD;
    mpz_t mpzU, mpzV, mpzNplus1, mpzU2m, mpzV2m, mpzQm, mpz2Qm,
        mpzT1, mpzT2, mpzT3, mpzT4, mpzD;

    #undef RETURN
    #define RETURN(n)           \
    {                         \
    mpz_clear(mpzU);          \
    mpz_clear(mpzV);          \
    mpz_clear(mpzNplus1);     \
    mpz_clear(mpzU2m);        \
    mpz_clear(mpzV2m);        \
    mpz_clear(mpzQm);         \
    mpz_clear(mpz2Qm);        \
    mpz_clear(mpzT1);         \
    mpz_clear(mpzT2);         \
    mpz_clear(mpzT3);         \
    mpz_clear(mpzT4);         \
    mpz_clear(mpzD);          \
    return(n);                \
    }

    /* This implementation of the algorithm assumes N is an odd integer > 2,
    so we first eliminate all N < 3 and all even N. As a practical matter,
    we also need to filter out all perfect square values of N, such as
    1093^2 (a base-2 strong pseudoprime); this is because we will later
    require an integer D for which Jacobi(D,N) = -1, and no such integer
    exists if N is a perfect square. The algorithm as written would
    still eventually return zero in this case, but would require
    nearly sqrt(N)/2 iterations. */

    iComp2=mpz_cmp_si(mpzN, 2); //ok
    if(iComp2 < 0) return(false);
    if(iComp2==0) return(true);
    if(mpz_even_p(mpzN)) return(false);
    if(mpz_perfect_square_p(mpzN)) return(false);

    /* Allocate storage for the mpz_t variables. Most require twice
    the storage of mpzN, since multiplications of order O(mpzN)*O(mpzN)
    will be performed. */

    ulMaxBits=(unsigned long) (2*mpz_sizeinbase(mpzN, 2) + mp_bits_per_limb);
    mpz_init2(mpzU, ulMaxBits);
    mpz_init2(mpzV, ulMaxBits);
    mpz_init2(mpzNplus1, ulMaxBits);
    mpz_init2(mpzU2m, ulMaxBits);
    mpz_init2(mpzV2m, ulMaxBits);
    mpz_init2(mpzQm, ulMaxBits);
    mpz_init2(mpz2Qm, ulMaxBits);
    mpz_init2(mpzT1, ulMaxBits);
    mpz_init2(mpzT2, ulMaxBits);
    mpz_init2(mpzT3, ulMaxBits);
    mpz_init2(mpzT4, ulMaxBits);
    mpz_init(mpzD);

    /* Find the first element D in the sequence {5, -7, 9, -11, 13, ...}
    such that Jacobi(D,N) = -1 (Selfridge's algorithm). Although
    D will nearly always be "small" (perfect square N's having
    been eliminated), an overflow trap for D is present. */

    lDabs=5;
    iSign=1;
    while(1)
    {
    lD=iSign*lDabs;
    iSign = -iSign;
    ulGCD=mpz_gcd_ui(nullptr, mpzN, lDabs); //ok, ul type, overflow throws an exception
    /* if 1 < GCD < N then N is composite with factor lDabs, and
        Jacobi(D,N) is technically undefined (but often returned
        as zero). */
    if((ulGCD > 1) && mpz_cmp_ui(mpzN, ulGCD) > 0) RETURN(false); //ok, ul type
    mpz_set_si(mpzD, lD);  //ok
    iJ=mpz_jacobi(mpzD, mpzN);
    if(iJ==-1)break;
    lDabs += 2;
    if(lDabs > ulDmax)ulDmax=lDabs;  /* tracks global max of |D| */
    if(lDabs > INT32_MAX-2)
        {
        fprintf(stderr, "\n ERROR: D overflows signed long in Lucas-Selfridge test.");
        fprintf(stderr, "\n N=");
        mpz_out_str(stderr, 10, mpzN);
        fprintf(stderr, "\n |D|=%ld\n\n", lDabs);
        exit(EXIT_FAILURE);
        }
    }

    iP=1;         /* Selfridge's choice */
    lQ=(1-lD)/4;  /* Required so D = P*P - 4*Q */

    /* NOTE: The conditions (a) N does not divide Q, and
    (b) D is square-free or not a perfect square, are included by
    some authors; e.g., "Prime numbers and computer methods for
    factorization," Hans Riesel (2nd ed., 1994, Birkhauser, Boston),
    p. 130. For this particular application of Lucas sequences,
    these conditions were found to be immaterial. */

    mpz_add_ui(mpzNplus1, mpzN, 1); /* ok, must compute U_(N - Jacobi(D,N)) */

    /* mpzNplus1 is always even, so the accumulated values U and V
    are initialized to U_0 and V_0 (if the target index were odd,
    U and V would be initialized to U_1=1 and V_1=P). In either case,
    the values of U_2m and V_2m are initialized to U_1 and V_1;
    the FOR loop calculates in succession U_2 and V_2, U_4 and
    V_4, U_8 and V_8, etc. If the corresponding bits of N+1 are
    on, these values are then combined with the previous totals
    for U and V, using the composition formulas for addition
    of indices. */

    mpz_set_ui(mpzU, 0);           /* ok, U=U_0 */
    mpz_set_ui(mpzV, 2);           /* ok, V=V_0 */
    mpz_set_ui(mpzU2m, 1);         /* ok, U_1 */
    mpz_set_si(mpzV2m, iP);        /* ok, V_1 */
    mpz_set_si(mpzQm, lQ); //ok, ul type
    mpz_set_si(mpz2Qm, 2*lQ); //ok, should fit in 32 bits - 1Q is ul and than is divided by 4

    ulNbits=(unsigned long) mpz_sizeinbase(mpzNplus1, 2);
    for(ul=1; ul < ulNbits; ul++)  /* zero bit off, already accounted for */
    {
    /* Formulas for doubling of indices (carried out mod N). Note that
    * the indices denoted as "2m" are actually powers of 2, specifically
    * 2^(ul-1) beginning each loop and 2^ul ending each loop.
    *
    * U_2m = U_m*V_m
    * V_2m = V_m*V_m - 2*Q^m
    */
    mpz_mul(mpzU2m, mpzU2m, mpzV2m);
    mpz_mod(mpzU2m, mpzU2m, mpzN);
    mpz_mul(mpzV2m, mpzV2m, mpzV2m);
    mpz_sub(mpzV2m, mpzV2m, mpz2Qm);
    mpz_mod(mpzV2m, mpzV2m, mpzN);
    if(mpz_tstbit(mpzNplus1, ul))
        {
    /* Formulas for addition of indices (carried out mod N);
    *
    * U_(m+n) = (U_m*V_n + U_n*V_m)/2
    * V_(m+n) = (V_m*V_n + D*U_m*U_n)/2
    *
    * Be careful with division by 2 (mod N)!
    */
        mpz_mul(mpzT1, mpzU2m, mpzV);
        mpz_mul(mpzT2, mpzU, mpzV2m);
        mpz_mul(mpzT3, mpzV2m, mpzV);
        mpz_mul(mpzT4, mpzU2m, mpzU);
        mpz_mul_si(mpzT4, mpzT4, lD); //ok
        mpz_add(mpzU, mpzT1, mpzT2);
        if(mpz_odd_p(mpzU))mpz_add(mpzU, mpzU, mpzN);
        mpz_fdiv_q_2exp(mpzU, mpzU, 1);
        mpz_add(mpzV, mpzT3, mpzT4);
        if(mpz_odd_p(mpzV))mpz_add(mpzV, mpzV, mpzN);
        mpz_fdiv_q_2exp(mpzV, mpzV, 1);
        mpz_mod(mpzU, mpzU, mpzN);
        mpz_mod(mpzV, mpzV, mpzN);
        }
    /* Calculate Q^m for next bit position, doubling the exponent.
    The irrelevant final iteration is omitted. */
    if(ul < ulNbits - 1)  /* Q^m not needed for MSB. */
        {

        mpz_mul(mpzQm, mpzQm, mpzQm);
        mpz_mod(mpzQm, mpzQm, mpzN);  /* prevents overflow */
        mpz_add(mpz2Qm, mpzQm, mpzQm);
        }
    }

    /* If U_(N - Jacobi(D,N)) is congruent to 0 mod N, then N is
    a prime or a Lucas pseudoprime; otherwise it is definitely
    composite. */

    if(mpz_sgn(mpzU)==0) RETURN(true);
    RETURN(false);
}

unsigned long GetHistIndex(unsigned long Val, unsigned int Scale){
    // if (Val==0) return 0;  // Minimal Val on input is 1, no need for special hist column for 0 
    Val/=Scale;
    if (Val>PrimeTest::C_HistItems) Val = PrimeTest::C_HistItems;
    return Val;
}


/**********************************************************************/
// returns D coef
long PrimeTest::iLucasSelfridge_D(const mpz_t & mpzN){
    /* Test mpzN for primality using Lucas's test with Selfridge's parameters.
    Returns 1 if mpzN is prime or a Lucas-Selfridge pseudoprime. Returns
    0 if mpzN is definitely composite. Note that a Lucas-Selfridge test
    typically requires three to seven times as many bit operations as a
    single Miller's test. The frequency of Lucas-Selfridge pseudoprimes
    appears to be roughly four times that of base-2 strong pseudoprimes;
    the Baillie-PSW test is based on the hope (verified by the author,
    May, 2005, for all N < 10^13; and by Martin Fuller, January, 2007,
    for all N < 10^15) that the two tests have no common pseudoprimes. */

    int iComp2, iJ, iSign;
    long lD;
    unsigned long lDabs;
    unsigned long ulMaxBits, ulGCD;
    mpz_t mpzU, mpzV, mpzNplus1, mpzU2m, mpzV2m, mpzQm, mpz2Qm,
        mpzT1, mpzT2, mpzT3, mpzT4, mpzD;

    #undef RETURN
    #define RETURN(n)           \
    {                         \
    mpz_clear(mpzU);          \
    mpz_clear(mpzV);          \
    mpz_clear(mpzNplus1);     \
    mpz_clear(mpzU2m);        \
    mpz_clear(mpzV2m);        \
    mpz_clear(mpzQm);         \
    mpz_clear(mpz2Qm);        \
    mpz_clear(mpzT1);         \
    mpz_clear(mpzT2);         \
    mpz_clear(mpzT3);         \
    mpz_clear(mpzT4);         \
    mpz_clear(mpzD);          \
    return(n);                \
    }

    /* This implementation of the algorithm assumes N is an odd integer > 2,
    so we first eliminate all N < 3 and all even N. As a practical matter,
    we also need to filter out all perfect square values of N, such as
    1093^2 (a base-2 strong pseudoprime); this is because we will later
    require an integer D for which Jacobi(D,N) = -1, and no such integer
    exists if N is a perfect square. The algorithm as written would
    still eventually return zero in this case, but would require
    nearly sqrt(N)/2 iterations. */

    iComp2=mpz_cmp_si(mpzN, 2); //ok
    if(iComp2 < 0) return(0);
    if(iComp2==0) return(0);
    if(mpz_even_p(mpzN)) return(0);
    if(mpz_perfect_square_p(mpzN)) return(0);

    /* Allocate storage for the mpz_t variables. Most require twice
    the storage of mpzN, since multiplications of order O(mpzN)*O(mpzN)
    will be performed. */

    ulMaxBits=(unsigned long) (2*mpz_sizeinbase(mpzN, 2) + mp_bits_per_limb);
    mpz_init2(mpzU, ulMaxBits);
    mpz_init2(mpzV, ulMaxBits);
    mpz_init2(mpzNplus1, ulMaxBits);
    mpz_init2(mpzU2m, ulMaxBits);
    mpz_init2(mpzV2m, ulMaxBits);
    mpz_init2(mpzQm, ulMaxBits);
    mpz_init2(mpz2Qm, ulMaxBits);
    mpz_init2(mpzT1, ulMaxBits);
    mpz_init2(mpzT2, ulMaxBits);
    mpz_init2(mpzT3, ulMaxBits);
    mpz_init2(mpzT4, ulMaxBits);
    mpz_init(mpzD);

    /* Find the first element D in the sequence {5, -7, 9, -11, 13, ...}
    such that Jacobi(D,N) = -1 (Selfridge's algorithm). Although
    D will nearly always be "small" (perfect square N's having
    been eliminated), an overflow trap for D is present. */

    lDabs=5;
    iSign=1;
    while(1)
    {
    lD=iSign*lDabs;
    iSign = -iSign;
    ulGCD=mpz_gcd_ui(nullptr, mpzN, lDabs);  //ok, overflow throws an exception
    /* if 1 < GCD < N then N is composite with factor lDabs, and
        Jacobi(D,N) is technically undefined (but often returned
        as zero). */
    if((ulGCD > 1) && mpz_cmp_ui(mpzN, ulGCD) > 0) RETURN(false); //ok
    mpz_set_si(mpzD, lD); //ok
    iJ=mpz_jacobi(mpzD, mpzN);
    if(iJ==-1)break;
    lDabs += 2;
    if(lDabs > ulDmax)ulDmax=lDabs;  /* tracks global max of |D| */
    if(lDabs > INT32_MAX-2)
        {
        fprintf(stderr,
        "\n ERROR: D overflows signed long in Lucas-Selfridge test.");
        fprintf(stderr, "\n N=");
        mpz_out_str(stderr, 10, mpzN);
        fprintf(stderr, "\n |D|=%ld\n\n", lDabs);
        exit(EXIT_FAILURE);
        }
    }

    RETURN(lD);
}

void PrimeTest::iLucasSelfridge_DHistogram(const mpz_t & mpzN){
    long long D = iLucasSelfridge_D(mpzN);

    if (abs(D)>abs(MaxD)) {
        MaxD = D;
        mpz_set(NumberWithMaxD, mpzN);
    }

    // mpz_cmp(mpz)
    
    // smallest D coef is 5, move it to 0
    unsigned long Val = (unsigned long) abs(D);
    if (Val>0){
        Val-=5;
        Val/=2;
    };

    // array index 0 stores value 5
    DHist1[GetHistIndex(Val, 1)]++;
    // array index 0 stores values first 10 computation steps of D: 5, -7, 9, -11, 13, -15, 17, -19, 21, -23
    DHist10[GetHistIndex(Val, 10)]++;
    DHist100[GetHistIndex(Val, 100)]++;
    DHist1000[GetHistIndex(Val, 1000)]++;

} 

bool PrimeTest::IsPrimeBPSW(const uint128_t & X){

    utils_mpz::mpz_set_ul128(mpz_X, X);
    if (iMillerRabinBase2(mpz_X)==0) return false;
    if (iLucasSelfridge(mpz_X)==0) return false;
    return true;
}


bool PrimeTest::IsPrimeBPSW(const mpz_t & X){
    if (iMillerRabinBase2(X)==0) return false;
    if (iLucasSelfridge(X)==0) return false;
    return true;
}

bool PrimeTest::IsPrimeBPSW(const unsigned long long X){
    // mpz_t mpzX;
    mpz_init2(mpz_X,64);
    utils_mpz::mpz_set_ull(mpz_X, X);
    bool res = IsPrimeBPSW(mpz_X);
    // mpz_clear(mpzX);
    return res;
}

bool PrimeTest::IsPrimeDivByPrime(mpz_t x)
{
    // mpz_t tmp;
    // mpz_init(tmp);
    // mpz_t mpz_Prime64;
    // mpz_init(mpz_Prime64);
    // no mpz_clear neccesary
   
    unsigned long long MaxToTest;
    mpz_sqrt(tmp,x);
    MaxToTest = utils_mpz::mpz_get<unsigned long long>(tmp); // should fit into 32 bits
    
    uint32_t Prime=0;
    if(mpz_cmp_ui(x,1)<=0) return false; //ok
    for(unsigned long long i = 0; i<PrimesCount32; i++)
    {
        Prime = Primes32 [i];
        if (Prime > MaxToTest) return true;
        mpz_mod_ui(tmp,x,Prime); //ok
        if(mpz_cmp_ui(tmp,0)==0) return false; //ok
    }
    for(unsigned long long i = 0; i<PrimesCount64; i++)
    {
        unsigned long long Prime64 = Primes64[i];
        if (Prime64 > MaxToTest) return true;
        utils_mpz::mpz_set_ull(mpz_Prime64, Prime64);
        mpz_mod(tmp, x, mpz_Prime64);
        if(mpz_cmp_ui(tmp,0)==0) return false;  //ok
    }
    if(MaxToTest>Limit64) std::cout << "IsPrimeDivByPrime: Limit exceeded" << endl;    
    return true;
}

bool PrimeTest::IsPrimeDivByPrime(const unsigned long long x)
{
    unsigned long long MaxToTest = (unsigned long long) sqrtl(x);
    uint32_t Prime=0;
    if (x<=1) return false;
    for (unsigned long long i = 0; i<PrimesCount32; i++)
    {
        Prime = Primes32 [i];
        if (Prime > MaxToTest) return true;
        if (x % Prime == 0) return false;
    }
    return true;
}

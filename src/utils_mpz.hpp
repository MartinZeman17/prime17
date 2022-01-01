#ifndef UTILS_MPZ_HPP
#define UTILS_MPZ_HPP

#include <gmp.h>
#include "int128_t.hpp"
using namespace primecount;

namespace utils_mpz {

    template <typename T>
    void inline mpz_set(mpz_t &rop, const T &op) {
        mpz_import(rop, 1, 1, sizeof(op), 0, 0, &op);
    }


    void inline mpz_set_ull(mpz_t &rop, const unsigned long long &op)
    {
    #if __SIZEOF_LONG__ !=8
        mpz_import(rop, 1, 1, sizeof(op), 0, 0, &op);
    #else
        mpz_set_ui(rop, op); //ok
    #endif
    }

    void inline mpz_set_ul128(mpz_t &rop, const uint128_t &op) {
        mpz_import(rop, 1, 1, sizeof(op), 0, 0, &op);
    }

    // #if __BYTE_ORDER__ == __ORDER_LITTLEEnd_IAN__
    // __ORDER_LITTLEEnd_IAN__
    // __ORDER_BIGEnd_IAN__
    // __ORDER_PDPEnd_IAN__
    // __BYTE_ORDER__ is defined to one of the values __ORDER_LITTLEEnd_IAN__, __ORDER_BIGEnd_IAN__, or __ORDER_PDPEnd_IAN__ 
    // to reflect the layout of multi-byte and multi-word quantities in memory. 
    // If __BYTE_ORDER__ is equal to __ORDER_LITTLEEnd_IAN__ or __ORDER_BIGEnd_IAN__, 
    // then multi-byte and multi-word quantities are laid out identically: 
    // the byte (word) at the lowest address is the least significant or most significant byte (word) of the quantity, respectively. 
    // If __BYTE_ORDER__ is equal to __ORDER_PDPEnd_IAN__, then bytes in 16-bit words are laid out in a little-endian fashion, 
    // whereas the 16-bit subwords of a 32-bit quantity are laid out in big-endian fashion.


    /*
    Function: void mpz_import (mpz_t rop, size_t count, int order, size_t size, int endian, size_t nails, const void *op)
    Set rop from an array of word data at op.
    The parameters specify the format of the data. count many words are read, each size bytes. 
    order can be 1 for most significant word first or -1 for least significant first. 
    Within each word endian can be 1 for most significant byte first, -1 for least significant first, or 0 for the native endianness of the host CPU.
    The most significant nails bits of each word are skipped, this can be 0 to use the full words.

    Function: void * mpz_export (void *rop, size_t *countp, int order, size_t size, int endian, size_t nails, const mpz_t op)
    Fill rop with word data from op.
    Each word will be size bytes and 
    order can be 1 for most significant word first or -1 for least significant first. 
    Within each word endian can be 1 for most significant byte first, -1 for least significant first, or 0 for the native endianness of the host CPU. 
    The most significant nails bits of each word are unused and set to zero, this can be 0 to produce full words.

    The number of words produced is written to *countp, or countp can be NULL to discard the count. 
    */

    template <typename T>
    T inline mpz_get(const mpz_t &op) {
        T ret;
        size_t WordsReturned = 0;
        mpz_export( &ret, &WordsReturned, 1, sizeof(T), 0, 0, op);
        if (WordsReturned!=1) {
            const std::string C_Msg("Fatal error in mpz_get: number to read does not fit in. Mission aborted!");
            std::cout << C_Msg << std::endl;
            throw(C_Msg);   
        }
        return ret;   
    }


// unsigned long long inline mpz_get_ull(const mpz_t &op) {
//     unsigned long long ret;
//     size_t WordsReturned = 0;
//     mpz_export( &ret, &WordsReturned, 1, sizeof(unsigned long long), 0, 0, op);
//     if (WordsReturned!=1) {
//         const std::string C_Msg("Fatal error in mpz_get_ull: number to read does not fit in ull. Mission aborted!");
//         cout << C_Msg << endl;
//         throw(C_Msg);   
//     }
//     return ret;   
// }

// uint128_t inline mpz_get_ul128(const mpz_t &op) {
//     uint128_t ret;
//     size_t WordsReturned = 0;
//     mpz_export( &ret, &WordsReturned, 1, sizeof(uint128_t), 0, 0, op);
//     if (WordsReturned!=1) {
//         const std::string C_Msg("Fatal error in mpz_get_ull: number to read does not fit in ull. Mission aborted!");
//         cout << C_Msg << endl;
//         throw(C_Msg);   
//     }
//     return ret;   
// }


}


// does not work with 32 bit ulong
// void Utils::UInt128tompz(uint128_t In, mpz_t &Out) {
        
//     mpz_set_ui(Out,0);
//     while (In > UINT64_MAX) {
//         mpz_add_ui(Out, Out, UINT64_MAX);
//         In -= UINT64_MAX;
//     }
//     unsigned long long offset = In;
//     mpz_add_ui(Out, Out, offset);
// }

#endif
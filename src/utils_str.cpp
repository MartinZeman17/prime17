#include "utils_str.hpp"
#include <iostream>
#include <string>

using std::string;

std::string utils_str::string_replace(string src, string const& target, string const& repl) {
    // handle error situations/trivial cases

    if (target.length() == 0) {
        // searching for a match to the empty string will result in 
        //  an infinite loop
        //  it might make sense to throw an exception for this case
        return src;
    }

    if (src.length() == 0) {
        return src;  // nothing to match against
    }

    size_t idx = 0;

    for (;;) {
        idx = src.find( target, idx);
        if (idx == string::npos)  break;

        src.replace( idx, target.length(), repl);
        idx += repl.length();
    }
    return src;
}

// template <typename T>
// std::string  utils_str::FormatNumber(T Input, unsigned int StrLen, unsigned int Precision){
//     std::stringstream Aux;
//     Aux.precision(Precision);
//     Aux << std::fixed;
//     Aux << Input;

//     long long filler = StrLen-Aux.tellp();
//     if (filler<0) filler = 0;
//     std::string res(" ", filler);
//     res.append(Aux.str());
//     return res;
//   }

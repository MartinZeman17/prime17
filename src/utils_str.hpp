#ifndef UTILS_STR_HPP
#define UTILS_STR_HPP

#include <iostream>
#include <string>
#include <sstream>  

#include <fstream>
#include <locale>
#include <memory>



using std::string;

namespace utils_str{
  struct split
  {
    enum empties_t { empties_ok, no_empties };
  };

  template <typename Container>
  Container& split(
    Container&                            result,
    const typename Container::value_type& s,
    const typename Container::value_type& delimiters,
    split::empties_t                      empties = split::empties_ok )
  {
    result.clear();
    size_t current;
    size_t next = -1;
    do
    {
      if (empties == split::no_empties)
      {
        next = s.find_first_not_of( delimiters, next + 1 );
        if (next == Container::value_type::npos) break;
        next -= 1;
      }
      current = next + 1;
      next = s.find_first_of( delimiters, current );
      result.push_back( s.substr( current, next - current ) );
    }
    while (next != Container::value_type::npos);
    return result;
  }

  // std::string FormatNumber(T Input, unsigned int StrLen, unsigned int Precision)
  

  // ToDo use std::fixed << std::setprecision(2) << std::setfill('0')
  template <typename T>
  std::string FormatNumber(T Input, unsigned int StrLen, unsigned int Precision){
    std::stringstream Aux;
    Aux.precision(Precision);
    Aux << std::fixed;
    Aux << Input;

    long long filler = StrLen-Aux.tellp();
    if (filler<0) filler = 0;
    std::string res(" ", filler);
    res.append(Aux.str());
    return res;
  }

struct separate_thousands : std::numpunct<char> {
    char_type do_thousands_sep() const override { return ' '; }  // separate with commas
    string_type do_grouping() const override { return "\3"; } // groups of 3 digit
};

template <typename T>
std::string FormatUInt(T Input){
    auto thousands = std::make_unique<separate_thousands>();

    std::stringstream Aux;
    Aux.imbue(std::locale(std::cout.getloc(), thousands.release()));
    Aux << Input;
    return Aux.str();
}
  

  string string_replace(string src, string const& target, string const& repl);
  

}


/*
#include <iostream>
#include <string>
#include <vector>
using namespace std;

void print( vector <string> & v )
{
  for (size_t n = 0; n < v.size(); n++)
    cout << "\"" << v[ n ] << "\"\n";
  cout << endl;
}

int main()
{
  string s = "One, two,, four , five,";

  vector <string> fields;

  cout << "\"" << s << "\"\n\n";

  split( fields, s, "," );
  print( fields );
  cout << fields.size() << " fields.\n\n";

  split( fields, s, ",", split::no_empties );
  print( fields );
  cout << fields.size() << " fields.\n";

  return 0;
}
*/

#endif

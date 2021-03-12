#ifndef UTILS_STR_HPP
#define UTILS_STR_HPP

#include <iostream>
#include <string>

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

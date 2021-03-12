#ifndef __VECTORTOFILE_HPP
#define __VECTORTOFILE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <fstream>


template <typename T> void VectorToFile(const std::vector<T> &Vec, std::ofstream & _myfile) { 
    for (auto i : Vec){
        std::cout << " " << i;
        _myfile << " " << i;
    }
    cout << endl;
    _myfile << endl;
}

#endif

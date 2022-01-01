#ifndef __VECTORTOFILE_HPP
#define __VECTORTOFILE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <fstream>


template <typename T> void VectorToFile(const std::vector<T> &Vec, std::ofstream & myfile_) { 
    for (auto i : Vec){
        std::cout << " " << i;
        myfile_ << " " << i;
    }
    cout << endl;
    myfile_ << endl;
}




#endif

#include "PrimesRange.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <filesystem>

#include "utils_str.hpp"


namespace fs = std::filesystem;  

using std::endl;
using std::cout;
using std::string;
using std::to_string;
using std::vector;


namespace{
    uint32_t* Eratosthenes32(uint32_t Begin, uint32_t Limit32, unsigned long long &PrimesCount32)
    {   
        unsigned long long Limit = Limit32;
        std::cout << "Sieve 32bit up to: " << Limit << endl;
        int progress=0;
        std::vector<bool> EratosArray(Limit+1, true);
        
        EratosArray[0] = false;
        EratosArray[1] = false;
        for (unsigned long long p = 2; p * p <= Limit; p++)
        {
            for (unsigned long long i = p * 2; i <= Limit; i += p)
            {
                EratosArray[i] = false;
            }
            if (progress == 100){
                std::cout << p << " ";
                progress=0;
            }
            progress++;
        }
        std::cout << endl;

        unsigned long long PrimeCounter = 0;
        for (unsigned long long i=Begin; i <= Limit; i++)
        {
            if (EratosArray[i] == true)
            {
                PrimeCounter++;
            }
        }
        std:: cout << "Primes detected: " << PrimeCounter << endl << endl;

        unsigned long long primes_array_size = (PrimeCounter)*4;
        uint32_t *PrimesArray = nullptr;
        PrimesArray = (uint32_t *)malloc(primes_array_size);
        PrimesCount32 = PrimeCounter;

        unsigned long long j = 0;
        for (unsigned long long i = Begin; i <= Limit; i++)
        {
            if (EratosArray[i] == true)
            {
                PrimesArray[j++] = (uint32_t) i;
            }
        }
        return PrimesArray;
    }

    uint64_t* Eratosthenes64(unsigned long long Begin, unsigned long long Limit, unsigned long long &PrimesCount64)
    {
        std::cout << "Sieve 64bit from: " << Begin <<  " up to: " << Limit << endl;
        std::cout << "Progress (up to square root of max): " << endl;
        int progress=0;
        unsigned long long Size = Limit - Begin + 1; 

        std::vector<bool> EratosArray(Size, true);

        for (unsigned long long p = 2; p * p <= Limit; p++)
        {
            unsigned long long StartOverBegin;
            unsigned long long X=Begin/p;
            StartOverBegin=p*X;

            if (StartOverBegin<Begin) { StartOverBegin += p; }
            if (StartOverBegin<2*p) { StartOverBegin = 2*p; }

            for (unsigned long long i = StartOverBegin; i <= Limit; i += p)
            {
                unsigned long long Index = (unsigned long long) i-Begin;
                EratosArray[Index] = false;
            }
            if (progress == 100){
                std::cout << p << " ";
                progress=0;
            }
            progress++;

        }
        std:: cout << endl;
        unsigned long long PrimeCounter = 0;
        for (unsigned long long i = 0; i < Size; i++)
        {
            if (EratosArray[i] == true)
            {
                PrimeCounter++;
            }
        }
        std::cout << "Primes detected: " << PrimeCounter << endl << endl;

        unsigned long long primes_array_size = (PrimeCounter)*8;
        uint64_t *PrimesArray = nullptr;
        PrimesArray = (uint64_t *)malloc(primes_array_size);
        PrimesCount64 = PrimeCounter;

        unsigned long long j = 0;
        for (unsigned long long i = 0; i < Size; i++)
        {
            if (EratosArray[i] == true)
            {
                unsigned long long Prime=i+Begin;
                PrimesArray[j++] = Prime;
                //std::cout << PrimesArray[j - 1] << endl;
            }
        }
        return PrimesArray;
    }

    // string GetFileName(const bool b32, const unsigned long long Begin, const unsigned long long End, const unsigned long long PrimesCount){
    //     string FileName("");
    //     // FileName += StoragePath;
   
    //     FileName += b32 ? "32_" : "64_";
    //     FileName += to_string(Begin);
    //     FileName += "_";
    //     FileName += to_string(End);
    //     FileName += "_";
    //     FileName += to_string(PrimesCount);
    //     FileName += "_.dat";  
    //     return FileName;
    // }

    auto SplitFileName(string FileName){
        struct res {bool b32; bool b64; unsigned long long Begin; unsigned long long End; unsigned long long PrimesCount;} r = {false,false,0,0,0};
        vector <string> fields;
        try{
            utils_str::split(fields, FileName, "_", utils_str::split::no_empties );
            if (fields.size()==5) {
                if (fields[0]=="32") r.b32 = true;
                if (fields[0]=="64") r.b64 = true;
                r.Begin = stoull(fields[1]);
                r.End = stoull(fields[2]);
                r.PrimesCount = stoull(fields[3]);
            }
            return r;
        
        }  catch (const std::exception& e){
            res empty = {false,false,0,0,0};
            return empty;
        }      
    }  


    uint32_t* LoadFile32(const unsigned long long PrimesCount32, const string FileName) {
        uint32_t* Primes32 = nullptr;
        Primes32 = (uint32_t *) malloc(PrimesCount32*4);

        std::ifstream iprimefile;
        iprimefile.open(FileName, std::ifstream::binary);
        iprimefile.read((char*) Primes32, (std::streamsize) PrimesCount32*4);
        iprimefile.close();
        if (!Primes32[0]) return nullptr;
        return Primes32;
    }

    uint64_t* LoadFile64(const unsigned long long PrimesCount64, const string FileName) {
        // FileName += "64_4294967296_6074001001_79550350_.dat"; //32_0_100_25_.dat    
        // PrimesCount64 = 79550350; //prime with index 203280221-1 is 4294967291 = FF FF FF FB = 2^32-4
        
        uint64_t* Primes64 = nullptr;
        Primes64 = (uint64_t *) malloc(PrimesCount64*8);

        std::ifstream iprimefile;
        iprimefile.open(FileName, std::ifstream::binary);
        iprimefile.read((char*) Primes64, (std::streamsize) PrimesCount64*8);
        iprimefile.close();
        if (!Primes64[0]) return nullptr;
        return Primes64;
    }

    void SaveArray32(const unsigned long long Begin, const unsigned long long End, 
                                const unsigned long long PrimesCount32, const uint32_t *Primes32) {
        std::string FileName("");
        FileName +="32_" + to_string(Begin) + "_" + to_string(End) + "_" + to_string(PrimesCount32) + "_.dat";

        fs::create_directory(StorageDir);
        fs::path Dir(StorageDir);
        fs::path File(FileName);
        fs::path FullFile = Dir / File;

        std::cout <<"Saving: " << FileName << endl;
        std::cout <<"Saving: " << fs::absolute(FullFile) << endl;

        std::ofstream oprimefile;
        oprimefile.open(FullFile.string(), std::ifstream::binary);
        oprimefile.write((char *)Primes32, (std::streamsize) PrimesCount32*4);
        oprimefile.close();
    }

    void SaveArray64(const unsigned long long Begin, const unsigned long long End, 
                                const unsigned long long PrimesCount64, const uint64_t *Primes64) {
        std::string FileName("");
        FileName +="64_" + to_string(Begin) + "_" + to_string(End) + "_" + to_string(PrimesCount64) + "_.dat";

        fs::create_directory(StorageDir);
        fs::path Dir(StorageDir);
        fs::path File(FileName);
        fs::path FullFile = Dir / File;

        std::cout <<"Saving: " << FileName << endl;
        std::cout <<"Saving: " << fs::absolute(FullFile) << endl;

        std::ofstream oprimefile;
        oprimefile.open(FullFile.string(), std::ifstream::binary);
        oprimefile.write((char *)Primes64, (std::streamsize) PrimesCount64*8);
        oprimefile.close();
    }

}


// inline uint32_t * PrimesRange::PrimesArray32() const {return p_PrimesArray32;}
// inline uint64_t * PrimesRange::PrimesArray64() const {return p_PrimesArray64;}

PrimesRange::PrimesRange(unsigned long long Begin, unsigned long long End){
    string FileName("");
    string FullFileName("");
    fs::path FullPath;

    bool b32 = (Begin<=UINT32_MAX && End<=UINT32_MAX);
    //take care of typical configurations
    if (Begin==0 && End==4294967295) { FileName = "32_0_4294967295_203280221_.dat";}    
    else if (Begin==4294967296 && End==6074001001) { FileName = "64_4294967296_6074001001_79550350_.dat"; } 
    
    try{
        if (!FileName.empty()){
            fs::path Dir(StorageDir);    
            fs::path File(FileName);
            FullPath = fs::current_path() / Dir;
            FullPath = FullPath / File;
        }
        if (fs::exists(FullPath)) {
            // file located
            FullFileName = fs::absolute(FullPath).string();
        } else {
            // try to find file for specific begin and end (and an unknown count of primes)
            string filenameToFind("");
            filenameToFind += b32 ? "32_" : "64_" ;
            filenameToFind += to_string(Begin) + "_";
            filenameToFind += to_string(End) + "_";

            fs::path SearchStartDir(StorageDir);
            if (!fs::exists(SearchStartDir)) {
                SearchStartDir = fs::current_path(); // dir iterator throws exception if dir does not exist
            }
            //search for file via recursive iteration 
            for ( auto & fentry : fs:: recursive_directory_iterator(SearchStartDir)){ 
                if (fentry.is_regular_file()){              
                    if (fentry.path().extension()==".dat"){
                        const auto fileToCheck = fentry.path().filename().string();                    
                        if (filenameToFind.compare(0, filenameToFind.length(), fileToCheck,0, filenameToFind.length())==0){
                            FileName = fileToCheck;
                            FullFileName = fs::absolute(fentry).string();
                            break; //file located via search
                        } 
                    }
                }
            }        
        }
    } catch (const std::exception& e) {
        FileName = "";
        FullFileName = "";
    }

    if (!FullFileName.empty()){
        // file must exist -> check properties using split
        auto r = SplitFileName(FileName);
        if ((r.b32 || r.b64 ) && r.Begin< r.End && r.PrimesCount>0) {
            // file name looks valid  
            if (b32) {
                p_PrimesArray32 = LoadFile32(r.PrimesCount, FullFileName);
            } else {
                p_PrimesArray64 = LoadFile64(r.PrimesCount, FullFileName);       
            }
            p_PrimesCount = r.PrimesCount;
        }
    }

    // load from file has not been succesfull -> compute and save 
    if (b32) { 
        if (!p_PrimesArray32){
            p_PrimesArray32 = Eratosthenes32((uint32_t) Begin, (uint32_t) End, p_PrimesCount);
            SaveArray32(Begin, End, p_PrimesCount, p_PrimesArray32);
        }        
        p_MinPrime = p_PrimesArray32[0];
        pMaxPrime_ = p_PrimesArray32[p_PrimesCount-1];
        
    } else {
        if (!p_PrimesArray64){
            p_PrimesArray64 = Eratosthenes64(Begin, End, p_PrimesCount);
            SaveArray64(Begin, End, p_PrimesCount, p_PrimesArray64);
        }
        p_MinPrime = p_PrimesArray64[0];
        pMaxPrime_ = p_PrimesArray64[p_PrimesCount-1];
    }

    // fill rest of the class variables
    pBegin_ = Begin;
    pEnd_ = End;

    cout << "Primes ready: " << p_MinPrime  << " ... " <<  pMaxPrime_ << endl;
    cout << "Begin: " << Begin << endl; 
    cout << "End: " << End << endl; 
    cout << "Count: " << p_PrimesCount << endl; 
}

PrimesRange::~PrimesRange(){
    // cout << "~PrimesRange" << endl;
    if (p_PrimesArray32) {
        std::free(p_PrimesArray32);
        p_PrimesArray32 = nullptr;
    }

    if (p_PrimesArray64) {
        std::free(p_PrimesArray64);
        p_PrimesArray64 = nullptr;
    }
}
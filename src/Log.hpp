#ifndef LOG_HPP
#define LOG_HPP

#include "utils.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <sstream>  
#include <mutex>
#include <filesystem>
#include <fstream>



// // there are some ompatibility issues and ncurses shoul be included after sstream
#ifdef __MINGW64__
# include <ncurses/ncurses.h>
#else
#include <ncurses.h> 
#endif


class DoNotCopy
{
protected:
    DoNotCopy(void) = default;
    DoNotCopy(const DoNotCopy&) = delete;
    DoNotCopy& operator=(const DoNotCopy&) = delete;
};

class DoNotMove
{
protected:
    DoNotMove(void) = default;
    DoNotMove(DoNotMove&&) = delete;
    DoNotMove& operator=(DoNotMove&&) = delete;
};

class DoNotCopyMove : public DoNotCopy,
    public DoNotMove
{
protected:
    DoNotCopyMove(void) = default;
};

template<class T>
class Singleton : public DoNotCopyMove
{
private:

public:
    
    static T& out(void)
    {
        static std::mutex mutex_;
        std::lock_guard<std::mutex> lock(mutex_);
        static T instance;
        return instance;
    }

protected:
    Singleton(void) = default;
};

class Log final: public Singleton<Log> {
    private:
    std::ofstream _LogFile;
    std::string _LogFileName;
    std::mutex _mutex;
    int _winCOLS  = 0;
    int _winLINES = 0;    
    
    public:
    WINDOW *win_left=nullptr;
    WINDOW *win_middle=nullptr;
    WINDOW *win_right=nullptr;

    void SmartFlush(std::stringstream &Msg) noexcept {
        Msg.seekg(-1, Msg.end);
        // char Ending = (char) Msg.get();
        int Ending =  Msg.get();
        if (Ending == '\n' || Ending == '\r') {
            _LogFile << std::flush;
        } 
    }
    
    public:
    template<typename T> Log& operator<<(const T& Input) {
        std::lock_guard<std::mutex> lock(_mutex);
        
        std::stringstream Msg;
        Msg << Input;

        wprintw(win_left, Msg.str().c_str());
        wrefresh(win_left);
        // std::cout << Msg.str();
        _LogFile << Input;
        SmartFlush(Msg);
        return *this;
    }

    void log(const std::string& Input) {
        operator<<<std::string>(Input);
    }


    template <typename T>
    void logRight(const T& Input) {
        std::lock_guard<std::mutex> lock(_mutex);
        std::stringstream Msg;
        Msg << Input;
        int y,x;
        getyx(win_right, y, x);
        if (x==0) x=0; // to supress an annoing warning
        mvwprintw(win_right, y, x, Msg.str().c_str());
        wrefresh(win_right); 
        // std::cout << Msg.str();
        _LogFile << Input;
        SmartFlush(Msg);
    }

    template <typename T>
    void logToFile(const T& Input) {
        std::lock_guard<std::mutex> lock(_mutex);
        std::stringstream Msg;
        Msg << Input;
        _LogFile << Input;
        SmartFlush(Msg);
    }

    void SetLogFileName(const std::string &FileName){
        _LogFileName = FileName;
    }

    void init(bool LogToFile=true) {
        if (win_left!=nullptr){
            if (COLS==_winCOLS && LINES == _winLINES) return;
            FreeWin();
        }
        initscr();
        // cbreak();			/* Line buffering disabled, Pass on everything to me 		*/
        // keypad(stdscr, TRUE);		/* I need that nifty F1 	*/

        // struct sigaction sa;
        // memset(&sa, 0, sizeof(struct sigaction));
        // sa.sa_handler = handle_winch;
        // sigaction(SIGWINCH, &sa, NULL);

        _winCOLS = COLS;
        _winLINES = LINES;
        int width_left = (int) ( COLS * (2.0/3.0));
        refresh();
        win_left = newwin(LINES, width_left, 0, 0);
        scrollok(win_left, TRUE);
        wrefresh(win_left);

        win_middle = newwin(LINES, 1, 0, width_left);
        wborder(win_middle, ' ', '|', ' ',' ', ' ', '|', ' ', '|');
        wrefresh(win_middle);

        win_right = newwin(LINES, COLS - (width_left + 1), 0, width_left+1);
        wborder(win_right, ' ', ' ', ' ',' ', ' ', ' ', ' ', ' ');
        wrefresh(win_right);

        if (LogToFile) {
            if (!_LogFile.is_open()) {
                fs::path p(utils::getConfigDir());
                if (_LogFileName.empty()) _LogFileName = utils::C_AppName;
                p /= _LogFileName.append(".log");
                _LogFile.open( p.string(), std::ios::trunc);     
            }   
        }
    }

    // void handle_winch(int sig)
    // {
    //     endwin();
    //     // Needs to be called after an endwin() so ncurses will initialize itself with the new terminal dimensions.
    //     refresh();
    //     clear();
    //     refresh();
    // }

    void FreeWin(){
        delwin(win_left);
        delwin(win_middle);
        delwin(win_right);
        endwin();    
    }
    void Free(){
        FreeWin();
        if (_LogFile.is_open()) _LogFile.close();    
    }

    std::string getlineLeft(){
        char str[200];
        wgetstr(win_left, str);
        return std::string(str);
    }

    void ClearRight(){
        wclear(win_right);
        wrefresh(win_right);
    }
        

};


#endif
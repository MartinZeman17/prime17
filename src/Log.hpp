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
#include <string.h>


#include <signal.h>
// there are some compatibility issues and ncurses should be included after sstream
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
    struct sigaction _sa;
    std::ofstream _LogFile;
    std::string _LogFileName;
    std::mutex _mutex;
    int _winCOLS  = 0;
    int _winLINES = 0;    
    
    public:    
    WINDOW *win_left=nullptr;
    WINDOW *win_middle=nullptr;
    WINDOW *win_right=nullptr;

    void static  handle_winch(__attribute__((unused)) int sig) {
        // endwin();
        // // Needs to be called after an endwin() so ncurses will initialize itself with the new terminal dimensions.
        // refresh();
        // clear();

        // mvprintw(0, 0, "COLS = %d, LINES = %d", COLS, LINES);
        // for (int i = 0; i < COLS; i++)
        //     mvaddch(1, i, '*');
        // refresh();
        Log::out().Resize();
        Log::out() << "resized \n" << COLS << "\n" << LINES << "\n";
    }

    void Resize(){
        endwin();
        // refresh();
        if (win_right!=nullptr) wrefresh(win_right);
        if (win_middle!=nullptr) wrefresh(win_middle);
        if (win_left!=nullptr) wrefresh(win_left);
        // clear();
        reinit();      
        _winCOLS = COLS;
        _winLINES = LINES;
    }

    void reinit(){
        if (win_left!=nullptr) delwin(win_left);
        if (win_middle!=nullptr) delwin(win_middle);
        if (win_right!=nullptr) delwin(win_right);
        win_left = nullptr;
        win_middle = nullptr;
        win_right = nullptr;

        // FreeWin();

        _winCOLS = COLS;
        _winLINES = LINES;
        if (_winCOLS>0 && _winLINES>0) {

            // refresh(); //???

            int width_left = (int) ( _winCOLS * (2.0/3.0));
            int width_right = _winCOLS - ((width_left + 1));
            if (width_right<=0) {
                width_left = _winCOLS;
                width_right=0;
            }            
            
            win_left = newwin(_winLINES, width_left, 0, 0);
            scrollok(win_left, TRUE);
            wrefresh(win_left);

            if (width_right != 0) {
                win_middle = newwin(_winLINES, 1, 0, width_left);
                wborder(win_middle, ' ', '|', ' ',' ', ' ', '|', ' ', '|');
                wrefresh(win_middle);

                win_right = newwin(_winLINES, _winCOLS - (width_left + 1), 0, width_left+1);
                wborder(win_right, ' ', ' ', ' ',' ', ' ', ' ', ' ', ' ');
                wrefresh(win_right);
            }
        }
        else {
            endwin();
        }
    }

    void init(bool LogToFile=true) {
        if (win_right!=nullptr){
            if (COLS==_winCOLS && LINES == _winLINES) return; 
            endwin(); //??? kdy se vola init?       
        }

        initscr();
        
        // cbreak();			/* Line buffering disabled, Pass on everything to me 		*/
        // keypad(stdscr, TRUE);		/* I need that nifty F1 	*/

        // memset(&_sa, 0, sizeof(struct sigaction));
        // _sa.sa_handler = Log::out().handle_winch;
        // sigaction(SIGWINCH, &_sa, NULL);
        
        reinit();

        if (LogToFile) {
            if (!_LogFile.is_open()) {
                fs::path p(utils::getConfigDir());
                if (_LogFileName.empty()) _LogFileName = utils::C_AppName;
                p /= _LogFileName.append(".log");
                _LogFile.open( p.string(), std::ios::trunc);     
            }   
        }
    }

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

        if (win_left!=nullptr){
            wprintw(win_left, Msg.str().c_str());
            wrefresh(win_left);
        }
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
        if (win_right != nullptr){
            int y,x;
            getyx(win_right, y, x);
            if (x==0) x=0; // to supress an annoing warning
            mvwprintw(win_right, y, x, Msg.str().c_str());
            wrefresh(win_right); 
        }
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

    void FreeWin(){
        if (win_left!=nullptr) delwin(win_left);
        if (win_middle!=nullptr) delwin(win_middle);
        if (win_right!=nullptr) delwin(win_right);
        endwin();    
    }
    void Free(){
        FreeWin();
        if (_LogFile.is_open()) _LogFile.close();    
    }

    std::string getlineLeft(){
        constexpr unsigned int C_MaxInputLength  = 1000;
        char str[C_MaxInputLength];
        if (win_left == nullptr) return "";
        if (wgetnstr(win_left, str, C_MaxInputLength)!=OK) return "";
        return std::string(str);
    }

    void ClearRight(){
        if (win_right != nullptr) {
            wclear(win_right);
            wrefresh(win_right);
        }
    }        
};


#endif
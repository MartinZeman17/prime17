#ifndef LOG_HPP
#define LOG_HPP

#include <iostream>
#include <string>
#include <thread>
#include <sstream>  
#include <mutex>


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
    public:
    WINDOW *win_left;
    WINDOW *win_right;
    
    public:
    template<typename T> Log& operator<<(const T& Input) {
        std::stringstream Msg;
        Msg << Input;
        wprintw(win_left, Msg.str().c_str());
        wrefresh(win_left);
        return *this;
    }

    void log(const std::string& str) {
        std::stringstream Msg;
        Msg << str;
        wprintw(win_left, Msg.str().c_str());
        wrefresh(win_left); 
    }

    void init() {
        //std::cout << "Logger Init" << std::endl;
        initscr();
        cbreak();			/* Line buffering disabled, Pass on everything to me 		*/
        // keypad(stdscr, TRUE);		/* I need that nifty F1 	*/

        int width_left = (int) ( COLS * (2.0/3.0));
        refresh();
        win_left = newwin(LINES, width_left, 0, 0);
        // box(win_left, 0 , 0);		/* 0, 0 gives default characters for the vertical and horizontal  lines			*/
	    wborder(win_left, ' ', '|', ' ',' ', ' ', ' ', ' ', ' ');
        wrefresh(win_left);

        win_right = newwin(LINES, COLS - width_left + 1, 0, width_left);
        box(win_right, 0 , 0);
        wrefresh(win_right);

    }

    WINDOW * GetRight(){
        return win_right;
    }

};


// struct LogStream 
// {
//     template<typename T> LogStream& operator<<(const T& mValue)
//     {
//         std::cout << mValue;
//         someLogStream << mValue;
//     }
// };

// inline LogStream& lo() { static LogStream l; return l; }

// int main()
// {
//     lo() << "hello!";
//     return 0;
// }


// int main()
// {
//     Logger::Instance().log("xx");
// }

#endif
#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include <string>
//#include <stdarg.h>

namespace _log{
    void init();
    void close();

    void out(const std::string&, ...);
}

#endif // LOG_H_INCLUDED

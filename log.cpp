#include <fstream>
#include <stdarg.h>
#include <mutex>
#include "log.h"

using namespace std;

fstream logFile;

std::mutex logMutex;

namespace _log
{
    void init()
    {
        logFile.open("log.txt", ios::out);
    }

    void close()
    {
        logFile.close();
    }

    void out(const std::string &format, ...)
    {
        logMutex.lock();
        if(format.find('%') == -1)
            logFile<<format;
        else
        {
            char buffer[1024];

            va_list ap;
            va_start(ap, format);
            vsprintf(buffer, format.c_str(), ap);
            va_end(ap);

            logFile<<buffer;
        }
        logFile.flush();
        logMutex.unlock();
    }
}



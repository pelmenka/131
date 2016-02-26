#ifndef MUSIC_INCLUDED
#define MUSIC_INCLUDED

#include "sound.h"

#include <string>

class musicEngine: public soundEngine
{
    std::string name;
public:

    float volume;

    musicEngine();
    void update();
    void setMusic(const char*);
};

#endif // MUSIC_H_INCLUDED

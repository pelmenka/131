#include "music.h"

musicEngine::musicEngine()
{
    name.clear();
}

void musicEngine::update()
{
    if(!ok) return;
    if(!name.size()) return;
    //setVolume(0.5);

    if(!engine->isCurrentlyPlaying(name.c_str()))
        playSound(name.c_str());

}

void musicEngine::setMusic(const char *song)
{
    engine->stopAllSounds();
    name = song;
}

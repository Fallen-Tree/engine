#pragma once
#include <string>
#include "math_types.hpp"
#include "bass.h"

enum SoundType {
    SOUND_FLAT,
    SOUND_3D
};

class Sound {
 public:
    explicit Sound(SoundType type, std::string path, bool looped = false);

    // Set volume of sound from [0, 1], 1 by default
    // BASS docs saying that it can be more than 1
    void SetVolume(float v);
    float GetVolume();

    // Set position of soundSorce
    // Positions of 3D sounds Objects with Transform component changing BY ENGINE every frame!
    void SetPosition(Vec3);

    // Radius in which sound can be heared (plays role only for 3D)
    // Very big by deafault
    void SetRadius(float r);

    SoundType GetType();

    void Start();
    void Pause();

    // Last volume will be saved
    void Mute();
    void Unmute();

    bool isPlaying();

    // This will free memory
    void StopForever();

 private:
    SoundType m_Type;
    DWORD m_Channel;
    float m_Volume;
};

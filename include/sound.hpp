#pragma once
#include <string>
#include "math_types.hpp"

enum SoundType {
    SOUND_FLAT,
    SOUND_3D
};

class Sound {
 public:
    Sound();
    explicit Sound(SoundType type, std::string path);

    // Will set default params
    void PlaySound(SoundType type, std::string path);

    // Set volume of sound from [0, 1], 1 by default
    void SetVolume(float v);
    float GetVolume();

    // Set position of soundSorce
    // Positions of 3D sounds Objects with Transform component changing BY ENGINE every frame!
    void SetPosition(Vec3);
    Vec3 GetPosition();

    // Radius in which sound can be heared (plays role only for 3D)
    // Very big by deafault
    void SetRadius(float r);
    float GetRadius();

    SoundType GetType();

    void Start();
    void Pause();

    // Last volume will be saved
    void Mute();
    void Unmute();

    // Repeat audio after ending, noloop by default
    void Loop();
    void Unloop();

    bool isPlaying();

    // This will free memory until new PlaySound
    void StopForever();

 private:
    SoundType m_Type;
   // irrklang::ISound* m_Sound;
   // irrklang::vec3df m_Position;
    float m_Volume;
    float m_Radius;

    void play(SoundType type, std::string path);
};

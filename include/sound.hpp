#pragma once
#include <conio.h>
#include <irrKlang.h>
#include <string>
#include "math_types.hpp"

class Sound {
 public:
    explicit Sound(std::string path, float volume = 1.f);

    void SetVolume(float v);
    void SetPosition(Vec3);

    void StopForever();
    void Start();
    void Pause();
    bool isPlaying();
 private:
    irrklang::ISound* m_Sound;
    irrklang::vec3df m_Position;
    float m_Volume;
};

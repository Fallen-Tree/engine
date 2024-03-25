#include "sound.hpp"
#include "path_resolver.hpp"
#include "bass.h"

// irrklang::ISoundEngine *SoundEngine = irrklang::createIrrKlangDevice();

void Sound::play(SoundType type, std::string path) {
    path = GetResourcePath(Resource::SOUND, path);

    BASS_Init(-1, 44100, 0, NULL, NULL);
    auto h = BASS_SampleLoad(false, path.c_str(), 0, 0, 55, 0);
    auto ch = BASS_SampleGetChannel(h, 0);
    BASS_ChannelPlay(ch, 0);

    // irrklang::vec3df position(0, 0, 0);
    // m_Type = type;
    // m_Position = position;
    // m_Volume = 1.f;
    // m_Radius = 100000000.f;

    // if (type == SOUND_FLAT) {
    //     m_Sound = SoundEngine->play2D(path.c_str(), false, true, true);
    // } else {
    //     m_Sound = SoundEngine->play3D(path.c_str(), position, false, true, true);
    // }
}

// void Sound::PlaySound(SoundType type, std::string path) {
//     StopForever();
//     play(type, path);
// }

Sound::Sound() {
}

Sound::Sound(SoundType type, std::string path) {
    play(type, path);
}

void Sound::SetVolume(float v) {
   // m_Sound->setVolume(v);
}
float Sound::GetVolume() {
    return m_Volume;
}

void Sound::SetPosition(Vec3 v) {
   // irrklang::vec3df pos(v.x, v.y, v.z);
   // m_Sound->setPosition(pos);
}
Vec3 Sound::GetPosition() {
    return Vec3(0);
    //return Vec3(m_Position.X, m_Position.Y, m_Position.Z);
}

void Sound::SetRadius(float r) {
    m_Radius = r;
}
float Sound::GetRadius() {
    return m_Radius;
}

SoundType Sound::GetType() {
    return m_Type;
}

void Sound::Pause() {
    //m_Sound->setIsPaused(true);
}
void Sound::Start() {
    //m_Sound->setIsPaused(false);
}

void Sound::Mute() {
    //m_Sound->setVolume(0);
}
void Sound::Unmute() {
    //m_Sound->setVolume(m_Volume);
}

void Sound::Loop() {
    //m_Sound->setIsLooped(true);
}
void Sound::Unloop() {
   //  m_Sound->setIsLooped(false);
}

bool Sound::isPlaying() {
    return true;
    //return m_Sound->isFinished();
}

void Sound::StopForever() {
   // m_Sound->stop();
   // m_Sound->drop();
}

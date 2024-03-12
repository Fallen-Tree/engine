#include "sound.hpp"
#include "path_resolver.hpp"

irrklang::ISoundEngine *SoundEngine = irrklang::createIrrKlangDevice();


// TODO(ZuZKho): if object doesnt have transform component, by deafult position will be 0,
// and the radius of hearing is infinite
// Add loop\unloop function
// initial Transform?
Sound::Sound(std::string path, float volume) {
    path = GetResourcePath(Resource::SOUND, path);
    irrklang::vec3df position(0, 0, 0);
    m_Sound = SoundEngine->play3D(path.c_str(), position, true, true, true);
}

void Sound::SetVolume(float v) {
    m_Sound->setVolume(v);
}

void Sound::SetPosition(Vec3 v) {
    irrklang::vec3df pos(v.x, v.y, v.z);
    m_Sound->setPosition(pos);
}

// This will free memory
void Sound::StopForever() {
    m_Sound->stop();
}
void Sound::Pause() {
    m_Sound->setIsPaused(true);
}
void Sound::Start() {
    m_Sound->setIsPaused(false);
}
bool Sound::isPlaying() {
    return m_Sound->isFinished();
}

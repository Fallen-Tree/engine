#include "sound.hpp"
#include "path_resolver.hpp"
#include "bass.h"

Sound::Sound(SoundType type, std::string path, bool looped) {
    path = GetResourcePath(Resource::SOUND, path);
    DWORD loop = looped ? BASS_SAMPLE_LOOP : 0;
    HSAMPLE sample;

    m_Type = type;
    m_Volume = 1.f;

    if (type == SOUND_FLAT) {
        sample = BASS_SampleLoad(false, path.c_str(), 0, 0, 10, 0);

        m_Channel = BASS_SampleGetChannel(sample, 0);
    } else {
        sample = BASS_SampleLoad(false, path.c_str(), 0, 0, 10,
            BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX | loop);

        m_Channel = BASS_SampleGetChannel(sample, 0);
        SetPosition(Vec3(100000000.f, 100000000.f, 100000000.f));
    }
}

void Sound::SetVolume(float v) {
    m_Volume = v;
    BASS_ChannelSetAttribute(m_Channel, BASS_ATTRIB_VOL, m_Volume);
}
float Sound::GetVolume() {
    return m_Volume;
}

void Sound::SetPosition(Vec3 v) {
    BASS_3DVECTOR pos = {v.x, v.y, v.z};
    if (m_Type == SOUND_3D) {
        BASS_ChannelSet3DPosition(m_Channel, &pos, NULL, NULL);
    }
}

void Sound::SetRadius(float r) {
    BASS_ChannelSet3DAttributes(m_Channel, -1, -1, r, -1, -1, -1);
}

SoundType Sound::GetType() {
    return m_Type;
}

void Sound::Pause() {
    BASS_ChannelPause(m_Channel);
}
void Sound::Start() {
    BASS_ChannelStart(m_Channel);
}

void Sound::Mute() {
    BASS_ChannelSetAttribute(m_Channel, BASS_ATTRIB_VOL, 0);
}

void Sound::Unmute() {
    BASS_ChannelSetAttribute(m_Channel, BASS_ATTRIB_VOL, m_Volume);
}

bool Sound::isPlaying() {
    return BASS_ChannelIsActive(m_Channel) == BASS_ACTIVE_PLAYING;
}

void Sound::StopForever() {
    BASS_ChannelStop(m_Channel);
    BASS_ChannelFree(m_Channel);
}

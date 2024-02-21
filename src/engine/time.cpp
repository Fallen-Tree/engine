#include "time.hpp"

unsigned int Time::m_fps = 0;
float Time::m_deltaTime = 0;
float Time::m_currentTime = 0;

int Time::GetCurrentFps() {
    return m_fps;
}
float Time::GetDeltaTime() {
    return m_deltaTime;
}
float Time::GetCurrentTime() {
    return m_currentTime;
}

void Time::SetCurrentFps(int fps) {
    m_fps = fps;
}
void Time::SetCurrentTime(float cur) {
    m_currentTime = cur;
}
void Time::SetDeltaTime(float deltaTime) {
    m_deltaTime = deltaTime;
}

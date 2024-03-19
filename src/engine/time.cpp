#include "time.hpp"

unsigned int Time::m_fps = 0;
float Time::m_deltaTime = 0;

int Time::GetCurrentFps() {
    return m_fps;
}

void Time::SetCurrentFps(int fps) {
    m_fps = fps;
}
float Time::GetDeltaTime() {
    return m_deltaTime;
}
void Time::SetDeltaTime(float deltaTime) {
    m_deltaTime = deltaTime;
}

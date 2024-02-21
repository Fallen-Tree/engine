#pragma once

class Time {
 private:
    static unsigned int m_fps;
    static float m_deltaTime;
    static float m_currentTime;

 public:
    static int GetCurrentFps();
    static float GetDeltaTime();
    static float GetCurrentTime();

    static void SetCurrentFps(int);
    static void SetCurrentTime(float);
    static void SetDeltaTime(float);
};

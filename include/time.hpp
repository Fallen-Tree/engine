#pragma once

class Time {
 private:
    static unsigned int m_fps;
    static float m_deltaTime;

 public:
    static int GetCurrentFps();
    static void SetCurrentFps(int);
    static float GetDeltaTime();
    static void SetDeltaTime(float);
};

#pragma once
#include <vector>
#include "modelInstance.hpp"
#include "camera.hpp"
#include "input.hpp"


struct GLFWwindow;

class Object {
 public:
     ModelInstance *m_modelInstance;
     Camera *m_Camera;
};

class Engine {
 public:
    Engine();
    ~Engine();

    // TODO(theblek): Ensure inlining for release builds
    template<class T>
    void AddObject(T *a) {
        AddObject(static_cast<Object *>(a));
    }

    // Switching between cameras
    void SwitchMainCamera();
    void SwitchObjectCamera(Object *o);

    void Run(int, int);
    Camera *m_Camera;
    Input m_Input;

 private:
    void AddObject(Object *);
    void Render(int, int);

    Camera m_MainCamera;
    std::vector<Object *> m_objects;
    GLFWwindow *m_Window;
};

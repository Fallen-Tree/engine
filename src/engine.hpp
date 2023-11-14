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

    // You have to switch to main camera yourself after using this method
    Camera* SwitchCamera(Camera *newCamera);

    void Run(int, int);
    Camera *m_Camera;
    Input m_Input;

 private:
    void AddObject(Object *);
    void Render(int, int);

    std::vector<Object *> m_objects;
    GLFWwindow *m_Window;
};

#pragma once
#include <vector>
#include "modelInstance.hpp"
#include "camera.hpp"
#include "controller.hpp"

struct GLFWwindow;

class Object {
 public:
     ModelInstance *m_modelInstance;
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

    void Run(int, int);
    Input *m_Input = nullptr;
    Camera m_Camera;
 private:
    void AddObject(Object *);
    void Render(int, int);


    std::vector<Object *> m_objects;
    GLFWwindow *m_Window;
};

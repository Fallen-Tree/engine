#pragma once
#include <vector>
#include "model/modelInstance.hpp"

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
 private:
    void AddObject(Object *);
    void Render();

    std::vector<Object *> m_objects;

    GLFWwindow *m_window;
};

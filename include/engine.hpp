#pragma once
#include <vector>
#include "render_data.hpp"
#include "transform.hpp"
#include "camera.hpp"
#include "input.hpp"

const int MAX_ENTITIES = 10000;

struct GLFWwindow;

class Object {
 public:
     Transform *m_Transform;
     RenderData *m_RenderData;
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
    Input m_Input;
    Camera m_Camera;

 private:
    void AddObject(Object *);
    void Render(int, int);


    std::vector<Object *> m_Objects;
    GLFWwindow *m_Window;
};

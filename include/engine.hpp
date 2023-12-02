#pragma once
#include <vector>
#include "collider.hpp"
#include "render_data.hpp"
#include "transform.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "animation.hpp"

struct GLFWwindow;

class Object {
 public:
     virtual void Update(float) {}

     Transform *transform = nullptr;
     RenderData *renderData = nullptr;
     Collider *collider = nullptr;
     Animation *animation = nullptr;
};

class Engine {
 public:
    Engine(int, int);
    ~Engine();

    // TODO(theblek): Ensure inlining for release builds
    template<class T>
    void AddObject(T *a) {
        AddObject(static_cast<Object *>(a));
    }

    Camera* SwitchCamera(Camera* newCamera);
    void Run(int, int);
    Input m_Input;
    Camera* camera;

 private:
    void AddObject(Object *);
    void Render(int, int);
    void updateObjects(float);

    std::vector<Object *> m_Objects;
    GLFWwindow *m_Window;
};

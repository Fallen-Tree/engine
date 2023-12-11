#pragma once
#include <vector>
#include "collider.hpp"
#include "render_data.hpp"
#include "transform.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "animation.hpp"
#include "text.hpp"
#include "font.hpp"
#include "logger.hpp"
#include "time.hpp"

struct GLFWwindow;

class Object {
 public:
     virtual void Update(float) {}

     Transform *transform = nullptr;
     RenderData *renderData = nullptr;
     Collider *collider = nullptr;
     Animation *animation = nullptr;
     Text *text = nullptr;
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

    Camera* SwitchCamera(Camera* newCamera);
    void Run();
    Input m_Input;
    Camera* camera;

 private:
    void AddObject(Object *);
    void Render(int, int);
    void updateObjects(float);

    std::vector<Object *> m_Objects;
    GLFWwindow *m_Window;
};

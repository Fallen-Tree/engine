#pragma once
#include <vector>
#include "render_data.hpp"
#include "transform.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "non_skeletal_animation.hpp"

struct GLFWwindow;

class Object {
 public:
     Transform *transform;
     RenderData *renderData;
     NonSkeletalAnimation *nonSkeletalAnimation;
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

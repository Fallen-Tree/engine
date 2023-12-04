#pragma once
#include <vector>
#include <variant>
#include "render_data.hpp"
#include "transform.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "animation.hpp"
#include "light.hpp"

struct GLFWwindow;

class Object {
 public:
     Transform *transform;
     RenderData *renderData;
     Animation *animation;
     // monostate is canonical c++ empty state in variant
     std::variant<DirLight*, PointLight*, SpotLight*, std::monostate> light
         = std::monostate();
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
    std::vector<PointLight*> m_PointLights = std::vector<PointLight*>();
    std::vector<DirLight*> m_DirLights = std::vector<DirLight*>();
    std::vector<SpotLight*> m_SpotLights = std::vector<SpotLight*>();
};

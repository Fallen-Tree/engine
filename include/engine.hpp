#pragma once
#include <vector>
#include <variant>
#include <string>
#include "collider.hpp"
#include "collisions.hpp"
#include "render_data.hpp"
#include "transform.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "animation.hpp"
#include "text.hpp"
#include "font.hpp"
#include "logger.hpp"
#include "time.hpp"
#include "light.hpp"
#include "path_resolver.hpp"
#include "sound.hpp"
#include "rigid_body.hpp"
#include "pretty_print.hpp"
#include "images.hpp"

extern Input *s_Input;

struct GLFWwindow;

class Object {
 public:
     virtual void Update(float) {}

     Transform *transform = nullptr;
     RenderData *renderData = nullptr;
     Collider *collider = nullptr;
     Animation *animation = nullptr;
     Text *text = nullptr;
     Sound *sound = nullptr;

     RigidBody *rigidbody = nullptr;
     Image *image = nullptr;

     // monostate is canonical c++ empty state in variant
     std::variant<DirLight*, PointLight*, SpotLight*, std::monostate> light
         = std::monostate();
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
    std::vector<PointLight*> m_PointLights = std::vector<PointLight*>();
    std::vector<DirLight*> m_DirLights = std::vector<DirLight*>();
    std::vector<SpotLight*> m_SpotLights = std::vector<SpotLight*>();
};

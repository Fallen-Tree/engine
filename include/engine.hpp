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
#include "model.hpp"
#include "path_resolver.hpp"
#include "packed_array.hpp"
#include "rigid_body.hpp"
#include "pretty_print.hpp"
#include "images.hpp"
#include "skeletal_animations_manager.hpp"
#include "skeletal_animation_data.hpp"

extern Input *s_Input;

class Object;
class Behaviour;

using ObjectHandle = int;

class Engine {
 public:
    Engine();
    ~Engine();

    Transform *GetTransform(ObjectHandle);
    Transform GetGlobalTransform(ObjectHandle);
    Model *GetModel(ObjectHandle);
    Collider *GetCollider(ObjectHandle);
    RigidBody *GetRigidBody(ObjectHandle);
    Animation *GetAnimation(ObjectHandle);
    Text *GetText(ObjectHandle);
    SkeletalAnimationsManager *GetSkeletalAnimationsManager(ObjectHandle);
    Image *GetImage(ObjectHandle);
    PointLight *GetPointLight(ObjectHandle);
    SpotLight *GetSpotLight(ObjectHandle);
    DirLight *GetDirLight(ObjectHandle);
    Behaviour *GetBehaviour(ObjectHandle);

    Transform &AddTransform(ObjectHandle, Transform);
    Model &AddModel(ObjectHandle, Model);
    Collider &AddCollider(ObjectHandle, Collider);
    RigidBody &AddRigidBody(ObjectHandle, RigidBody);
    Animation &AddAnimation(ObjectHandle, Animation);
    Text &AddText(ObjectHandle, Text);
    SkeletalAnimationsManager &AddSkeletalAnimationsManager(ObjectHandle, SkeletalAnimationsManager);
    Image &AddImage(ObjectHandle, Image);
    PointLight &AddPointLight(ObjectHandle, PointLight);
    SpotLight &AddSpotLight(ObjectHandle, SpotLight);
    DirLight &AddDirLight(ObjectHandle, DirLight);

    template<typename T>
    T &AddBehaviour(ObjectHandle id, T *t) {
        m_Behaviours.SetData(id, t);
        return static_cast<T &>(*m_Behaviours.GetData(id));
    }

    void RemoveObject(ObjectHandle);
    Object NewObject();
    void AddChild(ObjectHandle parent, ObjectHandle child);
    Object GetParent(ObjectHandle node);

    Camera* SwitchCamera(Camera* newCamera);
    void Run();
    Input m_Input;
    Camera* camera;

 private:
    void Render(int, int);
    void updateObjects(float);

    GLFWwindow *m_Window;

    // Components storage
    template<typename T>
    using ComponentArray = PackedArray<T, MAX_OBJECT_COUNT>;

    ComponentArray<Transform> m_Transforms;
    ComponentArray<Model> m_Models;
    ComponentArray<Collider> m_Colliders;
    ComponentArray<RigidBody> m_RigidBodies;
    ComponentArray<Animation> m_Animations;
    ComponentArray<Image> m_Images;
    ComponentArray<Text> m_Texts;
    ComponentArray<SkeletalAnimationsManager> m_SkeletalAnimationsManagers;

    ComponentArray<PointLight> m_PointLights;
    ComponentArray<DirLight> m_DirLights;
    ComponentArray<SpotLight> m_SpotLights;

    ComponentArray<Behaviour *> m_Behaviours;
    int m_ObjectCount;

    // Hierarchy tree
    PackedArray<ObjectHandle, MAX_OBJECT_COUNT> m_Parents;
    PackedArray<std::vector<ObjectHandle>, MAX_OBJECT_COUNT> m_Children;
};

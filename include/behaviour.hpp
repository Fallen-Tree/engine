#pragma once
#include "transform.hpp"
#include "collider.hpp"
#include "animation.hpp"
#include "text.hpp"
#include "light.hpp"
#include "render_data.hpp"
#include "object.hpp"

// Do we really need this class?
// Maybe add support for lambdas as behaviours?
class Behaviour {
 public:
    virtual void Start(float dt) {}
    virtual void Update(float dt) = 0;

    Behaviour() = default;

    Object self;
};

#pragma once

#define RESOURCE_DIR "./resources"

// Camera
#define MAX_PITCH                   89.0f
#define MIN_PITCH                   -89.0f
#define MAX_FOV                     70.0f 
#define MIN_FOV                     1.0f
#define DFL_ZOOM                    70.f
#define DFL_YAW                     -90.f
#define DFL_PITCH                   0.0f
#define DFL_SPEED                   2.5f
#define MAX_DOT                     0.95

#define DFL_NEAR_PLANE              0.1f
#define DFL_FAR_PLANE               100.0f

// textures
#define MAX_COUNT_TEXTURE           16

// engine
#define EPS                         0.001f
#define FPS_SHOWING_INTERVAL        0.5f
#define MAX_OBJECT_COUNT            1000
#define MAX_BONES                   100

// input
#define MAX_VALID_KEY               350

// Math
#define DOWN                        Vec3(0, -1, 0)


// Collisions
#define EJECTION_RATIO              3


// rigid body

#define DUMP                        0.05
#define TORQUE_RATIO                1

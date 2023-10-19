#pragma once

#include <glfw/include/GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "model.hpp"


enum key {
/* Printable keys */
Space = GLFW_KEY_SPACE,
Apostrophe = GLFW_KEY_APOSTROPHE,         /* ' */
Comma = GLFW_KEY_COMMA,              /* , */
Minus = GLFW_KEY_MINUS,              /* - */
Period = GLFW_KEY_PERIOD,             /* . */
Slash = GLFW_KEY_SLASH,              /* / */
K0 = GLFW_KEY_0,
K1 = GLFW_KEY_1,
K2 = GLFW_KEY_2,
K3 = GLFW_KEY_3,
K4 = GLFW_KEY_4,
K5 = GLFW_KEY_5,
K6 = GLFW_KEY_6,
K7 = GLFW_KEY_7,
K8 = GLFW_KEY_8,
K9 = GLFW_KEY_9,
Semicolon = GLFW_KEY_SEMICOLON,          /* ; */
Equal = GLFW_KEY_EQUAL,              /* = */
A = GLFW_KEY_A,
B = GLFW_KEY_B,
C = GLFW_KEY_C,
D = GLFW_KEY_D,
E = GLFW_KEY_E,
F = GLFW_KEY_F,
G = GLFW_KEY_G,
H = GLFW_KEY_H,
I = GLFW_KEY_I,
J = GLFW_KEY_J,
K = GLFW_KEY_K,
L = GLFW_KEY_L,
M = GLFW_KEY_M,
N = GLFW_KEY_N,
O = GLFW_KEY_O,
P = GLFW_KEY_P,
Q = GLFW_KEY_Q,
R = GLFW_KEY_R,
S = GLFW_KEY_S,
T = GLFW_KEY_T,
U = GLFW_KEY_U,
V = GLFW_KEY_V,
W = GLFW_KEY_W,
X = GLFW_KEY_X,
Y = GLFW_KEY_Y,
Z = GLFW_KEY_Z,
Lbracket = GLFW_KEY_LEFT_BRACKET,       /* [ */
Backslash = GLFW_KEY_BACKSLASH,          /* \ */
Rbracket = GLFW_KEY_RIGHT_BRACKET,      /* ] */
Grave_accent = GLFW_KEY_GRAVE_ACCENT,       /* ` */

/* Function keys */
Escape = GLFW_KEY_ESCAPE,
Enter = GLFW_KEY_ENTER,
Tab = GLFW_KEY_TAB,
Backspace = GLFW_KEY_BACKSPACE,
Insert = GLFW_KEY_INSERT,
Delete = GLFW_KEY_DELETE,
Right = GLFW_KEY_RIGHT,
Left = GLFW_KEY_LEFT,
Down = GLFW_KEY_DOWN,
Up = GLFW_KEY_UP,
PageUp = GLFW_KEY_PAGE_UP,
PageDown = GLFW_KEY_PAGE_DOWN,
Home = GLFW_KEY_HOME,
End = GLFW_KEY_END,
CapsLock = GLFW_KEY_CAPS_LOCK,
ScrollLock = GLFW_KEY_SCROLL_LOCK,
NumLock = GLFW_KEY_NUM_LOCK,
PrintSreen = GLFW_KEY_PRINT_SCREEN,
Pause = GLFW_KEY_PAUSE,
F1 = GLFW_KEY_F1,
F2 = GLFW_KEY_F2,
F3 = GLFW_KEY_F3,
F4 = GLFW_KEY_F4,
F5 = GLFW_KEY_F5,
F6 = GLFW_KEY_F6,
F7 = GLFW_KEY_F7,
F8 = GLFW_KEY_F8,
F9 = GLFW_KEY_F9,
F10 = GLFW_KEY_F10,
F11 = GLFW_KEY_F11,
F12 = GLFW_KEY_F12,
F13 = GLFW_KEY_F13,
F14 = GLFW_KEY_F14,
F15 = GLFW_KEY_F15,
F16 = GLFW_KEY_F16,
F17 = GLFW_KEY_F17,
F18 = GLFW_KEY_F18,
F19 = GLFW_KEY_F19,
F20 = GLFW_KEY_F20,
F21 = GLFW_KEY_F21,
F22 = GLFW_KEY_F22,
F23 = GLFW_KEY_F23,
F24 = GLFW_KEY_F24,
F25 = GLFW_KEY_F25,
KP0 = GLFW_KEY_KP_0,
KP1 = GLFW_KEY_KP_1,
KP2 = GLFW_KEY_KP_2,
KP3 = GLFW_KEY_KP_3,
KP4 = GLFW_KEY_KP_4,
KP5 = GLFW_KEY_KP_5,
KP6 = GLFW_KEY_KP_6,
KP7 = GLFW_KEY_KP_7,
KP8 = GLFW_KEY_KP_8,
KP9 = GLFW_KEY_KP_9,
Decimal = GLFW_KEY_KP_DECIMAL,
Divide = GLFW_KEY_KP_DIVIDE,
Multipy = GLFW_KEY_KP_MULTIPLY,
Subtract = GLFW_KEY_KP_SUBTRACT,
Add = GLFW_KEY_KP_ADD,
KEnter = GLFW_KEY_KP_ENTER,
KEqual = GLFW_KEY_KP_EQUAL,
LShift = GLFW_KEY_LEFT_SHIFT,
LControl = GLFW_KEY_LEFT_CONTROL,
LAlt = GLFW_KEY_LEFT_ALT,
LSuper = GLFW_KEY_LEFT_SUPER,
RShift = GLFW_KEY_RIGHT_SHIFT,
RControl = GLFW_KEY_RIGHT_CONTROL,
RAlt = GLFW_KEY_RIGHT_ALT,
RSuper = GLFW_KEY_RIGHT_SUPER,
Menu = GLFW_KEY_MENU,

// mouse
MouseLast = GLFW_MOUSE_BUTTON_LAST,
MouseLeft = GLFW_MOUSE_BUTTON_LEFT,
MouseRight = GLFW_MOUSE_BUTTON_RIGHT,
MouseMiddle = GLFW_MOUSE_BUTTON_MIDDLE,
};  // namespace key


// Initial (Default) conroller values
const int MODE = GLFW_CURSOR;
const int VALUE = GLFW_CURSOR_DISABLED;

class Input {
 private:
    GLFWwindow *m_Window;
    float m_ScrollOffset = 0.f;
    // m_X.x -- lastX, m_X.y -- currentX
    glm::vec2 m_X = glm::vec2(0);
    // m_Y.x -- lastY, m_Y.y -- currentY
    glm::vec2 m_Y = glm::vec2(0);

 public:
    explicit Input(GLFWwindow *window);

    bool IsKeyPressed(key button);
    float ScrollOfsset();
    float MouseX();
    float MouseY();
    float OffsetX();
    float OffsetY();


    void Update();
    void SetWindow(GLFWwindow * window);
    void SetMouseX(float x);
    void SetMouseY(float y);
    void SetScrollOffset(float offset);
};

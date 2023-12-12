#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "text.hpp"
#include "logger.hpp"
#include "shaders.hpp"
#include "user_config.hpp"
#include "engine_config.hpp"

void Text::RenderText() {
    this->font->RenderText(this->text, x, y, scale, color);
}

Text::Text(Font* font, std::string text, float x, float y, float scale, Vec3 color) {
    this->font = font;
    this->text = text;
    this->x = x;
    this->y = y;
    this->scale = scale;
    this->color = color;
}

void Text::SetCoordinates(float x, float y) {
    this->x = x;
    this->y = y;
}

void Text::SetScale(float scale) {
    this->scale = scale;
}

void Text::SetColor(Vec3 color) {
    this->color = color;
}

void Text::SetRenderData(float x, float y, float scale, Vec3 color) {
    this->x = x;
    this->y = y;
    this->scale = scale;
    this->color = color;
}

void Text::SetContent(std::string s) {
    this->text = s;
}

void Text::SetFont(Font *font) {
    this->font = font;
}

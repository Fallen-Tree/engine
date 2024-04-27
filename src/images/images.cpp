#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include "images.hpp"
#include "path_resolver.hpp"
#include "stb_image.h"
#include "user_config.hpp"

Image::Image(std::string path, float relX, float relY, float scale) {
  path = GetResourcePath(Resource::IMAGE, path);
  m_Visible = true;
  m_RelX = relX;
  m_RelY = relY;
  m_Scale = scale;

  int nrComponents;
  unsigned char *data = reinterpret_cast<unsigned char*>(
    stbi_load(path.c_str(), &m_Width, &m_Height, &nrComponents, 0));

  if (!data) {
      stbi_image_free(data);
      Logger::Error("IMAGE::LOADER::FILE_NOT_FOUND_FAILED: %s", path.c_str());
      return;
  }

  glGenTextures(1, &m_TextureID);
  glBindTexture(GL_TEXTURE_2D, m_TextureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(data);

  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);
  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Image::SetShaderProgram(ShaderProgram sp) {
    m_ShaderProgram = sp;
}

ShaderProgram Image::GetShaderProgram() {
    return m_ShaderProgram;
}

void Image::Render() {
  if (!m_Visible) return;

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  m_ShaderProgram.Use();
  glm::mat4 projection =
      glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
  m_ShaderProgram.SetMat4("projection", projection);
  m_ShaderProgram.SetVec3("textColor", Vec3(1));

  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(m_VAO);

  float xpos = m_RelX * SCR_WIDTH;
  float ypos = m_RelY * SCR_HEIGHT;
  float w = m_Width * m_Scale;
  float h = m_Height * m_Scale;

  float vertices[6][4] = {
    { xpos,     ypos + h,   0.0f, 0.0f },
    { xpos,     ypos,       0.0f, 1.0f },
    { xpos + w, ypos,       1.0f, 1.0f },

    { xpos,     ypos + h,   0.0f, 0.0f },
    { xpos + w, ypos,       1.0f, 1.0f },
    { xpos + w, ypos + h,   1.0f, 0.0f }
  };

  glBindTexture(GL_TEXTURE_2D, m_TextureID);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
}

void Image::Hide() {
  m_Visible = false;
}

void Image::Show() {
  m_Visible = true;
}

void Image::SetScale(float scale) {
  m_Scale = scale;
}

void Image::SetPosition(float relX, float relY) {
  m_RelX = relX;
  m_RelY = relY;
}


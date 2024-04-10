#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstddef>

#include "render_data.hpp"
#include "logger.hpp"

RenderMesh::RenderMesh(std::vector<Vertex> points, std::vector<unsigned int> indices, Material material) {
    this->setPoints(points);
    this->setIndices(indices);
    this->material = material;

    bindRenderData(this);
}

RenderMesh::RenderMesh(Mesh *mesh, Material material) {
    this->setPoints(mesh->getVecPoints());
    this->setIndices(mesh->getVecIndices());
    this->material = material;

    bindRenderData(this);
}

void RenderMesh::setMaterial(Material material) {
    this->material = material;
}

void bindRenderData(RenderMesh* render_data) {
    if (!render_data) {
        Logger::Error("RENDER_DATA::BINDER::RENDER_MESH_ARE_NULL");
    }
    glGenVertexArrays(1, &render_data->VAO);
    glGenBuffers(1, &render_data->VBO);
    glGenBuffers(1, &render_data->EBO);
    // bind the Vertex Array Object first,
    // then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).

    glBindVertexArray(render_data->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, render_data->VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        render_data->getLenArrPoints() * sizeof(Vertex),
        render_data->getPoints(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data->EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        render_data->getLenIndices() * sizeof(unsigned int),
        render_data->getIndices(),
        GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, Position)));
    // normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, Normal)));
    // texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));
    // ids
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, m_BoneIDs)));
    // weights
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, m_Weights)));

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as
    // the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as
    // the bound element buffer object IS stored in the VAO; keep the EBO bound.
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO,
    // but this rarely happens. Modifying other VAOs requires a call to glBindVertexArray anyways
    // so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}

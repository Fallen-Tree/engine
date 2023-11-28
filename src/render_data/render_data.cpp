#include "render_data.hpp"

void bindRenderData(RenderData* render_data) {
    if (!render_data || !render_data->model)
        return;
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
        render_data->model->getLenArrPoints() * sizeof(float),
        render_data->model->getPoints(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data->EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        render_data->model->getLenIndices() * sizeof(unsigned int),
        render_data->model->getIndices(),
        GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

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

#version 330 core

#define NR_SPOT_LIGHTS 3
#define NR_DIR_LIGHTS 1

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int lenArrSpotL;
uniform int lenArrPointL;
uniform int lenArrDirL;
uniform mat4 dirLightSpace[NR_DIR_LIGHTS];
uniform mat4 spotLightSpace[NR_SPOT_LIGHTS];

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosDirLight[NR_DIR_LIGHTS];
out vec4 FragPosSpotLight[NR_SPOT_LIGHTS];
void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    Normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * vec4(position, 1.0));
    for (int i = 0; i < lenArrDirL; i++)
        FragPosDirLight[i] = dirLightSpace[i] * vec4(FragPos, 1.0f);
    for (int i = 0; i < lenArrSpotL; i++)
        FragPosSpotLight[i] = spotLightSpace[i] * vec4(FragPos, 1.0f);
    TexCoord = aTexCoord;
}

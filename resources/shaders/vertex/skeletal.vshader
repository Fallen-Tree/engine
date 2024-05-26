#version 330 core

#define NR_SPOT_LIGHTS 3
#define NR_DIR_LIGHTS 1

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in ivec4 boneIds; 
layout(location = 4) in vec4 weights;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform int lenArrSpotL;
uniform int lenArrPointL;
uniform int lenArrDirL;
uniform mat4 dirLightSpace[NR_DIR_LIGHTS];
uniform mat4 spotLightSpace[NR_SPOT_LIGHTS];
	
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
	
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosDirLight[NR_DIR_LIGHTS];
out vec4 FragPosSpotLight[NR_SPOT_LIGHTS];

void main()
{
    int flag = 0;
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >= MAX_BONES) 
        {
            totalPosition = vec4(position,1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(position,1.0f);
        totalPosition += localPosition * weights[i];
        flag = 1;
    }

    if (flag == 0) {
      totalPosition = vec4(position,1.0f);
    }

    gl_Position =  projection * view * model * totalPosition;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * vec4(totalPosition, 1.0));
    for (int i = 0; i < lenArrDirL; i++)
        FragPosDirLight[i] = dirLightSpace[i] * vec4(FragPos, 1.0f);
    for (int i = 0; i < lenArrSpotL; i++)
        FragPosSpotLight[i] = spotLightSpace[i] * vec4(FragPos, 1.0f);
    TexCoord = aTexCoord;
}

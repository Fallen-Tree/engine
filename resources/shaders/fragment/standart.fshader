#version 460 core

struct Material {
    float shininess;
    sampler2D diffuse;
    sampler2D specular;
    vec3 diffuseColor;
    vec3 specularColor;
};

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constDistCoeff;
    float linearDistCoeff;
    float quadraticDistCoeff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constDistCoeff;
    float linearDistCoeff;
    float quadraticDistCoeff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 10
#define NR_SPOT_LIGHTS 3
#define NR_DIR_LIGHTS 2

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in vec4 FragPosDirLight[NR_DIR_LIGHTS];
in vec4 FragPosSpotLight[NR_SPOT_LIGHTS];

// function prototypes
vec3 CalcDirLight(int id, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(int id, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(int id, vec3 normal, vec3 viewDir);

// light uniform
uniform DirLight dirLights[NR_DIR_LIGHTS]; 
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int lenArrSpotL;
uniform int lenArrPointL;
uniform int lenArrDirL;
// other uniform
uniform int useTextures;
uniform Material material; 
uniform vec3 viewPos;

uniform sampler2D shadowMapDir[NR_DIR_LIGHTS];

out vec4 FragColor;

vec3 Diffuse;
vec3 Specular;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    if (useTextures == 0) {
        Diffuse = material.diffuseColor;
        Specular = material.specularColor;
    } else {
        Diffuse = vec3(texture(material.diffuse, TexCoord));
        Specular = vec3(texture(material.specular, TexCoord));
    }

    vec3 result = vec3(0);  
    // phase 1: directional lighting
    for (int i = 0; i < lenArrDirL; i++)
        result += CalcDirLight(i, norm, viewDir);
    // phase 2: point lights   
    for (int i = 0; i < lenArrPointL; i++)
        result += CalcPointLight(i, norm, viewDir);
    // phase 3: spot lights
    for (int i = 0; i < lenArrSpotL; i++)
        result += CalcSpotLight(i, norm, viewDir); 
    
    FragColor = vec4(result, 1.0);    
}

// calculates the color when using a point light.
vec3 CalcPointLight(int id, vec3 normal, vec3 viewDir)
{
    PointLight light = pointLights[id];
    vec3 lightDir = normalize(light.position - FragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constDistCoeff + light.linearDistCoeff * distance + light.quadraticDistCoeff * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * Diffuse * attenuation; 
    vec3 diffuse = light.diffuse * diff * Diffuse * attenuation; 
    vec3 specular = light.specular * spec * Specular * attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(int id, vec3 normal, vec3 viewDir)
{
    SpotLight light = spotLights[id];
    vec3 lightDir = normalize(light.position - FragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constDistCoeff + light.linearDistCoeff * distance + light.quadraticDistCoeff * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * Diffuse * attenuation * intensity; 
    vec3 diffuse = light.diffuse * diff * Diffuse * attenuation * intensity; 
    vec3 specular = light.specular * spec * Specular * attenuation * intensity;
    return (ambient + diffuse + specular);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(int id, vec3 normal, vec3 viewDir)
{
    DirLight light = dirLights[id];
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * Diffuse; 

    float shadow = 1.f;
    vec3 projCoords = FragPosDirLight[id].xyz / FragPosDirLight[id].w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMapDir[id], projCoords.xy).r;
    float currentDepth = projCoords.z;
    if (currentDepth - 0.005f > closestDepth)
        shadow = 0.f;

    vec3 diffuse = shadow * light.diffuse * diff * Diffuse; 
    vec3 specular = shadow * light.specular * spec * Specular;
    return (ambient + diffuse + specular);
}

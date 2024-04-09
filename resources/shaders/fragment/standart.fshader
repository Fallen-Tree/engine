#version 330 core

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
#define NR_DIR_LIGHTS 1

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

// light uniform
uniform DirLight dirLight[NR_DIR_LIGHTS]; 
uniform SpotLight spotLight[NR_SPOT_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int lenArrSpotL;
uniform int lenArrPointL;
uniform int lenArrDirL;
// other uniform
uniform int useTextures;
uniform Material material; 
uniform vec3 viewPos;

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
        result += CalcDirLight(dirLight[i], norm, viewDir);
    // phase 2: point lights   
    for (int i = 0; i < lenArrPointL; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    // phase 3: spot lights
    for (int i = 0; i < lenArrSpotL; i++)
        result += CalcSpotLight(spotLight[i], norm, FragPos, viewDir); 
    
    FragColor = vec4(result, 1.0);    
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constDistCoeff + light.linearDistCoeff * distance + light.quadraticDistCoeff * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * Diffuse * attenuation; 
    vec3 diffuse = light.diffuse * diff * Diffuse * attenuation; 
    vec3 specular = light.specular * spec * Specular * attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
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
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * Diffuse; 
    vec3 diffuse = light.diffuse * diff * Diffuse; 
    vec3 specular = light.specular * spec * Specular;
    return (ambient + diffuse + specular);
}

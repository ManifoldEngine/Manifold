#type vertex 
#version 100

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextureCoordinate;

uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragmentPosition;
out vec3 normal;
out vec2 textureCoordinate;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
    fragmentPosition = vec3(model * vec4(aPosition, 1.0));
    normal = normalMatrix * aNormal;
    textureCoordinate = aTextureCoordinate;
}

#type fragment
#version 100

struct Material
{
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};

struct DirectionLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct Spotlight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outterCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define MAX_DIRECTIONAL_LIGHTS 16
#define MAX_POINT_LIGHTS 32
#define MAX_SPOTLIGHTS 32

in vec3 fragmentPosition;
in vec3 normal;
in vec2 textureCoordinate;

uniform int directionalLightsCount;
uniform int pointLightsCount;
uniform int spotlightsCount;

uniform Material material;
uniform DirectionLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform Spotlight spotlights[MAX_SPOTLIGHTS];

uniform vec3 viewPosition;

out vec4 FragColor;

vec3 processDirectionLight(int index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor);
vec3 processPointLight(int index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor);
vec3 processSpotlight(int index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor);

void main()
{
    vec3 normalizedNormal = normalize(normal);
    vec3 diffuseColor = texture(material.diffuseMap, textureCoordinate).rgb;
    vec3 specularColor = texture(material.specularMap, textureCoordinate).rgb;
    vec3 viewDirection = normalize(viewPosition - fragmentPosition);

    vec3 outputColor = vec3(0.0);

    // directional lights
    for (int i = 0; i < min(directionalLightsCount, MAX_DIRECTIONAL_LIGHTS); i++)
    {
        outputColor += processDirectionLight(i, normalizedNormal, viewDirection, diffuseColor, specularColor);
    }

    // point lights
    for (int i = 0; i < min(pointLightsCount, MAX_POINT_LIGHTS); i++)
    {
        outputColor += processPointLight(i, normalizedNormal, viewDirection, diffuseColor, specularColor);
    }

    // spotlights
    for (int i = 0; i < min(spotlightsCount, MAX_POINT_LIGHTS); i++)
    {
        outputColor += processSpotlight(i, normalizedNormal, viewDirection, diffuseColor, specularColor);
    }
    
    FragColor = vec4(outputColor, 0.5);
}

vec3 processDirectionLight(int index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor)
{
    vec3 lightDirection = normalize(-directionalLights[index].direction);

    // ambient
    vec3 ambient = directionalLights[index].ambient * diffuseColor;

    // diffuse
    float diffuseScalar = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = directionalLights[index].diffuse * diffuseScalar * diffuseColor;

    // specular
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float specularScalar = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    vec3 specular = directionalLights[index].specular * specularScalar * specularColor;

    return ambient + diffuse + specular;
}

vec3 processPointLight(int index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor)
{
    PointLight light = pointLights[index];


    vec3 lightDirection = normalize(light.position - fragmentPosition);

    // point light attenuation factor
    float distance = length(light.position - fragmentPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                                light.quadratic * (distance * distance));

    // ambient
    vec3 ambient = light.ambient * diffuseColor * attenuation;

    // diffuse
    float diffuseScalar = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = light.diffuse * diffuseScalar * diffuseColor * attenuation;

    // specular
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float specularValue = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    vec3 specular = light.specular * specularValue * specularColor * attenuation;

    return ambient + diffuse + specular;
}

vec3 processSpotlight(int index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor)
{

    Spotlight light = spotlights[index];

    vec3 lightDirection = normalize(light.position - fragmentPosition);
    float theta = dot(lightDirection, normalize(-light.direction));
    float outterDelta = max(light.cutOff - light.outterCutOff, 0.0001);
    float intensity = clamp((theta - light.outterCutOff) / outterDelta, 0.0, 1.0);

    // ambient
    vec3 ambient = light.ambient * diffuseColor;

    if (theta > light.outterCutOff)
    {
        // diffuse
        float diffuseScalar = max(dot(normal, lightDirection), 0.0);
        vec3 diffuse = light.diffuse * diffuseScalar * diffuseColor * intensity;

        // specular
        vec3 reflectDirection = reflect(-lightDirection, normal);
        float specularValue = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
        vec3 specular = light.specular * specularValue * specularColor * intensity;

        return ambient + diffuse + specular;
    }
    else
    {
        return ambient;
    }
}
#type vertex 
#version 100

precision mediump float;

attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec2 aTextureCoordinate;

uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 view;
uniform mat4 projection;

varying vec3 fragmentPosition;
varying vec3 normal;
varying vec2 textureCoordinate;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
    fragmentPosition = vec3(model * vec4(aPosition, 1.0));
    normal = normalMatrix * aNormal;
    textureCoordinate = aTextureCoordinate;
}

#type fragment
#version 100

precision mediump float;

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

varying vec3 fragmentPosition;
varying vec3 normal;
varying vec2 textureCoordinate;

uniform float directionalLightsCount;
uniform float pointLightsCount;
uniform float spotlightsCount;

uniform Material material;
uniform DirectionLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform Spotlight spotlights[MAX_SPOTLIGHTS];

uniform vec3 viewPosition;

vec3 processDirectionLight(float index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor);
vec3 processPointLight(float index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor);
vec3 processSpotlight(float index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor);
float mininmum(float a, float b);

void main()
{
    vec3 normalizedNormal = normalize(normal);
    vec3 diffuseColor = texture2D(material.diffuseMap, textureCoordinate).rgb;
    vec3 specularColor = texture2D(material.specularMap, textureCoordinate).rgb;
    vec3 viewDirection = normalize(viewPosition - fragmentPosition);

    vec3 outputColor = vec3(0.0);

    // directional lights
    const float maxDirectionalLightsCount = mininmum(directionalLightsCount, MAX_DIRECTIONAL_LIGHTS);
    for (float i = 0.; i < maxDirectionalLightsCount; i++)
    {
        outputColor += processDirectionLight(i, normalizedNormal, viewDirection, diffuseColor, specularColor);
    }

    // point lights
    const float maxPointLightsCount = mininmum(pointLightsCount, MAX_POINT_LIGHTS);
    for (float i = 0.; i < maxPointLightsCount; i++)
    {
        outputColor += processPointLight(i, normalizedNormal, viewDirection, diffuseColor, specularColor);
    }

    // spotlights
    const float maxSpotlightsCount = mininmum(spotlightsCount, MAX_POINT_LIGHTS);
    for (float i = 0.; i < maxSpotlightsCount; i++)
    {
        outputColor += processSpotlight(i, normalizedNormal, viewDirection, diffuseColor, specularColor);
    }
    
    gl_FragColor = vec4(outputColor, 0.5);
}

vec3 processDirectionLight(float index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor)
{
    vec3 lightDirection = normalize(-directionalLights[int(index)].direction);

    // ambient
    vec3 ambient = directionalLights[int(index)].ambient * diffuseColor;

    // diffuse
    float diffuseScalar = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = directionalLights[int(index)].diffuse * diffuseScalar * diffuseColor;

    // specular
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float specularScalar = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    vec3 specular = directionalLights[int(index)].specular * specularScalar * specularColor;

    return ambient + diffuse + specular;
}

vec3 processPointLight(float index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor)
{
    PointLight light = pointLights[int(index)];


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

vec3 processSpotlight(float index, vec3 normal, vec3 viewDirection, vec3 diffuseColor, vec3 specularColor)
{

    Spotlight light = spotlights[int(index)];

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

float mininmum(float a, float b)
{
    return a < b ? a : b;
}
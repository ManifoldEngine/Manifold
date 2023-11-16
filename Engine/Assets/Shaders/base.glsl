#type vertex 
#version 330 core

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
#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 fragmentPosition;
in vec3 normal;
in vec2 textureCoordinate;

uniform vec3 color;

uniform Material material;
uniform Light light;

uniform vec3 viewPosition;

out vec4 FragColor;

void main()
{
    vec3 normalizedNormal = normalize(normal);
    vec3 lightDirection = normalize(light.position - fragmentPosition);

    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse
    float diffuseScalar = max(dot(normalizedNormal, lightDirection), 0.0);
    vec3 diffuse = light.diffuse * (diffuseScalar * material.diffuse);

    // specular
    vec3 viewDirection = normalize(viewPosition - fragmentPosition);
    vec3 reflectDirection = reflect(-lightDirection, normalizedNormal);
    float specularValue = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    vec3 specular = light.specular * (specularValue * material.specular);

    vec3 resultColor = (ambient + diffuse + specular) * color;
    FragColor = vec4(resultColor, 1.0);
}

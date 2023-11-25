#type vertex 
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextureCoordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragmentPosition;
out vec3 normal;
out vec2 textureCoordinate;

void main()
{
    gl_Position = projection* view * model * vec4(aPosition, 1.0);
    fragmentPosition = vec3(model * vec4(aPosition, 1.0));
    normal = aNormal;
    textureCoordinate = aTextureCoordinate;
}

#type fragment
#version 330 core

in vec3 fragmentPosition;
in vec3 normal;
in vec2 textureCoordinate;

uniform vec4 color;

out vec4 FragColor;

void main()
{
    FragColor = color;
}

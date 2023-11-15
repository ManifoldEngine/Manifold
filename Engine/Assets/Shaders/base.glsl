#type vertex 
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTextureCoordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 textureCoordinate;

void main()
{
    gl_Position = projection* view * model * vec4(aPosition, 1.0);
    textureCoordinate = aTextureCoordinate;
}

#type fragment
#version 330 core

in vec2 textureCoordinate;

uniform vec3 color;
uniform vec3 lightColor;
uniform sampler2D inputTexture1;

out vec4 FragColor;

void main()
{
    FragColor = mix(
        texture(inputTexture1, textureCoordinate),
        vec4(color * lightColor, 1.f),
        1.f
    );
}

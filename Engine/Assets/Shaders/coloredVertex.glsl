#type vertex 
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTextureCoordinate;

out vec3 color;
out vec2 textureCoordinate;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    color = aColor;
    textureCoordinate = aTextureCoordinate;
}

#type fragment
#version 330 core

in vec3 color;
in vec2 textureCoordinate;

out vec4 FragColor;

uniform sampler2D inputTexture1;
uniform sampler2D inputTexture2;

void main()
{
    FragColor = mix(
        texture(inputTexture1, textureCoordinate),
        texture(inputTexture2, textureCoordinate),
        0.2f
    );
}

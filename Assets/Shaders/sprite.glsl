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
    textureCoordinate = aTextureCoordinate.xy;
    gl_Position = projection * view * model * vec4(aPosition.xyz, 1.0);
}

#type fragment
#version 330 core

in vec2 textureCoordinate;

uniform sampler2D sprite;
uniform vec4 color;

out vec4 FragColor;

void main()
{
    vec4 textureColor = texture(sprite, textureCoordinate);
    if (textureColor.w < .01)
    {
        discard;
    }

    FragColor = color * textureColor;
}

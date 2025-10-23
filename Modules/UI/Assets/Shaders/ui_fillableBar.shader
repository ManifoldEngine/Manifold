{
	"name": "ui_fillableBar",
	"vertexSource": "#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextureCoordinate;

uniform mat4 model;
uniform mat4 projection;

out vec2 textureCoordinate;

void main()
{
    gl_Position = projection * model * vec4(aPosition, 1.0);
    textureCoordinate = aTextureCoordinate;
}

",
	"fragmentSource": "#version 330 core

in vec2 textureCoordinate;

uniform sampler2D mani_texture_0;
uniform vec4 color;
uniform float fillRatio;

out vec4 fragColor;

void main()
{
    vec4 finalColor;
    float current = textureCoordinate.x / 1.0;
    if (current > fillRatio)
    {
        finalColor = color;
    } 
    else
    {
        finalColor = texture(mani_texture_0, textureCoordinate);
    }

    if (finalColor.w < .001)
    {
        discard;
    }

    fragColor = finalColor;
}",
}
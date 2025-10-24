{
	"name": "ui_fillableBar",
	"vertexSource": "#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextureCoordinate;

uniform mat4 mani_model;
uniform mat4 mani_projection;

out vec2 textureCoordinate;

void main()
{
    gl_Position = mani_projection * mani_model * vec4(aPosition, 1.0);
    textureCoordinate = aTextureCoordinate;
}

",
	"fragmentSource": "#version 330 core

in vec2 textureCoordinate;

uniform sampler2D mani_texture_0;
uniform vec4 mani_color;
uniform float mani_ui_fillRatio;

out vec4 fragColor;

void main()
{
    vec4 finalColor;
    float current = textureCoordinate.x / 1.0;
    if (current > mani_ui_fillRatio)
    {
        finalColor = mani_color;
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
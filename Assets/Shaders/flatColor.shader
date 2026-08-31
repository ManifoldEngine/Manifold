{
	"name": "flatColor",
	"vertexSource": "#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextureCoordinate;

uniform mat4 mani_model;
uniform mat4 mani_view;
uniform mat4 mani_projection;

out vec3 fragmentPosition;
out vec3 normal;
out vec2 textureCoordinate;

void main()
{
    gl_Position = mani_projection * mani_view * mani_model * vec4(aPosition, 1.0);
    fragmentPosition = vec3(mani_model * vec4(aPosition, 1.0));
    normal = aNormal;
    textureCoordinate = aTextureCoordinate;
}

",
	"fragmentSource": "#version 330 core

in vec3 fragmentPosition;
in vec3 normal;
in vec2 textureCoordinate;

uniform vec4 mani_color;

out vec4 fragColor;

void main()
{
    fragColor = mani_color;
}
",
}
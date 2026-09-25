#type vertex 
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextureCoordinate;

uniform mat4 mani_model;
uniform mat4 mani_view;
uniform mat4 mani_projection;

out vec3 fragmentPosition;
out vec2 textureCoordinate;

void main()
{
    gl_Position = mani_projection * mani_view * mani_model * vec4(aPosition, 1.0);
    //gl_Position = vec4(aPosition, 1.0);
    //fragmentPosition = vec3(mani_model * vec4(aPosition, 1.0));
    textureCoordinate = aTextureCoordinate;
}

#type fragment
#version 330 core

in vec3 fragmentPosition;
in vec2 textureCoordinate;

uniform sampler2D mani_texture_0;
uniform vec4 mani_color = vec4(1.0, 1.0, 1.0, 1.0);

out vec4 fragColor;

void main()
{
    // vec4 textureColor = texture(mani_texture_0, textureCoordinate);
    // if (textureColor.w < .001)
    // {
    //     discard;
    // }

    // fragColor = textureColor;
    float alpha = texture(mani_texture_0, vec2(textureCoordinate.x, 1.0 - textureCoordinate.y)).r;
    fragColor = vec4(1.0, 1.0, 1.0, alpha);
}

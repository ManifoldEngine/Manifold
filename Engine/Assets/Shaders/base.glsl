#type vertex 
#version 330 core

layout(location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

#type fragment
#version 330 core

uniform vec4 color;

out vec4 FragColor;

void main()
{
    FragColor = color;
}

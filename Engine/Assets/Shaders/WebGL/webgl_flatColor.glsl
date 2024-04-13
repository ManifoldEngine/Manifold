#type vertex 
#version 100

precision mediump float;

attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec2 aTextureCoordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 fragmentPosition;
varying vec3 normal;
varying vec2 textureCoordinate;

void main()
{
    gl_Position = projection* view * model * vec4(aPosition, 1.0);
    fragmentPosition = vec3(model * vec4(aPosition, 1.0));
    normal = aNormal;
    textureCoordinate = aTextureCoordinate;
}

#type fragment
#version 100

precision mediump float;

varying vec3 fragmentPosition;
varying vec3 normal;
varying vec2 textureCoordinate;

uniform vec4 color;

void main()
{
    gl_FragColor = color;
}

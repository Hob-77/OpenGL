#version 330 core

layout (location = 0) in vec3 aPos;    // position attribute
layout (location = 1) in vec3 aColor;  // color attribute
layout (location = 2) in vec2 aTexCoord; // texcoord attribute

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Apply transformations to vertex position
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
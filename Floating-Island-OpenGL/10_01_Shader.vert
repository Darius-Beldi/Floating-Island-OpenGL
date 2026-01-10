#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_color;

out vec3 fragColor;

uniform mat4 myMatrix;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * myMatrix * vec4(in_position, 1.0);
    fragColor = in_color;
}
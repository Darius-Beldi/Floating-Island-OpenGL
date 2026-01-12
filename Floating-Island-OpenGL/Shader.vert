#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_color;

out vec3 fragPos;
out vec3 fragNormal;
out vec3 fragColor;

uniform mat4 myMatrix;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Transform position to world space
    vec4 worldPos = myMatrix * vec4(in_position, 1.0);
    fragPos = worldPos.xyz;
    
    // Transform normal to world space (using normal matrix)
    fragNormal = mat3(transpose(inverse(myMatrix))) * in_normal;
    
    // Pass color to fragment shader
    fragColor = in_color;
    
    // Final position
    gl_Position = projection * view * worldPos;
}
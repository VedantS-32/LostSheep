#shader vertex
#version 330 core
layout (location = 0) in vec2 aPosition;

uniform vec3 uQuadPos;
uniform vec2 uQuadSize;
uniform mat4 uViewProjection;

void main()
{
    vec2 worldPosition = uQuadPos.xy + (aPosition * uQuadSize);
    gl_Position = uViewProjection * vec4(worldPosition, uQuadPos.z, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;
uniform vec4 uColor;

void main()
{
    FragColor = uColor;
}
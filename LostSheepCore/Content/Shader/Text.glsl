#shader vertex
#version 330 core
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

uniform float uQuadPosZ;
uniform mat4 uViewProjection;

void main()
{
    vTexCoord = aTexCoord;

    gl_Position = uViewProjection * vec4(aPosition.xy, uQuadPosZ, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 vTexCoord;

uniform sampler2D uTexture;
uniform vec4 uTextColor;

void main()
{
    vec4 diffuse = vec4(1.0f, 1.0f, 1.0f, texture(uTexture, vTexCoord).r);

    FragColor = uTextColor * diffuse;
    //FragColor = vec4(vTexCoord, 0.0f, 1.0f);
}
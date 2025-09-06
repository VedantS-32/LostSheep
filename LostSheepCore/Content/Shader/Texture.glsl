#shader vertex
#version 330 core
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;
out vec2 vQuadSize;

uniform vec3 uQuadPos;
uniform vec2 uQuadSize;
uniform mat4 uViewProjection;

void main()
{
    vTexCoord = aTexCoord;
    vQuadSize = uQuadSize;

    vec2 worldPosition = uQuadPos.xy + (aPosition * uQuadSize);
    gl_Position = uViewProjection * vec4(worldPosition, uQuadPos.z, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 vTexCoord;
in vec2 vQuadSize;

uniform sampler2D uTexture;
uniform float uCornerRadius;

float sdRoundedRect(vec2 p, vec2 size, float radius) {
    vec2 d = abs(p) - size + radius;
    return length(max(d, 0.0f)) + min(max(d.x, d.y), 0.0f) - radius;
}

void main()
{
    // Convert texture coordinates to pixel coordinates
    vec2 pixelCoord = vTexCoord * vQuadSize;

    // Center the coordinate system
    vec2 center = vQuadSize * 0.5;
    vec2 p = pixelCoord - center;
    
    // Rectangle half-size
    vec2 rectSize = vQuadSize * 0.5;

    // Anti-aliasing factor (smooth edge)
    float smoothFactor = 1.0;

    // Calculate distance to rounded rectangle
    float dist = sdRoundedRect(p, rectSize, uCornerRadius);

    vec4 diffuse = texture(uTexture, vTexCoord);

    float alpha = 1.0 - smoothstep(-smoothFactor, smoothFactor, dist);
    FragColor = mix(vec4(0.0f, 0.0f, 0.0f, 0.0f), diffuse, alpha);
    //FragColor = vec4(vTexCoord, 0.0f, 1.0f);
}
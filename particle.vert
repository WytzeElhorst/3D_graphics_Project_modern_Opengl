#version 430 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
layout (location = 3) uniform mat4 mvp;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform vec4 offset;
uniform vec4 color;

void main()
{
    float scale = 0.01f;
    TexCoords = vertex.zw;
    ParticleColor = color;
    gl_Position = mvp * (vec4((vertex.xy * scale), 0.58, 1.0) + offset);
	//gl_Position = mpv * vec4(offset * scale);
}
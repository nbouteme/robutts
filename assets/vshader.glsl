#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

uniform mat4 model;
uniform mat4 proj;

out vec2 TexCoords;

void main()
{
    TexCoords = vertex.zw;
    gl_Position = proj * model * vec4(vertex.xy, 0.0, 1.0);
}

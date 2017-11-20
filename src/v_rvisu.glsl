#version 330 core
layout (location = 0) in vec2 vertex;

uniform float depth[64];
uniform int colors[64];
uniform mat4 proj;

out vec3 col;

vec3 itemcolors[4] = vec3[4](
	vec3(0, 1, 0),
	vec3(1, 1, 0),
	vec3(0, 1, 1),
	vec3(1, 1, 1));

const vec2 c = vec2(256, 256);

void main()
{
    col = itemcolors[colors[gl_VertexID / 2]];
	vec2 dir = vec2(0, 0);
	if (gl_VertexID % 2 == 1)
		dir = normalize(vertex - c) * depth[gl_VertexID / 2];
    gl_Position = proj * vec4(vertex + dir, 0.0, 1.0);
}

#version 130
in vec2 TexCoords;

uniform sampler2D tex;
uniform vec3 color;

out vec4 coloro;

void main()
{
	coloro = vec4(color, 1.0) * texture(tex, TexCoords);
}

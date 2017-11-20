#version 130

in vec3 col;

out vec4 coloro;

void main()
{    
    coloro = vec4(col, 1.0);
}

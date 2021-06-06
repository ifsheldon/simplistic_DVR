#version 450 core
in vec4 v2fColor;

layout (location = 0) out vec4 fColor;

void main()
{
    fColor = v2fColor;
}

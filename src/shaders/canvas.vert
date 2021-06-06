#version 450 core
// Vertex attributes.
layout (location = 0) in vec3 vPosition;
layout (location = 2) in vec2 vTex;

// View and projection matrices.
uniform mat4 ModelViewProjMatrix;

// To be sent to rasterizer and fragment shader.
out vec2 texCoord;

void main()
{
    gl_Position = ModelViewProjMatrix * vec4(vPosition, 1.0);
    texCoord = vTex;
}
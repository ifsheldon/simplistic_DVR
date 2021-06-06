#version 450 core
// Vertex attributes.
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vAttrib;

// View and projection matrices.
uniform mat4 ModelViewProjMatrix;

// To be sent to rasterizer and fragment shader.
out vec4 v2fColor;

void main()
{
    gl_Position = ModelViewProjMatrix * vec4(vPosition, 1.0);

    v2fColor = vec4(vAttrib, 1.0);
}
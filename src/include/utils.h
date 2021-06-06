//
// Created by liangf on 3/15/21.
//

#ifndef ASSIGNMENT2_UTILS_H
#define ASSIGNMENT2_UTILS_H
#include "glm/glm.hpp"
// query GPU functionality we need for OpenGL, return false when not available
bool queryGPUCapabilitiesOpenGL() {
    // =============================================================================
    // query and print (to console) OpenGL version and extensions:
    // - query and print GL vendor, renderer, and version using glGetString()
    //
    // query and print GPU OpenGL limits (using glGet(), glGetInteger() etc.):
    // - maximum number of vertex shader attributes
    // - maximum number of varying floats
    // - number of texture image units (in vertex shader and in fragment shader, respectively)
    // - maximum 2D texture size
    // - maximum 3D texture size
    // - maximum number of draw buffers
    // =============================================================================
    using namespace std;
    auto vendor = glGetString(GL_VENDOR);
    auto renderer = glGetString(GL_RENDERER);
    auto version = glGetString(GL_VERSION);
    if (vendor == 0 || renderer == 0 || version == 0)
        return false;
    cout << "Vendor: " << vendor << endl;
    cout << "Renderer: " << renderer << endl;
    cout << "Version: " << version << endl;
    cout << "------------------------" << endl;
    GLint maxVShaderAttributes = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVShaderAttributes);
    GLint maxVaryingFloats = 0;
    glGetIntegerv(GL_MAX_VARYING_FLOATS, &maxVaryingFloats);
    GLint maxTextureImageUnitsInVShader = 0;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxTextureImageUnitsInVShader);
    GLint maxTextureImageUnitsInFShader = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureImageUnitsInFShader);
    GLint max2DTextureSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max2DTextureSize);
    GLint max3DTexutreSize = 0;
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max3DTexutreSize);
    GLint maxDrawBufferNum = 0;
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBufferNum);
    if (maxVShaderAttributes == 0 ||
        maxVaryingFloats == 0 ||
        maxTextureImageUnitsInFShader == 0 ||
        maxTextureImageUnitsInVShader == 0 ||
        max2DTextureSize == 0 ||
        max3DTexutreSize == 0 ||
        maxDrawBufferNum == 0)
        return false;
    cout << "maximum number of vertex shader attributes: " << maxVShaderAttributes << endl;
    cout << "maximum number of varying floats: " << maxVaryingFloats << endl;
    cout << "number of texture image units in vertex shader: " << maxTextureImageUnitsInVShader << endl;
    cout << "number of texture image units in vertex shader: " << maxTextureImageUnitsInFShader << endl;
    cout << "maximum 2D texture size: " << max2DTextureSize << endl;
    cout << "maximum 3D texture size: " << max3DTexutreSize << endl;
    cout << "maximum number of draw buffers: " << maxDrawBufferNum << endl;
    cout << "----------------------------" << endl;
    return true;
}

// glfw error callback
void glfwErrorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

unsigned short* loadData(const char* filename, glm::u16vec3 &vol_dim) {
    fprintf(stderr, "loading data %s\n", filename);

    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Cannot open file %s for reading.\n", filename);
        return nullptr;
    }
    //read volume dimension
    fread(&vol_dim.x, sizeof(unsigned short), 1, fp);
    fread(&vol_dim.y, sizeof(unsigned short), 1, fp);
    fread(&vol_dim.z, sizeof(unsigned short), 1, fp);

    fprintf(stderr, "volume dimensions: x: %i, y: %i, z:%i \n", vol_dim.x, vol_dim.y, vol_dim.z);

    auto data = new unsigned short[vol_dim.x * vol_dim.y * vol_dim.z]; //for intensity volume
    fread(data, sizeof(unsigned short) * vol_dim.x * vol_dim.y * vol_dim.z, 1, fp);
    fclose(fp);
    for (int i = 0; i < vol_dim.x * vol_dim.y * vol_dim.z; i++) {
        data[i] <<= 4;
    }
    return data;
}

#endif //ASSIGNMENT2_UTILS_H

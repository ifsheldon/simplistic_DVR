#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glslprogram.h"
#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include "utils.h"
#include "consts.h"
#include "glm/gtc/matrix_transform.hpp"
#include "vbocube.h"
#include "vborectangle.h"
#include "tf_tables.h"
#include <random>
#include "CImg.h"

//#define TESTING

using namespace std;
using namespace glm;
using namespace cimg_library;

const char* VERTEX_SHADER_3D_PATH = "../src/shaders/shader3d.vert";
const char* FRAGMENT_SHADER_3D_PATH = "../src/shaders/shader3d.frag";
const char* VERTEX_CANVAS_SHADER_PATH = "../src/shaders/canvas.vert";
const char* FRAGMENT_CANVAS_DVR_SHADER_PATH = "../src/shaders/canvas_dvr.frag";
const char* FRAGMENT_CANVAS_ISO_SHADER_PATH = "../src/shaders/canvas_iso.frag";

// skewed_head x: 184, y: 256, z:170
const char* HEAD_DATA_PATH = "../data/skewed_head.dat";

struct FrameBuffer {
    GLuint handle;
    GLuint bindFrontFaceTexHandle;
    GLuint bindBackFaceTexHandle;
    GLuint bindDepthTexHandle;
};

const unsigned int FRAME_BUFFER_WIDTH = 1024;
const unsigned int FRAME_BUFFER_HEIGHT = 1024;

FrameBuffer* faceFrameBuffer = nullptr;
// rendering parameters
auto cam_wc = vec3(0.0, -2.5, 0.0);
float zoomRate = 1.0f;
auto cam_up_wc = vec3(0.0, 0.0, 1.0);
auto cam_center_wc = vec3(0.0, 0.0, 0.0);
auto objScaling = vec3(1.0);
vec3 rotateAngles = vec3(0.0f, 0.0f, 0.0f);
vec3 rotateAngles_deg = vec3(0.0f, 0.0f, 0.0f);

GLSLProgram* program3d;
GLSLProgram* dvrProgram;
GLSLProgram* isoProgram;

VBOCube* cube = nullptr;
VBORectangle* rectangle = nullptr;

bool enableShading = true;
float stepSize = 0.0025;
GLuint tex3D;
GLuint tf_texture;

unsigned short* data_array;
u16vec3 vol_dim;

int windowWidth;
int windowHeight;

inline void initPrograms() {
    program3d = new GLSLProgram();
    program3d->compileShader(VERTEX_SHADER_3D_PATH, GLSLShader::VERTEX);
    program3d->compileShader(FRAGMENT_SHADER_3D_PATH, GLSLShader::FRAGMENT);
    program3d->link();
    program3d->validate();

    dvrProgram = new GLSLProgram();
    dvrProgram->compileShader(VERTEX_CANVAS_SHADER_PATH, GLSLShader::VERTEX);
    dvrProgram->compileShader(FRAGMENT_CANVAS_DVR_SHADER_PATH, GLSLShader::FRAGMENT);
    dvrProgram->link();
    dvrProgram->validate();

    isoProgram = new GLSLProgram();
    isoProgram->compileShader(VERTEX_CANVAS_SHADER_PATH, GLSLShader::VERTEX);
    isoProgram->compileShader(FRAGMENT_CANVAS_ISO_SHADER_PATH, GLSLShader::FRAGMENT);
    isoProgram->link();
    isoProgram->validate();
}

bool initApplication() {
    std::string version((const char*) glGetString(GL_VERSION));
    std::stringstream stream(version);
    unsigned major, minor;
    char dot;

    stream >> major >> dot >> minor;

    assert(dot == '.');
    if (major > 3 || (major == 2 && minor >= 0)) {
        std::cout << "OpenGL Version " << major << "." << minor << std::endl;
    } else {
        std::cout << "The minimum required OpenGL version is not supported on this machine. Supported is only " << major
                  << "." << minor << std::endl;
        return false;
    }

    // default initialization
    glClearColor(BLACK.r, BLACK.g, BLACK.b, 1.0);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    return true;
}

void downloadVolumeAsTexture() {
    fprintf(stderr, "downloading volume to 3D texture\n");
    glGenTextures(1, &tex3D);
    glBindTexture(GL_TEXTURE_3D, tex3D);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, vol_dim.x, vol_dim.y, vol_dim.z, 0, GL_RED, GL_UNSIGNED_SHORT,
                 data_array);
    glGenerateMipmap(GL_TEXTURE_3D);
}

inline void initTF0() {
    for (int i = 0; i < 128; i++) {
        tf0[4 * i + 0] = (float) (i) / 128.0f;
        tf0[4 * i + 1] = (float) (i) / 128.0f;
        tf0[4 * i + 2] = (float) (i) / 128.0f;
        tf0[4 * i + 3] = (float) (i) / 128.0f;
    }
}

/*
 * download tf data to 1D texture
 */
void downloadTransferFunctionTexture(int tf_id) {
    fprintf(stderr, "downloading transfer function to 1D texture\n");

    glEnable(GL_TEXTURE_1D);

    // generate and bind 1D texture
    glGenTextures(1, &tf_texture);
    glBindTexture(GL_TEXTURE_1D, tf_texture);

    // set necessary texture parameters
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

    //download texture in correct format
    switch (tf_id) {
        case 0:
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 128, 0, GL_RGBA, GL_FLOAT, tf0);
            break;
        case 1:
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 2, 0, GL_RGBA, GL_FLOAT, tf1);
            break;
        case 2:
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 4, 0, GL_RGBA, GL_FLOAT, tf2);
            break;
        case 3:
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 12, 0, GL_RGBA, GL_FLOAT, tf3);
            break;
        case 4:
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 12, 0, GL_RGBA, GL_FLOAT, tf4);
            break;
        case 5:
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 12, 0, GL_RGBA, GL_FLOAT, tf5);
            break;
        default:
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 128, 0, GL_RGBA, GL_FLOAT, tf0);
            break;
    }
    glDisable(GL_TEXTURE_1D);
}

#ifdef TESTING

int main() {
    auto rotate_angles = vec3(129.8719, 65.3027, 300.3911);
    rotate_angles = radians(rotate_angles);
    auto rotate_mat = mat4(1.0);
    rotate_mat = rotate(rotate_mat, rotate_angles.x, vec3(1.0, 0.0, 0.0));
    rotate_mat = rotate(rotate_mat, rotate_angles.y, vec3(0.0, 1.0, 0.0));
    rotate_mat = rotate(rotate_mat, rotate_angles.z, vec3(0.0, 0.0, 1.0));
    auto v = rotate_mat * vec4(1.1, 1.5, 1.3, 1.0);
    printf("%.4f, %.4f, %.4f, %.4f", v.x, v.y, v.z, v.w);
}

#else

void setUpFBO() {
    faceFrameBuffer = new FrameBuffer();
    //Generate and bind frontFace FBO and texture
    glGenFramebuffers(1, &faceFrameBuffer->handle);
    glBindFramebuffer(GL_FRAMEBUFFER, faceFrameBuffer->handle);
    //Generate and bind color textures
    glGenTextures(1, &faceFrameBuffer->bindFrontFaceTexHandle);
    glBindTexture(GL_TEXTURE_2D, faceFrameBuffer->bindFrontFaceTexHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, faceFrameBuffer->bindFrontFaceTexHandle,
                           0);
    glGenTextures(1, &faceFrameBuffer->bindBackFaceTexHandle);
    glBindTexture(GL_TEXTURE_2D, faceFrameBuffer->bindBackFaceTexHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                           faceFrameBuffer->bindBackFaceTexHandle,
                           0);
    //Generate and bind depth texture
    glGenTextures(1, &faceFrameBuffer->bindDepthTexHandle);
    glBindTexture(GL_TEXTURE_2D, faceFrameBuffer->bindDepthTexHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           faceFrameBuffer->bindDepthTexHandle,
                           0);
    printf("Completed frame buffer %d\n", glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void rand_rotation() {
    static std::default_random_engine generator(time(nullptr));
    static std::uniform_real_distribution<float> uniform_distribution;
    auto rotate_angles = vec3(uniform_distribution(generator) * 360.f,
                              uniform_distribution(generator) * 360.f,
                              uniform_distribution(generator) * 360.f);
    rotateAngles = radians(rotate_angles);
    rotateAngles_deg = rotate_angles;
}

void render3D(bool frontFaceRendering) {
    if (frontFaceRendering) {
        glDepthFunc(GL_LESS);
        glClearDepth(1.0);
    } else {
        glDepthFunc(GL_GREATER);
        glClearDepth(0.0);
    }
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program3d->use();
    vec3 cam_pos_wc = cam_wc * zoomRate;
    auto projectionMat = perspective(radians(45.0f), 1.0f, 0.1f, 100.0f);

    auto modelMat = mat4(1.0f);
    modelMat = rotate(modelMat, rotateAngles.x, vec3(1.0, 0.0, 0.0));
    modelMat = rotate(modelMat, rotateAngles.y, vec3(0.0, 1.0, 0.0));
    modelMat = rotate(modelMat, rotateAngles.z, vec3(0.0, 0.0, 1.0));
    auto scale_mat = scale(mat4(1.0), objScaling);
    auto inverse_rotate_mat = inverse(mat3(modelMat));
    auto viewMat = lookAt(inverse_rotate_mat * cam_pos_wc, cam_center_wc, cam_up_wc);

    auto modelViewProjectionMat = projectionMat * viewMat * scale_mat;

    program3d->setUniform("ModelViewProjMatrix", modelViewProjectionMat);
    cube->render();
}

void renderCanvas() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    dvrProgram->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, faceFrameBuffer->bindFrontFaceTexHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, faceFrameBuffer->bindBackFaceTexHandle);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, tex3D);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_1D, tf_texture);
    auto projectionMat = ortho(-1.0, 1.0, -1.0, 1.0);

    dvrProgram->setUniform("ModelViewProjMatrix", projectionMat);
    dvrProgram->setUniform("enableShading", enableShading);
    dvrProgram->setUniform("StepSize", stepSize);

    rectangle->render();
}

void triplePass() {
    glBindFramebuffer(GL_FRAMEBUFFER, faceFrameBuffer->handle);
    rand_rotation();
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    render3D(true);
    glDrawBuffer(GL_COLOR_ATTACHMENT1);
    glViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    render3D(false);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, windowWidth, windowHeight);
    glDisable(GL_DEPTH_TEST);
    renderCanvas();
}

inline bool string_to_int(string &string, int* output) {
    stringstream ss(string);
    int i = 0;
    ss >> i;
    if (!ss) {
        return false;
    }
    *output = i;
    return true;
}

int main(int argc, char** argv) {
    // set glfw error callback
    glfwSetErrorCallback(glfwErrorCallback);

    // init glfw
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    if (argc < 2) {
        cerr << "Needs input" << endl;
        return -1;
    }
    string image_num_string(argv[1]);
    int image_num;
    if (!string_to_int(image_num_string, &image_num)) {
        cerr << "Invalid input" << endl;
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    windowWidth = 1024;
    windowHeight = 1024;
    GLFWwindow* window;
    window = glfwCreateWindow(windowWidth, windowHeight, "simplistic DVR", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // make context current (once is sufficient)
    glfwMakeContextCurrent(window);

    // init the OpenGL API (we need to do this once before any calls to the OpenGL API)
    gladLoadGL();

    // query OpenGL capabilities
    if (!queryGPUCapabilitiesOpenGL()) {
        // quit in case capabilities are insufficient
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // init our application
    if (!initApplication()) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    initPrograms();
    setUpFBO();
    data_array = loadData(HEAD_DATA_PATH, vol_dim);
    downloadVolumeAsTexture();
    float midVal;
    if ((vol_dim.x < vol_dim.z && vol_dim.x > vol_dim.y) || (vol_dim.x > vol_dim.z && vol_dim.x < vol_dim.y)) {
        midVal = vol_dim.x;
    } else if ((vol_dim.y < vol_dim.z && vol_dim.y > vol_dim.x) ||
               (vol_dim.y > vol_dim.z && vol_dim.y < vol_dim.x)) {
        midVal = vol_dim.y;
    } else
        midVal = vol_dim.z;

    objScaling = vec3(vol_dim.x / midVal, vol_dim.y / midVal, vol_dim.z / midVal);

    initTF0();
    downloadTransferFunctionTexture(5);

    cube = new VBOCube();
    rectangle = new VBORectangle();

    glfwSetWindowAspectRatio(window, windowWidth, windowHeight);
    // viewport
    glViewport(0, 0, windowWidth, windowHeight);
    unsigned char* pixelBuffer = new unsigned char[windowWidth * windowHeight * 3];
    unsigned char* tempBuff = new unsigned char[windowWidth * windowHeight * 3];
    CImg<unsigned char> image(windowWidth, windowHeight, 1, 3, 0);
    CImgDisplay display(image, "show");
    char name_buf[100];
    // start traversing the main loop
    // loop until the user closes the window
    for (int i = 0; i < image_num; i++) {
        triplePass();
        glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, pixelBuffer);
        for (int i = 0, ci = 0, g_base = windowWidth * windowHeight, b_base = windowWidth * windowHeight * 2;
             i < windowWidth * windowHeight; i++, ci += 3) {
            tempBuff[i] = pixelBuffer[ci];
            tempBuff[i + g_base] = pixelBuffer[ci + 1];
            tempBuff[i + b_base] = pixelBuffer[ci + 2];
        }
        image.assign(tempBuff, windowWidth, windowHeight, 1, 3);
        image.mirror("y");
        image.resize(512, 512, -100, -100, 3);
        display.display(image);
        snprintf(name_buf, sizeof(name_buf),
                 "head_%.4f_%.4f_%.4f.png",
                 rotateAngles_deg.x, rotateAngles_deg.y, rotateAngles_deg.z);
        image.save(name_buf);
        // poll and process input events (keyboard, mouse, window, ...)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    delete program3d;
    delete dvrProgram;
    delete isoProgram;
    delete cube;
    delete rectangle;
    delete faceFrameBuffer;
    delete[] data_array;
    delete[] pixelBuffer;
    delete[] tempBuff;
    glfwTerminate();
    return 0;
}

#endif

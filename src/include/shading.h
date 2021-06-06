//
// Created by liangf on 3/16/21.
//

#ifndef ASSIGNMENT2_SHADING_H
#define ASSIGNMENT2_SHADING_H

#include "glm/glm.hpp"

using namespace glm;

struct Light {
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

#endif //ASSIGNMENT2_SHADING_H

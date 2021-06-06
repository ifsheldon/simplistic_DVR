//
// Created by liangf on 3/27/21.
//

#ifndef ASSIGNMENT4_VBOCUBE_H
#define ASSIGNMENT4_VBOCUBE_H

#include "glad/glad.h"

class VBOCube {

private:
    unsigned int vaoHandle;

public:
    VBOCube() {
        const float side = 1.0f;
        const float side2 = side / 2.0f;
        const float vertexPositions[8 * 3] = {
                // 4 vertices on z = 0.5
                -side2, -side2, side2,
                side2, -side2, side2,
                side2, side2, side2,
                -side2, side2, side2,
                // 4 vertices on z = -0.5
                -side2, -side2, -side2,
                side2, -side2, -side2,
                side2, side2, -side2,
                -side2, side2, -side2,
        };
        const float attributesOnVertices[8 * 3] = {
                // attributes of 4 vertices on z = 0.5
                0, 0, side,
                side, 0, side,
                side, side, side,
                0, side, side,
                // attributes of 4 vertices on z = 0.5
                0, 0, 0,
                side, 0, 0,
                side, side, 0,
                0, side, 0,
        };
        const GLuint el[] = {
                0, 1, 3, 3, 1, 2,
                2, 1, 5, 2, 5, 6,
                3, 2, 7, 7, 2, 6,
                4, 0, 3, 4, 3, 7,
                4, 1, 0, 4, 5, 1,
                7, 6, 5, 7, 5, 4
        };
        glGenVertexArrays(1, &vaoHandle);
        glBindVertexArray(vaoHandle);
        unsigned int handles[3];
        glGenBuffers(3, handles);

        glBindBuffer(GL_ARRAY_BUFFER, handles[0]);
        glBufferData(GL_ARRAY_BUFFER, 8 * 3 * sizeof(float), vertexPositions, GL_STATIC_DRAW);
        glVertexAttribPointer((GLuint) 0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*) NULL + (0)));
        glEnableVertexAttribArray(0); // vertex positions

        glBindBuffer(GL_ARRAY_BUFFER, handles[1]);
        glBufferData(GL_ARRAY_BUFFER, 8 * 3 * sizeof(float), attributesOnVertices, GL_STATIC_DRAW);
        glVertexAttribPointer((GLuint) 1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*) NULL + (0)));
        glEnableVertexAttribArray(1);  // vertex attributes

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handles[2]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), el, GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void render() {
        glBindVertexArray(vaoHandle);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, ((GLubyte*) NULL + (0)));
    }
};

#endif //ASSIGNMENT4_VBOCUBE_H

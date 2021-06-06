//
// Created by liangf on 3/27/21.
//

#ifndef ASSIGNMENT4_VBORECTANGLE_H
#define ASSIGNMENT4_VBORECTANGLE_H

class VBORectangle {

private:
    unsigned int vaoHandle;

public:
    VBORectangle() {
        const float coord = 1.f;

        float v[4 * 3] = {
                // Front
                coord, coord, 0.,
                coord, -coord, 0.,
                -coord, -coord, 0.,
                -coord, coord, 0.,
        };

        float n[4 * 3] = {
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
        };

        float tex[4 * 2] = {
                1.0f, 1.0f,
                1.0f, 0.0f,
                0.0f, 0.0f,
                0.0f, 1.0f,
        };

        GLuint el[] = {
                0, 1, 2, 0, 2, 3
        };

        glGenVertexArrays(1, &vaoHandle);
        glBindVertexArray(vaoHandle);

        unsigned int handle[4];
        glGenBuffers(4, handle);

        glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
        glVertexAttribPointer((GLuint) 0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*) NULL + (0)));
        glEnableVertexAttribArray(0);  // Vertex position

        glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(n), n, GL_STATIC_DRAW);
        glVertexAttribPointer((GLuint) 1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*) NULL + (0)));
        glEnableVertexAttribArray(1);  // Vertex normal

        glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(tex), tex, GL_STATIC_DRAW);
        glVertexAttribPointer((GLuint) 2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*) NULL + (0)));
        glEnableVertexAttribArray(2);  // texture coords

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(el), el, GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void render() {
        glBindVertexArray(vaoHandle);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, ((GLubyte*) NULL + (0)));
    }
};

#endif //ASSIGNMENT4_VBORECTANGLE_H

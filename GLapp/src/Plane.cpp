// draw a simple plane model

#include "Plane.hpp"
#include "GLapp.hpp"
#include "Vec.inl"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// load the sphere data
Plane::Plane(fVec3 size, const char *texturePPM) :
    Object(texturePPM)
{
    // build texture coordinate, normal, and vertex arrays
    uv = {fVec2{0.f,0.f}, fVec2{1.f,0.f}, fVec2{0.f,1.f}, fVec2{1.f,1.f}};
    norm = {fVec3{0.f,0.f,1.f}, fVec3{0.f,0.f,1.f}, fVec3{0.f,0.f,1.f}, fVec3{0.f,0.f,1.f}};
    vert = {
        fVec3{-0.5f*size[0], -0.5f*size[1], 0.f},
        fVec3{ 0.5f*size[0], -0.5f*size[1], 0.f},
        fVec3{-0.5f*size[0],  0.5f*size[1], 0.f},
        fVec3{ 0.5f*size[0],  0.5f*size[1], 0.f}
    };

    // build index array linking sets of three vertices into triangles
    indices = {0, 1, 3,   0, 3, 2};

    // load vertex and index arrays to GPU
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(vert[0]), &vert[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[NORMAL_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(norm[0]), &norm[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[UV_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(uv[0]), &uv[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

    updateShaders();
}

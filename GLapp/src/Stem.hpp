#pragma once
// data and functions to draw a sphere

#include "Mat.hpp"
#include "Mat.inl"

#include <vector>

// sphere object
struct Stem  
{
    // hardcoded dimensions of a tree primitive
    static constexpr float size[3] = {5.f, 5.f, 10.f};
    static constexpr int width = 6;
    static constexpr int height = 1;

    // create sphere given latitude and longitude sizes and color texture. offset is the vertices offset for composite objects
    Stem(fMat4 _transform = translate<float>(0), int offset = 0);
    const std::vector<fVec3>& getVertices() const { return vert; }
    const std::vector<fVec3>& getNorm() const { return norm; }
    const std::vector<fVec2>& getTextures() const { return uv; }
    const std::vector<unsigned int>& getIndicies() const { return indices; }

    std::vector<fVec3> vert;        //   per-vertex position
    std::vector<fVec3> norm;        //   per-vertex normal
    std::vector<fVec2> uv;          //   per-vertex texture coordinate
    std::vector<unsigned int> indices;  //   3 vertex indices per triangle
    fMat4 transform;
};

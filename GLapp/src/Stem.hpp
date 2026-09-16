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
    static constexpr int pGranularity = 6; // smoothness of the xy cross-section of the stump
    static constexpr int zGranularity = 1; // z axis layers

    // create sphere given latitude and longitude sizes and color texture. 
    // Offset is the vertices offset for composite objects.
    // Scale is the size of the tree, first along the xy plane, then along the z axis.
    Stem(fMat4 _transform = translate<float>(0), int offset = 0, fVec2 scale = {1.f, 1.f});
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

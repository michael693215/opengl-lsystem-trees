#pragma once

#include "LSystem.hpp"
#include "Object.hpp"
#include "Stem.hpp"
#include "GLapp.hpp"
#include "Mat.inl"
#include "Vec.hpp"

#include <string>
#include <stack>
#include <random>

struct Tree : public Object
{
    // stores information about current rotations and twists
    struct State 
    {
        State(fMat4 _tMatrix = Mat<float, 4>(1.f)) : tMatrix(_tMatrix) {} 
        void xRotateMatrix(LSystem::Settings& settings, bool isPos, float);
        void zRotateMatrix(LSystem::Settings& settings, float);
        fMat4 getMatrix(LSystem::Settings&, const float);
        float scale = 1.f; // girth of the tree will reduce based on height
     private:
        fMat4 tMatrix; // most recent transformation
    };

    LSystem lsys;
    std::stack<State> stack;
    float stubThreshold = 0.15f; // can't reduce dimensions of a stub by more than this fraction of current size
    float angleThreshold = 0.15f;
    std::mt19937 rand;
    unsigned int offset = 0; // not a parameter; only for tree to use

    Tree(std::string file, int _generations = 4, fMat4 transform = translate<float>(0.f), const char *texturePPM = "bark");
};


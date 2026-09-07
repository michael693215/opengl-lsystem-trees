#pragma once

#include "LSystem.hpp"
#include "Object.hpp"
#include "Stem.hpp"
#include "GLapp.hpp"
#include "Mat.inl"

#include <string>
#include <stack>

struct Tree : public Object
{
    // stores information about current rotations and twists
    struct State 
    {
        State(fMat4 _tMatrix = Mat<float, 4>(1.f)) : tMatrix(_tMatrix) {} 
        void xRotateMatrix(LSystem::Settings& settings, bool isPos);
        void zRotateMatrix(LSystem::Settings& settings);
        fMat4 getMatrix(LSystem::Settings&, const float[3]);
     private:
        fMat4 tMatrix; // most recent transformation
    };

    LSystem lsys;
    std::stack<State> stack;
    unsigned int offset = 0; // not a parameter; only for tree to use

    Tree(std::string file, fMat4 transform = translate<float>(0.f), const char *texturePPM = "bark");
};


#include "Tree.hpp"
#include "Mat.inl"
#include "Stem.hpp"
#include <iostream>
#include <cctype>
#include <cmath>
#include <random>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void Tree::State::xRotateMatrix(LSystem::Settings &settings, bool isPos, float randomness)
{
    tMatrix = isPos ? tMatrix * xrotate(settings.turnAngle * randomness) : tMatrix * xrotate(-settings.turnAngle * randomness);
}

void Tree::State::zRotateMatrix(LSystem::Settings &settings, float randomness)
{
    tMatrix = tMatrix * zrotate(settings.twistAngle * randomness);
}

fMat4 Tree::State::getMatrix(LSystem::Settings &settings, const float size)
{
    fMat4 transforms = tMatrix;
    // move the transformation matrix to the tip
    tMatrix = tMatrix * translate<float>({0, 0, size * 2.f});
    return transforms;
}

Tree::Tree(std::string file, int _generations, fMat4 transform, const char *texturePPM) : lsys(file, _generations), Object(texturePPM)
{
    State state(transform);
    stack.push(state);
    std::random_device rd;
    rand.seed(rd());
    
    std::string::iterator it;
    for (it = lsys.generated.begin(); it != lsys.generated.end(); ++it)
    {
        std::uniform_real_distribution<> randomizer(1.f - angleThreshold, 1 + angleThreshold);
        switch (*it)
        {
            // push transformation stack (save current state, not initial state)
            case '[':
                stack.push(stack.top());
                break;
            // pop transformation stack
            case ']':
                stack.pop();
                break;
            // add some twist
            case '^':
                stack.top().zRotateMatrix(lsys.settings, randomizer(rand));
                break;
            // tilt towards negative x 
            case '-':
                stack.top().xRotateMatrix(lsys.settings, false, randomizer(rand));
                break;
            // tilt towards positive x
            case '+':
                stack.top().xRotateMatrix(lsys.settings, true, randomizer(rand));
                break;
            // add a branch
            default:
                if (std::isupper(*it)) 
                {
                    std::uniform_real_distribution<> genPlane(std::max(stack.top().scale - stubThreshold, 0.f), stack.top().scale); // reduction in circumfrence of stump
                    std::uniform_real_distribution<> genZ(1.f - stubThreshold, 1.f + stubThreshold); // Add variability to stump height
                    float zVariance = genZ(rand);
                    stack.top().scale = genPlane(rand);
                    Stem temp = Stem(stack.top().getMatrix(LSystem::settings, zVariance * Stem::size[2]), offset, {stack.top().scale, zVariance});
                    vert.insert(vert.end(), temp.getVertices().begin(), temp.getVertices().end()); 
                    norm.insert(norm.end(), temp.getNorm().begin(), temp.getNorm().end()); 
                    uv.insert(uv.end(), temp.getTextures().begin(), temp.getTextures().end()); 
                    indices.insert(indices.end(), temp.getIndicies().begin(), temp.getIndicies().end());
                    offset += temp.getVertices().size();
                }
                break;
        }
    }

    // load vertex and index array to GPU
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

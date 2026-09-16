#pragma once
// collected state for access in callbacks
// 

#include "Mat.hpp"
#include "Vec.hpp"
#include <vector>

class GLapp {
public:
    struct GLFWwindow *win;      // graphics window from GLFW system

    // uniform buffer data about the scene
    // must be plain old data, matching layout in shaders
    // rearrange or pad as necessary for vec4 alignment
    struct SceneData {
        fMat4 ProjFromWorld, WorldFromProj;  // viewing matrix & inverse
        fVec4 LightDir;         // xyz = light direction; w = ambient
    } scene;
    unsigned int sceneUniformsID;

    // view info
    bool active;                // clicked into window
    int width, height;          // current window dimensions
    float distance;             // distance from 0,0,0
    float pan, tilt;            // horizontal and vertical Euler angles
    float panRate, tiltRate, zoomRate;    // keyboard orbiting rate in radians/sec

    // mouse state
    double mouseX, mouseY;      // location of mouse at last event

    // drawing state
    bool wireframe;

    // time (in seconds) of last frame
    double prevTime;

    // objects to draw
    std::vector<class Object*> objects;

    // tree data
    size_t generations;
    char *rule;

public:
    // initialize and destroy app data
    GLapp(char *_rule, size_t _generations);
    ~GLapp();

    // update shader uniform state each frame
    void sceneUpdate(double dTime);

    // regenerate the tree (after changing generations)
    void regenerateTree();

    // main rendering loop
    void render();
};

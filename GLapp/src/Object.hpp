#pragma once
// base class for drawable objects

#include "Shader.hpp"
#include "Mat.hpp"
#include "Vec.hpp"
#include <vector>

class Object {
protected:
    // uniform buffer data about the object position
    // must be plain old data, matching layout in shaders
    // rearrange or pad as necessary for vec4 alignment
    struct ObjectData {
        fMat4 WorldFromModel, ModelFromWorld;
    } object;

    // arrays defining triangles for GPU
    unsigned int varrayID;              // GL vertex array object, containing:
    std::vector<fVec3> vert;        //   per-vertex position
    std::vector<fVec3> norm;        //   per-vertex normal
    std::vector<fVec2> uv;          //   per-vertex texture coordinate
    std::vector<unsigned int> indices;  //   3 vertex indices per triangle

    // GL texture ID(s), array for extensibility to more textures
    enum {COLOR_TEXTURE, NORM_TEXTURE, PROPS_TEXTURE, NUM_TEXTURES};
    unsigned int textureIDs[NUM_TEXTURES];

    // GL buffer object IDs
    enum {OBJECT_UNIFORM_BUFFER, POSITION_BUFFER, NORMAL_BUFFER, UV_BUFFER, INDEX_BUFFER, NUM_BUFFERS};
    unsigned int bufferIDs[NUM_BUFFERS];

    // GL shaders
    unsigned int shaderID;      // ID for shader program
    std::vector<ShaderInfo> shaderParts;  // vertex & fragment shader info

public:
    // base object constructor: create buffers and textures
    Object(const char *textureBase);

    // virtual destructor to delete any child class data
    virtual ~Object();

    // draw this object
    virtual void draw(class GLapp *app, double now);

    // set shader, textures, etc. for this draw
    virtual void setRenderState(class GLapp *app, double now);

    // load/reload shaders
    virtual void updateShaders();

    // load an image file into a texture object
    void loadPPM(const char *imagefile, unsigned int bufferID);
};

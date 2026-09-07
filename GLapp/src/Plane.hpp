#pragma once
// data and functions to draw a plane

#include "Object.hpp"
#include "Vec.hpp"

// plane object
class Plane : public Object {
public:
    // create plane from -size/2 to size/2
    Plane(fVec3 size, const char *texturePPM);
};

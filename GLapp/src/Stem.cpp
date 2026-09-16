// draw a simple sphere model

#include "Stem.hpp"
#include "Mat.inl"
#include "Vec.inl"
#include <math.h>

#ifndef F_PI
#define F_PI 3.1415926f
#endif

// load the sphere data
Stem::Stem(fMat4 _transform, int offset, fVec2 scale) : transform(_transform)
{
    // build vertex, normal and texture coordinate arrays
    // * x & y are longitude and latitude grid positions

    float zOffset = scale[1] * size[2]; // whole tree is above plane

    for(unsigned int y=0;  y <= Stem::zGranularity;  ++y) {
        for(unsigned int x=0;  x <= Stem::pGranularity;  ++x) {
            // Texture coordinates scaled from x and y. Be sure to cast before division!
            float u = float(x)/float(Stem::pGranularity), v = float(y) / float(Stem::zGranularity);
            uv.push_back(fVec2{u,v});

            // normal for sphere is normalized position in spherical coordinates
            float cx = cosf(2.f * F_PI * u), sx = sinf(2.f * F_PI * u);
            fVec4 N = fVec4{cx, sx, 0.f, 0.f};
            fVec4 Nt = transform * N;
            fVec3 Nflat = fVec3{Nt[0], Nt[1], Nt[2]};
            norm.push_back(Nflat);

            // 3d vertex location scaled by sphere size
            fVec4 p = {scale[0] * size[0] * N[0], scale[0] * size[1] * N[1], scale[1] * size[2] * ((v * 2.f) - 1.f) + zOffset, 1.f};
            fVec4 pt = transform * p;
            fVec3 pflat = {pt[0] / pt[3], pt[1] / pt[3], pt[2] / pt[3]};
            vert.push_back(pflat);
        }
    }

    // build index array linking sets of three vertices into triangles
    // two triangles per square in the grid. Each vertex index is
    // essentially its unfolded grid array position. Be careful that
    // each triangle ends up in counter-clockwise order
    for(unsigned int y=0; y<Stem::zGranularity; ++y) {
        for(unsigned int x=0; x<Stem::pGranularity; ++x) {
            indices.push_back(offset + (Stem::pGranularity+1)* y    + x);
            indices.push_back(offset + (Stem::pGranularity+1)* y    + x+1);
            indices.push_back(offset + (Stem::pGranularity+1)*(y+1) + x+1);

            indices.push_back(offset + (Stem::pGranularity+1)* y    + x);
            indices.push_back(offset + (Stem::pGranularity+1)*(y+1) + x+1);
            indices.push_back(offset + (Stem::pGranularity+1)*(y+1) + x);
        }
    }

    offset += vert.size();
    zOffset = scale[1] * size[2] * 2.f;

    // 2 segments for hemispherical top
    int segments = 2;
    for(unsigned int y=0;  y <= segments;  ++y) {
        for(unsigned int x=0;  x <= Stem::pGranularity;  ++x) {
            // Texture coordinates scaled from x and y. Be sure to cast before division!
            float u = float(x)/float(Stem::pGranularity), v = float(y) / float(segments);
            uv.push_back(fVec2{u,v});

            // normal for sphere is normalized position in spherical coordinates
            float cx = cosf(2.f * F_PI * u), sx = sinf(2.f * F_PI * u);
            float lat = F_PI * 0.5f * (1.f - v); // after pi / 2, cosine function goes negative, so stop at pi / 2 for just a half circle
            float r = sinf(lat);
            float cz = 0.5f * cosf(lat);

            // this is a dummy object so GL can't help us transform
            fVec4 N = fVec4{cx * r, sx * r, cz, 0.f};
            fVec4 Nt = transform * N;
            fVec3 Nflat = fVec3{Nt[0], Nt[1], Nt[2]};
            norm.push_back(Nflat);

            // 3d vertex location scaled by sphere size
            fVec4 p = {scale[0] * size[0] * N[0], scale[0] * size[1] * N[1], scale[0] * size[2] * cz + zOffset, 1.f};
            fVec4 pt = transform * p;
            fVec3 pflat = {pt[0] / pt[3], pt[1] / pt[3], pt[2] / pt[3]};
            vert.push_back(pflat);
        }
    }

    for(unsigned int y=0; y<segments; ++y) {
        for(unsigned int x=0; x<Stem::pGranularity; ++x) {
            indices.push_back(offset + (Stem::pGranularity+1)* y    + x);
            indices.push_back(offset + (Stem::pGranularity+1)* y    + x+1);
            indices.push_back(offset + (Stem::pGranularity+1)*(y+1) + x+1);

            indices.push_back(offset + (Stem::pGranularity+1)* y    + x);
            indices.push_back(offset + (Stem::pGranularity+1)*(y+1) + x+1);
            indices.push_back(offset + (Stem::pGranularity+1)*(y+1) + x);
        }
    }
}
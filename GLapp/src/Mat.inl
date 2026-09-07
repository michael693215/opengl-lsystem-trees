#pragma once
// NxN-dimensional matrix class template
// based on ideas from Nathan Reed
//    http://www.reedbeta.com/blog/2013/12/28/on-vector-math-libraries/

#include "Mat.hpp"
#include "Vec.inl"
#include <cmath>
#include <cassert>

//////////////////////////////////////////////////////////////////////
// constructors
// matrix with val on the diagonal
template <typename T, unsigned int N>
Mat<T,N>::Mat(T val) {
    for (int i=0; i<N; ++i) {
	    for (int j=0; j<N; ++j) {
      		data[i][j] = T(0);
    	}
        data[i][i] = val;
    }
}

// constructor from list of elements
template <typename T, unsigned int N>
Mat<T,N>::Mat(std::initializer_list<T> values) {
    assert(values.size() == N*N);
    int i = 0;
    T* dataptr = (T*)data;
    for (auto v = values.begin(); i<N*N; ++i, ++v) {
        dataptr[i] = *v;
    }
}

// constructor from list columns
template <typename T, unsigned int N>
Mat<T,N>::Mat(std::initializer_list<Vec<T,N> > cols) {
    assert(cols.size() == N);
    int i = 0;
    for (auto v = cols.begin(); i<N; ++i, ++v) {
        data[i] = *v;
    }
}

// build a mat4 from a mat3 with 0,0,0,1 for the extra row & column
template <typename T>
Mat<T,4> mat4(Mat<T,3> m) {
    return Mat<T,4>{
        m[0][0], m[0][1], m[0][2],0.f,
        m[1][0], m[1][1], m[1][2],0.f,
        m[2][0], m[2][1], m[2][2],0.f,
        0.f, 0.f, 0.f, 1.f
    };
}

// build a mat3 from a mat4 with the last row and column dropped
template <typename T>
Mat<T,3> mat3(Mat<T,4> m) {
    return Mat<T,3>{
        m[0][0], m[0][1], m[0][2],
        m[1][0], m[1][1], m[1][2],
        m[2][0], m[2][1], m[2][2],
    };
}

//////////////////////////////////////////////////////////////////////
// matrix size
template <typename T, unsigned int N>
inline int dimensions(const Mat<T,N> &m) {
    return N;
}

//////////////////////////////////////////////////////////////////////
// matrix addition and subtraction
template <typename T, unsigned int N>
inline Mat<T,N> operator+(Mat<T,N> m1, Mat<T,N> m2) {
    Mat<T,N> result;
    for(int i=0; i<N; ++i)
        for(int j=0; j<N; ++j)
            result[i][j] = m1[i][j] + m2[i][j];
    return result;
}

template <typename T, unsigned int N>
inline Mat<T,N> operator-(Mat<T,N> m1, Mat<T,N> m2) {
    Mat<T,N> result;
    for(int i=0; i<N; ++i)
        for(int j=0; j<N; ++j)
            result[i][j] = m1[i][j] - m2[i][j];
    return result;
}

//////////////////////////////////////////////////////////////////////
// scalar multiplication and division
template <typename T, unsigned int N>
inline Mat<T,N> operator-(Mat<T,N> m) {
    Mat<T,N> result;
    for(int i=0; i<N; ++i)
        for(int j=0; j<N; ++j)
            result[i][j] = -m[i][j];
    return result;
}

template <typename T, unsigned int N>
inline Mat<T,N> operator*(T s, Mat<T,N> m) {
    Mat<T,N> result;
    for(int i=0; i<N; ++i)
        for(int j=0; j<N; ++j)
            result[i][j] = s * m[i][j];
    return result;
}

template <typename T, unsigned int N>
inline Mat<T,N> operator*(Mat<T,N> m, T s) {
    Mat<T,N> result;
    for(int i=0; i<N; ++i)
        for(int j=0; j<N; ++j)
            result[i][j] = m[i][j] * s;
    return result;
}

template <typename T, unsigned int N>
inline Mat<T,N> operator/(Mat<T,N> m, T s) {
    Mat<T,N> result;
    for(int i=0; i<N; ++i)
        for(int j=0; j<N; ++j)
            result[i][j] = m[i][j] / s;
    return result;
}

//////////////////////////////////////////////////////////////////////
// matrix*matrix, matrix*vector, and vector*matrix
template <typename T, unsigned int N>
inline Mat<T,N> operator*(Mat<T,N> m1, Mat<T,N> m2) {
    Mat<T,N> result;
    for(int i=0; i<N; ++i) {
        for(int j=0; j<N; ++j) {
            result[j][i] = m1[0][i] * m2[j][0];

            for(int k=1; k<N; ++k)
                result[j][i] += m1[k][i] * m2[j][k];
        }
    }
    return result;
}

template <typename T, unsigned int N>
inline Vec<T,N> operator*(Vec<T,N> v, Mat<T,N> m) {
    Vec<T,N> result;
    for(int i=0; i<N; ++i) {
        result[i] = v[0] * m[i][0];

        for(int j=1; j<N; ++j)
            result[i] += v[j] * m[i][j];
    }
    return result;
}

template <typename T, unsigned int N>
inline Vec<T,N> operator*(Mat<T,N> m, Vec<T,N> v) {
    Vec<T,N> result;
    for(int j=0; j<N; ++j) {
        result[j] = m[0][j] * v[0];

        for(int i=1; i<N; ++i)
            result[j] += m[i][j] * v[i];
    }
    return result;
}

//////////////////////////////////////////////////////////////////////
// other generic matrix functions

// transpose (this'd be the place to add other matrix math operations)
template <typename T, unsigned int N>
inline Mat<T,N> transpose(Mat<T,N> m) {
    Mat<T,N> result;
    for(int i=0; i<N; ++i)
        for(int j=0; j<N; ++j)
            result[i][j] = m[j][i];
    return result;
}

// inverse of matrix, using Gaussian elimination
template <typename T, unsigned int N>
inline Mat<T,N> inverse(Mat<T,N> mat) {
    Mat<T,N> inv(1);
    // eliminate one row at a time
    for (int i=0; i<N; ++i) {
        // swap un-eliminated row with largest value
        for (int j=i+1; j<N; ++j) {
            if (std::abs(mat[j][i]) > std::abs(mat[i][i])) {
                std::swap(inv[i], inv[j]);
                std::swap(mat[i], mat[j]);
            }
        }
        
        // set first element to 1
        inv[i] = inv[i] / mat[i][i];
        mat[i] = mat[i] / mat[i][i];
        
        // eliminate from all other rows
        for (int j=0; j<N; ++j) {
            if (j == i) continue;
            inv[j] = inv[j] - inv[i] * mat[j][i];
            mat[j] = mat[j] - mat[i] * mat[j][i];
        }
    }
    
    return inv;
}

//////////////////////////////////////////////////////////////////////
// some specialized functions for 4x4 matrices

// build 4x4 perspective from field of view (in radians), 
// x/y aspect ratio, and near/far clipping planes
template <typename T>
inline Mat<T,4> perspective(T fov, T aspect, T near, T far) {
    T y = T(1)/std::tan(fov/T(2)), x = y/aspect;
    T z = (near+far)/(near-far), w = T(2)*near*far/(near-far);
    return Mat<T,4>{
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z,-1,
        0, 0, w, 0
    };
}

// build x, y, or z axis rotation for angle (in radians)
template <typename T>
inline Mat<T,4> xrotate(T angle) {
    T c = std::cos(angle), s = std::sin(angle);
    return Mat<T,4>{
        1, 0, 0, 0,
        0, c, s, 0,
        0,-s, c, 0,
        0, 0, 0, 1
    };
}
template <typename T>
inline Mat<T,4> yrotate(T angle) {
    T c = std::cos(angle), s = std::sin(angle);
    return Mat<T,4>{
        c, 0,-s, 0,
        0, 1, 0, 0,
        s, 0, c, 0,
        0, 0, 0, 1
    };
}
template <typename T>
inline Mat<T,4> zrotate(T angle) {
    T c = std::cos(angle), s = std::sin(angle);
    return Mat<T,4>{
        c,  s, 0, 0,
        -s, c, 0, 0,
        0,  0, 1, 0,
        0,  0, 0, 1
    };
}

// build 4x4 scale matrix
template <typename T>
inline fMat4 scale(fVec3 s) {
    return fMat4{
        s[0], 0,    0,    0,
        0,    s[1], 0,    0,
        0,    0,    s[2], 0,
        0,    0,    0,    1
    };
}

// build 4x4 translation matrix
template <typename T>
inline fMat4 translate(fVec3 t) {
    return fMat4{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        t[0], t[1], t[2], 1
    };
}

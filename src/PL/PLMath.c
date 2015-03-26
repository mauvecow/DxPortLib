/*
  DxPortLib - A portability library for DxLib-based software.
  Copyright (C) 2013-2014 Patrick McCarthy <mauve@sandwich.net>
  
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
  
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
    
  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required. 
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
 */

#include "PLInternal.h"

#include <math.h>

/* Matrix, Vector math functions. */

const PLMatrix g_matrixIdentity = {
    {
        {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        }
    }
};

PLVector3 *PL_Vector3_Set(PLVector3 *o, float x, float y, float z) {
    o->x = x;
    o->y = y;
    o->z = z;
    return o;
}
PLVector3 *PL_Vector3_Copy(PLVector3 *o, const PLVector3 *v) {
    o->x = v->x;
    o->y = v->y;
    o->z = v->z;
    return o;
}

float PL_Vector3_Dot(const PLVector3 *a, const PLVector3 *b) {
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

PLVector3 *PL_Vector3_Add(PLVector3 *o, const PLVector3 *a, const PLVector3 *b) {
    o->x = a->x + b->x;
    o->y = a->y + b->y;
    o->z = a->z + b->z;
    return o;
}
PLVector3 *PL_Vector3_Sub(PLVector3 *o, const PLVector3 *a, const PLVector3 *b) {
    o->x = a->x - b->x;
    o->y = a->y - b->y;
    o->z = a->z - b->z;
    return o;
}
PLVector3 *PL_Vector3_Mul(PLVector3 *o, const PLVector3 *a, const PLVector3 *b) {
    o->x = a->x * b->x;
    o->y = a->y * b->y;
    o->z = a->z * b->z;
    return o;
}

PLVector3 *PL_Vector3_Cross(PLVector3 *o, const PLVector3 *a, const PLVector3 *b) {
    /* perform on locals in case o == a or o == b */
    float x = (a->y * b->z) - (a->z * b->y);
    float y = (a->z * b->x) - (a->x * b->z);
    float z = (a->x * b->y) - (a->y * b->x);
    o->x = x; o->y = y; o->z = z;
    return o;
}

PLVector3 *PL_Vector3_Normalize(PLVector3 *o, const PLVector3 *v) {
    float l = sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
    o->x = v->x / l;
    o->y = v->y / l;
    o->z = v->z / l;
    return o;
}

PLMatrix *PL_Matrix_Copy(PLMatrix *o, const PLMatrix *m) {
    memcpy(o, m, sizeof(PLMatrix));
    return o;
}
PLMatrix *PL_Matrix_Transpose(PLMatrix *o, const PLMatrix *m) {
    int x, y;
    for (x = 0; x < 4; ++x) {
        o->m[x][x] = m->m[x][x];
        
        for (y = (x + 1); y < 4; ++y) {
            float a = m->m[x][y];
            float b = m->m[y][x];
            o->m[x][y] = b; o->m[y][x] = a;
        }
    }
    
    return o;
}

PLMatrix *PL_Matrix_CreateIdentity(PLMatrix *o) {
    o->m11 = 1; o->m12 = 0; o->m13 = 0; o->m14 = 0;
    o->m21 = 0; o->m22 = 1; o->m23 = 0; o->m24 = 0;
    o->m31 = 0; o->m32 = 0; o->m33 = 1; o->m34 = 0;
    o->m41 = 0; o->m42 = 0; o->m43 = 0; o->m44 = 1;
    return o;
}
PLMatrix *PL_Matrix_CreateScale(PLMatrix *o, float x, float y, float z) {
    o->m[0][0] = x; o->m[0][1] = 0; o->m[0][2] = 0; o->m[0][3] = 0;
    o->m[1][0] = 0; o->m[1][1] = y; o->m[1][2] = 0; o->m[1][3] = 0;
    o->m[2][0] = 0; o->m[2][1] = 0; o->m[2][2] = z; o->m[2][3] = 0;
    o->m[3][0] = 0; o->m[3][1] = 0; o->m[3][2] = 0; o->m[3][3] = 1;
    return o;
}
PLMatrix *PL_Matrix_CreateTranslation(PLMatrix *o, float x, float y, float z) {
    o->m[0][0] = 1; o->m[0][1] = 0; o->m[0][2] = 0; o->m[0][3] = 0;
    o->m[1][0] = 0; o->m[1][1] = 1; o->m[1][2] = 0; o->m[1][3] = 0;
    o->m[2][0] = 0; o->m[2][1] = 0; o->m[2][2] = 1; o->m[2][3] = 0;
    o->m[3][0] = x; o->m[3][1] = y; o->m[3][2] = z; o->m[3][3] = 1;
    return o;
}

PLMatrix *PL_Matrix_Multiply(PLMatrix *o, const PLMatrix *a, const PLMatrix *b) {
    PLMatrix n;
    int x, y, z;
    
    for (x = 0; x < 4; ++x) {
        for (y = 0; y < 4; ++y) {
            float v = 0;
            for (z = 0; z < 4; ++z) {
                v += a->m[x][z] * b->m[z][y];
            }
            n.m[x][y] = v;
        }
    }
    
    return PL_Matrix_Copy(o, &n);
}
PLMatrix *PL_Matrix_Inverse(PLMatrix *o, float *dDeterminant, const PLMatrix *m) {
    /* TO BE WRITTEN */
    PL_Matrix_CreateIdentity(o);
    return o;
}

PLMatrix *PL_Matrix_CreateOrthoOffCenterLH(PLMatrix *o, float left, float right,
                                        float bottom, float top, float zNear, float zFar) {
    float w = right - left;
    float h = top - bottom;
    
    o->m11 = 2.0f/w; o->m12 = 0; o->m13 = 0; o->m14 = 0;
    o->m21 = 0; o->m22 = 2.0f/h; o->m23 = 0; o->m24 = 0;
    o->m31 = 0; o->m32 = 0; o->m33 = 1.0f/(zFar - zNear); o->m34 = 0;
    o->m41 = -(left + right) / w; o->m42 = -(top + bottom) / h;
    o->m43 = zNear/(zNear - zFar); o->m44 = 1;
    
    return o;
}

PLMatrix *PL_Matrix_CreateOrthoOffCenterRH(PLMatrix *o, float left, float right,
                                        float bottom, float top, float zNear, float zFar) {
    float w = right - left;
    float h = top - bottom;
    
    o->m11 = 2.0f/w; o->m12 = 0; o->m13 = 0; o->m14 = 0;
    o->m21 = 0; o->m22 = 2.0f/h; o->m23 = 0; o->m24 = 0;
    o->m31 = 0; o->m32 = 0; o->m33 = 1.0f/(zNear - zFar); o->m34 = 0;
    o->m41 = -(left + right) / w; o->m42 = -(top + bottom) / h;
    o->m43 = zNear/(zNear - zFar); o->m44 = 1;
    
    return o;
}

PLMatrix *PL_Matrix_CreateOrthoLH(PLMatrix *o, float w, float h, float zNear, float zFar) {
    o->m11 = 2.0f/w; o->m12 = 0; o->m13 = 0; o->m14 = 0;
    o->m21 = 0; o->m22 = 2.0f/h; o->m23 = 0; o->m24 = 0;
    o->m31 = 0; o->m32 = 0; o->m33 = 1.0f/(zFar - zNear); o->m34 = 0;
    o->m41 = 0; o->m42 = 0; o->m43 = zNear/(zNear - zFar); o->m44 = 1;
    return o;
}

PLMatrix *PL_Matrix_CreateOrthoRH(PLMatrix *o, float w, float h, float zNear, float zFar) {
    o->m11 = 2.0f/w; o->m12 = 0; o->m13 = 0; o->m14 = 0;
    o->m21 = 0; o->m22 = 2.0f/h; o->m23 = 0; o->m24 = 0;
    o->m31 = 0; o->m32 = 0; o->m33 = 1.0f/(zNear - zFar); o->m34 = 0;
    o->m41 = 0; o->m42 = 0; o->m43 = zNear/(zNear - zFar); o->m44 = 1;
    return o;
}

PLMatrix *PL_Matrix_CreatePerspectiveFovLH(PLMatrix *o, float fovY, float aspectRatio, float zNear, float zFar) {
    float cotan = 1.0f / (float)tan(fovY / 2.0f);
    float zDelta = zFar - zNear;
    o->m11 = cotan / aspectRatio;
    o->m12 = 0;
    o->m13 = 0;
    o->m14 = 0;
    
    o->m21 = 0;
    o->m22 = cotan;
    o->m23 = 0;
    o->m24 = 0;
    
    o->m31 = 0;
    o->m32 = 0;
    o->m33 = zFar / zDelta;
    o->m34 = 1.0f;
    
    o->m41 = 0;
    o->m42 = 0;
    o->m43 = (zNear * zFar) / zDelta;
    o->m44 = 0;
    
    return o;
}

PLMatrix *PL_Matrix_CreatePerspectiveFovRH(PLMatrix *o, float fovY, float aspectRatio, float zNear, float zFar) {
    float cotan = 1.0f / (float)tan(fovY / 2.0f);
    float zDelta = zFar - zNear;

    o->m11 = cotan / aspectRatio;
    o->m12 = 0;
    o->m13 = 0;
    o->m14 = 0;
    
    o->m21 = 0;
    o->m22 = cotan;
    o->m23 = 0;
    o->m24 = 0;
    
    o->m31 = 0;
    o->m32 = 0;
    o->m33 = zFar / -zDelta;
    o->m34 = -1.0f;
    
    o->m41 = 0;
    o->m42 = 0;
    o->m43 = (zNear * zFar) / -zDelta;
    o->m44 = 0;
    return o;
}

PLMatrix *PL_Matrix_CreateLookAtLH(PLMatrix *o, const PLVector3 *eye, const PLVector3 *at, const PLVector3 *up) {
    PLVector3 x, y, z;
    
    /* z = normal(at - eye); */
    PL_Vector3_Normalize(&z, PL_Vector3_Sub(&z, at, eye));
    /* x = normal(cross(up, z)); */
    PL_Vector3_Normalize(&x, PL_Vector3_Cross(&x, up, &z));
    /* y = cross(z, x); */
    PL_Vector3_Cross(&y, &z, &x);
    
    o->m11 = x.x; o->m12 = y.x; o->m13 = z.x; o->m14 = 0;
    o->m21 = x.y; o->m22 = y.y; o->m23 = z.y; o->m14 = 0;
    o->m31 = x.z; o->m32 = y.z; o->m33 = z.z; o->m14 = 0;
    o->m41 = -PL_Vector3_Dot(&x, eye);
    o->m42 = -PL_Vector3_Dot(&y, eye);
    o->m43 = -PL_Vector3_Dot(&z, eye);
    o->m44 = 1;
    return o;
}

PLMatrix *PL_Matrix_CreateLookAtRH(PLMatrix *o, const PLVector3 *eye, const PLVector3 *at, const PLVector3 *up) {
    PLVector3 x, y, z;
    
    /* z = normal(eye - at); */
    PL_Vector3_Normalize(&z, PL_Vector3_Sub(&z, eye, at));
    /* x = normal(cross(up, z)); */
    PL_Vector3_Normalize(&x, PL_Vector3_Cross(&x, up, &z));
    /* y = cross(z, x); */
    PL_Vector3_Cross(&y, &z, &x);
    
    o->m11 = x.x; o->m12 = y.x; o->m13 = z.x; o->m14 = 0;
    o->m21 = x.y; o->m22 = y.y; o->m23 = z.y; o->m14 = 0;
    o->m31 = x.z; o->m32 = y.z; o->m33 = z.z; o->m14 = 0;
    o->m41 = PL_Vector3_Dot(&x, eye);
    o->m42 = PL_Vector3_Dot(&y, eye);
    o->m43 = PL_Vector3_Dot(&z, eye);
    o->m44 = 1;
    return o;
}


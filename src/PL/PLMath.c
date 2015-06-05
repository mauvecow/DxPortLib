/*
  DxPortLib - A portability library for DxLib-based software.
  Copyright (C) 2013-2015 Patrick McCarthy <mauve@sandwich.net>
  
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
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
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
    o->v.m11 = 1; o->v.m12 = 0; o->v.m13 = 0; o->v.m14 = 0;
    o->v.m21 = 0; o->v.m22 = 1; o->v.m23 = 0; o->v.m24 = 0;
    o->v.m31 = 0; o->v.m32 = 0; o->v.m33 = 1; o->v.m34 = 0;
    o->v.m41 = 0; o->v.m42 = 0; o->v.m43 = 0; o->v.m44 = 1;
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
PLMatrix *PL_Matrix_CreateRotationX(PLMatrix *o, float x) {
    float cosx = cosf(x);
    float sinx = sinf(x);

    PL_Matrix_CreateIdentity(o);
    o->v.m22 = cosx;
    o->v.m23 = -sinx;
    o->v.m32 = sinx;
    o->v.m33 = cosx;
    return o;
}
PLMatrix *PL_Matrix_CreateRotationY(PLMatrix *o, float y) {
    float cosy = cosf(y);
    float siny = sinf(y);

    PL_Matrix_CreateIdentity(o);

    o->v.m11 = cosy;
    o->v.m13 = -siny;
    o->v.m31 = siny;
    o->v.m33 = cosy;
    return o;
}
PLMatrix *PL_Matrix_CreateRotationZ(PLMatrix *o, float z) {
    float cosz = cosf(z);
    float sinz = sinf(z);

    PL_Matrix_CreateIdentity(o);

    o->v.m11 = cosz;
    o->v.m12 = -sinz;
    o->v.m21 = sinz;
    o->v.m22 = cosz;
    return o;
}

PLMatrix *PL_Matrix_CreateFromYawPitchRoll(PLMatrix *o, float yaw, float pitch, float roll) {
    float cosa = cosf(yaw);
    float sina = sinf(yaw);
    float cosb = cosf(pitch);
    float sinb = sinf(pitch);
    float cosc = cosf(roll);
    float sinc = sinf(roll);
    
    o->v.m11 = cosa * cosb;
    o->v.m12 = (cosa * sinb * sinc) - (sina * cosc);
    o->v.m13 = (cosa * sinb * cosc) + (sina * sinc);
    o->v.m14 = 0;
    o->v.m21 = sina * cosb;
    o->v.m22 = (sina * sinb * sinc) + (cosa * cosc);
    o->v.m23 = (sina * sinb * cosc) - (cosa * sinc);
    o->v.m24 = 0;
    o->v.m31 = -sinb;
    o->v.m32 = cosb * sinc;  
    o->v.m33 = cosb * cosc;  
    o->v.m34 = 0;
    o->v.m41 = 0;
    o->v.m42 = 0;
    o->v.m43 = 0;
    o->v.m44 = 1;

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
PLMatrix *PL_Matrix_Invert(PLMatrix *o, float *dDeterminant, const PLMatrix *m) {
    PLMatrix inv;
    float det;
    int x, y;
    
#define INV(x0, x1, x2, y0, y1, y2) \
    ((m->m[x0][y0] * ((m->m[x1][y1] * m->m[x2][y2]) - (m->m[x1][y2] * m->m[x2][y1]))) - \
     (m->m[x1][y0] * ((m->m[x0][y1] * m->m[x2][y2]) - (m->m[x0][y2] * m->m[x2][y1]))) + \
     (m->m[x2][y0] * ((m->m[x0][y1] * m->m[x1][y2]) - (m->m[x0][y2] * m->m[x1][y1]))))
    
    inv.m[0][0] = INV(1, 2, 3, 1, 2, 3);
    inv.m[0][1] = -INV(0, 2, 3, 1, 2, 3);
    inv.m[0][2] = INV(0, 1, 3, 1, 2, 3);
    inv.m[0][3] = -INV(0, 1, 2, 1, 2, 3);
    inv.m[1][0] = -INV(1, 2, 3, 0, 2, 3);
    inv.m[1][1] = INV(0, 2, 3, 0, 2, 3);
    inv.m[1][2] = -INV(0, 1, 3, 0, 2, 3);
    inv.m[1][3] = INV(0, 1, 2, 0, 2, 3);
    inv.m[2][0] = INV(1, 2, 3, 0, 1, 3);
    inv.m[2][1] = -INV(0, 2, 3, 0, 1, 3);
    inv.m[2][2] = INV(0, 1, 3, 0, 1, 3);
    inv.m[2][3] = -INV(0, 1, 2, 0, 1, 3);
    inv.m[3][0] = -INV(1, 2, 3, 0, 1, 2);
    inv.m[3][1] = INV(0, 2, 3, 0, 1, 2);
    inv.m[3][2] = -INV(0, 1, 3, 0, 1, 2);
    inv.m[3][3] = INV(0, 1, 2, 0, 1, 2);
    
#undef INV
    
    det = (m->m[0][0] * inv.m[0][0]) + (m->m[0][1] * inv.m[1][0]) +
          (m->m[0][2] * inv.m[2][0]) + (m->m[0][3] * inv.m[3][0]);
    
    if (det == 0.0f) {
       PL_Matrix_CreateIdentity(o);
    } else {
        det = 1.0f / det;
        
        for (x = 0; x < 4; ++x) {
            for (y = 0; y < 4; ++y) {
                o->m[x][y] = inv.m[x][y] * det;
            }
        }
    }
    
    if (dDeterminant != 0) {
        *dDeterminant = det;
    }
    
    return o;
}

PLMatrix *PL_Matrix_CreateOrthoOffCenterLH(PLMatrix *o, float left, float right,
                                        float bottom, float top, float zNear, float zFar) {
    float w = right - left;
    float h = top - bottom;
    
    o->v.m11 = 2.0f/w; o->v.m12 = 0; o->v.m13 = 0; o->v.m14 = 0;
    o->v.m21 = 0; o->v.m22 = 2.0f/h; o->v.m23 = 0; o->v.m24 = 0;
    o->v.m31 = 0; o->v.m32 = 0; o->v.m33 = 1.0f/(zFar - zNear); o->v.m34 = 0;
    o->v.m41 = -(left + right) / w; o->v.m42 = -(top + bottom) / h;
    o->v.m43 = zNear/(zNear - zFar); o->v.m44 = 1;
    
    return o;
}

PLMatrix *PL_Matrix_CreateOrthoOffCenterRH(PLMatrix *o, float left, float right,
                                        float bottom, float top, float zNear, float zFar) {
    float w = right - left;
    float h = top - bottom;
    
    o->v.m11 = 2.0f/w; o->v.m12 = 0; o->v.m13 = 0; o->v.m14 = 0;
    o->v.m21 = 0; o->v.m22 = 2.0f/h; o->v.m23 = 0; o->v.m24 = 0;
    o->v.m31 = 0; o->v.m32 = 0; o->v.m33 = 1.0f/(zNear - zFar); o->v.m34 = 0;
    o->v.m41 = -(left + right) / w; o->v.m42 = -(top + bottom) / h;
    o->v.m43 = zNear/(zNear - zFar); o->v.m44 = 1;
    
    return o;
}

PLMatrix *PL_Matrix_CreateOrthoLH(PLMatrix *o, float w, float h, float zNear, float zFar) {
    o->v.m11 = 2.0f/w; o->v.m12 = 0; o->v.m13 = 0; o->v.m14 = 0;
    o->v.m21 = 0; o->v.m22 = 2.0f/h; o->v.m23 = 0; o->v.m24 = 0;
    o->v.m31 = 0; o->v.m32 = 0; o->v.m33 = 1.0f/(zFar - zNear); o->v.m34 = 0;
    o->v.m41 = 0; o->v.m42 = 0; o->v.m43 = zNear/(zNear - zFar); o->v.m44 = 1;
    return o;
}

PLMatrix *PL_Matrix_CreateOrthoRH(PLMatrix *o, float w, float h, float zNear, float zFar) {
    o->v.m11 = 2.0f/w; o->v.m12 = 0; o->v.m13 = 0; o->v.m14 = 0;
    o->v.m21 = 0; o->v.m22 = 2.0f/h; o->v.m23 = 0; o->v.m24 = 0;
    o->v.m31 = 0; o->v.m32 = 0; o->v.m33 = 1.0f/(zNear - zFar); o->v.m34 = 0;
    o->v.m41 = 0; o->v.m42 = 0; o->v.m43 = zNear/(zNear - zFar); o->v.m44 = 1;
    return o;
}

PLMatrix *PL_Matrix_CreatePerspectiveFovLH(PLMatrix *o, float fovY, float aspectRatio, float zNear, float zFar) {
    float cotan = 1.0f / (float)tan(fovY / 2.0f);
    float zDelta = zFar - zNear;
    o->v.m11 = cotan / aspectRatio;
    o->v.m12 = 0;
    o->v.m13 = 0;
    o->v.m14 = 0;
    
    o->v.m21 = 0;
    o->v.m22 = cotan;
    o->v.m23 = 0;
    o->v.m24 = 0;
    
    o->v.m31 = 0;
    o->v.m32 = 0;
    o->v.m33 = zFar / zDelta;
    o->v.m34 = 1.0f;
    
    o->v.m41 = 0;
    o->v.m42 = 0;
    o->v.m43 = (zNear * zFar) / zDelta;
    o->v.m44 = 0;
    
    return o;
}

PLMatrix *PL_Matrix_CreatePerspectiveFovRH(PLMatrix *o, float fovY, float aspectRatio, float zNear, float zFar) {
    float cotan = 1.0f / (float)tan(fovY / 2.0f);
    float zDelta = zFar - zNear;

    o->v.m11 = cotan / aspectRatio;
    o->v.m12 = 0;
    o->v.m13 = 0;
    o->v.m14 = 0;
    
    o->v.m21 = 0;
    o->v.m22 = cotan;
    o->v.m23 = 0;
    o->v.m24 = 0;
    
    o->v.m31 = 0;
    o->v.m32 = 0;
    o->v.m33 = zFar / -zDelta;
    o->v.m34 = -1.0f;
    
    o->v.m41 = 0;
    o->v.m42 = 0;
    o->v.m43 = (zNear * zFar) / -zDelta;
    o->v.m44 = 0;
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
    
    o->v.m11 = x.x; o->v.m12 = y.x; o->v.m13 = z.x; o->v.m14 = 0;
    o->v.m21 = x.y; o->v.m22 = y.y; o->v.m23 = z.y; o->v.m14 = 0;
    o->v.m31 = x.z; o->v.m32 = y.z; o->v.m33 = z.z; o->v.m14 = 0;
    o->v.m41 = -PL_Vector3_Dot(&x, eye);
    o->v.m42 = -PL_Vector3_Dot(&y, eye);
    o->v.m43 = -PL_Vector3_Dot(&z, eye);
    o->v.m44 = 1;
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
    
    o->v.m11 = x.x; o->v.m12 = y.x; o->v.m13 = z.x; o->v.m14 = 0;
    o->v.m21 = x.y; o->v.m22 = y.y; o->v.m23 = z.y; o->v.m14 = 0;
    o->v.m31 = x.z; o->v.m32 = y.z; o->v.m33 = z.z; o->v.m14 = 0;
    o->v.m41 = PL_Vector3_Dot(&x, eye);
    o->v.m42 = PL_Vector3_Dot(&y, eye);
    o->v.m43 = PL_Vector3_Dot(&z, eye);
    o->v.m44 = 1;
    return o;
}


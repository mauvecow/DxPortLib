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

/*
 * Following classes are implemented here:
 * - CVector3D
 * - CMatrix
 * - CStyle
 */

#include "Luna.h"

#ifdef DXPORTLIB_LUNA_INTERFACE

#include "LunaInternal.h"

void CVector3D::TransformNormal(const CVector3D *v, const CMatrix *pM) {
    const CMatrix &m = *pM;
    Set(
        (v->x * m(0,0)) + (v->y * m(1,0)) + (v->z * m(2,0)),
        (v->x * m(0,1)) + (v->y * m(1,1)) + (v->z * m(2,1)),
        (v->x * m(0,2)) + (v->y * m(1,2)) + (v->z * m(2,2))
    );
}
void CVector3D::TransformCoord(const CVector3D *v, const CMatrix *pM) {
    const CMatrix &m = *pM;
    Set(
        (v->x * m(0,0)) + (v->y * m(1,0)) + (v->z * m(2,0)) + m(3,0),
        (v->x * m(0,1)) + (v->y * m(1,1)) + (v->z * m(2,1)) + m(3,1),
        (v->x * m(0,2)) + (v->y * m(1,2)) + (v->z * m(2,2)) + m(3,2)
    );
}
void CVector3D::TransformProjection(const CVector3D *v, const CMatrix *pM) {
    const CMatrix &m = *pM;
    float w = (v->x * m(0,3)) + (v->y * m(1,3)) + (v->z * m(2,3)) + m(3,3);
    float invw = 1.0f / w;
    Set(
        ((v->x * m(0,0)) + (v->y * m(1,0)) + (v->z * m(2,0)) + m(3,0)) * invw,
        ((v->x * m(0,1)) + (v->y * m(1,1)) + (v->z * m(2,1)) + m(3,1)) * invw,
        ((v->x * m(0,2)) + (v->y * m(1,2)) + (v->z * m(2,2)) + m(3,2)) * invw
    );
}

void CMatrix::Inverse(const CMatrix *src) {
    PL_Matrix_Inverse((PLMatrix *)this, NULL, (const PLMatrix *)src);
}
void CMatrix::Normalize(const CMatrix *src) {
    const CMatrix &n = *src;
    float ixl = 1.0f / ((n(0,0) * n(0,0)) + (n(1,0) * n(1,0)) + (n(2,0) * n(2,0)));
    float iyl = 1.0f / ((n(0,1) * n(0,1)) + (n(1,1) * n(1,1)) + (n(2,1) * n(2,1)));
    float izl = 1.0f / ((n(0,2) * n(0,2)) + (n(1,2) * n(1,2)) + (n(2,2) * n(2,2)));
    m[0][0] = n(0,0) * ixl;
    m[0][1] = n(0,1) * ixl;
    m[0][2] = n(0,2) * ixl;
    m[1][0] = n(1,0) * iyl;
    m[1][1] = n(1,1) * iyl;
    m[1][2] = n(1,2) * iyl;
    m[2][0] = n(2,0) * izl;
    m[2][1] = n(2,1) * izl;
    m[2][2] = n(2,2) * izl;
}
void CMatrix::Multiply(const CMatrix *a, const CMatrix *b) {
    PL_Matrix_Multiply((PLMatrix *)this, (const PLMatrix *)a, (const PLMatrix *)b);
}
void CMatrix::RotationX(Sint32 angle) {
    float fSin = LunaMath::Sin(angle);
    float fCos = LunaMath::Cos(angle);
    
    m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
    m[1][0] = 0; m[1][1] = fCos; m[1][2] = fSin; m[1][3] = 0;
    m[2][0] = 0; m[2][1] = -fSin; m[2][2] = fCos; m[2][3] = 0;
    m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
}
void CMatrix::RotationY(Sint32 angle) {
    float fSin = LunaMath::Sin(angle);
    float fCos = LunaMath::Cos(angle);
    
    m[0][0] = fCos; m[0][1] = 0; m[0][2] = -fSin; m[0][3] = 0;
    m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
    m[2][0] = fSin; m[2][1] = 0; m[2][2] = fCos; m[2][3] = 0;
    m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
}
void CMatrix::RotationZ(Sint32 angle) {
    float fSin = LunaMath::Sin(angle);
    float fCos = LunaMath::Cos(angle);
    
    m[0][0] = fCos; m[0][1] = fSin; m[0][2] = 0; m[0][3] = 0;
    m[1][0] = -fSin; m[1][1] = fCos; m[1][2] = 0; m[1][3] = 0;
    m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
    m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
}
void CMatrix::RotationAxis(Sint32 angle, const CVector3D *axis) {
    PLVector3 c;
    PL_Vector3_Set(&c, axis->x, axis->y, axis->z);
    PL_Vector3_Normalize(&c, &c);
    
    float fSin = LunaMath::Sin(angle);
    float fCos = LunaMath::Cos(angle);
    float nCos = 1.0f - fCos;
    
    m[0][0] = ((c.x*c.x) * nCos) + fCos;
    m[1][0] = ((c.y*c.x) * nCos) - (c.z * fSin);
    m[2][0] = ((c.z*c.x) * nCos) + (c.y * fSin);
    m[3][0] = 0;
    m[0][1] = ((c.x*c.y) * nCos) + (c.z * fSin);
    m[1][1] = ((c.y*c.y) * nCos) + fCos;
    m[2][1] = ((c.z*c.y) * nCos) - (c.x * fSin);
    m[3][1] = 0;
    m[0][2] = ((c.x*c.z) * nCos) - (c.y * fSin);
    m[1][2] = ((c.y*c.z) * nCos) + (c.x * fSin);
    m[2][2] = ((c.z*c.z) * nCos) + fCos;
    m[3][2] = 0;
    m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
}
void CMatrix::Rotation(Sint32 rotX, Sint32 rotY, Sint32 rotZ) {
    float sx = LunaMath::Sin(rotX / 2);
    float cx = LunaMath::Cos(rotX / 2);
    float sy = LunaMath::Sin(rotY / 2);
    float cy = LunaMath::Cos(rotY / 2);
    float sz = LunaMath::Sin(rotZ / 2);
    float cz = LunaMath::Cos(rotZ / 2);
    
    float x = (sx * cy * cz) - (cx * sy * sz);
    float y = (cx * sy * cz) + (sx * cy * sz);
    float z = (cx * cy * sz) - (sx * sy * cz);
    float w = (cx * cy * cz) + (sx * sy * sz);
    m[0][0] = 1 - (2 * ((y * y) + (z * z)));
    m[0][1] = 2 * ((x * y) - (w * z));
    m[0][2] = 2 * ((x * z) + (w * y));
    m[0][3] = 0;
    m[1][0] = 2 * ((x * y) + (w * z));
    m[1][1] = 1 - (2 * ((x * x) + (z * z)));
    m[1][2] = 2 * ((y * z) - (w * x));
    m[1][3] = 0;
    m[2][0] = 2 * ((x * z) - (w * y));
    m[2][1] = 2 * ((y * z) + (w * x));
    m[2][2] = 1 - (2 * ((x * x) + (z * z)));
    m[2][3] = 0;
    m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
}
void CMatrix::RotateVector(const CVector3D *a, const CVector3D *b, const CVector3D *axis) {
    CVector3D c;
    if (axis == NULL) {
        c.CrossProduct(a, b);
    } else {
        c = *axis;
    }
    
    float fCos = a->DotProduct(b) / (a->Length() * b->Length());
    float fSin = sqrtf(1 - (fCos * fCos));
    float nCos = 1.0f - fCos;
    
    m[0][0] = ((c.x*c.x) * nCos) + fCos;
    m[1][0] = ((c.y*c.x) * nCos) - (c.z * fSin);
    m[2][0] = ((c.z*c.x) * nCos) + (c.y * fSin);
    m[3][0] = 0;
    m[0][1] = ((c.x*c.y) * nCos) + (c.z * fSin);
    m[1][1] = ((c.y*c.y) * nCos) + fCos;
    m[2][1] = ((c.z*c.y) * nCos) - (c.x * fSin);
    m[3][1] = 0;
    m[0][2] = ((c.x*c.z) * nCos) - (c.y * fSin);
    m[1][2] = ((c.y*c.z) * nCos) + (c.x * fSin);
    m[2][2] = ((c.z*c.z) * nCos) + fCos;
    m[3][2] = 0;
    m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
}
void CMatrix::Perspective(float fov, float zNear, float zFar, float aspect) {
    PL_Matrix_CreatePerspectiveFovLH((PLMatrix *)this, fov, aspect, zNear, zFar);
}
void CMatrix::LookAt(const CVector3D *eye, const CVector3D *at, const CVector3D *up) {
    PL_Matrix_CreateLookAtLH((PLMatrix *)this, 
                     (const PLVector3 *)eye,
                     (const PLVector3 *)at, 
                     (const PLVector3 *)up);
}

void CMatrix::Ortho(float w, float h, float zNear, float zFar) {
    PL_Matrix_CreateOrthoLH((PLMatrix *)this, w, h, zNear, zFar);
}
void CMatrix::Resolution(CVector3D &pos, CVector3D &scale, CMatrix &rot) {
    rot = *this;
    pos = CVector3D(m[3][0], m[3][1], m[3][2]);
    rot(3,0) = 0.0f; rot(3,1) = 0.0f; rot(3,2) = 0.0f;
    
    CVector3D sx(1.0f, 0.0f, 0.0f);
    sx.TransformNormal(&rot);
    scale.x = 1.0f / sx.Length();
    
    CVector3D sy(1.0f, 0.0f, 0.0f);
    sy.TransformNormal(&rot);
    scale.y = 1.0f / sy.Length();
    
    CVector3D sz(1.0f, 0.0f, 0.0f);
    sz.TransformNormal(&rot);
    scale.z = 1.0f / sz.Length();
    
    CMatrix scaling;
    scaling.Scaling(scale.x, scale.y, scale.z);
    rot = scaling * rot;
}

CStyle::CStyle() {
    TransformInit();
}
CStyle::CStyle(const CStyle &style) {
    Set(&style);
}

void CStyle::Set(const CStyle *srcStyle) {
    TransformInit();
    
    m_mStyle = srcStyle->m_mStyle;
    m_mStyleInverse = srcStyle->m_mStyleInverse;
    m_mTransform = srcStyle->m_mTransform;
    m_mTransformInverse = srcStyle->m_mTransformInverse;
    
    m_vUp = srcStyle->m_vUp;
    m_vRight = srcStyle->m_vRight;
    m_vFront = srcStyle->m_vFront;
    m_vPosition = srcStyle->m_vPosition;
    m_vScale = srcStyle->m_vScale;
}
void CStyle::Get(CStyle *srcStyle) const {
    srcStyle->TransformInit();
    
    srcStyle->m_mStyle = m_mStyle;
    srcStyle->m_mStyleInverse = m_mStyleInverse;
    srcStyle->m_mTransform = m_mTransform;
    srcStyle->m_mTransformInverse = m_mTransformInverse;
    
    srcStyle->m_vUp = m_vUp;
    srcStyle->m_vRight = m_vRight;
    srcStyle->m_vFront = m_vFront;
    srcStyle->m_vPosition = m_vPosition;
    srcStyle->m_vScale = m_vScale;
}

void CStyle::TransformInit() {
    m_mStyle.Identity();
    m_mStyleInverse.Identity();
    m_mTransform.Identity();
    m_mTransformInverse.Identity();
    m_vUp.Set(0.0f, 1.0f, 0.0f);
    m_vRight.Set(1.0f, 0.0f, 0.0f);
    m_vFront.Set(0.0f, 0.0f, 1.0f);
    m_vPosition.Set(0.0f, 0.0f, 0.0f);
    m_vScale.Set(1.0f, 1.0f, 1.0f);
}
void CStyle::TransformUpdate() {
    CMatrix tempMatrix;
    
    PLMatrix *m = (PLMatrix *)&m_mStyle;
    m->m[0][0] = m_vRight.x;    m->m[0][1] = m_vRight.y;    m->m[0][2] = m_vRight.z;    m->m[0][3] = 0.0f;
    m->m[1][0] = m_vUp.x;       m->m[1][1] = m_vUp.y;       m->m[1][2] = m_vUp.z;       m->m[1][3] = 0.0f;
    m->m[2][0] = m_vFront.x;    m->m[2][1] = m_vFront.y;    m->m[2][2] = m_vFront.z;    m->m[2][3] = 0.0f;
    m->m[3][0] = 0.0f;          m->m[3][1] = 0.0f;          m->m[3][2] = 0.0f;          m->m[3][3] = 1.0f;
    m_mStyleInverse.Transpose(&m_mStyle);
    
    m = (PLMatrix *)&m_mTransform;
    m->m[0][0] = m_vRight.x;    m->m[0][1] = m_vRight.y;    m->m[0][2] = m_vRight.z;    m->m[0][3] = 0.0f;
    m->m[1][0] = m_vUp.x;       m->m[1][1] = m_vUp.y;       m->m[1][2] = m_vUp.z;       m->m[1][3] = 0.0f;
    m->m[2][0] = m_vFront.x;    m->m[2][1] = m_vFront.y;    m->m[2][2] = m_vFront.z;    m->m[2][3] = 0.0f;
    m->m[3][0] = m_vPosition.x; m->m[3][1] = m_vPosition.y; m->m[3][2] = m_vPosition.z; m->m[3][3] = 1.0f;
    
    tempMatrix.Scaling(m_vScale.x, m_vScale.y, m_vScale.z);
    m_mTransform = tempMatrix * m_mTransform;
    
    m = (PLMatrix *)&m_mTransformInverse;
    m->m[0][0] = m_vRight.x;    m->m[1][0] = m_vRight.y;    m->m[2][0] = m_vRight.z; 
    m->m[0][1] = m_vUp.x;       m->m[1][1] = m_vUp.y;       m->m[2][2] = m_vUp.z; 
    m->m[0][2] = m_vFront.x;    m->m[1][2] = m_vFront.y;    m->m[2][2] = m_vFront.z;
    m->m[0][3] = 0.0f;          m->m[1][3] = 0.0f;          m->m[2][3] = 0.0f;
    m->m[3][0] = -m_vPosition.DotProduct(&m_vRight);
    m->m[3][1] = -m_vPosition.DotProduct(&m_vUp);
    m->m[3][2] = -m_vPosition.DotProduct(&m_vFront);
    m->m[3][3] = 1.0f;
}

/*
void CStyle::RotationX(Uint32 rot) {
}
void CStyle::RotationY(Uint32 rot) {
}
void CStyle::RotationZ(Uint32 rot) {
}
void CStyle::Pitching(Uint32 rot) {
}
void CStyle::Heading(Uint32 rot) {
}
void CStyle::Rolling(Uint32 rot) {
}
void CStyle::RotationQuaternion(const CQuaternion *quat) {
}
 */

void CStyle::Translation(float px, float py, float pz) {
    m_vPosition.Set(px, py, pz);
}
void CStyle::Scaling(float sx, float sy, float sz) {
    m_vScale.Set(sx, sy, sz);
}
void CStyle::LookAt(const CVector3D *eye, const CVector3D *at, const CVector3D *pUp) {
    PLMatrix matrix;
    PLVector3 up;
    if (pUp == NULL) {
        PL_Vector3_Set(&up, 0, 1, 0);
    } else {
        PL_Vector3_Set(&up, pUp->x, pUp->y, pUp->z);
    }
    
    PL_Matrix_CreateLookAtLH(&matrix,
                     (const PLVector3 *)eye,
                     (const PLVector3 *)at,
                     &up);
    
    m_vFront.Set(matrix.m13, matrix.m23, matrix.m33);
    m_vRight.Set(-matrix.m11, -matrix.m21, -matrix.m31);
    m_vUp.Set(matrix.m12, matrix.m22, matrix.m32);
    
    m_vPosition = *eye;
}
void CStyle::FromMatrix(const CMatrix *m) {
    const CMatrix &n = *m;
    m_vRight.Set(n(0,0), n(0,1), n(0,2));
    m_vUp.Set(n(1,0), n(1,1), n(1,2));
    m_vFront.Set(n(2,0), n(2,1), n(2,2));
}
Sint32 CStyle::GetAngleX() const {
    return GET_ANGLE_PI(toF(asin(m_vFront.y)));
}
Sint32 CStyle::GetAngleY() const {
    if (m_vFront.y == -1.0f || m_vFront.y == 1.0f) {
        return GET_ANGLE_PI(toF(atan2(m_vUp.x, m_vUp.z)));
    } else {
        return GET_ANGLE_PI(toF(atan2(m_vFront.x, m_vFront.z)));
    }
}
Sint32 CStyle::GetAngleZ() const {
    float n = toF(asin(m_vRight.y / sqrtf(1.0f - (m_vFront.y * m_vFront.y))));
    if (m_vUp.y < 0) {
        return GET_ANGLE_PI(PI - n);
    } else {
        return GET_ANGLE_PI(n);
    }
}

#endif

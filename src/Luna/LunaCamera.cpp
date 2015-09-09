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

#include "Luna.h"

#ifdef DXPORTLIB_LUNA_INTERFACE

#include "LunaInternal.h"
#include "PL/PLInternal.h"

PLMatrix g_lunaProjectionMatrix;
PLMatrix g_lunaViewMatrix;

struct CameraData {
    PLMatrix projection;
    PLMatrix view;
    PLMatrix camera;
    PLVector3 position;
    
    CStyle style;
    
    float fov;
    float aspect;
    float zNear;
    float zFar;
};

LCAMERA LunaCamera::Create() {
    int cameraID;
    CameraData *camera;
    cameraID = PL_Handle_AcquireID(DXHANDLE_LUNACAMERA);
    if (cameraID < 0) {
        return INVALID_CAMERA;
    }
    
    camera = (CameraData *)PL_Handle_AllocateData(cameraID, sizeof(CameraData));
    memset(camera, 0, sizeof(CameraData));
    
    PL_Matrix_CreateIdentity(&camera->projection);
    PL_Matrix_CreateIdentity(&camera->view);
    PL_Matrix_CreateIdentity(&camera->camera);
    
    return cameraID;
}

void LunaCamera::Release(LCAMERA lCam) {
    CameraData *camera = (CameraData *)PL_Handle_GetData((int)lCam, DXHANDLE_LUNACAMERA);
    if (camera != NULL) {
        PL_Handle_ReleaseID(lCam, DXTRUE);
    }
}

void LunaCamera::CreateProjection(LCAMERA lCam, Float fFovY,
                                  Float fNearZ, Float fFarZ,
                                  Float fAspect) {
    CameraData *camera = (CameraData *)PL_Handle_GetData((int)lCam, DXHANDLE_LUNACAMERA);
    if (camera != NULL) {
        PLMatrix *matrix = (PLMatrix *)&camera->projection;
        
        camera->zNear = fNearZ;
        camera->zFar = fFarZ;
        camera->fov = fFovY;
        camera->aspect = fAspect;
        
        PL_Matrix_CreatePerspectiveFovLH(matrix, fFovY, fAspect, fNearZ, fFarZ);
    }
}

void LunaCamera::TransformInit(LCAMERA lCam) {
    CameraData *camera = (CameraData *)PL_Handle_GetData((int)lCam, DXHANDLE_LUNACAMERA);
    if (camera != NULL) {
        camera->style.TransformInit();
        PL_Matrix_CreateIdentity(&camera->view);
    }
}
void LunaCamera::TransformUpdate(LCAMERA lCam) {
    CameraData *camera = (CameraData *)PL_Handle_GetData((int)lCam, DXHANDLE_LUNACAMERA);
    if (camera != NULL) {
        camera->style.TransformUpdate();
        camera->style.GetPosition((CVector3D *)&camera->position);
        camera->style.GetTransformInverse((CMatrix *)&camera->view);
        
        PL_Matrix_Multiply(&camera->camera, &camera->view, &camera->projection);
    }
}

void LunaCamera::LookAt(LCAMERA lCam, CVector3D *pEye,
                        CVector3D *pAt, CVector3D *pUp) {
    CameraData *camera = (CameraData *)PL_Handle_GetData((int)lCam, DXHANDLE_LUNACAMERA);
    if (camera != NULL) {
        camera->style.LookAt(pEye, pAt, pUp);
    }
}

void LunaCamera_SetDevice(LCAMERA lCam) {
    CameraData *camera = (CameraData *)PL_Handle_GetData((int)lCam, DXHANDLE_LUNACAMERA);
    if (camera != NULL) {
        PLMatrix o;
        
        PL_Matrix_Copy(&g_lunaProjectionMatrix, &camera->projection);
        PL_Matrix_Multiply(&g_lunaProjectionMatrix, &g_lunaProjectionMatrix, PL_Matrix_CreateScale(&o, 1, -1, 1));
        PL_Matrix_Copy(&g_lunaViewMatrix, &camera->view);
    }
}

#endif /* #ifdef DXPORTLIB_LUNA_INTERFACE */

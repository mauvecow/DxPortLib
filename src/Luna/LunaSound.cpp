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

#ifndef DXPORTLIB_NO_SOUND

LSOUND LunaSound::CreateFromFile(const char *filename, bool IsNoStop,
                                 bool IsAyame, bool IsHardware, Uint32 Layer) {
    /* We don't care about IsAyame or IsHardware. Or even Layer.
     * We do care about IsNoStop. */
    char buf[2048];
    return PL_LoadSoundMem(
        PL_Text_ConvertStrncpyIfNecessary(
            buf, -1, filename, g_lunaUseCharSet, 2048)
    );
}
Bool LunaSound::IsPlay(LSOUND lSnd, Uint32 Layer) {
    if (PL_CheckSoundMem((int)lSnd) == DXTRUE) {
        return true;
    } else {
        return false;
    }
}
void LunaSound::Play(LSOUND lSnd, Uint32 Layer) {
    PL_PlaySoundMem((int)lSnd, DX_PLAYTYPE_BACK, DXTRUE);
}
void LunaSound::Loop(LSOUND lSnd, Uint32 LoopCount, Uint32 LoopInSample, Uint32 Layer) {
    PL_PlaySoundMem((int)lSnd, DX_PLAYTYPE_LOOP, DXTRUE);
}
void LunaSound::Stop(LSOUND lSnd, Uint32 Layer) {
    PL_StopSoundMem((int)lSnd);
}
void LunaSound::Pause(LSOUND lSnd, Uint32 Layer) {
    /* WRITEME N/A */
}
void LunaSound::Release(LSOUND lSnd) {
    PL_DeleteSoundMem((int)lSnd);
    return;
}
void LunaSound::SetVolume(LSOUND lSnd, Float fParam, Uint32 Layer) {
    PL_SetVolumeSoundMemDirect((int)(fParam * 255.0f / 100.0f), (int)lSnd);
}
void LunaSound::SetPan(LSOUND lSnd, Float fParam, Uint32 Layer) {
    /* WRITEME N/A */
}
void LunaSound::SetMax(Uint32 Max) {
    /* Sets maximum number of sounds. Doesn't matter for PL. */
}
void LunaSound::SetAyamePath(const char *pPath) {
    /* Stub. Does nothing. */
}
void LunaSound::EXTSetLoopSamples(LSOUND lSnd, Uint64 loopTarget, Uint64 loopPoint) {
    PL_Audio_SetLoopSamples((int)lSnd, loopTarget, loopPoint);
}
void LunaSound::EXTSetLoopTimes(LSOUND lSnd, double loopTarget, double loopTime) {
    PL_Audio_SetLoopTimes((int)lSnd, loopTarget, loopTime);
}

#else
/* DXPORTLIB_NO_SOUND */

LSOUND LunaSound::CreateFromFile(const char *filename, bool IsNoStop,
                                 bool IsAyame, bool IsHardware, Uint32 Layer) {
    return -1;
}
Bool LunaSound::IsPlay(LSOUND lSnd, Uint32 Layer) {
    return false;
}
void LunaSound::Play(LSOUND lSnd, Uint32 Layer) {
}
void LunaSound::Loop(LSOUND lSnd, Uint32 LoopCount, Uint32 LoopInSample, Uint32 Layer) {
}
void LunaSound::Stop(LSOUND lSnd, Uint32 Layer) {
}
void LunaSound::Pause(LSOUND lSnd, Uint32 Layer) {
}
void LunaSound::Release(LSOUND lSnd) {
}
void LunaSound::SetVolume(LSOUND lSnd, Float fParam, Uint32 Layer) {
}
void LunaSound::SetPan(LSOUND lSnd, Float fParam, Uint32 Layer) {
}
void LunaSound::SetMax(Uint32 Max) {
}
void LunaSound::SetAyamePath(const char *pPath) {
}
void LunaSound::EXTSetLoopSamples(LSOUND lSnd, Uint64 loopTarget, Uint64 loopPoint) {
}
void LunaSound::EXTSetLoopTimes(LSOUND lSnd, double loopTarget, double loopTime) {
}

#endif /* DXPORTLIB_NO_SOUND */

#endif /* #ifdef DXPORTLIB_LUNA_INTERFACE */

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

#include "Luna.h"

#ifdef DXPORTLIB_LUNA_INTERFACE

#include "PLInternal.h"

/* TODO: LunaRand is thread-safe. We are not.
 */

void LunaRand::Seed(Uint32 Param) {
    PL_Random_SeedLuna(Param);
}
Uint32 LunaRand::GetInt32() {
    return PL_Random_Get32();
}
Uint64 LunaRand::GetInt64() {
    Uint64 valueA = PL_Random_Get32();
    Uint64 valueB = PL_Random_Get32();
    return (valueA << 32) | valueB;
}
Float LunaRand::GetFloat32() {
    return F(PL_Random_Get32() % 100000001) / F(100000000);
}
Sint32 LunaRand::GetInt(Sint32 Min, Sint32 Max) {
    Sint32 range = Max + 1 - Min;
    return Min + ((Sint32)(LunaRand::GetInt32() >> 1) % range);
}
Float LunaRand::GetFloat(Float Min, Float Max) {
    Float f = GetFloat32();
    return Min + (f * (Max - Min));
}

#endif

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

/* DxLib uses the Mersenne Twister, of course.
 * 
 * Rather than the more modern variants, it uses an
 * older 1999 implementation to seed with.
 * 
 * Based on the original implementations
 * by Takuji Nishimura and Makoto Matsumoto.
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
 */

static int s_initialized = DXFALSE;

static unsigned long s_mt[624] = { 0 };
static unsigned int s_rngIndex = 0;

static void s_GenerateNextSet() {
    static const unsigned long table[2] = { 0, 2567483615UL };
    unsigned long *mt = s_mt;
    unsigned long v;
    int i;
    
    for (i = 0; i < (624 - 397); ++i) {
        v = (mt[i]&0x80000000) | (mt[i+1]&0x7fffffff);
        mt[i] = mt[i + 397] ^ (v>>1) ^ table[v&1];
    }
    for (; i < 623; ++i) {
        v = (mt[i]&0x80000000) | (mt[i+1]&0x7fffffff);
        mt[i] = mt[i + (397 - 624)] ^ (v>>1) ^ table[v&1];
    }
    
    v = (mt[623]&0x80000000) | (mt[0]&0x7fffffff);
    mt[623] = mt[396] ^ (v>>1) ^ table[v&1];
    
    s_rngIndex = 0;
}

static void s_SeedDx(unsigned long seed) {
    unsigned long *mt = s_mt;
    unsigned long i;
    
    mt[0] = seed;
    for (i = 0; i < 624; ++i) {
        mt[i] = seed & 0xffff0000;
        seed = 69069 * seed + 1;
        mt[i] |= (seed & 0xffff0000) >> 16;
        seed = 69069 * seed + 1;
    }
    
    s_rngIndex = 624;
    s_initialized = DXTRUE;
}
static void s_SeedLuna(unsigned long seed) {
    unsigned long *mt = s_mt;
    unsigned long i;
    
    mt[0] = seed;
    for (i = 0; i < 624; ++i) {
        mt[i] = (1812433253UL * (mt[i-1] ^ (mt[i-1] >> 30)) + i);
    }
    
    s_rngIndex = 624;
    s_initialized = DXTRUE;
}

static unsigned long s_GetNextLong() {
    unsigned long v;
    
    if (s_rngIndex >= 624) {
        s_GenerateNextSet();
    }
    
    v = s_mt[s_rngIndex++];
    
    v ^= (v>>11);
    v ^= (v<<7) & 0x9d2c5680;
    v ^= (v<<15) & 0xefc60000;
    v ^= (v>>18);
    
    return v;
}

unsigned int PL_Random_Get32() {
    if (s_initialized == DXFALSE) {
        s_SeedDx(4357); /* Default random seed value. */
    }
    return (unsigned int)s_GetNextLong();
}

int PL_Random_Get(int maxValue) {
    if (s_initialized == DXFALSE) {
        s_SeedDx(4357); /* Default random seed value. */
    }
    
    maxValue += 1;
    
    return (int)(((int64_t)s_GetNextLong() * maxValue) >> 32);
}

int PL_Random_SeedDx(int randomSeed) {
    s_SeedDx((unsigned long)randomSeed);
    
    return 0;
}
int PL_Random_SeedLuna(int randomSeed) {
    s_SeedLuna((unsigned long)randomSeed);
    
    return 0;
}

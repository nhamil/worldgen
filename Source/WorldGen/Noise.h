#pragma once 

#include <Fjord/Fjord.h> 

#define PRIME_0 1214520721ULL
#define PRIME_1 3850096717ULL
#define PRIME_2 2152656683ULL
#define PRIME_3 4038220471ULL

using namespace Fjord; 

// inspired by libnoise 
static float ValueNoise3Di(int x, int y, int z, int seed) 
{
    uint64 n = (
        PRIME_0 * x + 
        PRIME_1 * y + 
        PRIME_2 * z + 
        PRIME_3 * seed 
    ); 
    n = (n >> 13) ^ n; 
    n = (n * (n * n * 169670299ULL + 51482759298437ULL) + 3347709955261254437ULL);// & 0x7fffffff; 

    float res = (double) n / UINT64_MAX * 2 - 1; 
    return Clamp(res, -1.0, 1.0); 
}

static float ValueNoise3D(float x, float y, float z, int seed) 
{
    int x0 = (int) std::floor(x); 
    int y0 = (int) std::floor(y); 
    int z0 = (int) std::floor(z); 
    
    int x1 = x0 + 1; 
    int y1 = y0 + 1; 
    int z1 = z0 + 1; 
    
    float xa = x - x0; 
    float ya = y - y0; 
    float za = z - z0; 
    
    xa = (1 - std::cos(xa * FJ_PI)) * 0.5f; 
    ya = (1 - std::cos(ya * FJ_PI)) * 0.5f; 
    za = (1 - std::cos(za * FJ_PI)) * 0.5f; 
    
    float v000 = ValueNoise3Di(seed, x0, y0, z0); 
    float v100 = ValueNoise3Di(seed, x1, y0, z0); 
    float v010 = ValueNoise3Di(seed, x0, y1, z0); 
    float v110 = ValueNoise3Di(seed, x1, y1, z0); 
    float v001 = ValueNoise3Di(seed, x0, y0, z1); 
    float v101 = ValueNoise3Di(seed, x1, y0, z1); 
    float v011 = ValueNoise3Di(seed, x0, y1, z1); 
    float v111 = ValueNoise3Di(seed, x1, y1, z1); 
    
    float i00 = Lerp(v000, v100, xa); 
    float i10 = Lerp(v010, v110, xa); 
    float i01 = Lerp(v001, v101, xa); 
    float i11 = Lerp(v011, v111, xa); 
    
    float i0 = Lerp(i00, i10, ya); 
    float i1 = Lerp(i01, i11, ya); 
    
    return Lerp(i0, i1, za); 
}

static float FractalNoise(Vector3 pos, float freq, float pers, int iterations, long seed) 
{
    float sum = 0; 
    float total = 0; 
    
    float amp = 1; 
    
    for (int i = 0; i < iterations; i++) {
        total += amp; 
        sum += amp * ValueNoise3D(pos.X * freq, pos.Y * freq, pos.Z * freq, seed + i); 
        
        freq *= 2.0; 
        amp *= pers; 
    }
    
    return sum / total; 
}
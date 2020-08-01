#pragma once 

#define FJ_PI (3.14159265358797323846264)
#define FJ_2_PI (2 * FJ_PI)
#define FJ_4_PI (4 * FJ_PI)
#define FJ_PI_2 (0.5 * FJ_PI)
#define FJ_PI_4 (0.25 * FJ_PI)
#define FJ_INV_PI (1.0 / FJ_PI)

#define FJ_INV_180 (1.0 / 180.0)
#define FJ_TO_RAD (FJ_PI * FJ_INV_180)
#define FJ_TO_DEG (180.0 * FJ_INV_PI)

#define FJ_E (2.71828182845904523536)

#define FJ_SQRT_2 (1.41421356237309504880)

template <class T> 
inline T Min(const T& a, const T& b) 
{
    return a < b ? a : b; 
}

template <class T> 
inline T Max(const T& a, const T& b) 
{
    return a < b ? b : a; 
}

template <class T, class S> 
inline T Clamp(const T& x, const S& min, const S& max) 
{
    return x < min ? min : (x < max ? x : max); 
}

template <class T, class S> 
inline T Lerp(const T& x, const T& y, const S& a) 
{
    return x * (1 - a) + y * a; 
}

template <class T, class S> 
inline T SmoothLerp(const T& x, const T& y, const S& a) 
{
    return Lerp(x, y, a*a*(3-2*a));  
}

template <class T> 
inline T Round(const T& x) 
{
    return std::round(x); 
}

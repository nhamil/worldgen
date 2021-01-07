#version 330 

#define LIGHT_DIRECTIONAL 0 
#define LIGHT_POINT 1 
#define LIGHT_SPOT 2 

struct LightData 
{
    vec4 Ambient; 
    vec4 Color; 
    vec3 Position; 
    vec3 Direction; 
    int Type; 
    float Radius; 
};

in vec3 a_Position; 
in vec3 a_Normal; 
in vec4 a_Color0; 
in vec2 a_Texture0; 

out vec3 v_ViewPosition; 
out vec3 v_ViewNormal; 
out vec4 v_Color; 
out vec2 v_Texture0; 

uniform mat4 fj_Projection; 
uniform mat4 fj_ModelView; 
uniform mat3 fj_NormalMatrix; 

uniform vec3 u_TopLeft; 
uniform vec3 u_TopRight; 
uniform vec3 u_BottomLeft; 
uniform vec3 u_BottomRight; 

uniform float u_MinHeight = 1.0; 
uniform float u_MaxHeight = 1.5; 

uniform sampler2D u_Heightmap; 

const float InvSize = 1.0 / 32.0; 

uniform vec3 u_WaterColor = vec3(0.0, 0.0, 0.5); 
uniform vec3 u_LandColor = vec3(0.1, 0.5, 0.1); 

float Noise2Di(vec2 co){
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

float Noise3Di(vec3 co){
    return fract(sin(dot(co.xyz, vec3(12.9898,78.233,144.7272))) * 43758.5453);
}

float Noise3D(vec3 x) 
{
    vec3 f = floor(x); 
    vec3 i = x - f; 
    float v000 = Noise3Di(f); 
    float v001 = Noise3Di(v000 + vec3(0.0, 0.0, 1.0)); 
    float v010 = Noise3Di(v000 + vec3(0.0, 1.0, 0.0)); 
    float v011 = Noise3Di(v000 + vec3(0.0, 1.0, 1.0)); 
    float v100 = Noise3Di(v000 + vec3(1.0, 0.0, 0.0)); 
    float v101 = Noise3Di(v000 + vec3(1.0, 0.0, 1.0)); 
    float v110 = Noise3Di(v000 + vec3(1.0, 1.0, 0.0)); 
    float v111 = Noise3Di(v000 + vec3(1.0, 1.0, 1.0)); 

    return mix(
        mix(
            mix(v000, v100, i.x), 
            mix(v010, v110, i.x), 
            i.y
        ), 
        mix(
            mix(v001, v101, i.x), 
            mix(v011, v111, i.x), 
            i.y
        ), 
        i.z
    );
}

float FractalNoise(vec3 pos, float freq, float pers, int iterations) 
{
    float sum = 0.0; 
    float val = 0.0; 
    float amt = 1.0; 

    // for (int i = 0; i < iterations; i++) 
    // {
        sum += amt; 
        val += amt * Noise3D(pos * freq); 

        // freq *= 2.0; 
        // amt *= pers; 
    // }

    return val / sum; 
}

float GetHeight(float xOff, float yOff) 
{
    // return mix(u_MinHeight, u_MaxHeight, texture(u_Heightmap, vec2(
    //     a_Texture0.x + xOff * InvSize, 
    //     a_Texture0.y + yOff * InvSize 
    // )).r); 

    return mix(u_MinHeight, u_MaxHeight, 0.5 + 0.5 * FractalNoise(a_Position, 0.001, 0.6, 5)); 
}

vec3 GetPosition(float xOff, float yOff) 
{
    // float x = a_Position.x + xOff * InvSize; 
    // float y = a_Position.y + yOff * InvSize; 
    // return normalize(mix(
    //     mix(u_TopLeft, u_TopRight, x), 
    //     mix(u_BottomLeft, u_BottomRight, x), 
    //     y
    // )) * GetHeight(xOff, yOff); 

    return a_Position * FractalNoise(a_Position, 2.0, 0.6, 5); 
}

void main() 
{
    v_Texture0 = a_Texture0; 
    v_ViewNormal = fj_NormalMatrix * a_Normal; 

    vec3 dir = normalize(mix(
        mix(u_TopLeft, u_TopRight, a_Position.x), 
        mix(u_BottomLeft, u_BottomRight, a_Position.x), 
        a_Position.y
    )); 

    vec3 normal = normalize(cross(
        normalize(GetPosition(0, 1) - GetPosition(0, 0)), 
        normalize(GetPosition(1, 0) - GetPosition(0, 0)) 
    )); 

    float c, u, d, l, r; 
    c = GetHeight(0, 0); 
    u = GetHeight(0, 1); 
    d = GetHeight(0, -1); 
    l = GetHeight(-1, 0); 
    r = GetHeight(1, 0); 

    float h = texture(u_Heightmap, a_Texture0).r; 

    v_Color = vec4(mix(
        u_WaterColor, u_LandColor, clamp(h * 99, 0.0, 1.0)
    ), 1.0); 

    v_ViewNormal = fj_NormalMatrix * normal; 

    vec3 vert = dir * c; 

    vec4 pos = fj_ModelView * vec4(vert, 1.0); 

    v_ViewPosition = pos.xyz; 
    gl_Position = fj_Projection * pos; 
}

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

float Noise3Di(vec3 co, int seed)
{
    // return fract(sin(dot(co.xyz, vec3(12.9898,78.233,144.7272))) * 43758.5453);
    uint n = (
        1214520721u * uint(int(co.x)) + 
        3850096717u * uint(int(co.y)) + 
        2152656683u * uint(int(co.z))
    ); 
    n += 4038220471u * uint(seed); 

    n = (n >> 13) ^ n; 
    n = (n * (n * n * 169670299u + 829480213u) + 3136919911u);

    return float(n) / float(0xFFFFFFFFu) * 2.0 - 1.0; 
}

float Noise3D(vec3 x, int seed) 
{
    vec3 f = floor(x); 
    vec3 i = x - f; 
    i = i*i*(3-2*i); 
    float v000 = Noise3Di(f, seed); 
    float v001 = Noise3Di(f + vec3(0.0, 0.0, 1.0), seed); 
    float v010 = Noise3Di(f + vec3(0.0, 1.0, 0.0), seed); 
    float v011 = Noise3Di(f + vec3(0.0, 1.0, 1.0), seed); 
    float v100 = Noise3Di(f + vec3(1.0, 0.0, 0.0), seed); 
    float v101 = Noise3Di(f + vec3(1.0, 0.0, 1.0), seed); 
    float v110 = Noise3Di(f + vec3(1.0, 1.0, 0.0), seed); 
    float v111 = Noise3Di(f + vec3(1.0, 1.0, 1.0), seed); 

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

float FractalNoise(vec3 pos, float freq, float pers, int iterations, int seed) 
{
    float sum = 0.0; 
    float val = 0.0; 
    float amt = 1.0; 

    for (int i = 0; i < iterations; i++) 
    {
        sum += amt; 
        val += amt * (1 - abs(Noise3D(pos * freq, seed + i))); 

        freq *= 2.0; 
        amt *= pers; 
    }

    return val / sum; 
}

float GetHeight(vec3 pos) 
{
    // return mix(u_MinHeight, u_MaxHeight, texture(u_Heightmap, vec2(
    //     a_Texture0.x + xOff * InvSize, 
    //     a_Texture0.y + yOff * InvSize 
    // )).r); 

    float h = FractalNoise(pos, 2.0, 0.5, 20, 0) * 2 - 1.5; 
    h = max(0.0, h); 

    return mix(u_MinHeight, u_MaxHeight, h); 
}

void main() 
{
    vec3 location = normalize(mix(
        mix(u_TopLeft, u_TopRight, a_Position.x), 
        mix(u_BottomLeft, u_BottomRight, a_Position.x), 
        a_Position.y
    )); 

    float height = GetHeight(location); 

    vec3 vert = location * height; 
    vec3 vert_l = normalize(mix(
        mix(u_TopLeft, u_TopRight, a_Position.x+InvSize), 
        mix(u_BottomLeft, u_BottomRight, a_Position.x+InvSize), 
        a_Position.y
    )); 
    vec3 vert_u = normalize(mix(
        mix(u_TopLeft, u_TopRight, a_Position.x), 
        mix(u_BottomLeft, u_BottomRight, a_Position.x), 
        a_Position.y+InvSize
    )); 
    vert_l *= GetHeight(vert_l); 
    vert_u *= GetHeight(vert_u); 

    vec3 normal = normalize(-cross(
        normalize(vert_l - vert), 
        normalize(vert_u - vert)
    ));

    vec4 pos = fj_ModelView * vec4(vert, 1.0); 

    v_ViewPosition = pos.xyz; 
    v_ViewNormal = fj_NormalMatrix * normal; 
    float h = (height - u_MinHeight) / (u_MaxHeight - u_MinHeight); 
    v_Color = vec4(mix(u_WaterColor, u_LandColor, clamp(h * 99, 0.0, 1.0)), 1.0); 
    gl_Position = fj_Projection * pos; 
}

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

in vec3 v_ViewPosition; 
in vec3 v_ViewNormal; 
in vec4 v_Color; 
in vec2 v_Texture0; 
in vec3 v_NormPosition; 

out vec4 f_Color; 

uniform LightData fj_LightData; 
uniform vec4 fj_Emissive; 

uniform sampler2D fj_MainTex; 

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

void main() 
{
    vec2 tex = v_Texture0; 
    float nAmt = 0.001; 
    float freq = 200.0; 
    float pers = 0.5; 
    int it = 4; 
    tex.x += nAmt * FractalNoise(v_NormPosition, freq, pers, it, 0); 
    tex.y += nAmt * FractalNoise(v_NormPosition, freq, pers, it, 100); 

    vec3 N = normalize(v_ViewNormal); 
    vec3 E = normalize(-v_ViewPosition); // view direction
    vec3 L; // light direction
    vec3 H; // half

    float atten = 1.0; 

    if (fj_LightData.Type == LIGHT_DIRECTIONAL) 
    {
        L = -normalize(fj_LightData.Direction); 
    }
    else // point or spotlight
    {
        L = v_ViewPosition - fj_LightData.Position; 
        float len = length(L);
        L = -L/len; 
        len /= fj_LightData.Radius; 
        atten = 1.0 / (1.0 + len*len); 
    }

    H = normalize(E + normalize(L)); 

    float diffuseAmt = atten * max(dot(N, L), 0.0); 
    vec3 diffuse = diffuseAmt * fj_LightData.Color.rgb; 

    float specAmt = 0.0; 
    // if (diffuseAmt > 0.0) 
    if (v_Color.r < 0.2)
    {
        specAmt = (1.0 - v_Color.r / 0.2) * pow(max(dot(H, N), 0.0), 64.0); 
    }
    vec3 spec = 0.5 * atten * specAmt * vec3(1);// * fj_LightData.Color.rgb; 

    // f_Color = v_Color;
    f_Color.rgb = fj_Emissive.rgb + texture(fj_MainTex, tex).rgb * v_Color.rgb * (diffuse + fj_LightData.Ambient.rgb) + spec; 
    f_Color.a = v_Color.a * texture(fj_MainTex, tex).a; 
}

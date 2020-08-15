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

out vec4 f_Color; 

uniform LightData fj_LightData; 
uniform vec4 fj_Emissive; 

uniform vec3 u_Color = vec3(0.3, 0.5, 0.9); 
uniform float u_Strength = 1.0; 

float saturate(float x) 
{
    return clamp(x, 0.0, 1.0); 
}

void main() 
{
    vec3 N = normalize(v_ViewNormal); 
    vec3 E = normalize(-v_ViewPosition); // view direction
    vec3 L; // light direction
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

    vec3 twilight = vec3(0.9, 0.8, 0.0); 
    vec3 mainColor = u_Color; 

    float colorLerp = saturate(saturate(dot(-L, N) + 0.6) - saturate(dot(-L, N) - 0.8)); 

    // vec3 color = fj_LightData.Color.rgb * mix(mainColor, twilight, colorLerp); 
    vec3 color = mix(mainColor, twilight, colorLerp); 
    // vec3 color = mix(twilight, mainColor, 1-colorLerp); 

    float diffuseAmt = 1.0 * pow(max(dot(N, L), 0.0), 1); 

    float viewDotNormal = saturate(dot(E, N)); 

    float angleKeep = (1 - pow(viewDotNormal - 0.2, 1.0)) * 1;
    float edge = 1; 
    float limit = 0.3; 
    if (viewDotNormal < limit) 
    {
        edge = pow(viewDotNormal / limit, 2); 
    }

    f_Color.rgb = color; 
    f_Color.a = u_Strength * diffuseAmt * edge * angleKeep; 
}

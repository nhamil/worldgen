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

out vec4 f_Color; 

uniform LightData fj_LightData; 
uniform vec4 fj_Emissive; 

uniform sampler2D fj_MainTex; 

void main() 
{
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
    f_Color.rgb = fj_Emissive.rgb + texture(fj_MainTex, v_Texture0).rgb * v_Color.rgb * (diffuse + fj_LightData.Ambient.rgb) + spec; 
    f_Color.a = v_Color.a; 
}

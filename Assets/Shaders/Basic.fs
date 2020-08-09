#version 330

#define LIGHT_DIRECTIONAL 0 
#define LIGHT_POINT 1 
#define LIGHT_SPOT 2 

struct LightData 
{
    vec4 Color; 
    vec3 Position; 
    vec3 Direction; 
    int Type; 
};

in vec3 v_ViewPosition; 
in vec3 v_ViewNormal; 
in vec4 v_Color; 

out vec4 f_Color; 

uniform LightData fj_LightData; 
uniform vec4 fj_Emissive; 

void main() 
{
    vec3 N = normalize(v_ViewNormal); 
    vec3 E = vec3(0.0, 0.0, 1.0); // view direction
    vec3 L; // light direction
    vec3 H; // half

    if (fj_LightData.Type == LIGHT_DIRECTIONAL) 
    {
        L = -normalize(fj_LightData.Direction); 
    }
    else // point or spotlight
    {
        L = -normalize(v_ViewPosition - fj_LightData.Position); 
    }

    H = normalize(N + L); 

    float diffuseAmt = max(dot(N, L), 0.0); 
    vec3 diffuse = diffuseAmt * fj_LightData.Color.rgb; 

    float specAmt = 0.0; 
    // if (diffuseAmt > 0.0) 
    // {
    //     specAmt = pow(max(dot(H, E), 0.0), 32.0); 
    // }
    vec3 spec = specAmt * fj_LightData.Color.rgb; 

    vec3 lighting = diffuse + spec; 

    // f_Color = v_Color;
    f_Color.rgb = fj_Emissive.rgb + v_Color.rgb * lighting; 
    f_Color.a = v_Color.a; 
}

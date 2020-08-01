#version 330

in vec3 v_Normal; 
in vec4 v_Color; 

out vec4 f_Color; 

uniform vec3 u_LightDirection = vec3(1, 0, 0); 

void main() 
{
    vec3 normal = normalize(v_Normal); 
    float lightAmt = clamp(dot(u_LightDirection, normal), 0, 1); 
    f_Color.rgb = v_Color.rgb * lightAmt; 
    f_Color.a = 1.0; 
}

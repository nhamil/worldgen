#version 330 

in vec2 v_Texture0; 

out vec4 f_Color; 

uniform sampler2D u_Texture; 
uniform float u_Threshold = 0.9; 

float Brightness(vec3 color) 
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722)); 
}

vec3 Threshold(vec3 color) 
{
    return Brightness(color) > u_Threshold ? color : vec3(0.0); 
}

void main() 
{
    f_Color.rgb = Threshold(texture(u_Texture, v_Texture0).rgb); 
    f_Color.a = 1.0; 
}
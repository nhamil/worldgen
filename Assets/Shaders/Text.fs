#version 330 
in vec4 v_Color; 
in vec2 v_Texture; 

out vec4 f_Color; 

uniform sampler2D u_Texture; 

void main() 
{
    f_Color = v_Color; 
    f_Color.a *= texture(u_Texture, v_Texture).r; 
}
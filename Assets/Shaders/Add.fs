#version 330 

in vec2 v_Texture0; 

out vec4 f_Color; 

uniform sampler2D u_TextureBase; 
uniform sampler2D u_TextureAdd; 

void main() 
{
    f_Color.rgb = texture(u_TextureBase, v_Texture0).rgb + texture(u_TextureAdd, v_Texture0).rgb; 
    f_Color.a = 1.0; 
}
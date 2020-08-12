#version 330 

in vec2 v_Texture0; 

out vec4 f_Color; 

uniform sampler2D u_Texture; 

void main() 
{
    vec4 tex = texture(u_Texture, v_Texture0); 
    f_Color = vec4(tex.rgb, 1.0); 
}
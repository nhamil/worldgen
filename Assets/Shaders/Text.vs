#version 330 
in vec3 a_Position; 
in vec4 a_Color0; 
in vec2 a_Texture0; 

out vec4 v_Color; 
out vec2 v_Texture; 

uniform mat4 u_Matrix; 

void main() 
{
    v_Color = a_Color0; 
    v_Texture = a_Texture0; 
    gl_Position = u_Matrix * vec4(a_Position, 1.0); 
}
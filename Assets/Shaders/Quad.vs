#version 330 

in vec3 a_Position; 
in vec2 a_Texture0; 

out vec2 v_Texture0; 

void main() 
{
    v_Texture0 = a_Texture0; 
    gl_Position = vec4(a_Position.x, a_Position.y, a_Position.z, 1.0); 
}
#version 330 

in vec3 a_Position; 
in vec3 a_Normal; 
in vec4 a_Color0; 

out vec3 v_Normal; 
out vec4 v_Color; 

uniform mat4 u_Model; 

void main() 
{
    v_Color = a_Color0; 
    v_Normal = normalize(a_Normal); 
    gl_Position = u_Model * vec4(a_Position, 1.0); 
}

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

in vec3 a_Position; 
in vec3 a_Normal; 

out vec3 v_ViewPosition; 
out vec3 v_ViewNormal; 

uniform mat4 fj_Projection; 
uniform mat4 fj_ModelView; 
uniform mat3 fj_NormalMatrix; 

uniform float u_Radius = 1.06; 

void main() 
{
    v_ViewNormal = fj_NormalMatrix * a_Normal; 

    vec4 pos = fj_ModelView * vec4(a_Position * u_Radius, 1.0); 

    v_ViewPosition = pos.xyz; 
    gl_Position = fj_Projection * pos; 
}

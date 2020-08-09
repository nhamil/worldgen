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

in vec3 a_Position; 
in vec3 a_Normal; 
in vec4 a_Color0; 

out vec3 v_ViewPosition; 
out vec3 v_ViewNormal; 
out vec4 v_Color; 

uniform mat4 fj_Projection; 
uniform mat4 fj_ModelView; 
uniform mat3 fj_NormalMatrix; 

void main() 
{
    v_ViewNormal = fj_NormalMatrix * a_Normal; 
    v_Color = a_Color0; 

    vec4 pos = fj_ModelView * vec4(a_Position, 1.0); 

    v_ViewPosition = pos.xyz; 
    gl_Position = fj_Projection * pos; 
}

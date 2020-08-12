#version 330 

in vec2 v_Texture0; 

out vec4 f_Color; 

uniform sampler2D u_Texture; 
uniform int u_Horizontal = 1; 
// uniform float u_Weight[5] = float[] (1.0, 1.0, 1.0, 1.0, 1.0);
uniform float u_Weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

uniform float u_Threshold = 0.9; 

void main() 
{
    vec2 texOffset = 1.0 / textureSize(u_Texture, 0);
    vec4 tex = texture(u_Texture, v_Texture0); 

    vec3 result = tex.rgb * u_Weight[0]; 

    if (u_Horizontal == 1) 
    {
        for (int i = 1; i < 5; i++) 
        {
            result += texture(u_Texture, v_Texture0 + vec2(
                texOffset.x * i, 0.0
            )).rgb * u_Weight[i]; 

            result += texture(u_Texture, v_Texture0 - vec2(
                texOffset.x * i, 0.0
            )).rgb * u_Weight[i]; 
        }
    }
    else 
    {
        for (int i = 1; i < 5; i++) 
        {
            result += texture(u_Texture, v_Texture0 + vec2(
                0.0, texOffset.y * i
            )).rgb * u_Weight[i]; 

            result += texture(u_Texture, v_Texture0 - vec2(
                0.0, texOffset.y * i
            )).rgb * u_Weight[i]; 
        }
    }
    
    f_Color.rgb = result; 
    f_Color.a = 1.0; 
}
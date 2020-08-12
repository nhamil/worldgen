#version 330 

in vec2 v_Texture0; 

out vec4 f_Color; 

uniform sampler2D u_Texture; 
uniform int u_Horizontal = 1; 
// uniform float u_Weight[5] = float[] (1.0, 1.0, 1.0, 1.0, 1.0);
uniform float u_Weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

uniform float u_Threshold = 0.9; 
uniform int u_Strength = 5; 

// from https://stackoverflow.com/questions/8166384/how-to-get-a-glow-shader-effect-in-opengl-es-2-0
float CalcGauss( float x, float sigma ) 
{
    float coeff = 1.0 / (2.0 * 3.14157 * sigma);
    float expon = -(x*x) / (2.0 * sigma);
    return (coeff*exp(expon));
}

void main() 
{
    vec2 texOffset = 1.0 / textureSize(u_Texture, 0);
    vec4 tex = texture(u_Texture, v_Texture0); 

    float sigma = 1.0 / u_Strength; 

    float invStrength = 1.0 / u_Strength; 

    vec4 result = tex * CalcGauss(0, sigma); 

    if (u_Horizontal == 1) 
    {
        for (int i = 1; i < u_Strength; i++) 
        {
            float weight = CalcGauss(float(i) * invStrength, sigma); 

            result += vec4(texture(u_Texture, v_Texture0 + vec2(
                texOffset.x * i, 0.0
            )).rgb * weight, weight); 

            result += vec4(texture(u_Texture, v_Texture0 - vec2(
                texOffset.x * i, 0.0
            )).rgb * weight, weight); 
        }
    }
    else 
    {
        for (int i = 1; i < u_Strength; i++) 
        {
            float weight = CalcGauss(float(i) * invStrength, sigma); 

            result += vec4(texture(u_Texture, v_Texture0 + vec2(
                0.0, texOffset.y * i
            )).rgb * weight, weight); 

            result += vec4(texture(u_Texture, v_Texture0 - vec2(
                0.0, texOffset.y * i
            )).rgb * weight, weight); 
        }
    }
    
    f_Color = vec4(result.rgb / result.a, 1.0); 
}
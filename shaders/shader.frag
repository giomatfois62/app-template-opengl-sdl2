#version 330 core

in vec3 vColor;
in vec3 vNormal;
in vec2 vTexCoords;
//in vec2 vSol;
in float vSol;

uniform sampler2D texture_diffuse1;

uniform float vMin;
uniform float vMax;

out vec4 FragColor;

float colormapRed(float x) {
    if (x < 0.7) {
        return 4.0 * x - 1.5;
    } else {
        return -4.0 * x + 4.5;
    }
}

float colormapGreen(float x) {
    if (x < 0.5) {
        return 4.0 * x - 0.5;
    } else {
        return -4.0 * x + 3.5;
    }
}

float colormapBlue(float x) {
    if (x < 0.3) {
        return 4.0 * x + 0.5;
    } else {
        return -4.0 * x + 2.5;
    }
}

float clamp(float x, float mi, float ma)
{
    return max(min(x, ma), mi);
}

  
void main()
{
        vec3 lightDir = vec3(0.0, 0.0, 1.0);
        float intensity = max(dot(vNormal, lightDir), 0.0);

        //float val = (vSol.y - vMin) / (vMax - vMin);
        float val = (vSol - vMin) / (vMax - vMin);

        float r = clamp(colormapRed(val), 0.0, 1.0);
        float g = clamp(colormapGreen(val), 0.0, 1.0);
        float b = clamp(colormapBlue(val), 0.0, 1.0);

        //FragColor = vec4(intensity * vColor * texture(texture_diffuse1, vTexCoords).xyz, 1.0);
        FragColor = vec4(r, g, b, 1.0);
}

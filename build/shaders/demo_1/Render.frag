#version 440
//inputs
//output
layout(location = 0) out vec4 outputColor; 

uniform sampler3D noiseTexture;

in vec3 gFragPos;

void main()
{
    // float noise = texture(noiseTexture, gFragPos).x;
    outputColor = texture(noiseTexture, gFragPos);
    // outputColor = vec4(gFragPos,1);
    // outputColor = vec4(gFragPos.xyz, 1.0);
    // outputColor = vec4(gFragPos, 1.0);
}
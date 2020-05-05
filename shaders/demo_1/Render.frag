#version 440
//inputs
//output
layout(location = 0) out vec4 outputColor; 

uniform sampler3D[] noiseTexture;

in vec3 gFragPos;
uniform vec3 blockPosition;
uniform float time;
void main()
{
    vec3 ws = gFragPos;
    vec3 wp = (floor(gFragPos * 33.0) * (1.0/32.0)) + blockPosition;
    // vec3 wp = (floor(gFragPos * 33.0) * (1.0/30.0)) + blockPosition - (1.0/30.0);
    float density = -wp.y;
    
    density += (texture(noiseTexture[0], wp*0.5).x);
    // density += (texture(noiseTexture[0], wp*0.5).x + 1) * 2;
    // density += (texture(noiseTexture[1], wp*0.5).x) * 0.4;
    // density += (texture(noiseTexture[2], wp*0.25).x) * 0.8;
    // density += (texture(noiseTexture[3], wp*0.1).x ) * 3;
    // density += (texture(noiseTexture[1], wp*0.2).x + 1)/2;
    // vec3 wFragpos = round( 40* ws);
    // ivec3 ts = ivec3(wFragpos);
    // ts %=32;
    
    // float density = -(ws.y - 0.1);
    
    // density += texelFetch(noiseTexture[0], ts, 0).x;
    // ivec3 _ts = ivec3((gFragPos*33+blockPosition*32)*0.5) % 32;
    
    // _ts = (ts * 2) % 32;
    // density += texelFetch(noiseTexture[1], _ts, 0).x * 0.1;
    
    // _ts = (ts * 1) % 32;
    // density += texelFetch(noiseTexture[2], _ts, 0).x * 0.2;
    
    // _ts = (ts /2) % 32;
    // density += texelFetch(noiseTexture[2], _ts, 0).x * 0.4;


    // density += texture(noiseTexture[0], ws*0.5).x*2.0;
    // density += texture(noiseTexture[0], ws*1.00).x*1.00;
    // density += texture(noiseTexture[4], ws*0.24).x*4.00;
    // density += texture(noiseTexture[5], ws*0.13).x*8.00;
    // density += texture(noiseTexture[6], ws*0.05).x*16.00;
    // density += texture(noiseTexture[7], ws*1.01).x*1.00;
    // density += texture(noiseTexture[8], ws*1.01).x*1.00;
    

    outputColor = vec4(density, 0, 0, 0);
    // float noise = texture(noiseTexture, gFragPos).x;
    // outputColor = texture(noiseTexture[3], gFragPos);
    // outputColor = vec4(gFragPos,1);
    // outputColor = vec4(gFragPos.xyz, 1.0);
    // outputColor = vec4(gFragPos, 1.0);
}
#version 440
 //attribs
layout(location = 0) in vec3 position;

out vec3 vfragPos;
out int instanceId;
//main
void main()
{
    instanceId = gl_InstanceID;
    vfragPos = (position.xyz + 1.0)/2.0;
    // vfragPos = position.xyz;
    gl_Position = vec4(position.xyz,1.0);
}
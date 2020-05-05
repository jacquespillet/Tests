#version 440 core
layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

in int instanceId[];
in vec3 vfragPos[];
out vec3 gFragPos;

void main()
{
    for(int i = 0; i < 3; i++) { 
        gFragPos = vfragPos[i];
        gFragPos.z = float(instanceId[i])/33.0f;


        gl_Position = gl_in[i].gl_Position;
        gl_Layer = instanceId[i];
        EmitVertex();
    }
    EndPrimitive();
}  
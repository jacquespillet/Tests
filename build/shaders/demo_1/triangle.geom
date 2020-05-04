#version 440 core

layout (points) in;
layout (triangle_strip, max_vertices = 15) out;



in vec3[] posValue;
out vec3 outPosition;
out vec3 outNormal;

uniform sampler3D densityFunction;
uniform isampler2D triTable;
uniform vec3 blockPosition;
uniform float blockSize;

const float d = (1.0f/32.0f);
const vec3 vs[8]={vec3(0,0,0),vec3(0,1,0),vec3(1,1,0),vec3(1,0,0),vec3(0,0,1),vec3(0,1,1),vec3(1,1,1),vec3(1,0,1)};
const int edgeVertices[12][2] = {{0, 1},{1, 2},{2, 3},{3, 0},{4, 5},{5, 6},{6, 7},{7, 4},{0, 4},{1, 5},{2, 6},{3, 7}};
float fs[8]={0,0,0,0,0,0,0,0};

int f(vec3 uvw, out float value) {
    value = texture(densityFunction, uvw).x;
    if(value>0) return 1;
    else return 0;
}

vec3 ProcessEdge(vec3 v, int edgeNum, float p0, float p1, out vec3 grad) {                    
    int _vInx0 = edgeVertices[edgeNum][0]; //Index of the first vertex in the vs array
    int _vInx1 = edgeVertices[edgeNum][1]; //Index of the second vertex in the vs array

    vec3 _v0 = vs[_vInx0];
    vec3 _v1 = vs[_vInx1];
    float mu = (-p0) / (p1 - p0);
    vec3 outPos = _v0 + mu * (_v1-_v0);
    
    vec3 dx = vec3(1,0,0) * d;
    vec3 dy = vec3(0,1,0) * d;
    vec3 dz = vec3(0,0,1) * d;
    
    {
        float gp, gn;
        f(v + outPos*d + dx, gp); f(v + outPos*d - dx, gn);
        grad.x = gn - gp;
    }
    {
        float gp, gn;
        f(v + outPos*d + dy, gp); f(v + outPos*d - dy, gn);
        grad.y = gn - gp;
    }
    {
        float gp, gn;
        f(v + outPos*d + dz, gp); f(v + outPos*d - dz, gn);
        grad.z = gn - gp;
    }

    grad =-normalize(grad); 

    return outPos;
}


int GetTriTable(int x, int y) {
    float u =float(x)/16.0f;
    float v = 1.0 - float(y)/255.0f;
    return texture(triTable, vec2(u, v)).x;
}

void main() {    
    vec4 position = gl_in[0].gl_Position;
    vec3 normalizedPos = d * posValue[0];
    
    int mask=0;
    mask = 
        f(normalizedPos + d * vs[0], fs[0]) << 0 | 
        f(normalizedPos + d * vs[1], fs[1]) << 1 | 
        f(normalizedPos + d * vs[2], fs[2]) << 2 | 
        f(normalizedPos + d * vs[3], fs[3]) << 3 | 
        f(normalizedPos + d * vs[4], fs[4]) << 4 | 
        f(normalizedPos + d * vs[5], fs[5]) << 5 | 
        f(normalizedPos + d * vs[6], fs[6]) << 6 | 
        f(normalizedPos + d * vs[7], fs[7]) << 7;

    for(int i=0; i<16; i+=3) {
        // if(e0 < 0) break;
        vec3 out0;
        vec3 out1;
        vec3 out2;
        vec3 outN0;
        vec3 outN1;
        vec3 outN2;
        
        {
            int e0 = GetTriTable(i, mask); //Returns the index of the edge where the vertex will be
            if(e0<0)break;
            int _vInx0 = edgeVertices[e0][0]; //Index of the first vertex in the vs array
            int _vInx1 = edgeVertices[e0][1]; //Index of the second vertex in the vs array
            
            float p0 = fs[_vInx0];
            float p1 = fs[_vInx1];
            vec3 v0 = ProcessEdge(normalizedPos, e0, p0, p1, outN0);
            out0 = v0;
        }

        {
            int e1 = GetTriTable(i+1, mask); //Returns the index of the edge where the vertex will be
            
            int _vInx0 = edgeVertices[e1][0]; //Index of the first vertex in the vs array
            int _vInx1 = edgeVertices[e1][1]; //Index of the second vertex in the vs array
            
            float p0 = fs[_vInx0];
            float p1 = fs[_vInx1];
            vec3 v0 = ProcessEdge(normalizedPos, e1, p0, p1, outN1);
            out1 = v0;
        }

        {
            int e2 = GetTriTable(i+2, mask); //Returns the index of the edge where the vertex will be
            int _vInx0 = edgeVertices[e2][0]; //Index of the first vertex in the vs array
            int _vInx1 = edgeVertices[e2][1]; //Index of the second vertex in the vs array
            float p0 = fs[_vInx0];
            float p1 = fs[_vInx1];
            vec3 v0 = ProcessEdge(normalizedPos, e2, p0, p1, outN2);
            out2 = v0;
        }

        out0 = (((posValue[0] + out0) /32.0f)*blockSize) + blockPosition;
        out1 = (((posValue[0] + out1) /32.0f)*blockSize) + blockPosition;
        out2 = (((posValue[0] + out2) /32.0f)*blockSize) + blockPosition;

        outPosition = out0;    // 1:bottom-left;
        outNormal=outN0;
        EmitVertex();   
        
        outPosition = out1;
        outNormal=outN1;
        EmitVertex();
        
        outPosition = out2;
        outNormal=outN2;
        EmitVertex();
        EndPrimitive();
    }

    // if(texture(densityFunction, normalizedPos).x > 0) {
    //     outPosition =posValue[0].xyz + vec3(-1,0,0);    // 1:bottom-left;
    //     outNormal=vec3(0,0,0);
    //     EmitVertex();   
        
    //     outPosition =posValue[0].xyz + vec3(1,0,0);
    //     outNormal=vec3(0,0,0);
    //     EmitVertex();
        
    //     outPosition =posValue[0].xyz + vec3(0,1,0);
    //     outNormal=vec3(0,0,0);
    //     EmitVertex();
    //     EndPrimitive();
    // }
}  
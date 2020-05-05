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
float numVoxels=32.0f;

const vec3 vs[8]={
                  vec3(-1 , -1, -1),
                  vec3( 1,  -1, -1),
                  vec3( 1,  -1, 1),
                  vec3(-1,  -1, 1),

                  vec3(-1 ,  1, -1),
                  vec3( 1,   1, -1),
                  vec3( 1,   1,  1),
                  vec3(-1,   1,  1)
                  };
                  
const int edgeVertices[12][2] = {{0, 1},{1, 2},{2, 3},{3, 0},{4, 5},{5, 6},{6, 7},{7, 4},{0, 4},{1, 5},{2, 6},{3, 7}};
float fs[8]={0,0,0,0,0,0,0,0};

int f(vec3 uvw, out float value) {
    // ivec3 iuvw = ivec3(uvw*numVoxels);
    // // iuvw %= 33;
    // value = texelFetch(densityFunction, iuvw + ivec3(1,1,1), 0).x;
    
    value = texture(densityFunction, uvw).x;
    // value -=0.5;
    // value= (value0.5;
    if(value>0) return 1;
    else return 0;
}

vec3 ProcessEdge(vec3 v, int edgeNum, float p0, float p1, out vec3 grad) {       
    float voxelSize = 1/numVoxels;
    float halfVoxelSize = voxelSize/2.0f;

    int vertexInx0 = edgeVertices[edgeNum][0]; //Index of the first vertex in the vs array
    int vertexInx1 = edgeVertices[edgeNum][1]; //Index of the second vertex in the vs array

    //Edge vertices
    vec3 _v0 = v +  halfVoxelSize * (vs[vertexInx0]);
    vec3 _v1 = v +  halfVoxelSize * (vs[vertexInx1]);

    float mu = (-p0) / (p1 - p0);
    vec3 outPos = _v0 + mu * (_v1-_v0);
    // outPos = 0.5 * (_v0 + _v1);
    
    vec3 dx = vec3(1,0,0) * voxelSize;
    vec3 dy = vec3(0,1,0) * voxelSize;
    vec3 dz = vec3(0,0,1) * voxelSize;
        
    {
        float gp, gn;
        vec3 next = outPos + dx;
        vec3 prev = outPos - dx;
        f(next, gp); f(prev, gn);
        grad.x = gn - gp;
    }
    {
        float gp, gn;
        vec3 next = outPos + dy;
        vec3 prev = outPos - dy;
        f(next, gp); f(prev, gn);
        grad.y = gn - gp;    
    }
    {
        float gp, gn;
        vec3 next = outPos + dz; 
        vec3 prev = outPos - dz;
        f(next, gp); f(prev, gn);
        grad.z = gn - gp;
    }

    grad =normalize(grad); 

    return outPos;
}

int GetTriTable(int x, int y) {
    return texelFetch(triTable, ivec2(x, y), 0).x;
}

int ProcessTriangle(int offset, vec3 v, int mask, out vec3 position, out vec3 normal) {
    int edge = GetTriTable(offset, mask); //Returns the index of the edge where the vertex will be
    if(edge<0) return 0;
    
    int vertexInx0 = edgeVertices[edge][0]; //Index of the first vertex in the vs array
    int vertexInx1 = edgeVertices[edge][1]; //Index of the second vertex in the vs array

    //Density value at endpoints of the edge
    float p0 = fs[vertexInx0];
    float p1 = fs[vertexInx1];
    
    position = ProcessEdge(v, edge, p0, p1, normal);

    return 1;    
}

void main() { 
    float voxelSize = 1/numVoxels;
    float halfVoxelSize = voxelSize/2.0f;
    vec3 voxelCenter = vec3(halfVoxelSize, halfVoxelSize, halfVoxelSize);
    
    //Normalized space position for sampling texture : [0, 1[, [0, 1[, [0, 1[
    //The normalized position is positioned at the center of the voxel.
    vec3 normalizedPos = voxelCenter + voxelSize * posValue[0];




    
    //Sampling the function and check what corner of the cube is inside.
    int mask=0;
    for(int i=0; i<8; i++) {
        mask |= f(normalizedPos + halfVoxelSize * (vs[i]), fs[i]) << i;
    }

    //For each polygon created
    for(int i=0; i<16; i+=3) {
        vec3 out0; vec3 out1; vec3 out2; vec3 outN0; vec3 outN1; vec3 outN2;

        int t=ProcessTriangle(i, normalizedPos, mask, out0, outN0);
        if(t==0) break;
        ProcessTriangle(i+1, normalizedPos, mask, out1, outN1);
        ProcessTriangle(i+2, normalizedPos, mask, out2, outN2);

        //Goes from local block space to world space
        out0 = out0 * blockSize + blockPosition;
        out1 = out1 * blockSize + blockPosition;
        out2 = out2 * blockSize + blockPosition;

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
    
}

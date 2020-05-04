#include "Demo_1.hpp"
#include "GL_Helpers/Util.hpp"

#include "Noise.hpp"
#include "Functions.hpp"
#include <GL/glew.h>
#include "GL_Helpers/GL_Shader.hpp"

Demo_1::Demo_1() {
    vs[0] = glm::vec3(0,0,0);
    vs[1] = glm::vec3(0,1,0);
    vs[2] = glm::vec3(1,1,0);
    vs[3] = glm::vec3(1,0,0);
    vs[4] = glm::vec3(0,0,1);
    vs[5] = glm::vec3(0,1,1);
    vs[6] = glm::vec3(1,1,1);
    vs[7] = glm::vec3(1,0,1);


    noiseTexture.reserve(texSize*texSize*texSize);
    for (int x = 0; x < texSize; x++)
    {
        for (int y = 0; y < texSize; y++)
        {
            for (int z = 0; z < texSize; z++)
            {
                glm::vec3 p = glm::vec3 (x, y, z) / (float)texSize;
                // float n = GetPerlinNoise3D(p.x, p.y, p.z, 1);
                // float n = FbM(p.x, p.y, p.z, 1);
                // float n = perlinfbm(p, 1, 9);
                float n0 = gradientNoise(p, 1);
                noiseTexture.push_back(n0);
                
                float n1 = gradientNoise(p+glm::vec3(1), 1);
                float n2 = gradientNoise(p+glm::vec3(2), 1);
                float n3 = gradientNoise(p+glm::vec3(3), 1);
                float n4 = gradientNoise(p+glm::vec3(4), 1);
                float n5 = gradientNoise(p+glm::vec3(5), 1);
                float n6 = gradientNoise(p+glm::vec3(6), 1);
                float n7 = gradientNoise(p+glm::vec3(7), 1);
                float n8 = gradientNoise(p+glm::vec3(8), 1);
            }
        }
    }


    //PointsBuffer
    for (float x = 0; x < numVoxels; x++)
    {
        for (float y = 0; y < numVoxels; y++)
        {
            for (float z = 0; z < numVoxels; z++)
            {
                gridBuffer.push_back(glm::vec3(x, y,z));
            }
        }
    }    
}

float Demo_1::SampleNoise(glm::vec3 uvw) {
    glm::vec3 nuvw = glm::floor(texSize * uvw);
    int x = (int)nuvw.x % texSize;
    int y = (int)nuvw.y % texSize;
    int z = (int)nuvw.z % texSize;
    int inx = (int)x + texSize * ((int)y + texSize * (int)z);
    if(inx >=0 && inx < noiseTexture.size()) {
        return noiseTexture[inx];
    } else {
        return 0;
    }
}

int Demo_1::f(glm::vec3 p, float *result) {
    glm::vec3 np = p/numVoxels;
    float density = -np.y;
    density += SampleNoise(np);
    *result = density;

    
    if(*result>0) return 1;
    else return 0;
}

GL_Mesh::Vertex Demo_1::ProcessEdge(glm::vec3 v,int edgeNum, float p0, float p1) {                    
    glm::vec3 out;
    glm::vec3 outN;
    int _vInx0 = edgeVertices[edgeNum][0]; //Index of the first vertex in the vs array
    int _vInx1 = edgeVertices[edgeNum][1]; //Index of the second vertex in the vs array

    glm::vec3 _v0 = vs[_vInx0];
    glm::vec3 _v1 = vs[_vInx1];
    float mu = (-p0) / (p1 - p0);
    out = _v0 + mu * (_v1-_v0);
    out = (_v0 + _v1) / 2.0f;

    glm::vec3 grad;
    {
        float gp, gn;
        f(v + out+dx, &gp); f(v + out-dx, &gn);
        grad.x = gn - gp;
    }
    {
        float gp, gn;
        f(v + out+dy, &gp); f(v + out-dy, &gn);
        grad.y = gn - gp;
    }
    {
        float gp, gn;
        f(v + out+dz, &gp); f(v + out-dz, &gn);
        grad.z = gn - gp;
    }

    outN = glm::normalize(grad); 
    
    return {
        out,
        outN,
        glm::vec2(0),
        glm::vec3(0)  
    };
}


void Demo_1::GenerateBlockCPU(glm::vec3 position, std::vector<unsigned int>& triangles, std::vector<GL_Mesh::Vertex>& vertices) {
    for (float x = 0; x < numVoxels; x++)
    {
        for (float y = 0; y < numVoxels; y++)
        {
            for (float z = 0; z < numVoxels; z++)
            {
                glm::vec3 v = glm::vec3(x, y, z);
                //                  ^0-32            ^0-blockSize      ^position-position+blockSize
                // float f0, f1, f2, f3, f4, f5, f6, f7;
                float fs[8]={};
                int mask=0;
                mask = 
                    f(position + v + vs[0], &fs[0]) << 0 | 
                    f(position + v + vs[1], &fs[1]) << 1 | 
                    f(position + v + vs[2], &fs[2]) << 2 | 
                    f(position + v + vs[3], &fs[3]) << 3 | 
                    f(position + v + vs[4], &fs[4]) << 4 | 
                    f(position + v + vs[5], &fs[5]) << 5 | 
                    f(position + v + vs[6], &fs[6]) << 6 | 
                    f(position + v + vs[7], &fs[7]) << 7;

                // int numEdges = edgeTable[mask];
                // int tris = triTable[mask][0];
                // std::cout << mask << std::endl;
                for(int i=0; i<16; i+=3) {
                    if(triTable[mask][i]<0) break;
                    
                    glm::vec3 out0, out1, out2;
                    glm::vec3 outN0, outN1, outN2;

                    {
                        int e0 = triTable[mask][i]; //Returns the index of the edge where the vertex will be
                        
                        int _vInx0 = edgeVertices[e0][0]; //Index of the first vertex in the vs array
                        int _vInx1 = edgeVertices[e0][1]; //Index of the second vertex in the vs array
                        
                        float p0 = fs[_vInx0];
                        float p1 = fs[_vInx1];
                        GL_Mesh::Vertex v0 = ProcessEdge(v, e0, p0, p1);
                        out0 = v0.position;
                        outN0 = v0.normal;
                    }

                    {
                        int e1 = triTable[mask][i+1]; //Returns the index of the edge where the vertex will be
                        
                        int _vInx0 = edgeVertices[e1][0]; //Index of the first vertex in the vs array
                        int _vInx1 = edgeVertices[e1][1]; //Index of the second vertex in the vs array
                        
                        float p0 = fs[_vInx0];
                        float p1 = fs[_vInx1];
                        GL_Mesh::Vertex v1 = ProcessEdge(v, e1, p0, p1);
                        out1 = v1.position;
                        outN1 = v1.normal;
                    }

                    {
                        int e2 = triTable[mask][i+2]; //Returns the index of the edge where the vertex will be
                        
                        int _vInx0 = edgeVertices[e2][0]; //Index of the first vertex in the vs array
                        int _vInx1 = edgeVertices[e2][1]; //Index of the second vertex in the vs array
                        
                        float p0 = fs[_vInx0];
                        float p1 = fs[_vInx1];
                        GL_Mesh::Vertex v2 = ProcessEdge(v, e2, p0, p1);
                        out2 = v2.position;
                        outN2 = v2.normal;
                    }
                    
                    vertices.push_back({((out0 + v) / numVoxels-glm::vec3(0.5)) * blockSize,
                        outN0,
                        glm::vec2(0),
                        glm::vec3(0)     
                    });
                    vertices.push_back({((out1 + v) / numVoxels-glm::vec3(0.5)) * blockSize,
                        outN1,
                        glm::vec2(0),
                        glm::vec3(0)
                    });
                    vertices.push_back({((out2 + v) / numVoxels-glm::vec3(0.5)) * blockSize,
                        outN2,
                        glm::vec2(0),
                        glm::vec3(0)
                    });

                    int inx= (int)triangles.size();
                    triangles.push_back(inx+0);
                    triangles.push_back(inx+1);
                    triangles.push_back(inx+2);
                }
            }
        }
    }
    
}

void Demo_1::InitPointsBuffer() {

    glGenVertexArrays(1, &gridVAO);
    glBindVertexArray(gridVAO);
    
        glGenBuffers(1, &gridVBO);
        glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
        glBufferData(GL_ARRAY_BUFFER, gridBuffer.size() * sizeof(glm::vec3), (uint8_t*)&gridBuffer[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

        glGenBuffers(1, &outTrianglesVBO);
        glBindBuffer(GL_ARRAY_BUFFER, outTrianglesVBO);
        glBufferData(GL_ARRAY_BUFFER, gridBuffer.size() * sizeof(float) * 3, nullptr, GL_STATIC_READ);        
    
    glBindVertexArray(0);
}

void Demo_1::Load() {
    // numVoxels = glm::ivec3(32, 32, 32);
    // std::vector<unsigned int> triangles;
    // // std::vector<glm::vec3> positions;
    // std::vector<GL_Mesh::Vertex> vertices;
    // GenerateBlockCPU(glm::vec3(0,-0.5,0), triangles, vertices);
    // mesh = GL_Mesh(vertices, triangles);
    tex3D = CreateTexture3D(noiseTexture, texSize);
    triTableTexture = CreateIntTexture((uint8_t*)&triTable, 16, 256);

    InitPointsBuffer();
    triangleShader = CreateShader("shaders/demo_1/triangle.vert", "shaders/demo_1/triangle.geom", "shaders/demo_1/triangle.frag");
    
    mesh2 = MeshFromFile("resources/models/suzanne/Suzanne_lp.obj");
    cam = GL_Camera();

    fbo = CreateFramebuffer3D(32, &_tex3D);
    _fbo = CreateFramebuffer3D(32, &__tex3D);
    renderShader = new GL_Shader("shaders/demo_1/Render.vert", "shaders/demo_1/Render.geom","shaders/demo_1/Render.frag");
    // renderShader = new GL_Shader("shaders/demo_1/Render.vert", "","shaders/demo_1/Render.frag");
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);
    {
        glm::vec3 quad[6] = {
            glm::vec3(1, 1, 0),
            glm::vec3(-1, 1, 0),
            glm::vec3(-1, -1, 0),
            
            glm::vec3(1, -1, 0),
            glm::vec3(1, 1, 0),
            glm::vec3(-1, -1, 0),
        };

        glGenBuffers(1, &quadVBO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec3), &quad, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }    
    glBindVertexArray(0);



    // Create points VAO for COMPUTE ONLY
    //Will generate triangles for a single block in a geometry shader.
    //Gets as input a 32x32x32 grid of vertices, will process each vertex as a cell
    //And do the marching cubes algo on each cell.
    //Will output btw 0/5 triangles for each cell.
    glGenVertexArrays(1, &gridVAO);
    glBindVertexArray(gridVAO);
    {        
        glGenBuffers(1, &gridVBO);
        glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
        glBufferData(GL_ARRAY_BUFFER, gridBuffer.size()*sizeof(glm::vec3), gridBuffer.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Create transform feedback buffer
        glGenQueries(1, &outTrianglesQuery);
        glGenBuffers(1, &outTrianglesVBO);
        glBindBuffer(GL_ARRAY_BUFFER, outTrianglesVBO);
        // glBufferData(GL_ARRAY_BUFFER, gridBuffer.size()*sizeof(Vertex) * 3, nullptr, GL_DYNAMIC_READ);
        glBufferData(GL_ARRAY_BUFFER, gridBuffer.size()*sizeof(Vertex) * 3, nullptr, GL_DYNAMIC_READ);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }    
    glBindVertexArray(0);

    //VAO for a single block.
    //has enough space for having 32x32x32x5 triangles in it
    //After each pass of geometry generation, we copy the content of the feedback buffer
    //into this buffer
    glGenVertexArrays(1, &blockVAO);
    glBindVertexArray(blockVAO);
    {        
        uint32_t maxSize = (uint32_t)gridBuffer.size() * 5 * 3; //Each point in the grid will generate btw 0 and 5 triangles (3 vert)
        glGenBuffers(1, &blockVBO);
        //bind buffers
        glBindBuffer(GL_ARRAY_BUFFER, blockVBO);
        //set vertex attributes
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void*)((uintptr_t)12));
        glBufferData(GL_ARRAY_BUFFER, maxSize * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);

}

void Demo_1::MouseMove(float x, float y) {
    cam.mouseMoveEvent(x, y);
}

void Demo_1::Render() {    
    // std::vector<float>d(32*32*32, 10);
    // __tex3D = CreateTexture3D(d, texSize);
    mesh2.Render(cam);
    

    //Density func pass
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        GLint m_viewport[4];
        glGetIntegerv( GL_VIEWPORT, m_viewport );
        glViewport(0, 0, 32, 32);
            glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                glUseProgram(renderShader->programShaderObject);
                renderShader->SetTexture3D("noiseTexture", tex3D, 0);
                glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 32);
            glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_viewport[2], m_viewport[3]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glFlush();
    glBindTexture(GL_TEXTURE_3D, 0);    



    //Transform feedback : Creates all the triangles
    //in a geometry shader. Compute pipeline, no rasterization done.
    glUseProgram(triangleShader->programShaderObject);
    triangleShader->SetTexture3D("densityFunction", _tex3D, 10);
    triangleShader->SetTexture2D("triTable", triTableTexture, 1);
    triangleShader->SetVector3("blockPosition", glm::vec3(0,0,0));
    triangleShader->SetFloat("blockSize", 2.0f);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glEnable(GL_RASTERIZER_DISCARD);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, outTrianglesVBO);
    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, outTrianglesQuery);
    glBeginTransformFeedback(GL_TRIANGLES);
        glDrawArrays(GL_POINTS, 0, (GLsizei)gridBuffer.size());
    glEndTransformFeedback();
    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
    glDisable(GL_RASTERIZER_DISCARD);
    glFlush();
    
    GLuint primitives;
    glGetQueryObjectuiv(outTrianglesQuery, GL_QUERY_RESULT, &primitives);
    //Copies all the resulting triangles into the triangle VAO
    GLint size = primitives * 3 * sizeof(Vertex);
    glBindBuffer           (GL_COPY_READ_BUFFER, outTrianglesVBO);
    glBindBuffer           (GL_COPY_WRITE_BUFFER, blockVBO);
    glBufferData           (GL_COPY_WRITE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glCopyBufferSubData    (GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);


    //Render the triangles
    glUseProgram(mesh2.shader.programShaderObject);
    glBindVertexArray(blockVAO);
    glBindBuffer(GL_ARRAY_BUFFER, blockVBO);
    glDrawArrays(GL_TRIANGLES, 0, size);
    glBindVertexArray(0);    
    glUseProgram(0);    
    glBindVertexArray(0);



}

void Demo_1::Unload() {
    mesh.Unload();
    triangleShader->Unload();
    delete triangleShader;
    renderShader->Unload();
    delete renderShader;
}

void Demo_1::LeftClickDown() {
    cam.mousePressEvent(0);
}

void Demo_1::LeftClickUp() {
    cam.mouseReleaseEvent(0);
}

void Demo_1::RightClickDown() {
    cam.mousePressEvent(1);
}

void Demo_1::RightClickUp() {
    cam.mouseReleaseEvent(1);
}

void Demo_1::Scroll(float offset) {
    cam.Scroll(offset);
}

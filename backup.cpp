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
                float n = perlinfbm(p, 1, 9);
                noiseTexture.push_back(n);
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
                pointBuffer.push_back(glm::vec3(x, y,z));
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

    glGenVertexArrays(1, &pointsVAO);
    glBindVertexArray(pointsVAO);
    
        glGenBuffers(1, &pointsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
        glBufferData(GL_ARRAY_BUFFER, pointBuffer.size() * sizeof(glm::vec3), (uint8_t*)&pointBuffer[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

        glGenBuffers(1, &outPointsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, outPointsVBO);
        glBufferData(GL_ARRAY_BUFFER, pointBuffer.size() * sizeof(float) * 3, nullptr, GL_STATIC_READ);        
    
    glBindVertexArray(0);
}

void Demo_1::Load() {
    // numVoxels = glm::ivec3(32, 32, 32);
    // std::vector<unsigned int> triangles;
    // // std::vector<glm::vec3> positions;
    // std::vector<GL_Mesh::Vertex> vertices;
    // GenerateBlockCPU(glm::vec3(0,-0.5,0), triangles, vertices);
    // mesh = GL_Mesh(vertices, triangles);
    
    GLuint tex3D = CreateTexture3D(noiseTexture, texSize);

    InitPointsBuffer();
    triangleShader = CreateShader("shaders/demo_1/triangle.vert", "shaders/demo_1/triangle.geom", "shaders/demo_1/triangle.frag");
    
    renderShader= new GL_Shader();

    // renderShader->fragSrc="shaders/demo_1/Render.frag";
    // renderShader->vertSrc="shaders/demo_1/Render.vert";
    std::ifstream t("shaders/demo_1/Render.vert");
    std::stringstream vertBuffer;
    vertBuffer << t.rdbuf();
    renderShader->vertSrc= vertBuffer.str();

    t = std::ifstream ("shaders/demo_1/Render.frag");
    std::stringstream fragBuffer;
    fragBuffer << t.rdbuf();
    renderShader->fragSrc= fragBuffer.str();
    renderShader->Compile();

    mesh2 = MeshFromFile("resources/models/suzanne/Suzanne_lp.obj");
    cam = GL_Camera();
    





    
    glUseProgram(triangleShader->programShaderObject);
    // Create VAO
    glGenVertexArrays(1, &pointsVAO);
    glBindVertexArray(pointsVAO);
    {        
        glGenBuffers(1, &pointsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
        glBufferData(GL_ARRAY_BUFFER, pointBuffer.size()*sizeof(glm::vec3), pointBuffer.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Create transform feedback buffer
        glGenBuffers(1, &outPointsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, outPointsVBO);
        glBufferData(GL_ARRAY_BUFFER, pointBuffer.size()*sizeof(glm::vec3) * 3, nullptr, GL_STATIC_READ);
    }    

    {
        std::vector<GL_Mesh::Vertex> vertices = {
            {glm::vec3(-2, 0, 0.1), glm::vec3(0), glm::vec2(0), glm::vec3(0)},
            {glm::vec3(0, 2, -0.1), glm::vec3(0), glm::vec2(0), glm::vec3(0)},
            {glm::vec3(2, 0, 0.1), glm::vec3(0), glm::vec2(0), glm::vec3(0)}
        };
        std::vector<unsigned int> triangles = {0, 1, 2};
        
        glGenBuffers(1, &trianglesVBO);
        //bind buffers
        glBindBuffer(GL_ARRAY_BUFFER, trianglesVBO);
        //set vertex attributes
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(GL_Mesh::Vertex), (void*)((uintptr_t)0));
        glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(GL_Mesh::Vertex), (void*)((uintptr_t)12));
        glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(GL_Mesh::Vertex), (void*)((uintptr_t)24));
        glVertexAttribPointer(3, 3, GL_FLOAT, true, sizeof(GL_Mesh::Vertex), (void*)((uintptr_t)32));

        // //copy data to buffers
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GL_Mesh::Vertex), (uint8_t*)&vertices[0], GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);        //Unbind VAO
    glBindVertexArray(0);
}

void Demo_1::MouseMove(float x, float y) {
    cam.mouseMoveEvent(x, y);
}

void Demo_1::Render() {
    // mesh.Render(cam);
    mesh2.Render(cam);

    glUseProgram(triangleShader->programShaderObject);

    glBindVertexArray(pointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    // // Perform feedback transform
    glEnable(GL_RASTERIZER_DISCARD);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, outPointsVBO);
    glBeginTransformFeedback(GL_TRIANGLES);
        glDrawArrays(GL_POINTS, 0, pointBuffer.size());
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);
    glFlush();
    


    // // glDrawArrays(GL_POINTS, 0, pointBuffer.size());

	// glUseProgram(renderShader->programShaderObject);
    // //bind VAO	
    // glBindBuffer(GL_ARRAY_BUFFER, trianglesVBO);
    // glDrawArrays(GL_TRIANGLES, 0, 1);


	glUseProgram(mesh2.shader.programShaderObject);

    mesh2.shader.SetMat4("projectionMatrix", cam.GetProjectionMatrix());
    mesh2.shader.SetMat4("viewMatrix", cam.GetViewMatrix());
    mesh2.shader.SetMat4("modelMatrix", glm::mat4(1));
    
    //bind VAO	
    glBindBuffer(GL_ARRAY_BUFFER, trianglesVBO);
    // glDrawElements(GL_TRIANGLES, (GLsizei)3, GL_UNSIGNED_INT, (void*)0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
    //unbind VAO
	glBindVertexArray(0);    
	glUseProgram(0);    
    
    // //unbind VAO
	// glBindVertexArray(0);    
	// glUseProgram(0);   


    
    // glUseProgram(renderShader->programShaderObject);
    // GLint size;
    // glBindBuffer           (GL_COPY_READ_BUFFER, outPointsVBO);
    // glGetBufferParameteriv (GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);

    // glBindBuffer           (GL_COPY_WRITE_BUFFER, trianglesVBO);
    // glBufferData           (GL_COPY_WRITE_BUFFER, size, nullptr, GL_STATIC_DRAW);

    // glCopyBufferSubData    (GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);
    
    // glBindBuffer(GL_ARRAY_BUFFER, trianglesVBO);

    // glBindVertexArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER,0);












}

void Demo_1::Unload() {
    mesh.Unload();
    delete triangleShader;
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

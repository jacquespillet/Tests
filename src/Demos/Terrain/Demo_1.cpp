#include "Demo_1.hpp"
#include "GL_Helpers/Util.hpp"

#include "Noise.hpp"
#include "Functions.hpp"
#include <GL/glew.h>
#include "GL_Helpers/GL_Shader.hpp"

Demo_1::Demo_1() {
    noiseTexture = std::vector<std::vector<float>>(9, std::vector<float>(texSize*texSize*texSize));
    int i=0;
    for (int x = 0; x < texSize; x++)
    {
        for (int y = 0; y < texSize; y++)
        {
            for (int z = 0; z < texSize; z++)
            {
                glm::vec3 p = glm::vec3 (x, y, z) / ((float)texSize-1);
                // float n = GetPerlinNoise3D(p.x, p.y, p.z, 1);
                // float n = FbM(p.x, p.y, p.z, 1);
                // float n = perlinfbm(p, 1, 9);
                // float n0 = y - 1 +  0.01 * Noise(glm::vec2(p.x, p.z),20.0f);
                // // std::cout << n0 << std::endl;
                // float n0 = 0.5 * gradientNoise(glm::vec3(0) + 2*p, 2);
                // float n1 = 0.5f * gradientNoise(p*5, 2*10);
                // float n2 = 0.5f * gradientNoise(p*5, 3*10);
                // float n3 = 0.5f * gradientNoise(p*5, 4*10);
                // float n4 = 0.5f * gradientNoise(p*5, 5*10);
                // float n5 = 0.5f * gradientNoise(p*5, 6*10);
                // float n6 = 0.5f * gradientNoise(p*5, 7*10);
                // float n7 = 0.5f * gradientNoise(p*5, 8*10);
                // float n8 = 0.5f * gradientNoise(p*5, 9*10);
                
                // noiseTexture[0][i]= n0;
                // // noiseTexture[0][i]= (1.0f + n0) / 2.0f;
                // noiseTexture[1][i]= (1.0f + n1) / 2.0f;
                // noiseTexture[2][i]= (1.0f + n2) / 2.0f;
                // noiseTexture[3][i]= (1.0f + n3) / 2.0f;
                // noiseTexture[4][i]= (1.0f + n4) / 2.0f;
                // noiseTexture[5][i]= (1.0f + n5) / 2.0f;
                // noiseTexture[6][i]= (1.0f + n6) / 2.0f;
                // noiseTexture[7][i]= (1.0f + n7) / 2.0f;
                // noiseTexture[8][i]= (1.0f + n8) / 2.0f;
                // // std::cout << n0 <<std::endl;

                float n0 = gradientNoise(glm::vec3(0.0f) + 2.0F*p, 2.0f);
                noiseTexture[0][i]= n0;
                float n1 = gradientNoise(glm::vec3(1.0f) + 2.0F*p, 2.0f);
                noiseTexture[1][i]= n1;
                float n2 = gradientNoise(glm::vec3(2.0f) + 2.0F*p, 2.0f);
                noiseTexture[2][i]= n2;
                float n3 = gradientNoise(glm::vec3(3.0f) + 2.0F*p, 2.0f);
                noiseTexture[3][i]= n3;
                float n4 = gradientNoise(glm::vec3(4.0f) + 2.0F*p, 2.0f);
                noiseTexture[4][i]= n4;
                float n5 = gradientNoise(glm::vec3(5.0f) + 2.0F*p, 2.0f);
                noiseTexture[5][i]= n5;
                float n6 = gradientNoise(glm::vec3(6.0f) + 2.0F*p, 2.0f);
                noiseTexture[6][i]= n6;
                float n7 = gradientNoise(glm::vec3(7.0f) + 2.0F*p, 2.0f);
                noiseTexture[7][i]= n7;
                float n8 = gradientNoise(glm::vec3(8.0f) + 2.0F*p, 2.0f);
                noiseTexture[8][i]= n8;


                i++;
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
    tex3D.resize(9);
    tex3D[0] = CreateTexture3D(noiseTexture[0], texSize);
    tex3D[1] = CreateTexture3D(noiseTexture[1], texSize);
    tex3D[2] = CreateTexture3D(noiseTexture[2], texSize);
    tex3D[3] = CreateTexture3D(noiseTexture[3], texSize);
    tex3D[4] = CreateTexture3D(noiseTexture[4], texSize);
    tex3D[5] = CreateTexture3D(noiseTexture[5], texSize);
    tex3D[6] = CreateTexture3D(noiseTexture[6], texSize);
    tex3D[7] = CreateTexture3D(noiseTexture[7], texSize);
    tex3D[8] = CreateTexture3D(noiseTexture[8], texSize);

    triTableTexture = CreateIntTexture((uint8_t*)&triTable, 16, 256);

    InitPointsBuffer();
    triangleShader = CreateShader("shaders/demo_1/triangle.vert", "shaders/demo_1/triangle.geom", "shaders/demo_1/triangle.frag");
    
    mesh2 = MeshFromFile("resources/models/suzanne/Suzanne_lp.obj");
    cam = GL_Camera();

    fbo = CreateFramebuffer3D(texSize, &_tex3D);
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
        glBufferData(GL_ARRAY_BUFFER, gridBuffer.size() * sizeof(Vertex) * 5, nullptr, GL_DYNAMIC_READ);
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
        glGenBuffers(1, &blockVBO);
        //bind buffers
        glBindBuffer(GL_ARRAY_BUFFER, blockVBO);
        //set vertex attributes
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void*)((uintptr_t)12));
        glBufferData(GL_ARRAY_BUFFER,  gridBuffer.size() * sizeof(Vertex) * 5, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);

}

void Demo_1::MouseMove(float x, float y) {
    cam.mouseMoveEvent(x, y);
}

void Demo_1::Render() {   
    mesh2.Render(cam);
    
    clock_t _t = clock();
    t = ((float)_t)/CLOCKS_PER_SEC;

    int inx=0;
    for (float x = -3; x <= 1; x+=1)
    {
        for (float y = -3; y <= 1; y+=1)
        {
            for (float z = -3; z <= 1; z+=1)
            {
                // if(emptyBlocks.find(inx) == emptyBlocks.end()){
                    glm::vec3 blockPosition(x, y, z);
                    
                    //Density func pass
                    glBindVertexArray(quadVAO);
                    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
                        GLint m_viewport[4];
                        glGetIntegerv( GL_VIEWPORT, m_viewport );
                        glViewport(0, 0, texSize, texSize);
                            glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);
                                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                                glUseProgram(renderShader->programShaderObject);
                                renderShader->SetVector3("blockPosition", blockPosition);
                                renderShader->SetTexture3D("noiseTexture[0]", tex3D[0], 0, true);
                                renderShader->SetTexture3D("noiseTexture[1]", tex3D[1], 1, true);
                                renderShader->SetTexture3D("noiseTexture[2]", tex3D[2], 2, true);
                                renderShader->SetTexture3D("noiseTexture[3]", tex3D[3], 3, true);
                                renderShader->SetTexture3D("noiseTexture[4]", tex3D[4], 4, true);
                                renderShader->SetTexture3D("noiseTexture[5]", tex3D[5], 5, true);
                                renderShader->SetTexture3D("noiseTexture[6]", tex3D[6], 6, true);
                                renderShader->SetTexture3D("noiseTexture[7]", tex3D[7], 7, true);
                                renderShader->SetTexture3D("noiseTexture[8]", tex3D[8], 8, true);
                                renderShader->SetFloat("time", t);
                                glDrawArraysInstanced(GL_TRIANGLES, 0, 6, texSize);
                            glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
                        glViewport(0, 0, m_viewport[2], m_viewport[3]);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);



                    //Transform feedback : Creates all the triangles
                    //in a geometry shader. Compute pipeline, no rasterization done.
                    glUseProgram(triangleShader->programShaderObject);
                    triangleShader->SetTexture3D("densityFunction", _tex3D, 10);
                    triangleShader->SetTexture2D("triTable", triTableTexture, 1);
                    triangleShader->SetVector3("blockPosition", blockPosition);
                    triangleShader->SetFloat("blockSize", 1.0f);
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
                    if(primitives>0) {
                        //Copies all the resulting triangles into the triangle VAO
                        GLint size = primitives * 3 * sizeof(Vertex);
                        glBindBuffer           (GL_COPY_READ_BUFFER, outTrianglesVBO);
                        glBindBuffer           (GL_COPY_WRITE_BUFFER, blockVBO);
                        glCopyBufferSubData    (GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);


                        //Render the triangles
                        glUseProgram(mesh2.shader.programShaderObject);
                        glBindVertexArray(blockVAO);
                        glBindBuffer(GL_ARRAY_BUFFER, blockVBO);
                        glDrawArrays(GL_TRIANGLES, 0, primitives * 3);
                        glBindVertexArray(0);    
                        glUseProgram(0);    
                        glBindVertexArray(0);    
                    } else {
                        emptyBlocks.insert(std::pair<int, bool>(inx, true));
                    }                    
                }
                inx++;    
            // }
        }
    }
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

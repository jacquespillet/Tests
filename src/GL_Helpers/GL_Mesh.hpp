#pragma once

#include "Common.h"

#include "GL_Helpers/GL_Shader.hpp"
#include "GL_Camera.hpp"

class GL_Mesh {
public:
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 tan;
    }; 

    GL_Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> triangles);
    GL_Mesh();

    void Unload();

    void Render(const GL_Camera &camera);
    // void Render();

    void Init();
    void Destroy();
    // std::shared_ptr<GL_Material> material;
    
    bool inited;

    void Translate(glm::vec3 dPos);
    void Rotate(glm::vec3 dRot);
    void Scale(glm::vec3 dScale);

    void SetPos(glm::vec3 pos);
    void SetRot(glm::vec3 rot);
    void SetScale(glm::vec3 scale);

    void RecalculateMatrices();

    glm::mat4 modelMatrix;
    glm::mat4 invModelMatrix;


    GL_Shader shader;    
private:
    //CPU buffers
    std::vector<Vertex> vertices;
    std::vector<unsigned int> triangles;
    
    //GPU buffers
    unsigned int vertexBuffer;
    unsigned int elementBuffer;
    unsigned int vertexArrayObject;


    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};


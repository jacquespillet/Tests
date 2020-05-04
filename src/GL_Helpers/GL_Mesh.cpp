#include "GL_Mesh.hpp"

#include <fstream>
#include <sstream>
#include <GL/glew.h>

GL_Mesh::GL_Mesh(): inited(false) {
}

GL_Mesh::GL_Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> triangles) : position(glm::vec3(0)), scale(glm::vec3(1)),rotation(glm::vec3(0)),   vertices(vertices), triangles(triangles), inited(false) {
    std::ifstream t("shaders/Unlit.vert");
    std::stringstream vertBuffer;
    vertBuffer << t.rdbuf();
    shader.vertSrc= vertBuffer.str();

    t = std::ifstream ("shaders/Unlit.frag");
    std::stringstream fragBuffer;
    fragBuffer << t.rdbuf();
    shader.fragSrc= fragBuffer.str();

    t.close();
    std::cout << "StandardShaders:Compile: Compiling unlitMeshShader" << std::endl; 
    shader.Compile();      

    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &elementBuffer);
    
    //Bind VAO
    glBindVertexArray(vertexArrayObject);
    
    //bind buffers
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(unsigned int), (uint8_t*)&triangles[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);        //Unbind VAO
    glBindVertexArray(0);
    //Unbind array and element buffers
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);    
}

void GL_Mesh::Render(const GL_Camera& camera) {

	glUseProgram(shader.programShaderObject);

    shader.SetMat4("projectionMatrix", camera.GetProjectionMatrix());
    shader.SetMat4("viewMatrix", camera.GetViewMatrix());
    shader.SetMat4("modelMatrix", modelMatrix);
    
    //bind VAO	
	glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glDrawElements(GL_TRIANGLES, (GLsizei)triangles.size(), GL_UNSIGNED_INT, (void*)0);
	//unbind VAO
	glBindVertexArray(0);    
	glUseProgram(0);    
}

void GL_Mesh::Translate(glm::vec3 dPos) {
    position += dPos;
    RecalculateMatrices();
}

void GL_Mesh::Rotate(glm::vec3 dRot) {
    rotation += dRot;
    RecalculateMatrices();
}

void GL_Mesh::Scale(glm::vec3 dScale) {
    scale += dScale;
    RecalculateMatrices();
}


void GL_Mesh::SetPos(glm::vec3 pos) {
    position = pos;
    RecalculateMatrices();
}

void GL_Mesh::SetRot(glm::vec3 rot) {
    rotation = rot;
    RecalculateMatrices();
}

void GL_Mesh::SetScale(glm::vec3 _scale) {
    scale = _scale;
    RecalculateMatrices();
}

void GL_Mesh::RecalculateMatrices() {
    glm::mat4 t=  glm::translate(glm::mat4(1.0), position);

    glm::mat4 rx=  glm::rotate(glm::mat4(1.0), glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 ry=  glm::rotate(glm::mat4(1.0), glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 rz=  glm::rotate(glm::mat4(1.0), glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 r = rz * ry * rx;

    glm::mat4 s = glm::scale(glm::mat4(1.0), scale);

    modelMatrix = t * s * r;
    invModelMatrix = glm::inverse(modelMatrix);
}

void GL_Mesh::Unload(){ 
    std::cout <<" DESTROYING" << std::endl;
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &elementBuffer);
    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteProgram(shader.programShaderObject);

    shader.Unload();
}
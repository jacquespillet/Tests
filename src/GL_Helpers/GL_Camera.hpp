#pragma once

#include "Common.h"

class GL_Camera {
public:
    GL_Camera(float fov, float nearPlane, float farPlane, float aspectRatio) : fov(fov), nearPlane(nearPlane), farPlane(farPlane), aspectRatio(aspectRatio), position(glm::vec3(1, 1, 1)), target(glm::vec3(0,0,0)), up(glm::vec3(0,1,0)){ RecalculateProjectionMatrix();RecalculateLookat();}
    GL_Camera() : fov(80.0f), nearPlane(0.001f), farPlane(1000.0f), aspectRatio(1.0f), position(glm::vec3(0, 0, 2)), target(glm::vec3(0,0,0)), up(glm::vec3(0,1,0)){RecalculateProjectionMatrix(); RecalculateLookat();}

    // void Render(const std::vector<GL_Mesh*>& object, GL_Framebuffer *target);
    // void Render(const GL_Scene& scene, GL_Framebuffer *target);

    void SetFov(float _fov) {this->fov = _fov; RecalculateProjectionMatrix();}
    void SetNearPlane(float _nearPlane) {this->nearPlane = _nearPlane; RecalculateProjectionMatrix();}
    void SetFarPlane(float _farPlane) {this->farPlane = _farPlane; RecalculateProjectionMatrix();}
    void SetAspectRatio(float _aspectRatio) {this->aspectRatio = _aspectRatio; RecalculateProjectionMatrix();}

    void RecalculateProjectionMatrix() {
        projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }

    glm::mat4 GetProjectionMatrix() const{return projectionMatrix;}
    glm::mat4 GetViewMatrix() const{return invModelMatrix;}

    void LookAt(glm::vec3 pos, glm::vec3 center, glm::vec3 up);
    void RecalculateLookat();

    void mousePressEvent(int button);
    void mouseReleaseEvent(int button);
    void mouseMoveEvent(float x, float y);
    void Scroll(float offset);


private:    
    float fov;
    float nearPlane, farPlane;
    float aspectRatio;
    
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float phi=0;
    float theta=0;
    float distance = 5;

    glm::mat4 projectionMatrix;

    bool IsLeftMousePressed=false;
    bool IsRightMousePressed=false;
    glm::vec2 prevPos = glm::vec2(-1.f, -1.f);

    glm::mat4 modelMatrix;
    glm::mat4 invModelMatrix;

};
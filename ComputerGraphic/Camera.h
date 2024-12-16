#pragma once
#include "Mathematic.h"
#include "GEBase.h"
#include <cmath>
#include <windows.h>

class Camera {
public:
    Vec3 position;     
    Vec3 rotation;     

    Vec3 upWorld;       
    Vec3 forward;       
    Vec3 right;         
    Vec3 upLocal;      

    Matrix viewMatrix;  
    Matrix projectionMatrix; 

    float fov;          // Field of view
    float nearPlane;    
    float farPlane;    

    POINT currentMousePos;
    POINT lastMousePos;
    bool firstMouseCapture;

    // Constructor
    Camera(float _fov = 1.5708f, float _nearPlane = 0.1f, float _farPlane = 100.0f)
        : fov(_fov), nearPlane(_nearPlane), farPlane(_farPlane), firstMouseCapture(true) {
        position = Vec3(0.0f, 0.0f, 0.0f);
        rotation = Vec3(0.0f, 0.0f, 0.0f);
        upWorld = Vec3(0.0f, 1.0f, 0.0f);
        updateVectors();
    }

    // Set position
    void setPosition(float x, float y, float z) {
        position = Vec3(x, y, z);
    }

    void setPosition(const Vec3& v) {
        position = v;
    }

    // Set rotation
    void setRotation(float pitch, float yaw, float roll) {
        rotation = Vec3(pitch, yaw, roll);
        updateVectors();
    }

    void setRotation(const Vec3& v) {
        rotation = v;
        updateVectors();
    }

    // Reset camera to default position and rotation
    void resetCamera() {
        position = Vec3(11.0f, 5.0f, 11.0f);
        rotation = Vec3(0.0f, 0.0f, 0.0f);
        updateVectors();
    }

    // //Process keyboard input for movement
    //void processInput(bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, bool reset, float speed, float dt) {
    //    if (reset) {
    //        resetCamera();
    //    }

    //    Vec3 horizontalForward = Vec3(forward.x, 0.0f, forward.z).normalize();
    //    Vec3 horizontalRight = Vec3(right.x, 0.0f, right.z).normalize();


    //    if (moveForward) position += forward * speed * dt;
    //    if (moveBackward) position -= forward * speed * dt;
    //    if (moveLeft) position -= right * speed * dt;
    //    if (moveRight) position += right * speed * dt;
    //}

    void processInput(bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, bool reset, float speed, float dt) {
        if (reset) {
            resetCamera();
        }

   
        Vec3 horizontalForward = Vec3(forward.x, 1.0f, forward.z).normalize();
        Vec3 horizontalRight = Vec3(right.x, 1.0f, right.z).normalize();
    
    
        if (moveForward) position += horizontalForward * speed * dt;
        if (moveBackward) position -= horizontalForward * speed * dt;
        if (moveLeft) position -= horizontalRight * speed * dt;
        if (moveRight) position += horizontalRight * speed * dt;

    
        position.y = 1.0f;
    }



    // Process mouse input to adjust rotation
    void processMouse(float deltaX, float deltaY, float mouseSensitivity) {
        rotation.y -= deltaX * mouseSensitivity;
        rotation.x -= deltaY * mouseSensitivity;

        // Clamp pitch to avoid flipping
        rotation.x = clamp(rotation.x, -1.5f, 1.5f); // ~85 degrees
        updateVectors();
    }


    void captureInput(HWND hwnd, float mouseSensitivity = 0.1f) {

        POINT windowCenter = { 512, 512 }; 
        POINT currentMouse;
        GetCursorPos(&currentMouse);
        ScreenToClient(hwnd, &currentMouse);

        float deltaX = static_cast<float>(currentMouse.x - windowCenter.x);
        float deltaY = static_cast<float>(currentMouse.y - windowCenter.y);

    
        if (!firstMouseCapture) {
            processMouse(deltaX, deltaY, mouseSensitivity);
        }
        firstMouseCapture = false;

      
        ClientToScreen(hwnd, &windowCenter);
        SetCursorPos(windowCenter.x, windowCenter.y);
    }


    // Update direction vectors based on rotation
    void updateVectors() {
        float cosPitch = cosf(rotation.x);
        float sinPitch = sinf(rotation.x);
        float cosYaw = cosf(rotation.y);
        float sinYaw = sinf(rotation.y);

        // Forward vector
        forward = Vec3(cosYaw * cosPitch, sinPitch, sinYaw * cosPitch).normalize();

        // Right vector
        right = forward.cross(upWorld).normalize();

        // Up local vector
        upLocal = right.cross(forward).normalize();
    }

    // Get view matrix
    Matrix getViewMatrix() {
        updateVectors();
        Vec3 target = position + forward;
        return lookAt(position, target, upLocal);
    }

    // Get projection matrix
    Matrix getProjectionMatrix(float aspectRatio) {
        return PerPro(aspectRatio, 1.0f, fov, farPlane, nearPlane);
    }
};

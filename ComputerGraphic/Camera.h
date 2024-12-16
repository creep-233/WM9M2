#pragma once
#include "Mathematic.h"
#include "GEBase.h"
#include <cmath>
#include <windows.h>

class Camera {
public:
    Vec3 position;      // Camera position
    Vec3 rotation;      // Camera rotation (pitch, yaw, roll)

    Vec3 upWorld;       // World up vector
    Vec3 forward;       // Forward vector
    Vec3 right;         // Right vector
    Vec3 upLocal;       // Local up vector

    Matrix viewMatrix;  // View matrix
    Matrix projectionMatrix; // Projection matrix

    float fov;          // Field of view
    float nearPlane;    // Near clipping plane
    float farPlane;     // Far clipping plane

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

        // 修正 forward 和 right，使其 Y 分量为零，保持水平移动
        Vec3 horizontalForward = Vec3(forward.x, 1.0f, forward.z).normalize();
        Vec3 horizontalRight = Vec3(right.x, 1.0f, right.z).normalize();
    
        // 使用修正后的向量更新位置
        if (moveForward) position += horizontalForward * speed * dt;
        if (moveBackward) position -= horizontalForward * speed * dt;
        if (moveLeft) position -= horizontalRight * speed * dt;
        if (moveRight) position += horizontalRight * speed * dt;

        // 确保相机始终停留在地面上（可选，如果需要强制 Y 不变）
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

    // Capture mouse input for rotation
    //void captureInput(HWND hwnd, float mouseSensitivity = 0.1f) {
    //    GetCursorPos(&currentMousePos);
    //    ScreenToClient(hwnd, &currentMousePos);

    //    float deltaX = 0.0f;
    //    float deltaY = 0.0f;

    //    if (!firstMouseCapture) {
    //        deltaX = static_cast<float>(currentMousePos.x - lastMousePos.x);
    //        deltaY = static_cast<float>(currentMousePos.y - lastMousePos.y);
    //    }
    //    else {
    //        firstMouseCapture = false;
    //    }

    //    lastMousePos = currentMousePos;

    //    if (deltaX != 0.0f || deltaY != 0.0f) {
    //        processMouse(deltaX, deltaY, mouseSensitivity);
    //    }
    //}

    void captureInput(HWND hwnd, float mouseSensitivity = 0.1f) {
        // 获取当前鼠标位置
        POINT windowCenter = { 512, 512 }; // 这里假设窗口大小是1024x1024
        POINT currentMouse;
        GetCursorPos(&currentMouse);
        ScreenToClient(hwnd, &currentMouse);

        float deltaX = static_cast<float>(currentMouse.x - windowCenter.x);
        float deltaY = static_cast<float>(currentMouse.y - windowCenter.y);

        // 处理鼠标位移
        if (!firstMouseCapture) {
            processMouse(deltaX, deltaY, mouseSensitivity);
        }
        firstMouseCapture = false;

        // 重置鼠标位置到窗口中心
        ClientToScreen(hwnd, &windowCenter); // 转换到屏幕坐标
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

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

    // Process keyboard input for movement
    void processInput(bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, bool reset, float speed, float dt) {
        if (reset) {
            resetCamera();
        }

        Vec3 horizontalForward = Vec3(forward.x, 0.0f, forward.z).normalize();
        Vec3 horizontalRight = Vec3(right.x, 0.0f, right.z).normalize();


        if (moveForward) position += forward * speed * dt;
        if (moveBackward) position -= forward * speed * dt;
        if (moveLeft) position -= right * speed * dt;
        if (moveRight) position += right * speed * dt;
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
    void captureInput(HWND hwnd, float mouseSensitivity = 0.1f) {
        GetCursorPos(&currentMousePos);
        ScreenToClient(hwnd, &currentMousePos);

        float deltaX = 0.0f;
        float deltaY = 0.0f;

        if (!firstMouseCapture) {
            deltaX = static_cast<float>(currentMousePos.x - lastMousePos.x);
            deltaY = static_cast<float>(currentMousePos.y - lastMousePos.y);
        }
        else {
            firstMouseCapture = false;
        }

        lastMousePos = currentMousePos;

        if (deltaX != 0.0f || deltaY != 0.0f) {
            processMouse(deltaX, deltaY, mouseSensitivity);
        }
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

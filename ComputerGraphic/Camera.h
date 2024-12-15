//#pragma once
//#include"DXCore.h"
//#include"Mathematic.h"
//#include<iostream>
//#include<cmath>  // 确保包含此头文件，用于数学运算
//
//#define DEG_TO_RAD 0.0174532925f  // 1 degree = 0.0174532925 radians
//
//class Camera {
//public:
//    Vec3 position;
//    Vec3 target;
//    Vec3 up;
//
//    float pitch = 0.0f; // 控制上下视角
//    float yaw = -90.0f; // 控制左右视角，初始设为 -90 度以便朝向 -Z 轴
//
//    Camera(Vec3 pos, Vec3 tgt, Vec3 upDir)
//        : position(pos), target(tgt), up(upDir) {}
//
//    // 获取 View 矩阵
//    Matrix getViewMatrix() {
//        return lookAt(position, target, up);
//    }
//
//    // 更新相机方向
//    void updateDirection(float deltaX, float deltaY) {
//        const float sensitivity = 0.1f;
//        yaw += deltaX * sensitivity;
//        pitch += deltaY * sensitivity;
//
//        // 限制俯仰角，防止视角翻转
//        if (pitch > 89.0f) pitch = 89.0f;
//        if (pitch < -89.0f) pitch = -89.0f;
//
//        // 将俯仰角和偏航角从度数转换为弧度
//        float pitchRadians = pitch * DEG_TO_RAD;
//        float yawRadians = yaw * DEG_TO_RAD;
//
//        // 根据俯仰和偏航角计算前向量
//        Vec3 front;
//        front.x = cosf(yawRadians) * cosf(pitchRadians);
//        front.y = sinf(pitchRadians);
//        front.z = sinf(yawRadians) * cosf(pitchRadians);
//
//        front = front.normalize();  // 确保前向量是单位向量
//        target = position + front;  // 更新相机的目标点
//    }
//
//    // 更新相机位置（例如前后左右移动）
//    void move(const Vec3& direction, float speed) {
//        Vec3 forward = (target - position);
//        forward.normalize();
//
//        // 侧向量
//        Vec3 right = cross(forward, up);
//        right.normalize();
//
//        Vec3 movement = direction.x * right + direction.z * forward + direction.y * up;
//
//        position += movement * speed;
//        target += movement * speed;
//    }
//
//private:
//    // Helper function to convert degrees to radians
//    float toRadians(float degrees) {
//        return degrees * DEG_TO_RAD;
//    }
//};



#pragma once
#include "DXCore.h"
#include "Mathematic.h"
#include <iostream>
#include <cmath>

#define DEG_TO_RAD 0.0174532925f // 1 degree = 0.0174532925 radians

class Camera {
public:
    Vec3 position;
    Vec3 upWorld;     // 世界坐标系的上向量
    Vec3 forward;     // 前向向量
    Vec3 right;       // 右向向量
    Vec3 upLocal;     // 本地上向量

    float pitch = 0.0f; // 控制上下视角
    float yaw = -90.0f; // 控制左右视角，初始设为 -90 度以便朝向 -Z 轴

    float fov = 45.0f;   // 视角
    float nearPlane = 0.1f; // 近平面
    float farPlane = 100.0f; // 远平面

    Camera(Vec3 pos, Vec3 upDir, float initialFov = 45.0f)
        : position(pos), upWorld(upDir), fov(initialFov) {
        updateVectors(); // 初始化方向向量
    }

    // 获取 View 矩阵
    Matrix getViewMatrix() {
        Vec3 target = position + forward;
        return lookAt(position, target, upLocal);
    }

    // 获取投影矩阵
    Matrix getProjectionMatrix(float aspectRatio) {
        return PerPro(aspectRatio, 1.0f, fov, farPlane, nearPlane);
    }

    // 鼠标输入更新方向
    void updateDirection(float deltaX, float deltaY, float sensitivity = 0.1f) {
        yaw += deltaX * sensitivity;
        pitch += deltaY * sensitivity;

        // 限制俯仰角，防止视角翻转
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        updateVectors(); // 更新方向向量
    }

    // 键盘输入更新位置（前后左右移动）
    void move(const Vec3& direction, float speed) {
        Vec3 movement = direction.x * right + direction.y * upLocal + direction.z * forward;
        movement.normalize(); // 确保移动方向是单位向量
        position += movement * speed;
    }

    // 重置相机到默认位置和方向
    void resetCamera(Vec3 defaultPosition = Vec3(0.0f, 5.0f, -10.0f), float defaultYaw = -90.0f, float defaultPitch = 0.0f) {
        position = defaultPosition;
        yaw = defaultYaw;
        pitch = defaultPitch;
        updateVectors(); // 更新方向向量
    }

    // 设置相机视角
    void setFov(float newFov) {
        fov = newFov;
    }

    // 鼠标捕获，用于窗口内的相机旋转
    void captureInput(HWND hwnd, float mouseSensitivity = 0.1f) {
        POINT currentMousePos;
        GetCursorPos(&currentMousePos);
        ScreenToClient(hwnd, &currentMousePos);

        static POINT lastMousePos = currentMousePos;

        float deltaX = (float)(currentMousePos.x - lastMousePos.x);
        float deltaY = (float)(currentMousePos.y - lastMousePos.y);

        lastMousePos = currentMousePos;

        if (deltaX != 0.0f || deltaY != 0.0f) {
            updateDirection(deltaX, deltaY, mouseSensitivity);
        }
    }

private:
    // 更新相机的方向向量
    void updateVectors() {
        float cosPitch = cosf(pitch * DEG_TO_RAD);
        float sinPitch = sinf(pitch * DEG_TO_RAD);
        float cosYaw = cosf(yaw * DEG_TO_RAD);
        float sinYaw = sinf(yaw * DEG_TO_RAD);

        // 计算前向量
        forward = Vec3(cosYaw * cosPitch, sinPitch, sinYaw * cosPitch).normalize();

        // 计算右向量
        right = forward.cross(upWorld).normalize();

        // 计算本地上向量
        upLocal = right.cross(forward).normalize();
    }
};


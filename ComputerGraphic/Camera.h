#pragma once
#include"DXCore.h"
#include"Mathematic.h"
#include<iostream>
#include<cmath>  // 确保包含此头文件，用于数学运算

#define DEG_TO_RAD 0.0174532925f  // 1 degree = 0.0174532925 radians

class Camera {
public:
    Vec3 position;
    Vec3 target;
    Vec3 up;

    float pitch = 0.0f; // 控制上下视角
    float yaw = -90.0f; // 控制左右视角，初始设为 -90 度以便朝向 -Z 轴

    Camera(Vec3 pos, Vec3 tgt, Vec3 upDir)
        : position(pos), target(tgt), up(upDir) {}

    // 获取 View 矩阵
    Matrix getViewMatrix() {
        return lookAt(position, target, up);
    }

    // 更新相机方向
    void updateDirection(float deltaX, float deltaY) {
        const float sensitivity = 0.1f;
        yaw += deltaX * sensitivity;
        pitch += deltaY * sensitivity;

        // 限制俯仰角，防止视角翻转
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        // 将俯仰角和偏航角从度数转换为弧度
        float pitchRadians = pitch * DEG_TO_RAD;
        float yawRadians = yaw * DEG_TO_RAD;

        // 根据俯仰和偏航角计算前向量
        Vec3 front;
        front.x = cosf(yawRadians) * cosf(pitchRadians);
        front.y = sinf(pitchRadians);
        front.z = sinf(yawRadians) * cosf(pitchRadians);

        front = front.normalize();  // 确保前向量是单位向量
        target = position + front;  // 更新相机的目标点
    }

    // 更新相机位置（例如前后左右移动）
    void move(const Vec3& direction, float speed) {
        Vec3 forward = (target - position);
        forward.normalize();

        // 侧向量
        Vec3 right = cross(forward, up);
        right.normalize();

        Vec3 movement = direction.x * right + direction.z * forward + direction.y * up;

        position += movement * speed;
        target += movement * speed;
    }

private:
    // Helper function to convert degrees to radians
    float toRadians(float degrees) {
        return degrees * DEG_TO_RAD;
    }
};

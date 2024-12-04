#pragma once
#include"DXCore.h"
#include"Mathematic.h"
#include<iostream>
#include<cmath>  // ȷ��������ͷ�ļ���������ѧ����

#define DEG_TO_RAD 0.0174532925f  // 1 degree = 0.0174532925 radians

class Camera {
public:
    Vec3 position;
    Vec3 target;
    Vec3 up;

    float pitch = 0.0f; // ���������ӽ�
    float yaw = -90.0f; // ���������ӽǣ���ʼ��Ϊ -90 ���Ա㳯�� -Z ��

    Camera(Vec3 pos, Vec3 tgt, Vec3 upDir)
        : position(pos), target(tgt), up(upDir) {}

    // ��ȡ View ����
    Matrix getViewMatrix() {
        return lookAt(position, target, up);
    }

    // �����������
    void updateDirection(float deltaX, float deltaY) {
        const float sensitivity = 0.1f;
        yaw += deltaX * sensitivity;
        pitch += deltaY * sensitivity;

        // ���Ƹ����ǣ���ֹ�ӽǷ�ת
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        // �������Ǻ�ƫ���ǴӶ���ת��Ϊ����
        float pitchRadians = pitch * DEG_TO_RAD;
        float yawRadians = yaw * DEG_TO_RAD;

        // ���ݸ�����ƫ���Ǽ���ǰ����
        Vec3 front;
        front.x = cosf(yawRadians) * cosf(pitchRadians);
        front.y = sinf(pitchRadians);
        front.z = sinf(yawRadians) * cosf(pitchRadians);

        front = front.normalize();  // ȷ��ǰ�����ǵ�λ����
        target = position + front;  // ���������Ŀ���
    }

    // �������λ�ã�����ǰ�������ƶ���
    void move(const Vec3& direction, float speed) {
        Vec3 forward = (target - position);
        forward.normalize();

        // ������
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

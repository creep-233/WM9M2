//#pragma once
//#include"DXCore.h"
//#include"Mathematic.h"
//#include<iostream>
//#include<cmath>  // ȷ��������ͷ�ļ���������ѧ����
//
//#define DEG_TO_RAD 0.0174532925f  // 1 degree = 0.0174532925 radians
//
//class Camera {
//public:
//    Vec3 position;
//    Vec3 target;
//    Vec3 up;
//
//    float pitch = 0.0f; // ���������ӽ�
//    float yaw = -90.0f; // ���������ӽǣ���ʼ��Ϊ -90 ���Ա㳯�� -Z ��
//
//    Camera(Vec3 pos, Vec3 tgt, Vec3 upDir)
//        : position(pos), target(tgt), up(upDir) {}
//
//    // ��ȡ View ����
//    Matrix getViewMatrix() {
//        return lookAt(position, target, up);
//    }
//
//    // �����������
//    void updateDirection(float deltaX, float deltaY) {
//        const float sensitivity = 0.1f;
//        yaw += deltaX * sensitivity;
//        pitch += deltaY * sensitivity;
//
//        // ���Ƹ����ǣ���ֹ�ӽǷ�ת
//        if (pitch > 89.0f) pitch = 89.0f;
//        if (pitch < -89.0f) pitch = -89.0f;
//
//        // �������Ǻ�ƫ���ǴӶ���ת��Ϊ����
//        float pitchRadians = pitch * DEG_TO_RAD;
//        float yawRadians = yaw * DEG_TO_RAD;
//
//        // ���ݸ�����ƫ���Ǽ���ǰ����
//        Vec3 front;
//        front.x = cosf(yawRadians) * cosf(pitchRadians);
//        front.y = sinf(pitchRadians);
//        front.z = sinf(yawRadians) * cosf(pitchRadians);
//
//        front = front.normalize();  // ȷ��ǰ�����ǵ�λ����
//        target = position + front;  // ���������Ŀ���
//    }
//
//    // �������λ�ã�����ǰ�������ƶ���
//    void move(const Vec3& direction, float speed) {
//        Vec3 forward = (target - position);
//        forward.normalize();
//
//        // ������
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
    Vec3 upWorld;     // ��������ϵ��������
    Vec3 forward;     // ǰ������
    Vec3 right;       // ��������
    Vec3 upLocal;     // ����������

    float pitch = 0.0f; // ���������ӽ�
    float yaw = -90.0f; // ���������ӽǣ���ʼ��Ϊ -90 ���Ա㳯�� -Z ��

    float fov = 45.0f;   // �ӽ�
    float nearPlane = 0.1f; // ��ƽ��
    float farPlane = 100.0f; // Զƽ��

    Camera(Vec3 pos, Vec3 upDir, float initialFov = 45.0f)
        : position(pos), upWorld(upDir), fov(initialFov) {
        updateVectors(); // ��ʼ����������
    }

    // ��ȡ View ����
    Matrix getViewMatrix() {
        Vec3 target = position + forward;
        return lookAt(position, target, upLocal);
    }

    // ��ȡͶӰ����
    Matrix getProjectionMatrix(float aspectRatio) {
        return PerPro(aspectRatio, 1.0f, fov, farPlane, nearPlane);
    }

    // ���������·���
    void updateDirection(float deltaX, float deltaY, float sensitivity = 0.1f) {
        yaw += deltaX * sensitivity;
        pitch += deltaY * sensitivity;

        // ���Ƹ����ǣ���ֹ�ӽǷ�ת
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        updateVectors(); // ���·�������
    }

    // �����������λ�ã�ǰ�������ƶ���
    void move(const Vec3& direction, float speed) {
        Vec3 movement = direction.x * right + direction.y * upLocal + direction.z * forward;
        movement.normalize(); // ȷ���ƶ������ǵ�λ����
        position += movement * speed;
    }

    // ���������Ĭ��λ�úͷ���
    void resetCamera(Vec3 defaultPosition = Vec3(0.0f, 5.0f, -10.0f), float defaultYaw = -90.0f, float defaultPitch = 0.0f) {
        position = defaultPosition;
        yaw = defaultYaw;
        pitch = defaultPitch;
        updateVectors(); // ���·�������
    }

    // ��������ӽ�
    void setFov(float newFov) {
        fov = newFov;
    }

    // ��겶�����ڴ����ڵ������ת
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
    // ��������ķ�������
    void updateVectors() {
        float cosPitch = cosf(pitch * DEG_TO_RAD);
        float sinPitch = sinf(pitch * DEG_TO_RAD);
        float cosYaw = cosf(yaw * DEG_TO_RAD);
        float sinYaw = sinf(yaw * DEG_TO_RAD);

        // ����ǰ����
        forward = Vec3(cosYaw * cosPitch, sinPitch, sinYaw * cosPitch).normalize();

        // ����������
        right = forward.cross(upWorld).normalize();

        // ���㱾��������
        upLocal = right.cross(forward).normalize();
    }
};


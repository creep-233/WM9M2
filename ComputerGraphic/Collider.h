
#pragma once
#include "Mathematic.h"

// ��Χ����
class BoundingVolume {
public:
    Vec3 upperCorner;         // ��������
    Vec3 lowerCorner;         // ��С�����
    std::vector<Vec3> points; // ���ڴ洢����

    BoundingVolume() {}

    // ���ð�Χ��
    void clear() {
        upperCorner = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        lowerCorner = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    }

    // ��չ��Χ�з�Χ�԰����µĵ�
    void expand(const Vec3& point) {
        upperCorner = Max(upperCorner, point);
        lowerCorner = Min(lowerCorner, point);
    }

    // ���������Χ���Ƿ��ཻ
    bool isOverlapping(const BoundingVolume& other) const {
        return (lowerCorner.x <= other.upperCorner.x && upperCorner.x >= other.lowerCorner.x) &&
            (lowerCorner.y <= other.upperCorner.y && upperCorner.y >= other.lowerCorner.y) &&
            (lowerCorner.z <= other.upperCorner.z && upperCorner.z >= other.lowerCorner.z);
    }

    // ���°�Χ�еı任�����š���ת��ƽ��
    void applyTransform(const Vec3& scale, const Vec3& rotation, const Vec3& translation) {
        clear(); // ��յ�ǰ�߽�

        // ���ɱ任����
        Matrix transformMatrix = Matrix::worldTrans(scale, rotation, translation);

        for (auto& point : points) {
            Vec4 tempPoint(point.x, point.y, point.z, 1.0f);
            Vec4 transformedPoint = transformMatrix.mulPointP(tempPoint);
            expand(Vec3(transformedPoint.x, transformedPoint.y, transformedPoint.z));
        }
    }
};

// ������ײ��
class RayCaster {
public:
    Vec3 origin;         // �������
    Vec3 direction;      // ���߷���
    Vec3 invDirection;   // ���߷���ĵ���

    RayCaster() {}

    RayCaster(const Vec3& start, const Vec3& dir) {
        initialize(start, dir);
    }

    // ��ʼ������
    void initialize(const Vec3& start, const Vec3& dir) {
        origin = start;
        direction = dir;
        invDirection = Vec3(1.0f, 1.0f, 1.0f) / direction; // Ԥ���㷽����
    }

    // ���������� t ������������
    Vec3 computePosition(float t) const {
        return origin + (direction * t);
    }

    // �������Χ����ײ���
    bool checkIntersection(const BoundingVolume& box, float& hitTime) {
        Vec3 tNearCorner = (box.lowerCorner - origin) * invDirection;
        Vec3 tFarCorner = (box.upperCorner - origin) * invDirection;

        // �ҵ��������Զ�Ľ���
        Vec3 tMinValues = Min(tNearCorner, tFarCorner);
        Vec3 tMaxValues = Max(tNearCorner, tFarCorner);

        float tNear = max(tMinValues.x, max(tMinValues.y, tMinValues.z));
        float tFar = min(tMaxValues.x, min(tMaxValues.y, tMaxValues.z));

        // ��� tNear ���� tFar������ tFar С�� 0����û�н���
        if (tNear > tFar || tFar < 0.0f) {
            return false;
        }

        // ����ʱ��
        hitTime = (tNear >= 0.0f) ? tNear : tFar;
        return true;
    }
};


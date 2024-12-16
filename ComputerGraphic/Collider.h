
#pragma once
#include "Mathematic.h"

// 包围盒类
class BoundingVolume {
public:
    Vec3 upperCorner;         // 最大坐标点
    Vec3 lowerCorner;         // 最小坐标点
    std::vector<Vec3> points; // 用于存储顶点

    BoundingVolume() {}

    // 重置包围盒
    void clear() {
        upperCorner = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        lowerCorner = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    }

    // 扩展包围盒范围以包含新的点
    void expand(const Vec3& point) {
        upperCorner = Max(upperCorner, point);
        lowerCorner = Min(lowerCorner, point);
    }

    // 检查两个包围盒是否相交
    bool isOverlapping(const BoundingVolume& other) const {
        return (lowerCorner.x <= other.upperCorner.x && upperCorner.x >= other.lowerCorner.x) &&
            (lowerCorner.y <= other.upperCorner.y && upperCorner.y >= other.lowerCorner.y) &&
            (lowerCorner.z <= other.upperCorner.z && upperCorner.z >= other.lowerCorner.z);
    }

    // 更新包围盒的变换：缩放、旋转和平移
    void applyTransform(const Vec3& scale, const Vec3& rotation, const Vec3& translation) {
        clear(); // 清空当前边界

        // 生成变换矩阵
        Matrix transformMatrix = Matrix::worldTrans(scale, rotation, translation);

        for (auto& point : points) {
            Vec4 tempPoint(point.x, point.y, point.z, 1.0f);
            Vec4 transformedPoint = transformMatrix.mulPointP(tempPoint);
            expand(Vec3(transformedPoint.x, transformedPoint.y, transformedPoint.z));
        }
    }
};

// 射线碰撞类
class RayCaster {
public:
    Vec3 origin;         // 射线起点
    Vec3 direction;      // 射线方向
    Vec3 invDirection;   // 射线方向的倒数

    RayCaster() {}

    RayCaster(const Vec3& start, const Vec3& dir) {
        initialize(start, dir);
    }

    // 初始化射线
    void initialize(const Vec3& start, const Vec3& dir) {
        origin = start;
        direction = dir;
        invDirection = Vec3(1.0f, 1.0f, 1.0f) / direction; // 预计算方向倒数
    }

    // 计算射线在 t 参数处的坐标
    Vec3 computePosition(float t) const {
        return origin + (direction * t);
    }

    // 射线与包围盒碰撞检测
    bool checkIntersection(const BoundingVolume& box, float& hitTime) {
        Vec3 tNearCorner = (box.lowerCorner - origin) * invDirection;
        Vec3 tFarCorner = (box.upperCorner - origin) * invDirection;

        // 找到最近和最远的交点
        Vec3 tMinValues = Min(tNearCorner, tFarCorner);
        Vec3 tMaxValues = Max(tNearCorner, tFarCorner);

        float tNear = max(tMinValues.x, max(tMinValues.y, tMinValues.z));
        float tFar = min(tMaxValues.x, min(tMaxValues.y, tMaxValues.z));

        // 如果 tNear 超过 tFar，或者 tFar 小于 0，则没有交点
        if (tNear > tFar || tFar < 0.0f) {
            return false;
        }

        // 交点时间
        hitTime = (tNear >= 0.0f) ? tNear : tFar;
        return true;
    }
};


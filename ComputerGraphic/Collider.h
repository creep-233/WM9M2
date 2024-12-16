//#pragma once
//#include "Mathematic.h"
//
//class AABB {
//public:
//    Vec3 maxDis;
//    Vec3 minDis;
//    std::vector<Vec3> vertices;
//    AABB() {}
//    void reset()
//    {
//        maxDis = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
//        minDis = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
//    }
//    void extend(const Vec3& p)
//    {
//        maxDis = Max(maxDis, p);
//        minDis = Min(minDis, p);
//    }
//
//    bool intersects(const AABB& other) const {
//        // check if intersects with other AABB
//        return (minDis.x <= other.maxDis.x && maxDis.x >= other.minDis.x) &&
//            (minDis.y <= other.maxDis.y && maxDis.y >= other.minDis.y) &&
//            (minDis.z <= other.maxDis.z && maxDis.z >= other.minDis.z);
//    }
//
//
//
//    void update(Vec3 scaling, Vec3 rotating, Vec3 translating) {
//        reset();
//        // 创建变换矩阵
//        Matrix world = Matrix::worldTrans(scaling, rotating, translating);
//
//        for (auto& vertex : vertices) {
//            // 将 Vec3 转换为 Vec4 并进行变换
//            Vec4 v(vertex.x, vertex.y, vertex.z, 1.0f);
//            Vec4 worldV = world.mulPointP(v);  // 使用 Matrix 进行变换
//            extend(Vec3(worldV.x, worldV.y, worldV.z));  // 更新 AABB 的边界
//        }
//    }
//
//};
//
//
//class Ray
//    // for bullet
//{
//public:
//    Vec3 o;
//    Vec3 dir;
//    Vec3 invdir;
//    Ray() {}
//    Ray(const Vec3 _o, const Vec3 _dir) {
//        init(_o, _dir);
//    }
//    void init(const Vec3 _o, const Vec3 _dir) {
//        o = _o;
//        dir = _dir;
//        invdir = Vec3(1.0f, 1.0f, 1.0f) / dir; // dir should not be 0
//    }
//    Vec3 at(const float t) {
//        return (o + (dir * t));
//    }
//
//    // the ray will apply for bullet and collide with AABB box collider
//    bool rayAABB(const AABB& box, float& t)
//    {
//
//        Vec3 tMin = (box.minDis - o) * invdir;
//        Vec3 tMax = (box.maxDis - o) * invdir;
//
//        // Ensure tMin is the near intersection and tMax is the far
//        Vec3 nearHit = Min(tMin, tMax);
//        Vec3 farHit = Max(tMin, tMax);
//
//        // The largest near intersection
//        float tNear = max(nearHit.x, max(nearHit.y, nearHit.z));
//        // The smallest far intersection
//        float tFar = min(farHit.x, min(farHit.y, farHit.z));
//
//        // If tNear is greater than tFar, no intersection.
//        // Also ensure tFar is >= 0, so the intersection isn't "behind" the ray.
//        if (tNear > tFar || tFar < 0.0f) {
//            return false;
//        }
//
//        // The ray intersects. If tNear < 0, it means we start inside the box, so take tFar.
//        t = (tNear >= 0.0f) ? tNear : tFar;
//        return true;
//    }
//
//};


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


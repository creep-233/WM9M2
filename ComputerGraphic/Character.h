#pragma once
#include "Collider.h"
#include "GEBase.h"
#include "Mathematic.h"
#include "mesh.h"
#include "anime.h"
#include "Camera.h"
#include <vector>

class Bullet {
public:
    Vec3 pos;
    Vec3 dir;
    float speed;
    bool isActive;
    float damage;
    Sphere bull;
    Vec3 prevPos;

    Bullet(Vec3 _pos, Vec3 _dir, float _spd, float _damage) {
        pos = _pos;
        dir = _dir;
        speed = _spd;
        damage = _damage;
        dir.normalize();
    }
    Bullet() {};

    void Attack() {
        isActive = false;
    }

    void update(float dt, float& hitDist) {
        if (!isActive) return;
        prevPos = pos;
        pos += dir * speed * dt;

        if (pos.getLength() > 100.0f) {
            isActive = false;
        }
    }

    bool checkCollision(const BoundingVolume& enemyBox, float& hitDist) {
        Vec3 segmentDir = pos - prevPos;
        float segmentLength = segmentDir.getLength();
        if (segmentLength == 0.0f) return false;

        segmentDir.normalize();
        RayCaster r(prevPos, segmentDir);

        float t;
        if (r.checkIntersection(enemyBox, t)) {
            if (t <= segmentLength) {
                hitDist = t;
                return true;
            }
        }
        return false;
    }
};


class Player {
public:
    Vec3 position;          // Player's position
    Vec3 facingDirection;   // Direction the player is facing
    float moveSpeed;        // Player's movement speed
    float health;           // Current health
    float maxHealth;        // Maximum health
    int currentAmmo;        // Current ammo count
    int maxAmmo;            // Maximum ammo capacity

    float fireCooldown;     // Cooldown timer for shooting
    float fireInterval;     // Interval between shots
    bool isAlive;           // Is the player still alive?

    std::vector<Bullet> bulletList; // List of bullets fired
    AnimationInstance playerAnimation;
    BoundingVolume hitbox;            // Collision box for the player

    Player(float initialHealth, int ammoCapacity, Vec3 startPosition, Vec3 startDirection, float speed, float fireRate)
        : health(initialHealth), maxHealth(initialHealth), currentAmmo(ammoCapacity), maxAmmo(ammoCapacity),
        position(startPosition), facingDirection(startDirection), moveSpeed(speed), fireInterval(fireRate), fireCooldown(0.0f), isAlive(true) {}

    void init(DXCore& core, TextureManager& textureManager) {
        playerAnimation.initTexture("Resources/model/Soldier1.gem", core, &textureManager);
        currentAmmo = maxAmmo;
        health = maxHealth;
        isAlive = true;
    }

    void takeDamage(float damageAmount) {
        if (isAlive) {
            health -= damageAmount;
            if (health <= 0) {
                health = 0;
                isAlive = false;
            }
        }
    }

    void handleDeath(shader& playerShader, float deltaTime, DXCore& dxcore, Matrix& viewProjMatrix, TextureManager& textureManager, ID3D11DeviceContext* devicecontext) {
        if (!isAlive || health <= 0) {
            playerAnimation.update("death from the front", deltaTime);
            draw(playerShader, deltaTime, dxcore, viewProjMatrix, textureManager, devicecontext);
        }
    }


    void healPlayer(float healAmount) {
        if (isAlive) {
            health = min(health + healAmount, maxHealth);
        }
    }

    void moveWithCamera(const Camera& camera) {
        position = camera.position;
        facingDirection = camera.rotation;
    }

    void reloadWeapon(float deltaTime, float reloadDuration) {
        static float reloadTimer = 0.0f;
        reloadTimer += deltaTime;
        if (reloadTimer >= reloadDuration) {
            currentAmmo = maxAmmo;
            reloadTimer = 0.0f;
        }
    }

    void shoot(float deltaTime, float bulletSpeed, float reloadDuration) {
        if (fireCooldown <= 0.0f && isAlive && currentAmmo > 0) {
            bulletList.emplace_back(position, facingDirection, bulletSpeed, 10.0f);
            currentAmmo--;
            fireCooldown = fireInterval;
        }
        if (currentAmmo <= 0) {
            reloadWeapon(deltaTime, reloadDuration);
        }
    }

    void update(float dt, float hitDistance) {
        if (fireCooldown > 0.0f) {
            fireCooldown -= dt;
            if (fireCooldown < 0.0f) fireCooldown = 0.0f;
        }

        for (auto& bullet : bulletList) {
            bullet.update(dt, hitDistance);
        }
        bulletList.erase(std::remove_if(bulletList.begin(), bulletList.end(), [](const Bullet& b) { return !b.isActive; }), bulletList.end());
    }

    void checkBulletsCollision(Player& player) {
        for (auto& bullet : player.bulletList) {
            if (bullet.isActive) {
                float hitDistance;
                if (bullet.checkCollision(hitbox, hitDistance)) {
                    takeDamage(bullet.damage);
                    bullet.Attack(); 
                }
            }
        }
    }


    void draw(shader& playerShader, float deltaTime, DXCore& dxcore, Matrix& viewProjMatrix, TextureManager& textureManager, ID3D11DeviceContext* devicecontext) {
        playerShader.updateConstantVS("Animated", "animatedMeshBuffer", "VP", &viewProjMatrix);
        playerAnimation.update("walking", deltaTime);
        playerShader.updateConstantVS("Animated", "animatedMeshBuffer", "bones", playerAnimation.matrices);
        Matrix worldMatrix = Matrix::worldTrans(Vec3(0.02f, 0.02f, 0.02f), Vec3(0, 0, 0), position);
        playerShader.updateConstantVS("Animated", "animatedMeshBuffer", "W", &worldMatrix);
        playerShader.apply(&dxcore);
        playerAnimation.drawTexture(&dxcore, playerShader, &textureManager, devicecontext);
    }

};


class Enemy {
public:
    Vec3 position;
    Vec3 moveDirection;
    float health;
    float maxHealth;
    float moveSpeed;
    bool isAlive;
    float damageCooldown; // Time between player hits

    AnimationInstance enemyAnimation;
    BoundingVolume hitbox;

    Enemy(float initialHealth, float speed)
        : position(Vec3(0, 0, 0)), moveDirection(Vec3(1, 0, 0)), health(initialHealth),
        maxHealth(initialHealth), moveSpeed(speed), isAlive(true), damageCooldown(0.0f) {}

    void init(DXCore& core, TextureManager& textureManager) {
        enemyAnimation.initTexture("Resources/TRex.gem", core, &textureManager);
    }

    void takeDamage(float damageAmount) {
        if (isAlive) {
            health -= damageAmount;
            if (health <= 0) {
                health = 0;
                isAlive = false;
            }
        }
    }


    void patrol(float deltaTime, float patrolDistance) {
        static float distanceTraveled = 0.0f;
        if (!isAlive) return;

        position += moveDirection * moveSpeed * deltaTime;
        distanceTraveled += moveSpeed * deltaTime;

        if (distanceTraveled >= patrolDistance) {
            moveDirection = -moveDirection;
            distanceTraveled = 0.0f;
        }

        hitbox.applyTransform(Vec3(0.9f, 0.9f, 0.9f), Vec3(0, 0, 0), position);
    }

    void checkCollisionWithBullet(Bullet& bullet) {
        if (bullet.isActive) {
            float hitDistance;
            if (bullet.checkCollision(hitbox, hitDistance)) {
                takeDamage(bullet.damage);
            }
        }
    }

    void checkCollisionWithPlayer(Player& player) {
        if (hitbox.isOverlapping(player.hitbox)) {
            if (damageCooldown >= 1.0f) {
                player.takeDamage(5.0f);
                damageCooldown = 0.0f;
            }
        }
    }


    void attackPlayer(Player& player, float deltaTime) {
        if (hitbox.isOverlapping(player.hitbox)) {
            damageCooldown += deltaTime;
            if (damageCooldown >= 1.0f) {
                player.takeDamage(5.0f);
                damageCooldown = 0.0f;
            }
        }
    }
};

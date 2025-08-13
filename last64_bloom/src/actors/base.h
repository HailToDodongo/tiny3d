/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

// Forward declarations
namespace Actor
{
  class Player;
  class Enemy;
  class Projectile;
  class Weapon;
  class ProjectileWeapon;
}

namespace Actor
{
  class Base
  {
    private:

    protected:
      fm_vec3_t pos{};

      bool checkFrustumAABB(const fm_vec3_t &aabbMin, const fm_vec3_t &aabbMax) const;
      bool checkFrustumSphere(const fm_vec3_t &center, float radius) const;

    public:
      constexpr static uint32_t FLAG_DISABLED = (1 << 0);

      uint32_t flags{0};

      virtual ~Base() {}
      virtual void update(float deltaTime) = 0;

      virtual void draw3D(float deltaTime) {}
      virtual void drawPTX(float deltaTime) {}
  };
}
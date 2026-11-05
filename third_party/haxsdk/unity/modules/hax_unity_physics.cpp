#include "hax_unity_physics.h"

namespace Unity
{
    System::Type Rigidbody::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.PhysicsModule", "UnityEngine", "Rigidbody", true);
        return s_Type;
    }

    void Rigidbody::AddForce(const Vector3& force, ForceMode mode)
    {
        THROW_IF_NULL();

        static auto s_Method = typeof().GetMethod("AddForce_Injected", nullptr, true).Wrap();
        s_Method.InternalCall<void>(GetCachedPtr(), &force, mode);
    }

    Vector3 Rigidbody::GetVelocity()
    {
        THROW_IF_NULL();

        static auto s_Method = typeof().GetMethod("get_velocity", nullptr, true).Wrap();
        return s_Method.Call<Vector3>(*this);
    }

    void Rigidbody::SetVelocity(const Vector3& value)
    {
        THROW_IF_NULL();

        static auto s_Method = Rigidbody::typeof().GetMethod("set_velocity", nullptr, true).Wrap();
        if constexpr (HAX_UNITY_IS_MONO)
            s_Method.Call<void>(*this, System::Box(value));
        else
            s_Method.Call<void>(*this, value);
    }

    System::Type RaycastHit::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.PhysicsModule", "UnityEngine", "RaycastHit", true);
        return s_Type;
    }

    Transform RaycastHit::GetTransform()
    {
        static auto s_Method = typeof().GetMethod("get_transform", nullptr, true).Wrap();
        if constexpr (HAX_UNITY_IS_MONO)
            return s_Method.Call<Transform>(System::Box(*this));
        else
            return s_Method.Call<Transform>(this);
    }

    System::Type Physics::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.PhysicsModule", "UnityEngine", "Physics", true);
        return s_Type;
    }

    System::Array<RaycastHit> Physics::SphereCastAll(const Vector3& origin, float radius, const Vector3& direction, float maxDistance, int layerMask)
    {
        static auto s_Method = typeof().GetMethod("SphereCastAll", "UnityEngine.RaycastHit[](UnityEngine.Vector3,System.Single,UnityEngine.Vector3,System.Single,System.Int32)", true).Wrap();

        if constexpr (HAX_UNITY_IS_MONO)
            return s_Method.Call<System::Array<RaycastHit>>(System::Box(origin), radius, System::Box(direction), maxDistance, layerMask);
        else
            return s_Method.Call<System::Array<RaycastHit>>(origin, radius, direction, maxDistance, layerMask);
    }

    System::Type Collider::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.PhysicsModule", "UnityEngine", "Collider", true);
        return s_Type;
    }

    bool Collider::GetEnabled()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_enabled", nullptr, true).Wrap();
        return s_Method.Call<bool>(*this);
    }

    void Collider::SetEnabled(bool value)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_enabled", nullptr, true).Wrap();
        return s_Method.Call<void>(*this, value);
    }

    Bounds Collider::GetBounds()
    {
        THROW_IF_NULL();

        static auto s_Method = typeof().GetMethod("get_bounds_Injected", nullptr, true).Wrap();

        Bounds ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);

        return ret;
    }

    System::Type BoxCollider::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.PhysicsModule", "UnityEngine", "BoxCollider2D", true);
        return s_Type;
    }

    Vector3 BoxCollider::GetCenter()
    {
        THROW_IF_NULL();

        static auto s_Method = typeof().GetMethod("get_center_Injected", nullptr, true).Wrap();

        Vector3 ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);

        return ret;
    }

    Vector3 BoxCollider::GetSize()
    {
        THROW_IF_NULL();

        static auto s_Method = typeof().GetMethod("get_size_Injected", nullptr, true).Wrap();

        Vector3 ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);

        return ret;
    }
}
#include "hax_unity_physics2d.h"

namespace Unity
{
    System::Type Collider2D::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.Physics2DModule", "UnityEngine", "Collider2D", true);
        return s_Type;
    }

    Bounds Collider2D::GetBounds()
    {
        THROW_IF_NULL();

        static auto s_Method = typeof().GetMethod("get_bounds_Injected", nullptr, true).Wrap();

        Bounds ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);

        return ret;
    }

    void Collider2D::SetIsTrigger(bool value)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_isTrigger", nullptr, true).Wrap();
        s_Method.Call<void>(*this, value);
    }

    System::Type BoxCollider2D::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.Physics2DModule", "UnityEngine", "BoxCollider2D", true);
        return s_Type;
    }

    System::Type PolygonCollider2D::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.Physics2DModule", "UnityEngine", "PolygonCollider2D", true);
        return s_Type;
    }

    System::Array<Vector2> PolygonCollider2D::GetPoints()
    {
        THROW_IF_NULL();
        static auto s_Method = PolygonCollider2D::typeof().GetMethod("get_points", nullptr, true).Wrap();
        return s_Method.Call<System::Array<Vector2>>(*this);
    }

    void PolygonCollider2D::SetPoints(System::Array<Vector2> value)
    {
        THROW_IF_NULL();
        static auto s_Method = PolygonCollider2D::typeof().GetMethod("set_points", nullptr, true).Wrap();
        s_Method.Call<void>(*this, value);
    }

    System::Type Rigidbody2D::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.Physics2DModule", "UnityEngine", "Rigidbody2D", true);
        return s_Type;
    }
}
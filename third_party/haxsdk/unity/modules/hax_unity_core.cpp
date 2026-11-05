#include "hax_unity_core.h"

namespace Unity
{
    System::Type Vector2::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Vector2", true);
        return s_Type;
    }

    System::Type Vector3::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Vector3", true);
        return s_Type;
    }

    float Vector3::Distance(const Vector3& a, const Vector3& b)
    {
        Vector3 vector(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
        return Hax::Sqrt(vector.X * vector.X + vector.Y * vector.Y + vector.Z * vector.Z);
    }

    float Vector3::Distance(const Vector3& other)
    {
        Vector3 vector(X - other.X, Y - other.Y, Z - other.Z);
        return std::sqrt(vector.X * vector.X + vector.Y * vector.Y + vector.Z * vector.Z);
    }

    Vector3 Vector3::Min(const Vector3& a, const Vector3& b)
    {
        return Vector3((Hax::Min)(a.X, b.X), (Hax::Min)(a.Y, b.Y), (Hax::Min)(a.Z, b.Z));
    }

    Vector3 Vector3::Max(const Vector3& a, const Vector3& b)
    {
        return Vector3(Hax::Max(a.X, b.X), Hax::Max(a.Y, b.Y), Hax::Max(a.Z, b.Z));
    }

    Vector3 Vector3::Normalized(const Vector3& v)
    {
        float mag = v.GetMagnitude();
        if (mag > 0.00001f)
            return v / mag;

        return Vector3::zero();
    }

    Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2)
    {
        return Vector3(v1.Y * v2.Z - v1.Z * v2.Y, v1.Z * v2.X - v1.X * v2.Z, v1.X * v2.Y - v1.Y * v2.X);
    }

    float Vector3::Dot(const Vector3& v1, const Vector3& v2)
    {
        return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
    }

    System::Type Vector4::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Vector4", true);
        return s_Type;
    }

    System::Type Quaternion::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Quaternion", true);
        return s_Type;
    }

    Quaternion Quaternion::Euler(float x, float y, float z)
    {
        x *= 0.5f;
        y *= 0.5f;
        z *= 0.5f;

        const float cx = cosf(x), sx = sinf(x);
        const float cy = cosf(y), sy = sinf(y);
        const float cz = cosf(z), sz = sinf(z);

        return Quaternion(sx * cy * cz + cx * sy * sz,
                          cx * sy * cz - sx * cy * sz,
                          cx * cy * sz - sx * sy * cz,
                          cx * cy * cz + sx * sy * sz);
    }

    Vector3 Quaternion::operator*(const Vector3& point) const
    {
        float num = X * 2.f;
        float num2 = Y * 2.f;
        float num3 = Z * 2.f;
        float num4 = X * num;
        float num5 = Y * num2;
        float num6 = Z * num3;
        float num7 = X * num2;
        float num8 = X * num3;
        float num9 = Y * num3;
        float num10 = W * num;
        float num11 = W * num2;
        float num12 = W * num3;
        Vector3 result;
        result.X = (1.f - (num5 + num6)) * point.X + (num7 - num12) * point.Y + (num8 + num11) * point.Z;
        result.Y = (num7 + num12) * point.X + (1.f - (num4 + num6)) * point.Y + (num9 - num10) * point.Z;
        result.Z = (num8 - num11) * point.X + (num9 + num10) * point.Y + (1.f - (num4 + num5)) * point.Z;
        return result;
    }

    System::Type Bounds::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Bounds", true);
        return s_Type;
    }

    System::Type Color::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Color", true);
        return s_Type;
    }

    System::Type Matrix4x4::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Matrix4x4", true);
        return s_Type;
    }

    Vector4 Matrix4x4::operator*(const Vector4& v) const
    {
        Vector4 result;
        result.X = m00 * v.X + m01 * v.Y + m02 * v.Z + m03 * v.W;
        result.Y = m10 * v.X + m11 * v.Y + m12 * v.Z + m13 * v.W;
        result.Z = m20 * v.X + m21 * v.Y + m22 * v.Z + m23 * v.W;
        result.W = m30 * v.X + m31 * v.Y + m32 * v.Z + m33 * v.W;
        return result;
    }

    Vector3 Matrix4x4::MultiplyPoint(const Vector3& point) const
    {
        Vector3 result;
        result.X = m00 * point.X + m01 * point.Y + m02 * point.Z + m03;
        result.Y = m10 * point.X + m11 * point.Y + m12 * point.Z + m13;
        result.Z = m20 * point.X + m21 * point.Y + m22 * point.Z + m23;
        float num = m30 * point.X + m31 * point.Y + m32 * point.Z + m33;
        num = 1.f / num;
        result.X *= num;
        result.Y *= num;
        result.Z *= num;
        return result;
    }

    System::Type Rect::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Rect", true);
        return s_Type;
    }

    System::Type AsyncOperation::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "AsyncOperation", true);
        return s_Type;
    }

    bool AsyncOperation::GetIsDone()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_IsDone", nullptr, true).Wrap();
        return s_Method.Call<bool>(*this);
    }

    float AsyncOperation::GetProgress()
    {
        THROW_IF_NULL();
        static auto s_Method = AsyncOperation::typeof().GetMethod("get_progress", nullptr, true).Wrap();
        return s_Method.Call<float>(*this);
    }

    System::Type Object::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Object", true);
        return s_Type;
    }

    Object Object::Instantiate(Object original)
    {
        static auto s_Method = typeof().GetMethod("Instantiate", "UnityEngine.Object(UnityEngine.Object)", true).Wrap();
        return s_Method.Call<Object>(original);
    }

    Object Object::Instantiate(Object original, const Vector3& position, const Quaternion& rotation)
    {
        static auto s_Method = typeof().GetMethod("Internal_InstantiateSingle_Injected", nullptr, true).Wrap();
        return s_Method.InternalCall<Object>(original.GetCachedPtr(), &position, &rotation);
    }

    System::Array<Object> Object::FindObjectsOfType(System::Type type)
    {
        static auto s_Method = typeof().GetMethod("FindObjectsOfType", "UnityEngine.Object[](System.Type)", true).Wrap();
        return s_Method.Call<System::Array<Object>>(type);
    }

    System::Array<Object> Object::FindObjectsOfTypeAll(System::Type type)
    {
        static auto s_Method = typeof().GetMethod("FindObjectsOfTypeAll", "UnityEngine.Object[](System.Type)", true).Wrap();
        return s_Method.Call<System::Array<Object>>(type);
    }

    Object Object::FindObjectOfType(System::Type type, bool includeInactive)
    {
        static auto s_Method = typeof().GetMethod("FindObjectOfType", "UnityEngine.Object(System.Type,System.Boolean)", true).Wrap();
        return s_Method.Call<Object>(type, includeInactive);
    }

    void Object::Destroy(Object obj)
    {
        static auto s_Method = typeof().GetMethod("Destroy", "System.Void(UnityEngine.Object)", true).Wrap();
        return s_Method.Call<void>(obj);
    }

    System::String Object::GetName()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_name", nullptr, true).Wrap();
        return s_Method.Call<System::String>(*this);
    }

    void Object::SetName(System::String name)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_name", nullptr, true).Wrap();
        s_Method.Call<void>(*this, name);
    }

    void Object::SetHideFlags(HideFlags flags)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_hideFlags", nullptr, true).Wrap();
        s_Method.Call<void>(*this, (int)flags);
    }

    int Object::GetInstanceID()
    {
        THROW_IF_NULL();
        static auto s_Method = Object::typeof().GetMethod("GetInstanceID", nullptr, true).Wrap();
        return s_Method.Call<int>(*this);
    }

    System::Type Component::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Component", true);
        return s_Type;
    }

    Transform Component::GetTransform() const
    {
        THROW_IF_NULL();
        static auto s_Method = Component::typeof().GetMethod("get_transform", nullptr, true).Wrap();
        return s_Method.Call<Transform, Component>(*this);
    }

    GameObject Component::GetGameObject() const
    {
        THROW_IF_NULL();
        static auto s_Method = Component::typeof().GetMethod("get_gameObject", nullptr, true).Wrap();
        return s_Method.Call<GameObject, Component>(*this);
    }

    Component Component::GetComponentInChildren(System::Type type) const
    {
        THROW_IF_NULL();
        static auto s_Method = Component::typeof().GetMethod("GetComponentInChildren", "UnityEngine.Component(System.Type)", true).Wrap();
        return s_Method.Call<Component, Component, System::Type>(*this, type);
    }

    System::Array<Component> Component::GetComponentsInChildren(System::Type type) const
    {
        THROW_IF_NULL();
        static auto s_Method = Component::typeof().GetMethod("GetComponentsInChildren", "UnityEngine.Component[](System.Type)", true).Wrap();
        return s_Method.Call<System::Array<Component>, Component, System::Type>(*this, type);
    }

    System::Array<Component> Component::GetComponentsInChildren(System::Type type, bool includeInactive) const
    {
        THROW_IF_NULL();
        static auto s_Method = Component::typeof().GetMethod("GetComponentsInChildren", "UnityEngine.Component[](System.Type,System.Boolean)", true).Wrap();
        return s_Method.Call<System::Array<Component>, Component, System::Type, bool>(*this, type, includeInactive);
    }

    Component Component::GetComponent(System::Type type) const
    {
        THROW_IF_NULL();
        static auto s_Method = Component::typeof().GetMethod("GetComponent", "UnityEngine.Component(System.Type)", true).Wrap();
        return s_Method.Call<Component, Component, System::Type>(*this, type);
    }

    System::Type Transform::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Transform", true);
        return s_Type;
    }

    Vector3 Transform::GetPosition()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_position_Injected", nullptr, true).Wrap();
        Vector3 ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);
        return ret;
    }

    void Transform::SetPosition(const Vector3& value)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_position_Injected", nullptr, true).Wrap();
        s_Method.InternalCall<void>(GetCachedPtr(), &value);
    }

    Vector3 Transform::GetLocalPosition()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_localPosition_Injected", nullptr, true).Wrap();
        Vector3 ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);
        return ret;
    }

    void Transform::SetLocalPosition(const Vector3& value)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_localPosition_Injected", nullptr, true).Wrap();
        s_Method.InternalCall<void>(GetCachedPtr(), &value);
    }

    Quaternion Transform::GetRotation()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_rotation_Injected", nullptr, true).Wrap();
        Quaternion ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);
        return ret;
    }

    void Transform::SetRotation(const Quaternion& value)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_rotation_Injected", nullptr, true).Wrap();
        s_Method.InternalCall<void>(GetCachedPtr(), &value);
    }

    Transform Transform::GetParent()
    {
        THROW_IF_NULL();
        static auto s_Method = Transform::typeof().GetMethod("get_parent", nullptr, true).Wrap();
        return s_Method.Call<Transform>(*this);
    }

    void Transform::SetParent(Transform value)
    {
        THROW_IF_NULL();
        static auto s_Method = Transform::typeof().GetMethod("set_parent", nullptr, true).Wrap();
        return s_Method.Call<void>(*this, value);
    }

    Vector3 Transform::GetForward()
    {
        THROW_IF_NULL();
        return GetRotation() * Vector3::forward();
    }

    Vector3 Transform::GetUp()
    {
        THROW_IF_NULL();
        return GetRotation() * Vector3::up();
    }

    Vector3 Transform::GetLocalScale()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_localScale_Injected", nullptr, true).Wrap();
        Vector3 ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);
        return ret;
    }

    void Transform::SetLocalScale(const Vector3& value)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_localScale_Injected", nullptr, true).Wrap();
        s_Method.InternalCall<void>(GetCachedPtr(), &value);
    }

    Transform Transform::Find(System::String n)
    {
        THROW_IF_NULL();
        static auto s_Method = Transform::typeof().GetMethod("Find", nullptr, true).Wrap();
        return s_Method.Call<Transform>(*this, n);
    }

    Transform Transform::GetChild(int index)
    {
        THROW_IF_NULL();
        static auto s_Method = Transform::typeof().GetMethod("GetChild", nullptr, true).Wrap();
        return s_Method.Call<Transform>(*this, index);
    }

    System::Type Behaviour::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Behaviour", true);
        return s_Type;
    }

    bool Behaviour::GetEnabled()
    {
        THROW_IF_NULL();
        static auto s_Method = Behaviour::typeof().GetMethod("get_enabled", nullptr, true).Wrap();
        return s_Method.Call<bool, Behaviour>(*this);
    }

    void Behaviour::SetEnabled(bool value)
    {
        THROW_IF_NULL();
        static auto s_Method = Behaviour::typeof().GetMethod("set_enabled", nullptr, true).Wrap();
        return s_Method.Call<void, Behaviour, bool>(*this, value);
    }

    bool Behaviour::GetIsActiveAndEnabled()
    {
        THROW_IF_NULL();
        static auto s_Method = Behaviour::typeof().GetMethod("get_isActiveAndEnabled", nullptr, true).Wrap();
        return s_Method.Call<bool, Behaviour>(*this);
    }

    System::Type MonoBehaviour::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "MonoBehaviour", true);
        return s_Type;
    }

    System::Type Time::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Time", true);
        return s_Type;
    }

    float Time::GetTimeScale()
    {
        static auto s_Method = typeof().GetMethod("get_timeScale", nullptr, true).Wrap();
        return s_Method.InternalCall<float>();
    }

    void Time::SetTimeScale(float scale)
    {
        static auto s_Method = typeof().GetMethod("set_timeScale", nullptr, true).Wrap();
        s_Method.InternalCall<void>(scale);
    }

    GameObject GameObject::New()
    {
        return System::Activator::CreateInstanceDefaultCtor<GameObject>();
    }

    GameObject GameObject::New(const char* name)
    {
        static auto s_Method = GameObject::typeof().GetMethod(".ctor", "System.Void(System.String)", true).Wrap();
        
        GameObject go = System::Activator::CreateInstance<GameObject>();
        s_Method.Call<void>(go, System::String::New(name));

        return go;
    }

    System::Type GameObject::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "GameObject", true);
        return s_Type;
    }

    Transform GameObject::GetTransform()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_transform", nullptr, true).Wrap();
        return s_Method.Call<Transform>(*this);
    }

    int GameObject::GetLayer()
    {
        THROW_IF_NULL();
        static auto s_Method = GameObject::typeof().GetMethod("get_layer", nullptr, true).Wrap();
        return s_Method.Call<int, GameObject>(*this);
    }

    void GameObject::SetLayer(int value)
    {
        THROW_IF_NULL();
        static auto s_Method = GameObject::typeof().GetMethod("set_layer", nullptr, true).Wrap();
        return s_Method.Call<void, GameObject, int>(*this, value);
    }

    void GameObject::SetTag(System::String tag)
    {
        THROW_IF_NULL();
        static auto s_Method = GameObject::typeof().GetMethod("set_tag", nullptr, true).Wrap();
        return s_Method.Call<void, GameObject, System::String>(*this, tag);
    }

    bool GameObject::CompareTag(System::String tag)
    {
        THROW_IF_NULL();
        static auto s_Method = GameObject::typeof().GetMethod("CompareTag", "System.Boolean(System.String)", true).Wrap();
        return s_Method.Call<bool>(*this, tag);
    }

    bool GameObject::GetActiveSelf()
    {
        THROW_IF_NULL();
        static auto s_Method = GameObject::typeof().GetMethod("get_activeSelf", nullptr, true).Wrap();
        return s_Method.Call<bool, GameObject>(*this);
    }

    void GameObject::SetActive(bool value)
    {
        THROW_IF_NULL();
        static auto s_Method = GameObject::typeof().GetMethod("set_active", nullptr, true).Wrap();
        return s_Method.Call<void>(*this, value);
    }

    bool GameObject::GetActive()
    {
        THROW_IF_NULL();
        static auto s_Method = GameObject::typeof().GetMethod("get_active", nullptr, true).Wrap();
        return s_Method.Call<bool>(*this);
    }

    bool GameObject::GetActiveInHierarchy()
    {
        THROW_IF_NULL();
        static auto s_Method = GameObject::typeof().GetMethod("get_activeInHierarchy", nullptr, true).Wrap();
        return s_Method.Call<bool>(*this);
    }

    Component GameObject::GetComponent(System::Type type)
    {
        THROW_IF_NULL();
        static auto s_Method = GameObject::typeof().GetMethod("GetComponent", "UnityEngine.Component(System.Type)", true).Wrap();
        return s_Method.Call<Component>(*this, type);
    }

    Component GameObject::AddComponent(System::Type componentType)
    {
        THROW_IF_NULL();
        static auto s_Method = GameObject::typeof().GetMethod("AddComponent", "UnityEngine.Component(System.Type)", true).Wrap();
        return s_Method.Call<Component>(*this, componentType);
    }

    System::Array<Component> GameObject::GetComponentsInChildren(System::Type type, bool includeInactive)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("GetComponentsInChildren", "Component[](System.Type,System.Boolean)", true).Wrap();
        return s_Method.Call<System::Array<Component>>(*this, type, includeInactive);
    }

    System::Type LayerMask::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "LayerMask", true);
        return s_Type;
    }

    int LayerMask::NameToLayer(System::String name)
    {
        static auto s_Method = typeof().GetMethod("NameToLayer", nullptr, true).Wrap();
        return s_Method.Call<int, System::String>(name);
    }

    CommandBuffer CommandBuffer::New()
    {
        return System::Activator::CreateInstanceDefaultCtor<CommandBuffer>();
    }

    System::Type CommandBuffer::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine.Rendering", "CommandBuffer", true);
        return s_Type;
    }

    void CommandBuffer::DrawRenderer(Renderer renderer, Material material)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("DrawRenderer", "System.Void(UnityEngine.Renderer,UnityEngine.Material)", true).Wrap();
        s_Method.Call<void>(*this, renderer, material);
    }

    void CommandBuffer::DrawRenderer(Renderer renderer, Material material, int submeshIndex, int shaderPass)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("DrawRenderer", "System.Void(UnityEngine.Renderer,UnityEngine.Material,System.Int32,System.Int32)", true).Wrap();
        s_Method.Call<void>(*this, renderer, material, submeshIndex, shaderPass);
    }

    void CommandBuffer::Clear()
    {
        THROW_IF_NULL();
        static auto s_Method = CommandBuffer::typeof().GetMethod("Clear", nullptr, true).Wrap();
        s_Method.Call<void>(*this);
    }

    System::Type Camera::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Camera", true);
        return s_Type;
    }

    Camera Camera::GetMain()
    {
        static auto s_Method = Camera::typeof().GetMethod("get_main", nullptr, true).Wrap();
        return s_Method.Call<Camera>();
    }

    Vector3 Camera::WorldToScreenPoint(const Vector3& worldPos, MonoOrStereoscopicEye eye)
    {
        static auto s_Method = typeof().GetMethod("WorldToScreenPoint_Injected", nullptr, true).Wrap();

        Vector3 ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &worldPos, (int)eye, &ret);

        return ret;
    }

    void Camera::RemoveCommandBuffer(CameraEvent event, CommandBuffer cb)
    {
        THROW_IF_NULL();
        static auto s_Method = Camera::typeof().GetMethod("RemoveCommandBuffer", nullptr, true).Wrap();
        s_Method.Call<void>(*this, event, cb);
    }

    void Camera::AddCommandBuffer(CameraEvent event, CommandBuffer cb)
    {
        THROW_IF_NULL();
        static auto s_Method = Camera::typeof().GetMethod("AddCommandBuffer", nullptr, true).Wrap();
        s_Method.Call<void>(*this, event, cb);
    }

    float Camera::GetOrthographicSize()
    {
        THROW_IF_NULL();
        static auto s_Method = Camera::typeof().GetMethod("get_orthographicSize", nullptr, true).Wrap();
        return s_Method.Call<float>(*this);
    }

    void Camera::SetOrthographicSize(float value)
    {
        THROW_IF_NULL();
        static auto s_Method = Camera::typeof().GetMethod("set_orthographicSize", nullptr, true).Wrap();
        return s_Method.Call<void>(*this, value);
    }

    int Camera::GetPixelWidth()
    {
        THROW_IF_NULL();
        static auto s_Method = Camera::typeof().GetMethod("get_pixelWidth", nullptr, true).Wrap();
        return s_Method.Call<int>(*this);
    }

    int Camera::GetPixelHeight()
    {
        THROW_IF_NULL();
        static auto s_Method = Camera::typeof().GetMethod("get_pixelHeight", nullptr, true).Wrap();
        return s_Method.Call<int>(*this);
    }

    Rect Camera::GetPixelRect()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_pixelRect_Injected", nullptr, true).Wrap();
        Rect ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);
        return ret;
    }

    Matrix4x4 Camera::GetProjectionMatrix()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_projectionMatrix_Injected", nullptr, true).Wrap();
        Matrix4x4 ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);

        return ret;
    }

    Matrix4x4 Camera::GetWorldToCameraMatrix()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_worldToCameraMatrix_Injected", nullptr, true).Wrap();
        Matrix4x4 ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);
        return ret;
    }

    float Camera::GetFarClipPlane()
    {
        THROW_IF_NULL();
        static auto s_Method = Camera::typeof().GetMethod("get_farClipPlane", nullptr, true).Wrap();
        return s_Method.Call<float>(*this);
    }

    void Camera::SetFarClipPlane(float value)
    {
        THROW_IF_NULL();
        static auto s_Method = Camera::typeof().GetMethod("set_farClipPlane", nullptr, true).Wrap();
        s_Method.Call<void>(*this, value);
    }

    float Camera::GetNearClipPlane()
    {
        THROW_IF_NULL();
        static auto s_Method = Camera::typeof().GetMethod("get_nearClipPlane", nullptr, true).Wrap();
        return s_Method.Call<float>(*this);
    }

    float Camera::GetFieldOfView()
    {
        THROW_IF_NULL();
        static auto s_Method = Camera::typeof().GetMethod("get_fieldOfView", nullptr, true).Wrap();
        return s_Method.Call<float>(*this);
    }

    void Camera::SetFieldOfView(float value)
    {
        THROW_IF_NULL();
        static auto s_Method = Camera::typeof().GetMethod("set_fieldOfView", nullptr, true).Wrap();
        s_Method.Call<void>(*this, value);
    }

    System::Type Cursor::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Cursor", true);
        return s_Type;
    }

    void Cursor::SetVisible(bool value)
    {
        static auto s_Method = typeof().GetMethod("set_visible", nullptr, true).Wrap();
        return s_Method.Call<void>(value);
    }

    System::Type Light::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Light", true);
        return s_Type;
    }

    void Light::SetIntensity(float value)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_intensity", nullptr, true).Wrap();
        s_Method.Call<void>(*this, value);
    }

    float Light::GetIntensity()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_intensity", nullptr, true).Wrap();
        return s_Method.Call<float>(*this);
    }

    void Light::SetRange(float value)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_range", nullptr, true).Wrap();
        s_Method.Call<void>(*this, value);
    }

    float Light::GetRange()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_range", nullptr, true).Wrap();
        return s_Method.Call<float>(*this);
    }

    void Light::SetSpotAngle(float value)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_spotAngle", nullptr, true).Wrap();
        s_Method.Call<void>(*this, value);
    }

    float Light::GetSpotAngle()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_spotAngle", nullptr, true).Wrap();
        return s_Method.Call<float>(*this);
    }

    System::Type Shader::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Shader", true);
        return s_Type;
    }

    Shader Shader::Find(System::String name)
    {
        static auto s_Method = Shader::typeof().GetMethod("Find", nullptr, true).Wrap();
        return s_Method.Call<Shader>(name);
    }

    void Shader::WarmupAllShaders()
    {
        static auto s_Method = Shader::typeof().GetMethod("WarmupAllShaders", nullptr, true).Wrap();
        s_Method.Call<void>();
    }

    bool Shader::IsSupported()
    {
        THROW_IF_NULL();
        static auto s_Method = Shader::typeof().GetMethod("get_isSupported", nullptr, true).Wrap();
        return s_Method.Call<bool>(*this);
    }

    System::Type Material::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Material", true);
        return s_Type;
    }

    Material Material::New(Shader shader)
    {
        static auto s_Method = Material::typeof().GetMethod(".ctor", "System.Void(UnityEngine.Shader)", true).Wrap();
        Material mat = System::Activator::CreateInstance<Material>();
        s_Method.Call<void>(mat, shader);
        return mat;
    }

    void Material::SetInt(System::String name, int value)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("SetInt", "System.Void(System.String,System.Int32)", true).Wrap();
        s_Method.Call<void>(*this, name, value);
    }

    void Material::SetColor(Color color)
    {
        THROW_IF_NULL();
        static auto s_Method = Material::typeof().GetMethod("set_color", nullptr, true).Wrap();

        if constexpr (HAX_UNITY_IS_MONO)
            s_Method.Call<void>(*this, System::Box(color));
        else
            s_Method.Call<void>(*this, color);
    }

    void Material::SetColor(System::String name, Color color)
    {
        THROW_IF_NULL();
        static auto s_Method = Material::typeof().GetMethod("SetColor", "System.Void(System.String,UnityEngine.Color)", true).Wrap();
        if constexpr (HAX_UNITY_IS_MONO)
            s_Method.Call<void>(*this, name, System::Box(color));
        else
            s_Method.Call<void>(*this, name, color);
    }

    int Material::GetPassCount()
    {
        THROW_IF_NULL();
        static auto s_Method = Material::typeof().GetMethod("get_passCount", nullptr, true).Wrap();
        return s_Method.Call<int>(*this);
    }

    System::Type Renderer::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Renderer", true);
        return s_Type;
    }

    Bounds Renderer::GetBounds()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_bounds_Injected", nullptr, true).Wrap();
        Bounds ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);
        return ret;
    }

    bool Renderer::GetEnabled()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_enabled", nullptr, true).Wrap();
        return s_Method.Call<bool>(*this);
    }

    Material Renderer::GetSharedMaterial()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_sharedMaterial", nullptr, true).Wrap();
        return s_Method.Call<Material>(*this);
    }

    void Renderer::SetSharedMaterial(Material material)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_sharedMaterial", nullptr, true).Wrap();
        s_Method.Call<void>(*this, material);
    }

    void Renderer::SetMaterial(Material material)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_material", nullptr, true).Wrap();
        s_Method.Call<void>(*this, material);
    }

    void Renderer::SetEnabled(bool value)
    {
        THROW_IF_NULL();
        static auto s_Method = Renderer::typeof().GetMethod("set_enabled", nullptr, true).Wrap();
        s_Method.Call<void>(*this, value);
    }

    void Renderer::SetSortingOrder(int value)
    {
        THROW_IF_NULL();
        static auto s_Method = Renderer::typeof().GetMethod("set_sortingOrder", nullptr, true).Wrap();
        s_Method.Call<void>(*this, value);
    }

    bool Renderer::IsVisible()
    {
        THROW_IF_NULL();
        static auto s_Method = Renderer::typeof().GetMethod("get_isVisible", nullptr, true).Wrap();
        return s_Method.Call<bool>(*this);
    }

    System::Type SpriteRenderer::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "SpriteRenderer", true);
        return s_Type;
    }

    Color SpriteRenderer::GetColor()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_color_Injected", nullptr, true).Wrap();
        Color ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);
        return ret;
    }

    void SpriteRenderer::SetColor(const Color& color)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_color_Injected", nullptr, true).Wrap();
        return s_Method.InternalCall<void>(GetCachedPtr(), &color);
    }

    System::Type MeshRenderer::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "MeshRenderer", true);
        return s_Type;
    }

    System::Type LineRenderer::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "LineRenderer", true);
        return s_Type;
    }

    void LineRenderer::SetStartWidth(float value)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_startWidth", nullptr, true).Wrap();
        s_Method.Call<void>(*this, value);
    }

    void LineRenderer::SetEndWidth(float value)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_endWidth", nullptr, true).Wrap();
        s_Method.Call<void>(*this, value);
    }

    void LineRenderer::SetPositionCount(int value)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("set_positionCount", nullptr, true).Wrap();
        s_Method.Call<void>(*this, value);
    }

    void LineRenderer::SetPosition(int index, const Vector3& position)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("SetPosition_Injected", nullptr, true).Wrap();
        return s_Method.InternalCall<void>(GetCachedPtr(), index, &position);
    }

    System::Type SkinnedMeshRenderer::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "SkinnedMeshRenderer", true);
        return s_Type;
    }

    System::Type AssetBundle::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.AssetBundleModule", "UnityEngine", "AssetBundle", true);
        return s_Type;
    }

    AssetBundle AssetBundle::LoadFromMemory(System::Array<System::Char> binary)
    {
        static auto s_Method = typeof().GetMethod("LoadFromMemory", "UnityEngine.AssetBundle(System.Byte[])", true).Wrap();
        return s_Method.Call<AssetBundle>(binary);
    }

    AssetBundle AssetBundle::LoadFromFile(System::String path)
    {
        static auto s_Method = typeof().GetMethod("LoadFromFile", "UnityEngine.AssetBundle(System.String)", true).Wrap();
        return s_Method.Call<AssetBundle>(path);
    }

    Object AssetBundle::LoadAsset(System::String name, System::Type type)
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("LoadAsset", "UnityEngine.Object(System.String,System.Type)", true).Wrap();
        return s_Method.Call<Object>(*this, name, type);
    }

    System::Type Application::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Application", true);
        return s_Type;
    }

    System::String Application::GetDataPath()
    {
        static auto s_Method = typeof().GetMethod("get_dataPath", nullptr, true).Wrap();
        return s_Method.Call<System::String>();
    }

    System::Type Screen::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Screen", true);
        return s_Type;
    }

    int Screen::GetHeight()
    {
        static auto s_Method = Screen::typeof().GetMethod("get_height", nullptr, true).Wrap();
        return s_Method.Call<int>();
    }

    int Screen::GetWidth()
    {
        static auto s_Method = Screen::typeof().GetMethod("get_width", nullptr, true).Wrap();
        return s_Method.Call<int>();
    }

    System::Type Sprite::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Sprite", true);
        return s_Type;
    }

    System::Array<Vector2> Sprite::GetUV()
    {
        THROW_IF_NULL();
        static auto s_Method = Sprite::typeof().GetMethod("get_uv", nullptr, true).Wrap();
        return s_Method.Call<System::Array<Vector2>>(*this);
    }

    Vector4 Sprite::GetOuterUVs()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("GetInnerUVs_Injected", nullptr, true).Wrap();
        Vector4 ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);
        return ret;
    }

    Rect Sprite::GetRect()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_rect_Injected", nullptr, true).Wrap();
        Rect ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);
        return ret;
    }

    Rect Sprite::GetTextureRect()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("GetTextureRect_Injected", nullptr, true).Wrap();
        Rect ret;
        s_Method.InternalCall<void>(GetCachedPtr(), &ret);
        return ret;
    }

    Texture2D Sprite::GetTexture()
    {
        THROW_IF_NULL();
        static auto s_Method = Sprite::typeof().GetMethod("get_texture", nullptr, true).Wrap();
        return s_Method.Call<Texture2D>(*this);
    }

    System::Type RenderSettings::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "RenderSettings", true);
        return s_Type;
    }

    bool RenderSettings::GetFog()
    {
        static auto s_Method = typeof().GetMethod("get_fog", nullptr, true).Wrap();
        return s_Method.Call<bool>();
    }

    void RenderSettings::SetFog(bool value)
    {
        static auto s_Method = typeof().GetMethod("set_fog", nullptr, true).Wrap();
        s_Method.Call<void>(value);
    }

    System::Type Texture::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Texture", true);
        return s_Type;
    }

    void* Texture::GetNativeTexturePtr()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("GetNativeTexturePtr", nullptr, true).Wrap();
        return s_Method.Call<void*>(*this);
    }

    int Texture::GetDataWidth()
    {
        THROW_IF_NULL();
        static auto s_Method = Texture::typeof().GetMethod("GetDataWidth", nullptr, true).Wrap();
        return s_Method.Call<int>(*this);
    }

    int Texture::GetDataHeight()
    {
        THROW_IF_NULL();
        static auto s_Method = Texture::typeof().GetMethod("GetDataHeight", nullptr, true).Wrap();
        return s_Method.Call<int>(*this);
    }

    System::Type Texture2D::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Texture2D", true);
        return s_Type;
    }

    System::Type PlayerLoopSystemInternal::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine.LowLevel", "PlayerLoopSystemInternal", true);
        return s_Type;
    }

    System::Type PlayerLoopSystem::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine.LowLevel", "PlayerLoopSystem", true);
        return s_Type;
    }

    System::Type LowLevel::PlayerLoop::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine.LowLevel", "PlayerLoop", true);
        return s_Type;
    }

    System::Array<PlayerLoopSystemInternal> LowLevel::PlayerLoop::GetDefaultPlayerLoopInternal()
    {
        static auto s_Method = PlayerLoop::typeof().GetMethod("GetDefaultPlayerLoopInternal", nullptr, true).Wrap();
        return s_Method.Call<System::Array<PlayerLoopSystemInternal>>();
    }

    PlayerLoopSystem LowLevel::PlayerLoop::GetDefaultPlayerLoop()
    {
        static auto s_Method = LowLevel::PlayerLoop::typeof().GetMethod("GetDefaultPlayerLoop", nullptr, true).Wrap();
        if constexpr (HAX_UNITY_IS_MONO)
            return s_Method.Call<System::Boxed<PlayerLoopSystem>>().Unbox();
        else
            return s_Method.Call<PlayerLoopSystem>();
    }

    System::Type ResourceRequest::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "ResourceRequest", true);
        return s_Type;
    }

    Unity::Object ResourceRequest::GetAsset()
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("get_asset", nullptr, true).Wrap();
        return s_Method.Call<Unity::Object>(*this);
    }

    System::Type Resources::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine", "Resources", true);
        return s_Type;
    }

    ResourceRequest Resources::LoadAsync(System::String path)
    {
        static auto s_Method = typeof().GetMethod("LoadAsync", "UnityEngine.ResourceRequest(System.String)", true).Wrap();
        return s_Method.Call<ResourceRequest>(path);
    }

    System::Type PlayerLoop::PostLateUpdate::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine.PlayerLoop", "PostLateUpdate", true);
        return s_Type;
    }

    System::Type PlayerLoop::PlayerSendFrameStarted::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine.PlayerLoop", "PostLateUpdate/PlayerSendFrameStarted", true);
        return s_Type;
    }

    System::Type PlayerLoop::PresentAfterDraw::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine.PlayerLoop", "PostLateUpdate/PresentAfterDraw", true);
        return s_Type;
    }

    System::Type PlayerLoop::Update::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine.PlayerLoop", "Update", true);
        return s_Type;
    }

    System::Type PlayerLoop::ScriptRunBehaviourUpdate::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.CoreModule", "UnityEngine.PlayerLoop", "Update/ScriptRunBehaviourUpdate", true);
        return s_Type;
    }

    System::Type EventSystem::typeof()
    {
        static System::Type s_Type = System::Type::GetType("UnityEngine.UI", "UnityEngine.EventSystems", "EventSystem", true);
        return s_Type;
    }

    EventSystem EventSystem::GetCurrent()
    {
        static auto s_Method = typeof().GetMethod("get_current", nullptr, true).Wrap();
        return s_Method.Call<EventSystem>();
    }

    System::Type PostProcessLayer::typeof()
    {
        static System::Type s_Type = System::Type::GetType("Unity.Postprocessing.Runtime", "UnityEngine.Rendering.PostProcessing", "PostProcessLayer", true);
        return s_Type;
    }

    CommandBuffer PostProcessLayer::m_LegacyCmdBufferBeforeReflections()
    {
        static int s_Offset = typeof().GetField("m_LegacyCmdBufferBeforeReflections", true).GetFieldOffset();
        return *(CommandBuffer*)((char*)m_Ptr + s_Offset);
    }

    CommandBuffer PostProcessLayer::m_LegacyCmdBufferBeforeLighting()
    {
        static int s_Offset = typeof().GetField("m_LegacyCmdBufferBeforeLighting", true).GetFieldOffset();
        return *(CommandBuffer*)((char*)m_Ptr + s_Offset);
    }

    CommandBuffer PostProcessLayer::m_LegacyCmdBufferOpaque()
    {
        static int s_Offset = typeof().GetField("m_LegacyCmdBufferOpaque", true).GetFieldOffset();
        return *(CommandBuffer*)((char*)m_Ptr + s_Offset);
    }

    CommandBuffer PostProcessLayer::m_LegacyCmdBuffer()
    {
        static int s_Offset = typeof().GetField("m_LegacyCmdBuffer", true).GetFieldOffset();
        return *(CommandBuffer*)((char*)m_Ptr + s_Offset);
    }
}
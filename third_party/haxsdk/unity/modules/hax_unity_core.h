#pragma once

#include "hax_unity_mscorlib.h"

namespace Unity
{
    enum class HideFlags : int;
    enum class CursorLockMode : int;

    class GameObject;
    class Transform;
    class Texture2D;
    class Object;
    class Component;

    template <typename T>
    concept IsObject = std::derived_from<T, Object>;

    template <typename T>
    concept IsComponent = std::derived_from<T, Component>;

    enum class HideFlags : int
    {
        None                    = 0,
        HideInHierarchy         = 1,
        HideInInspector         = 2,
        DontSaveInEditor        = 4,
        NotEditable             = 8,
        DontSaveInBuild         = 16,
        DontUnloadUnusedAsset   = 32,
        DontSave                = 52,
        HideAndDontSave         = 61
    };

    enum class CursorLockMode : int
    {
        None                    = 0,
        Locked                  = 1,
        Confined                = 2
    };

    enum class ForceMode : int
    {
        Force,
        Acceleration            = 5,
        Impulse                 = 1,
        VelocityChange
    };

    struct Vector3
    {
                                        Vector3() = default;
                                        Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}
                                        Vector3(float x, float y) : X(x), Y(y), Z(0.f) {}

        static System::Type             typeof();

        Vector3                         operator+(const Vector3& a) const { return Vector3(X + a.X, Y + a.Y, Z + a.Z); }
        Vector3                         operator-(const Vector3& a) const { return Vector3(X - a.X, Y - a.Y, Z - a.Z); }
        Vector3                         operator*(float mult) const { return Vector3(X * mult, Y * mult, Z * mult); }
        Vector3                         operator/(float div) const { return Vector3(X / div, Y / div, Z / div); }
        Vector3&                        operator=(const Vector3& o) = default;
        bool                            operator==(const Vector3&) const = default;

        static Vector3                  zero() { return Vector3(0, 0, 0); }
        static Vector3                  one() { return Vector3(1, 1, 1); }
        static Vector3                  up() { return Vector3(0, 1, 0); }
        static Vector3                  down() { return Vector3(0, -1, 0); }
        static Vector3                  left() { return Vector3(-1, 0, 0); }
        static Vector3                  right() { return Vector3(1, 0, 0); }
        static Vector3                  forward() { return Vector3(0, 0, 1); }
        static Vector3                  back() { return Vector3(0, 0, -1); }

        static float                    Distance(const Vector3& a, const Vector3& b);
        static Vector3                  Min(const Vector3& a, const Vector3& b);
        static Vector3                  Max(const Vector3& a, const Vector3& b);
        static Vector3                  Normalized(const Vector3& v);
        static Vector3                  Cross(const Vector3& v1, const Vector3& v2);
        static float                    Dot(const Vector3& v1, const Vector3& v2);

        float                           Distance(const Vector3& other);
        float                           GetMagnitude() const { return sqrt(X * X + Y * Y + Z * Z); }
        Vector3                         GetNormalized() const { return Normalized(*this); }
        Vector3                         Cross(const Vector3& other) const { return Cross(*this, other); }
        float                           Dot(const Vector3& other) const { return Dot(*this, other); }

        float                           X;
        float                           Y;
        float                           Z;
    };

    struct Vector2
    {
                                        Vector2() = default;
                                        Vector2(float x, float y) : X(x), Y(y) {}
                                        Vector2(const Vector3& v3) : X(v3.X), Y(v3.Y) {}

        bool                            operator==(const Vector2&) const = default; 
                                        operator Vector3() const { return Vector3(X, Y, 0.f); }
                                        operator Hax::Vector2() const { return Hax::Vector2(X, Y); }

        static System::Type             typeof();

        Vector2                         operator+(const Vector2& a) const { return Vector2(X + a.X, Y + a.Y); }
        Vector2                         operator-(const Vector2& a) const { return Vector2(X - a.X, Y - a.Y); }
        Vector2                         operator*(float mult) const { return Vector2(X * mult, Y * mult); }

        static Vector2                  down() { return Vector2(0.f, -1.f); }
        static Vector2                  zero() { return Vector2(0.f, 0.f); }

        float X = 0.f;
        float Y = 0.f;
    };

    struct Vector4
    {
                                        Vector4() = default;
                                        Vector4(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}

        static System::Type             typeof();

        float                           X;
        float                           Y;
        float                           Z;
        float                           W;
    };

    struct Quaternion
    {
        static System::Type             typeof();

        Vector3                         operator*(const Vector3& point) const;

        static Quaternion               identity() { return Quaternion(0.f, 0.f, 0.f, 1.f); }

        static Quaternion               Euler(float x, float y, float z);

        float                           X;
        float                           Y;
        float                           Z;
        float                           W;
    };

    struct Bounds
    {
                                        Bounds() = default;
                                        Bounds(const Vector3& center, const Vector3& size) : Center(center), Extents(size * 0.5f) {}

        static System::Type             typeof();

        inline Vector3                  GetSize() const { return Extents * 2.f; }
        inline void                     SetSize(const Vector3& size) { Extents = size * 0.5f; }
        inline Vector3                  GetMin() const { return Center - Extents; }
        inline Vector3                  GetMax() const { return Center + Extents; }

        inline void                     SetMinMax(const Vector3& min, const Vector3& max) { Extents = (max - min) * 0.5f; Center = min + Extents; }
        inline void                     Encapsulate(const Vector3& point) { SetMinMax(Vector3::Min(GetMin(), point), Vector3::Max(GetMax(), point)); }
        inline void                     Encapsulate(const Bounds& bounds) { Encapsulate(bounds.Center - bounds.Extents); Encapsulate(bounds.Center + bounds.Extents); }

        Vector3                         Center;
        Vector3                         Extents;
    };

    struct Color
    {
                                        Color() = default;
                                        Color(float r, float g, float b, float a) : R(r), G(g), B(b), A(a) {}

        static System::Type             typeof();

        static Color                    red() { return Color(1.f, 0.f, 0.f, 1.f); }
        static Color                    green() { return Color(0.f, 1.f, 0.f, 1.f); }
        static Color                    blue() { return Color(0.f, 0.f, 1.f, 1.f); }
        static Color                    white() { return Color(1.f, 1.f, 1.f, 1.f); }
        static Color                    black() { return Color(0.f, 0.f, 0.f, 1.f); }
        static Color                    yellow() { return Color(1.f, 0.92156863f, 0.015686275f, 1.f); }
        static Color                    cyan() { return Color(0.f, 1.f, 1.f, 1.f); }
        static Color                    magenta() { return Color(1.f, 0.f, 1.f, 1.f); }
        static Color                    gray() { return Color(0.5f, 0.5f, 0.5f, 0.5f); }
        static Color                    grey() { return Color(0.5f, 0.5f, 0.5f, 0.5f); }
        static Color                    clear() { return Color(0.f, 0.f, 0.f, 0.f); }

        float                           R;
        float                           G;
        float                           B;
        float                           A;
    };

    struct Matrix4x4
    {
        static System::Type             typeof();

        Vector4                         operator*(const Vector4& v) const;

        Vector3                         MultiplyPoint(const Vector3& point) const;

        float                           m00, m10, m20, m30;
        float                           m01, m11, m21, m31;
        float                           m02, m12, m22, m32;
        float                           m03, m13, m23, m33;
    };

    struct Rect
    {
                                        Rect() = default;
                                        Rect(const Rect&) = default;
                                        Rect(float x, float y, float width, float height) : X(x), Y(y), Width(width), Height(height) {}
                                        Rect(Vector2 pos, Vector2 size) : X(pos.X), Y(pos.Y), Width(size.X), Height(size.Y) {}

                                        operator Hax::Rect() const { return Hax::Rect({X, Y}, {X + Width, Y + Height}); }

        static System::Type             typeof();

        inline float                    xMax() const { return X + Width; }
        inline float                    xMin() const { return X; }
        inline float                    yMin() const { return Y; }
        inline float                    yMax() const { return Y + Height; }

        inline Vector2                  GetCenter() const { return Vector2(X + Width / 2.f, Y + Height / 2.f); }
        inline bool                     Contains(Vector2 point) const { return point.X >= xMin() && point.X < xMax() && point.Y >= yMin() && point.Y < yMax(); }
        inline bool                     Overlaps(const Rect& other)const  { return other.xMax() > xMin() && other.xMin() < xMax() && other.yMax() > yMin() && other.yMin() < yMax(); }

        static inline Rect              MinMaxRect(float xmin, float ymin, float xmax, float ymax) { return Rect(xmin, ymin, xmax - xmin, ymax - ymin); }

        float                           X;
        float                           Y;
        float                           Width;
        float                           Height;
    };

    class AsyncOperation : public System::Object
    {
    public:
        using System::Object::Object;

        static System::Type             typeof();

        bool                            GetIsDone();
        float                           GetProgress();
    };

    class Object : public System::Object
    {
        struct Fields { void* Object[2]; void* CachedPtr; };

    public:
        using System::Object::Object;

        bool                            operator==(nullptr_t) const     { return m_Ptr == nullptr || ((Fields*)m_Ptr)->CachedPtr == nullptr; }
        bool                            operator==(Object o) const      { return CompareBaseObjects(*this, o); }
                                        operator bool() const           { return !operator==(nullptr); }

        static System::Type             typeof();

        System::String                  GetName();
        void                            SetName(System::String name);
        void                            SetHideFlags(HideFlags flags);

        static Object                   FindObjectOfType(System::Type type, bool includeInactive);
        static Object                   FindObjectOfType(System::Type type)     { return FindObjectOfType(type, false); }
        template <IsObject T> static T  FindObjectOfType(bool includeInactive)  { return std::bit_cast<T>(FindObjectOfType(T::typeof(), includeInactive)); }
        template <IsObject T> static T  FindObjectOfType()                      { return std::bit_cast<T>(FindObjectOfType(T::typeof())); }

        static System::Array<Object>    FindObjectsOfType(System::Type type);
        template <IsObject T> 
        static System::Array<T>         FindObjectsOfType() { return std::bit_cast<System::Array<T>>(FindObjectsOfType(T::typeof())); }
        static System::Array<Object>    FindObjectsOfTypeAll(System::Type type);

        static void                     Destroy(Object obj);

        static Object                   Instantiate(Object original);
        static Object                   Instantiate(Object original, const Vector3& position, const Quaternion& rotation);
        template <System::IsRefType T> 
        static T                        Instantiate(T original) { return std::bit_cast<T>(Instantiate(original)); }
        template <System::IsRefType T> 
        static T                        Instantiate(T original, const Vector3& position, const Quaternion& rotation) { return std::bit_cast<T>(Instantiate((Object)original, position, rotation)); }

        int                             GetInstanceID();
        void*                           GetCachedPtr() const { return m_Ptr ? ((Fields*)m_Ptr)->CachedPtr : nullptr; }

    private:
        static bool                     IsNativeObjectAlive(Object o) { return o == null; }
        static bool                     CompareBaseObjects(Object lhs, Object rhs)
        {
            bool nullLhs = lhs == null;
            bool nullRhs = rhs == null;
            
            return (nullLhs && nullRhs) || (!nullLhs && !nullRhs && lhs.m_Ptr == rhs.m_Ptr);
        }
    };

    class Component : public Object
    {
    public:
        using Object::Object;

        static System::Type             typeof();

        Transform                       GetTransform() const;
        GameObject                      GetGameObject() const;

        Component                       GetComponentInChildren(System::Type) const;
        template <System::IsRefType T> 
        T                               GetComponentInChildren() const { return std::bit_cast<T>(GetComponentInChildren(T::typeof())); }

        System::Array<Component>        GetComponentsInChildren(System::Type) const;
        System::Array<Component>        GetComponentsInChildren(System::Type, bool includeInactive) const;
        template <IsComponent T>
        System::Array<T>                GetComponentsInChildren() const { return std::bit_cast<System::Array<T>>(GetComponentsInChildren(T::typeof())); }
        template <IsComponent T>
        System::Array<T>                GetComponentsInChildren(bool includeInactive) const { return std::bit_cast<System::Array<T>>(GetComponentsInChildren(T::typeof(), includeInactive)); }
        Component                       GetComponent(System::Type) const;
        template <IsComponent T>
        T                               GetComponent() const { return std::bit_cast<T>(GetComponent(T::typeof())); }
    };

    class Transform : public Component
    {
    public:
        using Component::Component;

        static System::Type             typeof();

        Vector3                         GetPosition();
        void                            SetPosition(const Vector3& value);
        Vector3                         GetLocalPosition();
        void                            SetLocalPosition(const Vector3& value);
        Transform                       GetParent();
        void                            SetParent(Transform value);
        Vector3                         GetForward();
        Vector3                         GetUp();
        void                            SetLocalScale(const Vector3& value);
        Vector3                         GetLocalScale();
        Quaternion                      GetRotation();
        void                            SetRotation(const Quaternion& value);

        Transform                       GetChild(int index);

        Transform                       Find(System::String n);
    };

    class Behaviour : public Component
    {
    public:
        using Component::Component;

        static System::Type             typeof();

        bool                            GetEnabled();
        void                            SetEnabled(bool value);
        bool                            GetIsActiveAndEnabled();
    };

    class MonoBehaviour : public Behaviour
    {
    public:
        using Behaviour::Behaviour;

        static System::Type             typeof();
    };

    class Time : public Object
    {
    public:
        using Object::Object;

        static System::Type             typeof();

        static float                    GetTimeScale();
        static void                     SetTimeScale(float scale);
    };

    class GameObject : public Object
    {
    public:
        using Object::Object;

        static GameObject               New();
        static GameObject               New(const char* name);

        static System::Type             typeof();

        Transform                       GetTransform();
        int                             GetLayer();
        void                            SetLayer(int value);
        bool                            GetActiveSelf();
        void                            SetActive(bool value);
        bool                            GetActive();
        bool                            GetActiveInHierarchy();
        void                            SetTag(System::String tag);
        bool                            CompareTag(System::String tag);

        Component                       GetComponent(System::Type type);
        template <IsComponent T>
        T                               GetComponent() { return std::bit_cast<T>(GetComponent(T::typeof())); }
        Component                       AddComponent(System::Type componentType);
        template <IsComponent T>
        T                               AddComponent() { return std::bit_cast<T>(AddComponent(T::typeof())); }
        System::Array<Component>        GetComponentsInChildren(System::Type pType, bool includeInactive);
    };

    struct LayerMask
    {
        static System::Type             typeof();

        static int                      NameToLayer(System::String name);

        int                             m_Mask;
    };

    enum class CameraEvent
    {
        BeforeDepthTexture,
        AfterDepthTexture,
        BeforeDepthNormalsTexture,
        AfterDepthNormalsTexture,
        BeforeGBuffer,
        AfterGBuffer,
        BeforeLighting,
        AfterLighting,
        BeforeFinalPass,
        AfterFinalPass,
        BeforeForwardOpaque,
        AfterForwardOpaque,
        BeforeImageEffectsOpaque,
        AfterImageEffectsOpaque,
        BeforeSkybox,
        AfterSkybox,
        BeforeForwardAlpha,
        AfterForwardAlpha,
        BeforeImageEffects,
        AfterImageEffects,
        AfterEverything,
        BeforeReflections,
        AfterReflections,
        BeforeHaloAndLensFlares,
        AfterHaloAndLensFlares
    };

    class Material;
    class Renderer;
    class CommandBuffer : public System::Object
    {
    public:
        using System::Object::Object;

        static CommandBuffer            New();
        static System::Type             typeof();

        void                            DrawRenderer(Renderer renderer, Material material);
        void                            DrawRenderer(Renderer renderer, Material material, int submeshIndex, int shaderPass);
        void                            Clear();
    };

    enum class MonoOrStereoscopicEye
    {
        Left,
        Right,
        Mono
    };

    class Camera : public Behaviour
    {
    public:
        using Behaviour::Behaviour;

        static System::Type             typeof();
        static Camera                   GetMain();

        float                           GetOrthographicSize();
        void                            SetOrthographicSize(float value);
        int                             GetPixelWidth();
        int                             GetPixelHeight();
        Rect                            GetPixelRect();
        Matrix4x4                       GetProjectionMatrix();
        Matrix4x4                       GetWorldToCameraMatrix();
        float                           GetFarClipPlane();
        void                            SetFarClipPlane(float value);
        float                           GetNearClipPlane();
        float                           GetFieldOfView();
        void                            SetFieldOfView(float value);

        Vector3                         WorldToScreenPoint(const Vector3& position, MonoOrStereoscopicEye eye = MonoOrStereoscopicEye::Mono);
        void                            RemoveCommandBuffer(CameraEvent event, CommandBuffer cb);
        void                            AddCommandBuffer(CameraEvent event, CommandBuffer cb);
    };

    class Cursor : public System::Object
    {
    public:
        static System::Type             typeof();
        static void                     SetVisible(bool value);
    };

    class Light : public Behaviour
    {
    public:
        using Behaviour::Behaviour;

        static System::Type             typeof();

        void                            SetIntensity(float value);
        float                           GetIntensity();

        void                            SetRange(float value);
        float                           GetRange();

        void                            SetSpotAngle(float value);
        float                           GetSpotAngle();
    };

    class Shader : public Object
    {
    public:
        using Object::Object;

        static System::Type             typeof();

        static Shader                   Find(System::String name);
        static void                     WarmupAllShaders();

        bool                            IsSupported();
    };

    class Material : public Object
    {
    public:
        using Object::Object;

        static Material                 New(Shader shader);

        static System::Type             typeof();

        void                            SetInt(System::String name, int value);
        void                            SetColor(Color color);
        void                            SetColor(System::String name, Color color);

        int                             GetPassCount();

    };

    class Renderer : public Component
    {
    public:
        using Component::Component;

        static System::Type             typeof();

        Bounds                          GetBounds();
        Material                        GetSharedMaterial();
        void                            SetSharedMaterial(Material pMaterial);
        void                            SetMaterial(Material pMaterial);
        bool                            GetEnabled();
        void                            SetEnabled(bool value);
        void                            SetSortingOrder(int value);

        bool                            IsVisible();
    };

    class SpriteRenderer : public Renderer
    {
    public:
        using Renderer::Renderer;

        static System::Type             typeof();

        Color                           GetColor();
        void                            SetColor(const Color& color);
    };

    class MeshRenderer : public Renderer
    {
    public:
        using Renderer::Renderer;

        static System::Type             typeof();
    };

    class LineRenderer : public Renderer
    {
    public:
        using Renderer::Renderer;

        static System::Type             typeof();

        void                            SetStartWidth(float value);
        void                            SetEndWidth(float value);
        void                            SetPositionCount(int value);
        void                            SetPosition(int index, const Vector3& position);
    };

    class SkinnedMeshRenderer : public Renderer
    {
    public:
        using Renderer::Renderer;

        static System::Type             typeof();
    };

    struct AssetBundle : Object
    {
        using Object::Object;

        static System::Type             typeof();

        static AssetBundle              LoadFromMemory(System::Array<System::Char> binary);
        static AssetBundle              LoadFromFile(System::String);

        Object                          LoadAsset(System::String name, System::Type type);
    };

    struct Application
    {
        static System::Type             typeof();

        static System::String           GetDataPath();
    };

    class Screen
    {
    public:
        static System::Type             typeof();

        static int                      GetHeight();
        static int                      GetWidth();
    };

    class Sprite : public Object
    {
    public:
        using Object::Object;

        static System::Type             typeof();

        System::Array<Vector2>          GetUV();
        Vector4                         GetOuterUVs();
        Texture2D                       GetTexture();
        Rect                            GetRect();
        Rect                            GetTextureRect();
    };

    class RenderSettings : public Object
    {
    public:
        using Object::Object;

        static System::Type             typeof();

        static bool                     GetFog();
        static void                     SetFog(bool value);
    };

    class Texture : public Object
    {
    public:
        using Object::Object;

        static System::Type             typeof();

        void*                           GetNativeTexturePtr();

        int                             GetDataWidth();
        int                             GetDataHeight();
    };

    class Texture2D : public Texture
    {
    public:
        using Texture::Texture;

        static System::Type typeof();
    };

    struct PlayerLoopSystemInternal
    {
        static System::Type typeof();

        System::Type                    m_Type;
        void*                           m_UpdateDelegate;
        void**                          m_UpdateFunction;
        void*                           m_LoopConditionFunction;
        int                             m_NumSubSystems;
    };

    struct PlayerLoopSystem
    {
        static System::Type typeof();

        System::Type                    m_Type;
        System::Array<PlayerLoopSystem> m_SubSystemList;
        void*                           m_UpdateDelegate;
        void**                          m_UpdateFunction;
        void*                           m_LoopConditionFunction;
    };

    namespace LowLevel
    {
        class PlayerLoop
        {
        public:
            static System::Type             typeof();

            static System::Array<PlayerLoopSystemInternal> GetDefaultPlayerLoopInternal();
            static PlayerLoopSystem GetDefaultPlayerLoop();
        };
    }

    class ResourceRequest : public AsyncOperation
    {
    public:
        using AsyncOperation::AsyncOperation;

        static System::Type typeof();
        Unity::Object GetAsset();
    };

    class Resources
    {
    public:
        static System::Type typeof();
        static ResourceRequest LoadAsync(System::String path);
    };

    namespace PlayerLoop
    {
        struct PostLateUpdate
        {
            static System::Type typeof();
        };

        struct PlayerSendFrameStarted
        {
            static System::Type typeof();
        };

        struct PresentAfterDraw
        {
            static System::Type typeof();
        };

        struct Update
        {
            static System::Type typeof();
        };

        struct ScriptRunBehaviourUpdate
        {
            static System::Type typeof();
        };
    }

    class EventSystem : public MonoBehaviour
    {
    public:
        using MonoBehaviour::MonoBehaviour;

        static System::Type             typeof();

        static EventSystem              GetCurrent();
    };

    class PostProcessLayer : public MonoBehaviour
    {
    public:
        using MonoBehaviour::MonoBehaviour;

        static System::Type             typeof();

        CommandBuffer                   m_LegacyCmdBufferBeforeReflections();
        CommandBuffer                   m_LegacyCmdBufferBeforeLighting();
        CommandBuffer                   m_LegacyCmdBufferOpaque();
        CommandBuffer                   m_LegacyCmdBuffer();
    };
}
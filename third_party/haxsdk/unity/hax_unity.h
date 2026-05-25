#pragma once

#include "../hax.h"

namespace Hax::Unity
{
    void Initialize(LogFile* logFile);
    void Shutdown();

    bool IsUnityProcess();
    Handle GetUvmHandle();

    void ThrowNullRef();
    void ThrowOutOfRange();

    struct ClassInfo
    {
        const char* Assembly;
        const char* Namespace;
        const char* Name;
    };

    struct Offset
    {
        explicit                Offset(const ClassInfo& classInfo, const char* name);
        int                     Value = -1;
    };

    struct StaticField
    {
        explicit                StaticField(const ClassInfo& classInfo, const char* name);
        void*                   Address = nullptr;
    };

    struct EnumLiteral
    {
        explicit                EnumLiteral(const ClassInfo& classInfo, const char* name);
        int                     Value = -1;
    };
}

namespace UVM // Unity Virtual Machine
{
    struct Assembly;
    struct Domain;
    struct Method;
    struct MethodSignature;
    struct Class;
    struct Image;
    struct Thread;
    struct Field;
    struct Type;
    struct VTable;
    struct Object;
    struct Exception;
    struct ReflectionType;
    struct ArrayType;
    struct GenericParam;
    struct GenericClass;
    struct UnityObject;
    struct String;

    Image&              AssemblyGetImage(Assembly& assembly);

    Domain&             GetRootDomain();
    Assembly&           FindAssembly(const char* name);
    Assembly*           TryFindAssembly(const char* name);

    Image&              ImageGetCorlib();
    const char*         ImageGetName(Image& image);
    Class&              ImageFindClass(Image& image, const char* nameSpace, const char* name);
    Class*              ImageTryFindClass(Image& image, const char* nameSpace, const char* name);

    Thread&             ThreadAttach();
    void                ThreadDetach(Thread& thread);

    Field&              ClassFindField(Class& klass, const char* name);
    Field*              ClassTryFindField(Class& klass, const char* name);
    Method&             ClassFindMethod(Class& klass, const char* name, const char* sig = nullptr);
    Method*             ClassTryFindMethod(Class& klass, const char* name, const char* sig = nullptr);
    Type&               ClassGetType(Class& klass);
    VTable&             ClassGetVTable(Class& klass);
    const char*         ClassGetName(Class& klass);
    uint8_t*            ClassGetStaticFieldData(Class& klass);
    Class*              ClassGetParent(Class& klass);

    void*               MethodGetPointer(Method& method);
    Object*             MethodInvoke(Method& method, void* __this, void** args, Exception** ex);
    void*               MethodGetThunk(Method& method);
    Class&              MethodGetClass(Method& method);
    const char*         MethodGetName(Method& method);
    MethodSignature&    MethodGetSignature(Method& method);

    Type&               SignatureGetReturnType(MethodSignature& sig);
    uint32_t            SignatureGetParamCount(MethodSignature& sig);
    Type*               SignatureGetParams(MethodSignature& sig, void** iter);

    const char*         TypeGetName(Type& type);
    ReflectionType&     TypeGetReflectionType(Type& type);
    ReflectionType&     TypeCreateReflectionType(Type& type);
    Class&              TypeGetClass(Type& type);

    Object&             ObjectNew(Class& klass);
    void                ObjectCtor(Object& obj);
    Class&              ObjectGetClass(Object& obj);

    bool                FieldIsStatic(Field& field);
    int                 FieldGetOffset(Field& field);
    const char*         FieldGetName(Field& field);
    bool                FieldIsLiteral(Field& field);
    void*               FieldGetValuePtr(Field& field, void* __this);
    void                FieldGetStaticValue(Field& field, void* value);

    String*             ExceptionGetMessage(Exception& exc);
    String*             ExceptionGetStackTrace(Exception& exc);
    Exception&          ExceptionGetNullReference();
    Exception&          ExceptionGetArgumentOutOfRange();
    Exception&          ExceptionGetTargetException(String* message);

    uint32_t            GCHandleNew(Object& obj, bool pinned);
    uint32_t            GCHandleNewWeak(Object& obj, bool trackResurrection);
    Object*             GCHandleGetTarget(uint32_t handle);
    void                GCHandleFree(uint32_t handle);

    Object*             ArrayNew(Class& klass, size_t size);
    String*             StringNew(const char* content);
    ReflectionType*     StringGetReflectionType();

    struct Object
    {
        VTable*             VTable;
        void*               Monitor;
    };

    struct ReflectionType : Object
    {
                            ReflectionType() = default;
                            ReflectionType(Type* type);

        Type*               Type;
    };

    struct UnityObject : Object
    {
        void* m_CachedPtr;
    };

    template <typename T>
    struct Array : Object
    {
        T&                      operator[](size_t i)            { return m_Items[i]; }
        const T&                operator[](size_t i) const      { return m_Items[i]; }

        T*                      begin()                         { return m_Items; }
        const T*                begin() const                   { return m_Items; }
        T*                      end()                           { return m_Items + m_Size; }
        const T*                end() const                     { return m_Items + m_Size; }

        size_t                  GetLength() const               { return m_Size; }

    private:
        void*                   m_Bounds;
        size_t                  m_Size;
        __declspec(align(8)) T  m_Items[1];
    };

    struct String : Object
    {
                                String()                        = delete;
                                String(const String&)           = delete;
                                String(String&&)                = delete;

        wchar_t&                operator[](int i)               { return m_Chars[i]; }
        const wchar_t&          operator[](int i) const         { return m_Chars[i]; }

        wchar_t*                begin()                         { return m_Chars; }
        const wchar_t*          begin() const                   { return m_Chars; }
        wchar_t*                end()                           { return m_Chars + m_Length; }
        const wchar_t*          end() const                     { return m_Chars + m_Length; }

        wchar_t*                GetRawStringData()              { return m_Chars; }
        int                     GetLength() const               { return m_Length; }

        Hax::WStringView        ToView() const                  { return Hax::WStringView(m_Chars, (size_t)m_Length); }

    private:
        int                     m_Length;
        wchar_t                 m_Chars[32]{0};
    };

    template <typename T>
    struct List : Object
    {
                                List()                          = delete;
                                List(const List<T>&)            = delete;
                                List(List<T>&&)                 = delete;

        T&                      operator[](size_t i)            { return m_Items->operator[](i); }
        const T&                operator[](size_t i) const      { return m_Items->operator[](i); }

        T*                      begin()                         { return m_Items->begin(); }
        const T*                begin() const                   { return m_Items->begin(); }
        T*                      end()                           { return m_Items->begin() + m_Size; }
        const T*                end() const                     { return m_Items->begin() + m_Size; }

        int                     GetLength() const               { return m_Size; }
        size_t                  GetCapacity() const             { return m_Items->GetLength(); }

    private:
        Array<T>*               m_Items;
        int                     m_Size;
        int                     m_Version;
        void*                   m_SyncRoot;
    };

    template <typename TKey, typename TValue>
    struct Dictionary : Object
    {
        struct Entry
        {
            int     HashCode;
            int     Next;
            TKey    Key;
            TValue  Value;
        };

                                Dictionary()                    = delete;
                                Dictionary(const Dictionary&)   = delete;
                                Dictionary(Dictionary&&)        = delete;

        const Entry*            begin() const                   { return Entries->begin(); }
        Entry*                  begin()                         { return Entries->begin(); }
        const Entry*            end() const                     { return Entries->begin() + Count; }
        Entry*                  end()                           { return Entries->begin() + Count; }

        TValue*                 GetItem(const TKey& key)        { for (int i = 0; i < Count; ++i) if (Entries->operator[](i).Key == key) return &Entries->operator[](i).Value; return nullptr; }

        Array<int>*             Buckets;
        Array<Entry>*           Entries;
        void*                   Comparer;
        void*                   Keys;
        void*                   Values;
        void*                   SyncRoot;
        int                     Count;
        /* ... */
    };
}

namespace System
{
    #define THROW_IF_NULL() if (this->null()) Hax::Unity::ThrowNullRef()
    #define THROW_IF_RANGE(i, l, r) if (i < l || i > r) Hax::Unity::ThrowOutOfRange()
    #define THROW_IF_GE(i, v) if (i >= v) Hax::Unity::ThrowOutOfRange()

    struct Assembly;
    struct Type;
    struct MethodInfo;
    struct FieldInfo;

    struct Int32
    {
        static Type typeof();
    
        operator int() const { return Value; }
    
        int Value;
    };
    
    struct Single
    {
        static Type typeof();
    
        operator float() const { return Value; }
    
        float Value;
    };
    
    struct Char
    {
        static Type typeof();
    
        operator wchar_t() const { return Value; }
    
        wchar_t Value;
    };
    
    struct Boolean
    {
        static Type typeof();
    
        operator bool() const { return Value; }
    
        bool Value;
    };
    
    struct IntPtr
    {
        static Type typeof();
    
        operator void*() const { return Value; }
    
        void* Value;
    };

    struct Object
    {
        explicit                Object() = default;
                                Object(nullptr_t ptr) : m_Ptr(ptr) {}
        explicit                Object(UVM::Object* ptr) : m_Ptr(ptr) {}
        explicit                Object(UVM::Exception* ptr) : m_Ptr(ptr) {}
        explicit                Object(UVM::String* ptr) : m_Ptr(ptr) {}
        explicit                Object(UVM::ReflectionType* ptr) : m_Ptr(ptr) {}

        operator bool() const                                                           { return !null(); }

        static Type             typeof();

        static bool             ReferenceEquals(Object o1, Object o2)                   { return o1.m_PtrObj == o2.m_PtrObj; }

        bool                    null() const                                            { return m_PtrObj == nullptr; }

        Type                    GetType();
        UVM::Object*            GetPtr() const                                         { return m_PtrObj; }

    protected:
        union
        {
            void*                m_Ptr;
            UVM::Object*         m_PtrObj;
            UVM::Exception*      m_PtrExc;
            UVM::String*         m_PtrStr;
            UVM::ReflectionType* m_PtrType;
        };
    };

    struct String : Object
    {
                                String() = default;
                                String(UVM::Object* ptr) : Object(ptr) {}
                                String(UVM::String* ptr) : Object(ptr) {}
        
        static Type             typeof();
        static String           New(const char* str) { return String(UVM::StringNew(str)); }

        wchar_t&                operator[](int i);
        const wchar_t&          operator[](int i) const;
        bool                    operator==(const String& o) const;
        bool                    operator==(const wchar_t* wstr) const;
                                operator bool() const { return !this->null(); }

        wchar_t*                begin();
        const wchar_t*          begin() const;
        wchar_t*                end();
        const wchar_t*          end() const;

        static String           Concat(String s1, String s2);
        static String           Empty();

        int                     GetLength() const;
        bool                    StartsWith(const wchar_t* prefix);
        bool                    EndsWith(const wchar_t* postfix);
        bool                    Contains(const wchar_t* substr);
        String                  Replace(String s1, String s2);
        String                  ToUpper();

        wchar_t*                GetRawStringData() const;

        UVM::String*            GetPtr() const { return m_PtrStr; }
        Hax::WStringView        ToView() const { return m_PtrStr->ToView(); }
    };

    template <typename T>
    struct Array : Object
    {
                                Array(UVM::Object* ptr) : Object(ptr) {}
                                Array(UVM::Array<T>* ptr) : Object(ptr) {}

        const T&                operator[](size_t i) const  { THROW_IF_NULL(); THROW_IF_RANGE(i + 1, 1, GetPtr()->GetLength()); return GetPtr()->operator[](i); }
        T&                      operator[](size_t i)        { THROW_IF_NULL(); THROW_IF_RANGE(i + 1, 1, GetPtr()->GetLength()); return GetPtr()->operator[](i); }
                                operator bool() const       { return !this->null(); }

        T*                      begin()                     { THROW_IF_NULL(); return GetPtr()->begin(); }
        const T*                begin() const               { THROW_IF_NULL(); return GetPtr()->begin(); }
        T*                      end()                       { THROW_IF_NULL(); return GetPtr()->end(); }
        const T*                end() const                 { THROW_IF_NULL(); return GetPtr()->end(); }

        size_t                  GetLength() const           { THROW_IF_NULL(); return GetPtr()->GetLength(); }

        UVM::Array<T>*          GetPtr() const              { return (UVM::Array<T>*)m_PtrObj; }
    };

    struct Type : Object
    {
                                Type(nullptr_t) : Object(nullptr) {}
                                Type(UVM::Object* ptr) : Object(ptr) {}
                                Type(UVM::ReflectionType* ptr) : Object(ptr) {}
                                Type(const Hax::Unity::ClassInfo& info);

                                operator bool() const { return !null(); }
        bool                    operator==(const Type& other) const;

        FieldInfo               GetField(const char* name);
        bool                    HasField(const char* name, FieldInfo* out = nullptr);
        MethodInfo              GetMethod(const char* name, const char* sig = nullptr);

        static Object           CreateInstanceForAnotherGenericParameter(Type genericType, Type genericArgument);

        UVM::ReflectionType*    GetPtr() const { return m_PtrType; }
    };

    struct AppDomain
    {
                                AppDomain(UVM::Domain& ptr) : m_Ptr(&ptr) {}

        static AppDomain        GetCurrent() { return AppDomain(UVM::GetRootDomain()); }

        Assembly                Load(const char* name);

        bool                    null() { return m_Ptr == nullptr; }

    private:
        UVM::Domain*            m_Ptr;
    };

    struct Exception : Object
    {
        explicit                Exception(UVM::Exception* ptr) : Object(ptr) {}

                                operator bool() const { return !this->null(); }

        String                  GetMessage() { THROW_IF_NULL(); return String(UVM::ExceptionGetMessage(*GetPtr())); }
        String                  GetStackTrace() { THROW_IF_NULL(); return String(UVM::ExceptionGetStackTrace(*GetPtr())); }
        String                  ToString();

        UVM::Exception*         GetPtr() const { return m_PtrExc; }
    };

    struct NullReferenceException : Exception
    {
                                NullReferenceException(UVM::Exception* ptr) : Exception(ptr) {}
        static auto             New() { return NullReferenceException(&UVM::ExceptionGetNullReference()); }

                                operator bool() const { return !this->null(); }
    };

    struct ArgumentOutOfRangeException : Exception
    {
                                ArgumentOutOfRangeException(UVM::Exception* ptr) : Exception(ptr) {}

        static auto             New() { return ArgumentOutOfRangeException(&UVM::ExceptionGetArgumentOutOfRange()); }

                                operator bool() const { return !this->null(); }
    };

    struct TargetException : Exception
    {
                                TargetException(UVM::Exception* ptr) : Exception(ptr) {}

        static auto             New(String message) { return TargetException(&UVM::ExceptionGetTargetException(message.GetPtr())); }

                                operator bool() const { return !this->null(); }
    };

    struct Assembly
    {
                                Assembly(UVM::Image* ptr) : m_Ptr(ptr) {}

        Type                    GetType(const char* nameSpace, const char* name);
        Type                    TryGetType(const char* nameSpace, const char* name);

        bool                    null() { return m_Ptr == nullptr; }
        static Assembly         Corlib() { return Assembly(&UVM::ImageGetCorlib()); }

    private:
        UVM::Image*             m_Ptr;
    };

    struct FieldInfo
    {
                                FieldInfo(UVM::Field* ptr) : m_Ptr(ptr) {}

        bool                    IsLiteral()                             { THROW_IF_NULL(); return UVM::FieldIsLiteral(*m_Ptr); }
        bool                    IsStatic()                              { THROW_IF_NULL(); return UVM::FieldIsStatic(*m_Ptr); }
        void*                   GetValuePtr(Object __this);

        int                     GetOffset()                             { THROW_IF_NULL();  return UVM::FieldGetOffset(*m_Ptr); }

        bool                    null() const                            { return m_Ptr == nullptr; }
        int                     GetEnumValue()                          { THROW_IF_NULL(); int v; UVM::FieldGetStaticValue(*m_Ptr, &v); return v; }

        UVM::Field*             m_Ptr;
    };

    struct MethodInfo
    {
                                MethodInfo() = default;
        explicit                MethodInfo(const Hax::Unity::ClassInfo& classInfo, const char* name, const char* sig = nullptr);
        
        template <typename Ret, typename... Args>
        Ret CallThunk(const Args&... args)
        {
            HAX_ASSERT(m_Thunk != 0);
            THROW_IF_NULL();

            Exception ex = Exception(nullptr);

            if constexpr (std::is_void_v<Ret>)
            {
                ((void(*)(Args..., Exception*))m_Thunk)(args..., &ex);
                if (ex)
                    throw ex;
            }
            else if constexpr (std::is_base_of_v<Object, Ret>)
            {
                void* res = ((void*(*)(Args..., Exception*))m_Thunk)(args..., &ex);
                if (ex)
                    throw ex;
                return Ret((UVM::Object*)res);
            }
            else
            {
                Ret res = ((Ret(*)(Args..., Exception*))m_Thunk)(args..., &ex);
                if (ex)
                    throw ex;
                return res;
            }
        }

        template <typename Ret, typename... Args>
        Ret Invoke(const Args&... args)
        {
            THROW_IF_NULL();
            static_assert(sizeof... (args) > 0);

            UVM::Exception* ex = nullptr;

            void* packedArgs[] = {this->Pack(args)...};
            size_t nArgs = sizeof(packedArgs) / sizeof(void*);
            UVM::Object* res = UVM::MethodInvoke(*m_Base, packedArgs[0], nArgs == 1 ? nullptr : packedArgs + 1, &ex);

            if (ex)
                throw System::Exception(ex);

            if constexpr (std::is_void_v<Ret>) return;
            else if constexpr (std::is_base_of_v<System::Object, Ret>) return Ret(res);
            else return static_cast<Ret>(res);
        }

        template <typename Ret, typename... Args>
        Ret InvokeStatic(const Args&... args)
        {
            THROW_IF_NULL();
            if constexpr (sizeof...(args) == 0)
            {
                UVM::Exception* ex = nullptr;
                UVM::Object* res = UVM::MethodInvoke(*m_Base, nullptr, nullptr, &ex);

                if (ex)
                    throw System::Exception(ex);

                if constexpr (std::is_void_v<Ret>) return;
                else if constexpr (std::is_base_of_v<System::Object, Ret>) return Ret(res);
                else return static_cast<Ret>(res);
            }
            else
            {
                UVM::Exception* ex = nullptr;
                void* packedArgs[] = {this->Pack(args)...};
                UVM::Object* res = UVM::MethodInvoke(*m_Base, nullptr, packedArgs, &ex);

                if (ex)
                    throw System::Exception(ex);

                if constexpr (std::is_void_v<Ret>) return;
                else if constexpr (std::is_base_of_v<System::Object, Ret>) return Ret(res);
                else return static_cast<Ret>(res);
            }
        }

         bool null() { return m_Base == nullptr; }

    private:
        template <typename T>
        void* Pack(const T& val)
        {
            if constexpr (std::is_pointer_v<T>) return val;
            if constexpr (std::is_base_of_v<System::Object, T>) return val.GetPtr();
            else return (void*)&val;
        }

    public:
        UVM::Method* m_Base = nullptr;
        void* m_Pointer = nullptr;
        void* m_Thunk = nullptr;
    };

    struct Path
    {
        static String Combine(String path1, String path2);
    };

    template <typename TKey, typename TValue>
    struct Dictionary : Object
    {
        explicit                Dictionary(UVM::Object* ptr) : Object(ptr) {}

        inline                  operator bool() const { return !null(); }

        const auto              begin() const                   { THROW_IF_NULL(); return GetPtr()->begin(); }
        auto                    begin()                         { THROW_IF_NULL(); return GetPtr()->begin(); }
        const auto              end() const                     { THROW_IF_NULL(); return GetPtr()->end(); }
        auto                    end()                           { THROW_IF_NULL(); return GetPtr()->end(); }

        inline bool             ContainsKey(const TKey& key)    { THROW_IF_NULL(); return GetPtr()->GetItem(key) != nullptr; }
        TValue&                 GetItem(const TKey& key)        { THROW_IF_NULL(); return *GetPtr()->GetItem(key); }
        int                     Count()                         { THROW_IF_NULL(); return GetPtr()->Count; }

        inline auto             GetPtr() { return (UVM::Dictionary<TKey, TValue>*)m_Ptr; }
    };

    template <typename T>
    struct List : Object
    {
                                List() = default;
                                List(UVM::Object* ptr) : Object(ptr) {}
                                List(UVM::List<T>* ptr) : Object(ptr) {}

        static                  Type typeof();

        T&                      operator[](size_t i)            { THROW_IF_NULL(); THROW_IF_RANGE(i, 0, GetPtr()->GetLength() - 1); return GetPtr()->operator[](i); }
        const T&                operator[](size_t i) const      { THROW_IF_NULL(); THROW_IF_RANGE(i, 0, GetPtr()->GetLength() - 1); return GetPtr()->operator[](i); }
                                operator bool() const {          return !null(); }

        T*                      begin()                         { THROW_IF_NULL(); return GetPtr()->begin(); }
        const T*                begin() const                   { THROW_IF_NULL(); return GetPtr()->begin(); }
        T*                      end()                           { THROW_IF_NULL(); return GetPtr()->end(); }
        const T*                end() const                     { THROW_IF_NULL(); return GetPtr()->end(); }

        inline int              GetCount() const                { THROW_IF_NULL(); return GetPtr()->GetLength(); }
        inline int              GetCapacity() const             { THROW_IF_NULL(); return GetPtr()->GetCapacity(); }
        bool                    Contains(T val);

        inline UVM::List<T>*    GetPtr() const                  { return (UVM::List<T>*)m_Ptr; }
    };

    template <typename T>
    bool List<T>::Contains(T val)
    {
        THROW_IF_NULL();
        static MethodInfo method = this->GetType().GetMethod("Contains");
        return method.CallThunk<bool, List<T>, T>(*this, val);
    }

    inline Type g_List_Typeof = (UVM::Object*)nullptr;
    template <typename T>
    Type List<T>::typeof()
    {
        if (!g_List_Typeof)
            g_List_Typeof = Assembly::Corlib().GetType("System.Collections.Generic", "List`1");
        return g_List_Typeof;
    }

    template<typename T> concept TValueType = !std::is_base_of_v<System::Object, T>;
    template <TValueType T>
    struct Boxed : UVM::Object
    {
        Boxed() : Boxed(T()) {}
        Boxed(const T& value) : m_Value(value), UVM::Object(&UVM::ClassGetVTable(UVM::TypeGetClass(*T::typeof().GetPtr()->Type)), nullptr) {}

        T m_Value;
    };
}

#define META(a, ns, c)\
private: inline static Hax::Unity::ClassInfo s_Meta = {a, ns, c};\
private: inline static System::Type s_Type{s_Meta}; \
public: static System::Type typeof() { return s_Type; }

#define STATIC_FIELD(n, t)                            \
private: static inline Hax::Unity::StaticField s_ ## n{s_Meta, #n};\
public: static t& n() { return *(t*)s_ ## n ## .Address; }

#define FIELD(n, t)                                   \
private: static inline Hax::Unity::Offset s_ ## n{s_Meta, #n};     \
public: t& n() { THROW_IF_NULL(); return *(t*)((char*)m_Ptr + s_ ## n ## .Value); }

#define FIELD_EX(f, n, t)                                   \
private: static inline Hax::Unity::Offset s_ ## f{s_Meta, n}; \
public: t& f() { THROW_IF_NULL(); return *(t*)((char*)m_Ptr + s_ ## f ## .Value); }

#define STATIC_PROPERTY(n, t)                            \
private: static inline Hax::Unity::StaticField s_ ## n{s_Meta, "<" ## #n ## ">k__BackingField"};        \
public: static t& n() { return *(t*)s_ ## n ## .Address; }

#define PROPERTY(n, t)                                   \
private: static inline Hax::Unity::Offset s_ ## n{s_Meta, "<" ## #n ## ">k__BackingField"}; \
public: t& n() { THROW_IF_NULL(); return *(t*)((char*)m_Ptr + s_ ## n ## .Value); }

#define ENUM_VALUE(n)                                 \
private: static inline Hax::Unity::EnumLiteral s_ ## n{s_Meta, #n}; \
public: static int n() { return s_ ## n ## .Value; }

#define METHOD(n)\
static inline System::MethodInfo s_ ## n{s_Meta, #n}

#define METHOD_EX(n, sig)\
static inline System::MethodInfo s_ ## n{s_Meta, #n, sig}

#define COMMA ,

namespace UnityEngine
{
    enum class HideFlags : int;
    enum class CursorLockMode : int;

    class GameObject;
    class Transform;
    class Texture2D;

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

    struct Vector2
    {
                                        Vector2() = default;
                                        Vector2(float x, float y) : x(x), y(y) {}

        bool                            operator==(const Vector2& o) const { return o.x == x && o.y == y; }
        //Vector2&                        operator=(const Vector2& o) { x = o.x; y = o.y; return *this; }

        inline Vector2                  operator+(const Vector2& a) const { return Vector2(x + a.x, y + a.y); }
        inline Vector2                  operator-(const Vector2& a) const { return Vector2(x - a.x, y - a.y); }
        inline Vector2                  operator*(float mult) const { return Vector2(x * mult, y * mult); }

        Hax::Vector2                    ToHax() const { return Hax::Vector2(x, y); }

        static inline Vector2           down() { return Vector2(0.f, -1.f); }
        static inline Vector2           zero() { return Vector2(0.f, 0.f); }

        float x = 0.f;
        float y = 0.f;
    };

    struct Vector3
    {
        Vector3() : x(0.f), y(0.f), z(0.f) {}
        Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
        Vector3(float x_, float y_) : x(x_), y(y_), z(0.f) {}
        Vector3(const Vector2& v) : x(v.x), y(v.y), z(0.f) {}

        static System::Type             typeof();

        inline Vector3                  operator+(const Vector3& a) const { return Vector3(x + a.x, y + a.y, z + a.z); }
        inline Vector3                  operator-(const Vector3& a) const { return Vector3(x - a.x, y - a.y, z - a.z); }
        inline Vector3                  operator*(float mult) const { return Vector3(x * mult, y * mult, z * mult); }
        inline Vector3                  operator/(float div) const { return Vector3(x / div, y / div, z / div); }
        inline Vector3&                 operator=(const Vector3& o) { if (&o != this) { x = o.x; y = o.y; z = o.z; } return *this; }
        inline bool                     operator==(const Vector3& o) const { return o.x == x && o.y == y && o.z == z; }

        operator Vector2() { return Vector2(x, y); }

        static inline Vector3           zero() { return Vector3(0, 0, 0); }
        static inline Vector3           one() { return Vector3(1, 1, 1); }
        static inline Vector3           up() { return Vector3(0, 1, 0); }
        static inline Vector3           down() { return Vector3(0, -1, 0); }
        static inline Vector3           left() { return Vector3(-1, 0, 0); }
        static inline Vector3           right() { return Vector3(1, 0, 0); }
        static inline Vector3           forward() { return Vector3(0, 0, 1); }
        static inline Vector3           back() { return Vector3(0, 0, -1); }

        static float                    Distance(const Vector3& a, const Vector3& b);
        static Vector3                  Min(const Vector3& a, const Vector3& b);
        static Vector3                  Max(const Vector3& a, const Vector3& b);
        static Vector3                  Normalized(const Vector3& v);
        static Vector3                  Cross(const Vector3& v1, const Vector3& v2);
        static float                    Dot(const Vector3& v1, const Vector3& v2);

        float                           Distance(const Vector3& other);
        inline Vector2                  ToVector2() { return Vector2(this->x, this->y); }
        float                           GetMagnitude() const { return sqrt(x * x + y * y + z * z); }
        Vector3                         GetNormalized() const { return Normalized(*this); }
        Vector3                         Cross(const Vector3& other) const { return Cross(*this, other); }
        float                           Dot(const Vector3& other) const { return Dot(*this, other); }

        float                           x;
        float                           y;
        float                           z;
    };

    struct Vector4
    {
        Vector4() = default;
        Vector4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}

        static System::Type             typeof();

        float                           x;
        float                           y;
        float                           z;
        float                           w;
    };

    struct NavMeshHit
    {
        static System::Type             typeof();

        Vector3                         m_Position;
        Vector3                         m_Normal;
        float                           m_Distance;
        int                             m_Mask;
        int                             m_Hit;
    };

    struct NavMesh
    {
        static System::Type             typeof();

        static bool                     SamplePosition(const Vector3& pos, NavMeshHit* hit, float maxDist, int areaMask);
    };


    struct Quaternion
    {
        explicit                        Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

        static System::Type             typeof();

        Vector3                         operator*(const Vector3& point) const;

        static inline Quaternion        identity() { return Quaternion(0.f, 0.f, 0.f, 1.f); }

        static Quaternion               Euler(float x, float y, float z);

        float                           x;
        float                           y;
        float                           z;
        float                           w;
    };

    struct Bounds
    {
        explicit                        Bounds(const Vector3& center, const Vector3& size) : m_Center(center), m_Extents(size * 0.5f) {}

        static System::Type             typeof();

        inline Vector3                  GetSize() const { return m_Extents * 2.f; }
        inline void                     SetSize(const Vector3& size) { m_Extents = size * 0.5f; }
        inline Vector3                  GetMin() { return m_Center - m_Extents; }
        inline Vector3                  GetMax() { return m_Center + m_Extents; }

        inline void                     SetMinMax(const Vector3& min, const Vector3& max) { m_Extents = (max - min) * 0.5f; m_Center = min + m_Extents; }
        inline void                     Encapsulate(const Vector3& point) { SetMinMax(Vector3::Min(GetMin(), point), Vector3::Max(GetMax(), point)); }
        inline void                     Encapsulate(const Bounds& bounds) { Encapsulate(bounds.m_Center - bounds.m_Extents); Encapsulate(bounds.m_Center + bounds.m_Extents); }

        Vector3                         m_Center;
        Vector3                         m_Extents;
    };

    struct Color
    {
        explicit                        Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}

        static System::Type             typeof();

        static inline Color             red() { return Color(1.f, 0.f, 0.f, 1.f); }
        static inline Color             green() { return Color(0.f, 1.f, 0.f, 1.f); }
        static inline Color             blue() { return Color(0.f, 0.f, 1.f, 1.f); }
        static inline Color             white() { return Color(1.f, 1.f, 1.f, 1.f); }
        static inline Color             black() { return Color(0.f, 0.f, 0.f, 1.f); }
        static inline Color             yellow() { return Color(1.f, 0.92156863f, 0.015686275f, 1.f); }
        static inline Color             cyan() { return Color(0.f, 1.f, 1.f, 1.f); }
        static inline Color             magenta() { return Color(1.f, 0.f, 1.f, 1.f); }
        static inline Color             gray() { return Color(0.5f, 0.5f, 0.5f, 0.5f); }
        static inline Color             grey() { return Color(0.5f, 0.5f, 0.5f, 0.5f); }
        static inline Color             clear() { return Color(0.f, 0.f, 0.f, 0.f); }

        float                           r;
        float                           g;
        float                           b;
        float                           a;
    };

    struct Matrix4x4
    {
        static System::Type             typeof();

        Vector4                         operator*(const Vector4& v) const;

        Vector3                         MultiplyPoint(Vector3& point);

        float                           m00, m10, m20, m30;
        float                           m01, m11, m21, m31;
        float                           m02, m12, m22, m32;
        float                           m03, m13, m23, m33;
    };

    struct Rect
    {
                                        Rect() = default;
                                        Rect(float _x, float _y, float _width, float _height) : x(_x), y(_y), width(_width), height(_height) {}
                                        Rect(const Vector2& pos, const Vector2& size) : x(pos.x), y(pos.y), width(size.x), height(size.y) {}
                                        //Rect(const Rect& source) : x(source.x), y(source.y), width(source.width), height(source.height) {}

        static System::Type             typeof();

        inline float                    xMax() const { return x + width; }
        inline float                    xMin() const { return x; }
        inline float                    yMin() const { return y; }
        inline float                    yMax() const { return y + height; }

        inline Vector2                  GetCenter() const { return Vector2(x + width / 2.f, y + height / 2.f); }
        inline bool                     Contains(const Vector2& point) const { return point.x >= xMin() && point.x < xMax() && point.y >= yMin() && point.y < yMax(); }
        inline bool                     Overlaps(const Rect& other)const  { return other.xMax() > xMin() && other.xMin() < xMax() && other.yMax() > yMin() && other.yMin() < yMax(); }

        static inline Rect              MinMaxRect(float xmin, float ymin, float xmax, float ymax) { return Rect(xmin, ymin, xmax - xmin, ymax - ymin); }
        Hax::Rect                       ToHax() { return Hax::Rect({x, y}, {x + width, y + height}); }

        float                           x;
        float                           y;
        float                           width;
        float                           height;
    };

    class AsyncOperation : public System::Object
    {
    public:
                                        AsyncOperation() : System::Object(nullptr) {}
                                        AsyncOperation(UVM::Object* ptr) : System::Object(ptr) {}

        static System::Type             typeof();

        inline                          operator bool() const { return !null(); }

        bool                            GetIsDone();
        float                           GetProgress();
    };

    class Object : public System::Object
    {
    public:
        Object() = default;
        explicit                        Object(UVM::Object* ptr) : System::Object(ptr) {}
                                        Object(const Object& o) = default;

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();

        System::String                  GetName();
        void                            SetName(System::String name);
        void                            SetHideFlags(HideFlags flags);

        static System::Array<Object>    FindObjectsOfType(System::Type type);
        static Object                   FindObjectOfType(System::Type type);
        static System::Array<Object>    FindObjectsOfTypeAll(System::Type type);
        static void                     Destroy(Object obj, float t = 0.f);

        static Object                   Instantiate(Object original);
        static Object                   Instantiate(Object original, const Vector3& position, const Quaternion& rotation);
        template <class T> static T     Instantiate(T original) { static_assert(std::is_base_of_v<Object, T>); return T(Object::Instantiate(Object(original.GetPtr())).GetPtr()); }
        template <class T> static T     Instantiate(T original, const Vector3& position, const Quaternion& rotation) { static_assert(std::is_base_of_v<Object, T>); return T(Object::Instantiate(Object(original.GetPtr()), position, rotation).GetPtr()); }

        int                             GetInstanceID();
        void*                           GetCachedPtr() { THROW_IF_NULL(); return GetPtr()->m_CachedPtr; }

        inline bool                     null() const { return !m_Ptr || !GetPtr()->m_CachedPtr; }
        inline UVM::UnityObject*        GetPtr() const { return (UVM::UnityObject*)m_Ptr; }
    };

    class Component : public Object
    {
    public:
        Component() = default;
        explicit                        Component(UVM::Object* ptr) : Object(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();

        Transform                       GetTransform();
        GameObject                      GetGameObject();

        Component                       GetComponentInChildren(System::Type);
        template <typename T> T         GetComponentInChildren() { static_assert(std::is_base_of_v<Component, T>); return T(GetComponentInChildren(T::typeof()).m_Ptrect); }

        System::Array<Component>        GetComponentsInChildren(System::Type);
        System::Array<Component>        GetComponentsInChildren(System::Type, bool includeInactive);
        template <typename T> System::Array<T> GetComponentsInChildren() { static_assert(std::is_base_of_v<Component, T>); return System::Array<T>(GetComponentsInChildren(T::typeof()).GetPtr()); }
        template <typename T> System::Array<T> GetComponentsInChildren(bool includeInactive) { static_assert(std::is_base_of_v<Component, T>); return System::Array<T>(GetComponentsInChildren(T::typeof(), includeInactive).GetPtr()); }


        Component                       GetComponent(System::Type);
        template <typename T> T         GetComponent() { static_assert(std::is_base_of_v<Component, T>); return T(GetComponent(T::typeof()).GetPtr()); }


    };

    class Transform : public Component
    {
    public:
        explicit                        Transform(UVM::Object* ptr) : Component(ptr) {}

        inline                          operator bool() const { return !null(); }

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
        Behaviour() = default;
        explicit                        Behaviour(UVM::Object* ptr) : Component(ptr) {}

        static System::Type             typeof();

        inline                          operator bool() const { return !null(); }

        bool                            GetEnabled();
        void                            SetEnabled(bool value);
        bool                            GetIsActiveAndEnabled();
    };

    class MonoBehaviour : public Behaviour
    {
    public:
        MonoBehaviour() = default;
        explicit                        MonoBehaviour(UVM::Object* ptr) : Behaviour(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();
    };

    class Time : public Object
    {
    public:
        static System::Type             typeof();

        static float                    GetTimeScale();
        static void                     SetTimeScale(float scale);
    };

    class GameObject : public Object
    {
    public:
                                        GameObject(UVM::Object* ptr) : Object(ptr) {}

        static GameObject               New();
        static GameObject               New(const char* name);

        static System::Type             typeof();

        inline                          operator bool() const { return !null(); }

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
        template <class T> T            GetComponent() { static_assert(std::is_base_of_v<Component, T>); return T(GetComponent(T::typeof()).GetPtr()); }
        Component                       AddComponent(System::Type componentType);
        template <typename T> T         AddComponent() { static_assert(std::is_base_of_v<Component, T>); return T(AddComponent(T::typeof()).GetPtr()); }
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
                                        CommandBuffer() : System::Object(nullptr) {}
                                        CommandBuffer(UVM::Object* ptr) : System::Object(ptr) {}

        static CommandBuffer            New();
        static System::Type             typeof();

        inline                          operator bool() const { return !null(); }

        void                            DrawRenderer(Renderer renderer, Material material);
        void                            DrawRenderer(Renderer renderer, Material material, int submeshIndex, int shaderPass);
        void                            Clear();
    };

    class Camera : public Behaviour
    {
    public:
        Camera(UVM::Object* ptr) : Behaviour(ptr) {}

        inline                          operator bool() const { return !null(); }
        inline bool                     operator==(const Camera& o) const { return GetPtr() == o.GetPtr(); }

        static System::Type             typeof();
        static Camera                   GetMain();

        float                           GetOrthographicSize();
        void                            SetOrthographicSize(float value);
        int                             GetPixelWidth();
        int                             GetPixelHeight();
        Rect                            GetPixelRect();
        Matrix4x4                       GetProjectionMatrix();
        void                            GetProjectionMatrix_Injected(Matrix4x4* ret);
        Matrix4x4                       GetWorldToCameraMatrix();
        void                            GetWorldToCameraMatrix_Injected(Matrix4x4* ret);
        float                           GetFarClipPlane();
        void                            SetFarClipPlane(float value);
        float                           GetNearClipPlane();
        float                           GetFieldOfView();
        void                            SetFieldOfView(float value);

        Vector3                         WorldToScreenPoint(const Vector3& position);
        void                            RemoveCommandBuffer(CameraEvent event, CommandBuffer cb);
        void                            AddCommandBuffer(CameraEvent event, CommandBuffer cb);

        static Vector3                  WorldToScreenPoint(const Vector3& worldPos, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix, float screenWidth, float screenHeight);
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
        explicit                        Light(UVM::Object* ptr) : Behaviour(ptr) {}

        inline                          operator bool() const { return !null(); }
        
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
        explicit                        Shader(UVM::Object* ptr) : Object(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();

        static Shader                   Find(System::String name);
        static void                     WarmupAllShaders();

        bool IsSupported();
    };

    class Material : public Object
    {
    public:
                                        Material() : Object(nullptr) {}
                                        Material(UVM::Object* ptr) : Object(ptr) {}

        static Material                 New(Shader shader);

        static System::Type             typeof();

        inline                          operator bool() const { return !null(); }

        void                            SetInt(System::String name, int value);
        void                            SetColor(Color color);
        void                            SetColor(System::String name, Color color);

        int                             GetPassCount();

    };

    class Renderer : public Component
    {
    public:
                                        Renderer() : Component(nullptr) {}
        explicit                        Renderer(UVM::Object* ptr) : Component(ptr) {}

        inline                          operator bool() const { return !null(); }

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
        SpriteRenderer() : Renderer(nullptr) {}
        SpriteRenderer(UVM::Object* ptr) : Renderer(ptr) {} 

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();

        Color                           GetColor();
        void                            SetColor(const Color& color);
    };

    class MeshRenderer : public Renderer
    {
    public:
        explicit                        MeshRenderer(UVM::Object* ptr) : Renderer(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();
    };

    class LineRenderer : public Renderer
    {
    public:
                                        LineRenderer() : Renderer(nullptr) {}
        explicit                        LineRenderer(UVM::Object* ptr) : Renderer(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();

        void                            SetStartWidth(float value);
        void                            SetEndWidth(float value);
        void                            SetPositionCount(int value);
        void                            SetPosition(int index, const Vector3& position);
    };

    class SkinnedMeshRenderer : public Renderer
    {
    public:
                                        SkinnedMeshRenderer() : Renderer(nullptr) {}
        explicit                        SkinnedMeshRenderer(UVM::Object* ptr) : Renderer(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();
    };

    struct AssetBundle : Object
    {
                                        AssetBundle() : Object(nullptr) {}
        explicit                        AssetBundle(UVM::Object* ptr) : Object(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();

        static AssetBundle              LoadFromMemory(System::Array<char> binary);
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
        Sprite() : Object(nullptr) {}
        explicit                        Sprite(UVM::Object* ptr) : Object(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();

        System::Array<Vector2>          GetUV();
        Vector4                         GetOuterUVs_Injected();
        Texture2D                       GetTexture();
        Rect                            GetRect_Injected();
        Rect                            GetTextureRect_Injected();
        Rect                            GetTextureRect();
    };

    class RenderSettings : public Object
    {
    public:
        explicit                        RenderSettings(UVM::Object* ptr) : Object(ptr) {}

        static System::Type             typeof();

        static bool                     GetFog();
        static void                     SetFog(bool value);
    };

    class Texture : public Object
    {
    public:
        Texture() : Object(nullptr) {}
        explicit                        Texture(UVM::Object* ptr) : Object(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();

        void*                           GetNativeTexturePtr();

        int                             GetDataWidth();
        int                             GetDataHeight();
    };

    class Texture2D : public Texture
    {
    public:
        Texture2D() : Texture(nullptr) {}
        explicit                        Texture2D(UVM::Object* ptr) : Texture(ptr) {}

        inline                          operator bool() const { return !null(); }
    };

    struct PlayerLoopSystemInternal
    {
        System::Type                    m_Type;
        void*                           m_UpdateDelegate;
        void**                          m_UpdateFunction;
        void*                           m_LoopConditionFunction;
        int                             m_NumSubSystems;
    };

    struct PlayerLoopSystem
    {
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
        ResourceRequest() : AsyncOperation(nullptr) {}
        ResourceRequest(UVM::Object* ptr) : AsyncOperation(ptr) {}

        static System::Type typeof();
        UnityEngine::Object GetAsset();
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

    class Collider2D : public Behaviour
    {
    public:
        Collider2D(UVM::Object* ptr) : Behaviour(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();

        Bounds                          GetBounds();
        void                            SetIsTrigger(bool value);
    };

    class BoxCollider2D : public Collider2D
    {
    public:
        BoxCollider2D(UVM::Object* ptr) : Collider2D(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();
    };

    class Collider : public Component
    {
    public:
        explicit                        Collider(UVM::Object* ptr) : Component(ptr) {}

        inline                          operator bool() { return !null(); }

        static System::Type             typeof();

        bool                            GetEnabled();
        void                            SetEnabled(bool value);
        Bounds                          GetBounds();
    };

    class BoxCollider : public Collider
    {
    public:
        explicit                        BoxCollider(UVM::Object* ptr) : Collider(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();

        Vector3                         GetCenter();
        Vector3                         GetSize();
    };

    class PolygonCollider2D : public Collider2D
    {
    public:
        PolygonCollider2D(UVM::Object* ptr) : Collider2D(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();

        System::Array<Vector2>          GetPoints();
        void                            SetPoints(System::Array<Vector2> value);
    };

    class Rigidbody : public Component
    {
    public:
        explicit                        Rigidbody(UVM::Object* ptr) : Component(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();

        void                            AddForce(const Vector3& force);
        Vector3                         GetVelocity();
        void                            SetVelocity(const Vector3& value);
    };

    class Rigidbody2D : public Component
    {
    public:
        explicit                        Rigidbody2D(UVM::Object* ptr) : Component(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();

        int                             GetAttachedCollidersList_Internal(System::List<Collider2D> results, bool findTriggers);
    };

    struct RaycastHit
    {
        Transform                       GetTransform();

        static System::Type             typeof();

        Vector3                         m_Point;
        Vector3                         m_Normal;
        uint32_t                        m_FaceID;
        float                           m_Distance;
        Vector2                         m_UV;
        int                             m_Collider;
    };

    class Physics
    {
    public:
        static System::Type              typeof();
        static System::Array<RaycastHit> SphereCastAll(const Vector3& origin, float radius, const Vector3& direction, float maxDistance, int layerMask);
    };

    class EventSystem : public MonoBehaviour
    {
    public:
        explicit                        EventSystem(UVM::Object* ptr) : MonoBehaviour(ptr) {}

        static System::Type             typeof();

        static EventSystem              GetCurrent();
    };

    class PostProcessLayer : public MonoBehaviour
    {
    public:
        PostProcessLayer(UVM::Object* ptr) : MonoBehaviour(ptr) {}

        inline                          operator bool() const { return !null(); }

        static System::Type             typeof();

        CommandBuffer                   m_LegacyCmdBufferBeforeReflections();
        CommandBuffer                   m_LegacyCmdBufferBeforeLighting();
        CommandBuffer                   m_LegacyCmdBufferOpaque();
        CommandBuffer                   m_LegacyCmdBuffer();
    };
}

namespace Photon
{
    enum RpcTarget
    {
        All,
        Others,
        MasterClient,
        AllBuffered,
        OthersBuffered,
        AllViaServer,
        AllBufferedViaServer
    };

    struct Player : System::Object
    {
        explicit                        Player(UVM::Object* ptr) : System::Object(ptr) {}

        inline bool                     operator==(const Player& p) const { return p.GetPtr() == this->GetPtr(); }
    };

    struct PhotonView : UnityEngine::MonoBehaviour
    {
        static System::Type             typeof();

        bool&                           IsMine();
        Player                          GetOwner();

        void                            RPC(System::String methodName, RpcTarget target, System::Array<System::Object> parameters);
    };

    struct PhotonNetwork
    {
        static Player                   GetMasterClient();
        static UnityEngine::GameObject  InstantiateRoomObject(System::String name,
                                                              const UnityEngine::Vector3& position,
                                                              const UnityEngine::Quaternion& rotation,
                                                              char8_t group = 0,
                                                              void* data = nullptr);
    };
}
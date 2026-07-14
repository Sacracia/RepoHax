#pragma once

#define HAX_UNITY_SRC
#include "../../hax.h"
#include "../hax_unity.h"

#include <concepts>
#include <initializer_list>

#define THROW_IF_NULL() if (*this == null) System::ThrowHelper::ThrowNullRefException()
#define THROW_IF_RANGE(i, l, r) if (i < l || i > r) System::ThrowHelper::ThrowArgumentOutOfRangeException()
#define THROW_IF_GE(i, v) if (i >= v) System::ThrowHelper::ThrowArgumentOutOfRangeException()
#define HAX_UNITY_ICLASS class __declspec(empty_bases)

inline constexpr std::nullptr_t null = nullptr;

struct Il2CppExceptionWrapper { void* Exception; };

namespace System
{
    class Type;
    class String;
    class Assembly;
    class FieldInfo;
    class MethodInfo;
    class Assembly;
    class Object;
    class ReferenceType;
    struct MethodInfoWrapper;

    struct Boolean;
    struct Byte;
    struct Char;
    struct Double;
    struct Single;
    struct Int32;
    struct UInt32;
    struct IntPtr;

    struct TypeRequest;
    struct MethodRequest;
    struct MethodWrapperRequest;
    struct FieldOffsetRequest;
    struct StaticFieldRequest;
    struct EnumRequest;

    template <typename T = void>
    HAX_UNITY_ICLASS Array;

    template <typename T = void>
    HAX_UNITY_ICLASS List;

    template <typename TKey = void, typename TValue = void>
    HAX_UNITY_ICLASS Dictionary;

    template <typename T>
    concept IsDotNetType = requires { T::typeof; };

    template <typename T>
    concept IsRefType = IsDotNetType<T> && std::derived_from<T, Object>;

    template <typename T>
    concept IsValType = IsDotNetType<T> && !std::derived_from<T, Object>;

    template <typename T>
    concept IsBuiltInValType =  std::is_same_v<T, Boolean> ||
                                std::is_same_v<T, Byte>    ||
                                std::is_same_v<T, Char>    ||
                                std::is_same_v<T, Double>  ||
                                std::is_same_v<T, Single>  ||
                                std::is_same_v<T, Int32>   ||
                                std::is_same_v<T, UInt32>  ||
                                std::is_same_v<T, IntPtr>;

    template <typename T>
    concept IsDictParam = IsRefType<T> || IsBuiltInValType<T>;

    template <IsValType T>
    class Boxed;

    template <typename T>
    concept IsBoxable = HAX_UNITY_IS_MONO && IsValType<T> && !IsBuiltInValType<T>;

    namespace Internal
    {
        void*   UvmMethodGetThunk(void* uvmMethod);
        void*   UvmMethodGetAddress(void* uvmMethod);
        void*   Invoke(void* method, void* __this, void** args, void** ex);
        void    GCSetField(const System::Object& __this, const System::Object* field, const System::Object& value);
        size_t  CreateBoxPool(System::Type type);
        void*   GetBoxedObject(size_t index);
    }
}

namespace System
{
    struct Boolean
    {
                                Boolean(bool val) : Value(val) {}

        static Type             typeof();

                                operator bool() const { return Value; }

        bool                    Value;
    };

    struct Byte
    {
                                Byte(uint8_t val) : Value(val) {}

        static Type             typeof();

                                operator uint8_t() const { return Value; }

        uint8_t                 Value;
    };

    struct Char
    {
                                Char(wchar_t val) : Value(val) {}

        static Type             typeof();

                                operator wchar_t() const { return Value; }

        wchar_t                 Value;
    };

    struct Double
    {
                                Double(double val) : Value(val) {}

        static Type             typeof();

                                operator double() const { return Value; }

        double                  Value;
    };

    struct Single
    {
                                Single(float val) : Value(val) {}

        static Type             typeof();

                                operator float() const { return Value; }

        float                   Value;
    };

    struct Int32
    {
                                Int32(FieldOffsetRequest& req);
                                Int32(int32_t val) : Value(val) {}

        static Type             typeof();

                                operator int32_t() const { return Value; }

        int32_t                 Value;
    };

    struct UInt32
    {
                                UInt32(EnumRequest& req);
                                UInt32(uint32_t val) : Value(val) {}

        static Type             typeof();

                                operator uint32_t() const { return Value; }

        uint32_t                Value;
    };

    struct IntPtr
    {
                                IntPtr(StaticFieldRequest& req);
                                IntPtr(void* val) : Value(val) {}

        static Type             typeof();

                                operator void*() const { return Value; }
        
        void*                   Value;
    };

    struct GCHandle
    {
                                GCHandle(void* v) : Handle(v) {}

        static GCHandle         Alloc(Object obj, bool pinned);
        void                    Free();

        void*                   Handle;
    };

    template <IsValType T>
    Boxed<T> Box(const T& val);

    template <IsValType T>
    Boxed<T> Box();

    template <typename T>
    class IEnumerable
    { };

    class Object
    {
    public:
                                Object() : m_Ptr(nullptr) {}
                                Object(nullptr_t) : m_Ptr(nullptr) {}

                                template <IsValType T>
                                Object(T v) { *this = Box(v); }

        bool                    operator==(const Object& o) const { return m_Ptr == o.m_Ptr; }
        bool                    operator==(std::nullptr_t) const { return m_Ptr == nullptr; }

        static Type             typeof();
        Type                    GetType() const;

        String                  ToString() const;
        bool                    Equals(Object obj) const;

        bool                    IsNull() const { return m_Ptr == nullptr; }
        void*                   GetPtr() const { return m_Ptr; }

    protected:
        union
        {
            void*               m_Ptr;
            struct
            {
                void*           Object[2];
                int             Length;
                wchar_t         Chars[1];
            }* m_UvmString;
            struct
            {
                void*           Object[2];
                void*           Bounds;
                size_t          Length;
                uint64_t        Elements[1];
            }* m_UvmArray;
            struct
            {
                void*           Object[2];
                void*           m_UvmAssembly;
            }* m_UvmReflectionAssembly;
            struct
            {
                void*           Object[2];
                void*           m_UvmMethod;
            }* m_UvmReflectionMethod;
            struct
            {
                void*           Object[2];
                void*           m_UvmClass;
                void*           m_UvmField;
            }* m_UvmReflectionField;
        };
    };

    class Assembly : public Object
    {
    public:
        using Object::Object;
        using Object::GetType;

        static Type             typeof();

        static Assembly         Load(const char* assemblyString);

        Type                    GetType(const char* namespaze, const char* name, bool assertOnError = false) const;
    };

    class Type : public Object
    {
    public:
        using Object::Object;
        using Object::GetType;

                                Type(TypeRequest& req);

        static Type             typeof();
        static Type             GetType(const char* assembly, const char* namespaze, const char* name, bool assertOnError = false);

        Assembly                Assembly() const;
        const char*             Name() const;
        const char*             Namespace() const;

        bool                    IsInstanceOfType(Object o) const;
        bool                    IsSubclassOf(Type type) const;
        bool                    IsValueType() const;

        FieldInfo               GetField(const char* name, bool assertOnError = false) const;
        MethodInfo              GetMethod(const char* name, const char* sig = nullptr, bool assertOnError = false) const;

        Type                    MakeArrayType() const;
        Type                    MakeGenericType(Array<Type>) const;

    protected:
        Object                  CreateInstance() const;
        Object                  CreateInstanceDefaultCtor() const;

        friend class Activator;
    };

    class ThrowHelper
    {
    public:
        [[noreturn]] static void ThrowNullRefException();
        [[noreturn]] static void ThrowArgumentOutOfRangeException();
    };

    class Activator
    {
    public:
        template <IsRefType T>
        static T                CreateInstance() { return std::bit_cast<T>(T::typeof().CreateInstance()); }

        template <IsRefType T>
        static T                CreateInstanceDefaultCtor() { return std::bit_cast<T>(T::typeof().CreateInstanceDefaultCtor()); }
    };

    class String : public Object
    {
    public:
        using Object::Object;

        static String           New(Hax::StringView str);
        static String           New(Hax::WStringView str);

        static Type             typeof();

        template <typename T>
        static String           Join(String separator, const IEnumerable<T>* values);

        wchar_t                 operator[](int i) const         { THROW_IF_NULL(); THROW_IF_RANGE(i, 0, m_UvmString->Length - 1); return m_UvmString->Chars[i]; }

        bool                    operator==(nullptr_t) const     { return m_Ptr == nullptr; }
        bool                    operator==(const String& o) const;
        bool                    operator==(Hax::WStringView o) const;

        const wchar_t*          begin() const                   { THROW_IF_NULL(); return m_UvmString->Chars; }
        const wchar_t*          end() const                     { THROW_IF_NULL(); return &m_UvmString->Chars[m_UvmString->Length]; }

        const wchar_t*          GetRawStringData() const        { return begin(); }

        static String           Concat(String s1, String s2);
        static String           Empty();

        int                     GetLength() const               { THROW_IF_NULL(); return m_UvmString->Length; }
        bool                    StartsWith(Hax::WStringView prefix) const;
        bool                    EndsWith(Hax::WStringView postfix) const;
        bool                    Contains(const wchar_t* s) const;
        String                  Replace(String s1, String s2) const;
        String                  ToUpper() const;

        Hax::WStringView        ToHaxView() const               { THROW_IF_NULL(); return Hax::WStringView{ m_UvmString->Chars, (size_t)m_UvmString->Length}; }
    };

    class Exception : public Object
    {
    public:
        using Object::Object;

        static Type             typeof();

        static Exception        New();
        static Exception        New(String message);

        String                  Message() const;
    };

    class NullReferenceException : public Exception
    {
    public:
        using Exception::Exception;

        static Type                     typeof();

        static NullReferenceException   New();
        static NullReferenceException   New(String message);
    };

    class ArgumentOutOfRangeException : public Exception
    {
    public:
        using Exception::Exception;

        static Type                         typeof();

        static ArgumentOutOfRangeException  New();
    };

    template <>
    class Array<void> : public Object
    {
    public:
        using Object::Object;

        static Type             typeof();

        static Array            CreateInstance(Type elementType, size_t len);

        int                     Length() const { THROW_IF_NULL(); return (int)m_UvmArray->Length; }
    };

    template <IsDotNetType T>
    HAX_UNITY_ICLASS Array<T> : public Object, public IEnumerable<T>
    {
    public:
        using Object::Object;

                                Array(std::initializer_list<T> list);

        static Type             typeof()                    { static Type s_Type = T::typeof().MakeArrayType(); return s_Type; }
        static Array<T>         CreateInstance(size_t len)  { return std::bit_cast<Array<T>>(Array<>::CreateInstance(T::typeof(), len)); }

        T*                      begin()                     { THROW_IF_NULL(); return (T*)&m_UvmArray->Elements; }
        T*                      end()                       { THROW_IF_NULL(); return (T*)&m_UvmArray->Elements + m_UvmArray->Length; }
        const T*                begin() const               { THROW_IF_NULL(); return (T*)&m_UvmArray->Elements; }
        const T*                end() const                 { THROW_IF_NULL(); return (T*)&m_UvmArray->Elements + m_UvmArray->Length; }

        T&                      operator[](size_t i)        { THROW_IF_NULL(); THROW_IF_GE(i, m_UvmArray->Length); return begin()[i]; }
        const T&                operator[](size_t i) const  { THROW_IF_NULL(); THROW_IF_GE(i, m_UvmArray->Length); return begin()[i]; }

        int                     Length() const              { THROW_IF_NULL(); return (int)m_UvmArray->Length; }

        const T&                GetValue(size_t i) const    { THROW_IF_NULL(); THROW_IF_GE(i, m_UvmArray->Length); return *((const T*)&m_UvmArray->Elements + i);  }
        void                    SetValue(const T& val, size_t i) const;

        friend class List<T>;
    };

    struct Enum
    {
        Enum() = delete;

        static Type             typeof();

        static Array<UInt32>    GetValues(Type enumType);
        static Array<String>    GetNames(Type enumType);

        const UInt32 Value;
    };

    template <IsValType T>
    class Boxed : public Object
    {
    public:
        using Object::Object;

        static Type             typeof();

        void                    SetValue(T val) const { Unbox() = val; }
        T&                      Unbox() const { THROW_IF_NULL(); return *(T*)((char*)m_Ptr + sizeof(void*) * 2); }

        static void             ResetIndex() { s_PoolIndex = 0; }

    private:
        inline static Hax::Optional<size_t> s_PoolOffset;
        inline static size_t s_PoolIndex = 0;

        friend Boxed<T> Box<T>(const T& val);
        friend Boxed<T> Box<T>();
    };

    template <IsValType T>
    Boxed<T> Box(const T& val)
    {
        if (!Boxed<T>::s_PoolOffset.HasValue())
            Boxed<T>::s_PoolOffset = Internal::CreateBoxPool(T::typeof());

        size_t& poolOffset = Boxed<T>::s_PoolOffset.Value();
        size_t& poolIndex = Boxed<T>::s_PoolIndex;

        Boxed<T> obj = std::bit_cast<Boxed<T>>(Internal::GetBoxedObject(poolOffset + poolIndex));
        poolIndex = (poolIndex + 1) % HAX_UNITY_BOX_POOL_SIZE;

        obj.SetValue(val);

        return obj;
    }

    template <IsValType T>
    Boxed<T> Box()
    {
        if (!Boxed<T>::s_PoolOffset.HasValue())
            Boxed<T>::s_PoolOffset = Internal::CreateBoxPool(T::typeof());

        size_t& poolOffset = Boxed<T>::s_PoolOffset.Value();
        size_t& poolIndex = Boxed<T>::s_PoolIndex;

        Boxed<T> obj = std::bit_cast<Boxed<T>>(Internal::GetBoxedObject(poolOffset + poolIndex));
        poolIndex = (poolIndex + 1) % HAX_UNITY_BOX_POOL_SIZE;

        return obj;
    }

    namespace MethodAttributes
    {
        enum : uint32_t
        {
            MemberAccessMask      = 0x0007,
            PrivateScope          = 0x0000,
            Private               = 0x0001,
            FamANDAssem           = 0x0002,
            Assembly              = 0x0003,
            Family                = 0x0004,
            FamORAssem            = 0x0005,
            Public                = 0x0006,
            Static                = 0x0010,
            Final                 = 0x0020,
            Virtual               = 0x0040,
            HideBySig             = 0x0080,
            VtableLayoutMask      = 0x0100,
            ReuseSlot             = 0x0000,
            NewSlot               = 0x0100,
            CheckAccessOnOverride = 0x0200,
            Abstract              = 0x0400,
            SpecialName           = 0x0800,
            PinvokeImpl           = 0x2000,
            UnmanagedExport       = 0x0008,
            RTSpecialName         = 0x1000,
            HasSecurity           = 0x4000,
            RequireSecObject      = 0x8000,
            ReservedMask          = 0xD000
        };
    }

    template <typename T>
    concept IsInvokeArgType = (IsRefType<T> || std::is_arithmetic_v<T>);

    struct MethodInfoWrapper;

    enum MethodInfoWrapperFlags
    {
        Address = 1 << 0,
        Thunk = 1 << 1,
        ICall = 1 << 2,
        Each = (1 << 3) - 1
    };

    class MethodInfo : public Object
    {
    public:
        using Object::Object;

                                MethodInfo(MethodRequest& req);

        static Type             typeof();

        MethodInfo              MakeGenericMethod(Array<Type> arr) const;
        MethodInfo              GetVirtualImpl(Object obj) const;
        Type                    DeclaringType() const;

        uint32_t                Attributes() const;
        bool                    IsVirtual() const                   { return (Attributes() & MethodAttributes::Virtual) != 0;  }
        bool                    IsStatic() const                    { return (Attributes() & MethodAttributes::Static) != 0;  }
        bool                    IsAbstract() const                  { return (Attributes() & MethodAttributes::Abstract) != 0;  }

        uint32_t                MethodImplementationFlags() const;
        bool                    IsInternalCall() const              { return MethodImplementationFlags() & 4096; }

        void*                   GetAddress() const;
        void*                   GetThunk() const;
        void*                   GetICall() const;
                                
                                template <typename RetT, typename... Args> requires IsRefType<RetT> && (IsInvokeArgType<Args> && ...)
                                RetT Invoke(Args... args);

        MethodInfoWrapper       Wrap(int flags = MethodInfoWrapperFlags::Each) const;

    private:
                                template <typename T> requires IsInvokeArgType<T>
                                void* Pack(const T& val) { if constexpr (IsRefType<T>) return val.GetPtr(); else return (void*)&val; }

                                friend struct MethodInfoWrapper;
    };

    struct MethodInfoWrapper
    {
                                MethodInfoWrapper() = default;
                                MethodInfoWrapper(MethodWrapperRequest& req);

                                template <typename RetT, typename... Args>
                                RetT Call(Args... args);

                                template <typename RetT, typename... Args>
                                RetT InternalCall(Args... args);

        MethodInfo              Method = null;
        void*                   Address;
        void*                   Thunk;
        void*                   ICall;
        uint32_t                Flags;
        uint32_t                ImplFlags;

    private:
        struct MonoError
        {
            union 
            { 
                uint32_t        Init;
                struct
                { 
                    uint16_t    ErrorCode, Flags;
                    void*       Hidden[30];
                }; 
            };
        };
    };

    namespace FieldAttributes 
    {
        enum : uint32_t
        {
            FieldAccessMask    = 0x0007,
            PrivateScope       = 0x0000,
            Private            = 0x0001,
            FamANDAssem        = 0x0002,
            Assembly           = 0x0003,
            Family             = 0x0004,
            FamORAssem         = 0x0005,
            Public             = 0x0006,    
            Static             = 0x0010,
            InitOnly           = 0x0020,
            Literal            = 0x0040,
            NotSerialized      = 0x0080,
            SpecialName        = 0x0200,
            PinvokeImpl        = 0x2000,
            RTSpecialName      = 0x0400,
            HasFieldMarshal    = 0x1000,
            HasDefault         = 0x8000,
            HasFieldRVA        = 0x0100,
            ReservedMask       = 0x9500
        };
    }

    class FieldInfo : public Object
    {
    public:
        using Object::Object;

        static Type             typeof();

        const char*             Name() const;

        Type                    FieldType() const;

        uint32_t                Attributes() const;
        bool                    IsStatic() const                { return (Attributes() & FieldAttributes::Static) != 0; }
        bool                    IsLiteral() const               { return (Attributes() & FieldAttributes::Literal) != 0; }
        bool                    IsPrivate() const               { return (Attributes() & FieldAttributes::FieldAccessMask) == FieldAttributes::Private; }
        bool                    IsPublic() const                { return (Attributes() & FieldAttributes::FieldAccessMask) == FieldAttributes::Public; }

        uint32_t                GetFieldOffset() const;
        void*                   GetStaticAddress() const;

        template <typename T>
        T                       GetStaticValue()                { T out; GetStaticValue(&out); return out; }
        void                    GetStaticValue(void* out) const;
    };

    template <>
    class List<void> : public Object
    {
    public:
        using Object::Object;

        static Type             typeof();
    };

    template <typename TKey, typename TValue>
    struct KeyValuePair
    {
        static Type             typeof();

        const TKey              Key;
        const TValue            Val;
    };

    template <IsDotNetType T>
    HAX_UNITY_ICLASS List<T> : public Object, public IEnumerable<T>
    {
        struct Fields 
        { 
            void*               Object[2];
            Array<T>            Items;
            int                 Size;
            int                 Version;
        };

    public:
        using Object::Object;

                                List(std::initializer_list<T> iniList);

        static List<T>          New()                           { return Activator::CreateInstanceDefaultCtor<List<T>>(); }
        static List<T>          New(IEnumerable<T>& o);

        void                    Add(const T& item);

        static Type             typeof()                        { static Type s_Type = List<>::typeof().MakeGenericType({T::typeof()}); return s_Type; }

        const T&                operator[](size_t i) const      { THROW_IF_NULL(); Fields* f = (Fields*)m_Ptr; THROW_IF_GE(i, f->Size); return begin()[i]; }

        const T*                begin() const                   { THROW_IF_NULL(); Fields* f = (Fields*)m_Ptr; return (T*)&f->Items.m_UvmArray->Elements; }
        const T*                end() const                     { THROW_IF_NULL(); Fields* f = (Fields*)m_Ptr; return (T*)&f->Items.m_UvmArray->Elements + f->Size; }
        T*                      begin() requires IsValType<T>   { THROW_IF_NULL(); Fields* f = (Fields*)m_Ptr; return (T*)&f->Items.m_UvmArray->Elements; }
        T*                      end() requires IsValType<T>     { THROW_IF_NULL(); Fields* f = (Fields*)m_Ptr; return (T*)&f->Items.m_UvmArray->Elements + f->Size; }

        int                     Count() const                   { THROW_IF_NULL(); return ((Fields*)m_Ptr)->Size; }
        int                     Capacity() const                { THROW_IF_NULL(); return ((Fields*)m_Ptr)->Items.Length(); }
        
        T                       GetValue(size_t i) const;
        void                    SetValue(const T& val, size_t i) const;
    };

    template <>
    class Dictionary<void> : public Object
    {
    public:
        using Object::Object;

        static Type             typeof();
    };

    template <IsDictParam TKey, IsDictParam TValue>
    HAX_UNITY_ICLASS Dictionary<TKey, TValue> : public Object, public IEnumerable<KeyValuePair<TKey, TValue>>
    {
    public:
        using Object::Object;

        static Type             typeof();

        static Dictionary<TKey, TValue> New()  { return Activator::CreateInstanceDefaultCtor<Dictionary<TKey, TValue>>(); }

        TValue                  GetItem(const TKey& key);
        void                    SetItem(const TKey& key, const TValue& val);
        void                    Add(const TKey& key, const TValue& val);
        int                     Count() const;
        bool                    ContainsKey(const TKey& key);
        bool                    ContainsValue(const TValue& val);
        bool                    TryGetValue(const TKey& key, TValue* val);

    private:

        struct Entry 
        { 
            static Type         typeof() { return null; } 
            
            int                 Hash; 
            int                 Next; 
            TKey                Key; 
            TValue              Value; 
        };

        struct Fields 
        { 
            void*               Object[2];
            void*               Buckets;
            Array<Entry>        Entries;
            int                 Count;
                                /*...*/
        };

    public:

        class Iterator
        {
        public:
                                Iterator(Entry* _this, Entry* end) : m_This(_this - 1), m_End(end) { MoveNext(); }

            Iterator&           operator++() { if (m_This != m_End) MoveNext(); return *this; }
            bool                operator!=(const Iterator& other) const { return m_This != other.m_This; }
            Entry&              operator*() const { return *m_This; }

        private:

            void                MoveNext() { do { ++m_This; } while (m_This != m_End && m_This->Hash < 0); }

            Entry*              m_This;
            Entry*              m_End;
        };

        Iterator begin()
        {
            THROW_IF_NULL();
            auto entries = ((Fields*)m_Ptr)->Entries;
            return Iterator(entries.begin(), entries.end()); 
        }

        Iterator end()
        {
            THROW_IF_NULL();
            auto entries = ((Fields*)m_Ptr)->Entries;
            return Iterator(entries.end(), entries.end()); 
        }

        template <typename T> requires (IsRefType<T> || IsValType<T>)
        auto Pack(const T& val)
        { 
            if constexpr (IsBoxable<T>)
                return Box(val);
            else 
                return val;
        }
    };

    //
    // IMPLEMENTATION
    //

    template <typename TKey, typename TValue>
    Type KeyValuePair<TKey, TValue>::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System.Collections.Generic", "KeyValuePair`2", true).MakeGenericType({TKey::typeof(), TValue::typeof()});
        return s_Type;
    }

    template <IsDotNetType T>
    T List<T>::GetValue(size_t i) const 
    { 
        THROW_IF_NULL();

        Fields* fields = (Fields*)m_Ptr; 

        THROW_IF_GE(i, (size_t)fields->Size); 
        return *((T*)&fields->Items.GetValue(i));  
    }

    template <IsDotNetType T>
    void List<T>::SetValue(const T& val, size_t i) const 
    { 
        THROW_IF_NULL(); 

        Fields* fields = (Fields*)m_Ptr; 
        THROW_IF_GE(i, fields->Size);

        fields->Items.SetValue(val, i);
        fields->Version++;
    }

    template <IsDotNetType T>
    void Array<T>::SetValue(const T& val, size_t i) const
    {
        THROW_IF_NULL();
        THROW_IF_GE(i, m_UvmArray->Length);

        T* target = (T*)&m_UvmArray->Elements + i;

        if constexpr (IsValType<T>)
            *target = val;
        else
            Internal::GCSetField(m_Ptr, target, val);
    }

    template <typename T>
    constexpr auto GetAbiRetType() 
    {
        if constexpr (std::is_void_v<T>)
            return std::type_identity<void>{};
        else if constexpr (IsRefType<T>)
            return std::type_identity<void*>{}; 
        else if constexpr (IsBoxable<T>)
            return std::type_identity<Boxed<T>>{};
        else if constexpr (IsBuiltInValType<T>)
            return std::type_identity<decltype(T::Value)>{};
        else
            return std::type_identity<T>{};
    }

    template <typename T>
    constexpr auto BoxOnNeed(const T& val)
    {
        if constexpr (IsBoxable<T>)
            return Box(val);
        else
            return val;
    }

    template <typename T>
    using AbiRetT = typename decltype(GetAbiRetType<T>())::type;

    template <typename RetT, typename... Args>
    RetT MethodInfoWrapper::Call(Args... args)
    {
        HAX_ASSERT(Thunk != nullptr);
        HAX_ASSERT((Flags & MethodAttributes::Abstract) == 0);

        using Func = AbiRetT<RetT>(*)(Args..., void*);
        Func thunk = (Func)Thunk;

        if constexpr (HAX_UNITY_IS_MONO)
        {
            static_assert((!IsBoxable<Args> && ...), "Structs must be boxed in mono");

            void* ex = nullptr;

            if constexpr (std::is_void_v<RetT>)
            {
                thunk(args..., &ex);

                if (ex != nullptr)
                    throw std::bit_cast<System::Exception>(ex);
            }
            else
            {
                AbiRetT<RetT> res = thunk(args..., &ex);

                if (ex != nullptr)
                    throw std::bit_cast<System::Exception>(ex);

                if constexpr (IsBoxable<RetT>)
                    return res.Unbox();
                else
                    return std::bit_cast<RetT>(res);
            }
        }
        else
        {
            try
            {
                if constexpr (std::is_void_v<RetT>)
                {
                    thunk(args..., Method.m_UvmReflectionMethod->m_UvmMethod);
                }
                else
                {
                    return std::bit_cast<RetT>(thunk(args..., Method.m_UvmReflectionMethod->m_UvmMethod));
                }
            }
            catch (Il2CppExceptionWrapper ex)
            {
                throw std::bit_cast<System::Exception>(ex);
            }
        }
    }

    template <typename RetT, typename... Args>
    RetT MethodInfoWrapper::InternalCall(Args... args)
    {
        HAX_ASSERT(ICall != nullptr);
        HAX_ASSERT((ImplFlags & 4096) != 0);

        using Func = AbiRetT<RetT>(*)(Args..., void*);
        Func icall = (Func)ICall;

        if constexpr (HAX_UNITY_IS_MONO)
        {
            MonoError err{};
            if constexpr (IsRefType<RetT>) return std::bit_cast<RetT>(icall(args..., &err));
            else return icall(args..., &err);
        }
        else
        {
            if constexpr (IsRefType<RetT>) return std::bit_cast<RetT>(icall(args..., nullptr));
            else return icall(args..., nullptr);
        }
    }

    template <typename RetT, typename... Args> requires IsRefType<RetT> && (IsInvokeArgType<Args> && ...)
    RetT MethodInfo::Invoke(Args... args)
    {
        HAX_ASSERT(HAX_UNITY_IS_MONO);
        THROW_IF_NULL();

        bool isStatic = IsStatic();
        void* uvmMethod = m_UvmReflectionMethod->m_UvmMethod;

        if constexpr (sizeof... (args) == 0)
            HAX_ASSERT(isStatic);

        void* ex = nullptr;

        void* packedArgs[] = { this->Pack(args)... };
        size_t nArgs = sizeof(packedArgs) / sizeof(void*);

        void* ret;

        if (isStatic)
            ret = Internal::Invoke(uvmMethod, nullptr, packedArgs, &ex);
        else
            ret = Internal::Invoke(uvmMethod, packedArgs[0], nArgs == 1 ? nullptr : packedArgs + 1, &ex);

        if (ex != nullptr)
            throw std::bit_cast<Exception>(ex);

        if constexpr (!std::is_void_v<RetT>) 
            return std::bit_cast<RetT>(ret);
    }

    template <IsDotNetType T>
    Array<T>::Array(std::initializer_list<T> list) : Object(null)
    {
        size_t size = list.size();
        Array<T> arr = CreateInstance(size);
        const T* listItems = list.begin();

        if constexpr (IsValType<T>)
        {
            memcpy(&arr.m_UvmArray->Elements, listItems, size * sizeof(T));
        }
        else
        {
            T* items = (T*)&arr.m_UvmArray->Elements;
            for (size_t i = 0; i < size; ++i)
            {
                Internal::GCSetField(arr, (Object*)(items + i), *(listItems + i));
            }
        }

        *this = arr;
    }

    template <IsDotNetType T>
    List<T>::List(std::initializer_list<T> iniList) : Object(null)
    {
        List<T> list = List<T>::New();

        for (const T& item : iniList)
            list.Add(item);

        *this = list;
    }

    template <IsDotNetType T>
    List<T> List<T>::New(IEnumerable<T>& o) 
    {
        static auto s_IEnumCtor = typeof().GetMethod(".ctor", "System.Void(System.Collections.Generic.IEnumerable", true).Wrap();

        List<T> list = Activator::CreateInstance<List<T>>();
        s_IEnumCtor.Call<void, List<T>, void*>(list, *(void**)&o);

        return list;
    }

    template <IsDotNetType T>
    void List<T>::Add(const T& item)
    {
        THROW_IF_NULL();

        static auto s_Add = GetType().GetMethod("Add", {}, true).Wrap();

        s_Add.Call<void>(*this, BoxOnNeed(item));
    }

    template <typename T>
    String String::Join(String separator, const IEnumerable<T>* values)
    {
        static auto s_Method = typeof().GetMethod("Join", "System.String(System.String,System.Collections.Generic.IEnumerable<T>)", true)
            .MakeGenericMethod({T::typeof()}).GetThunk();

        return s_Method.Call<String, String, void*>(separator, *(void**)values);
    }

    template <IsDictParam TKey, IsDictParam TValue>
    Type Dictionary<TKey, TValue>::typeof() 
    { 
        static Type s_Type = Dictionary<>::typeof().MakeGenericType({TKey::typeof(), TValue::typeof()}); 
        return s_Type; 
    }

    template <IsDictParam TKey, IsDictParam TValue>
    TValue Dictionary<TKey, TValue>::GetItem(const TKey& key)
    {
        THROW_IF_NULL();

        static auto s_Method = typeof().GetMethod("get_Item", nullptr, true).Wrap();
        return s_Method.Call<TValue>(*this, BoxOnNeed(key));
    }

    template <IsDictParam TKey, IsDictParam TValue>
    void Dictionary<TKey, TValue>::SetItem(const TKey& key, const TValue& val)
    {
        THROW_IF_NULL();

        static auto s_Method = typeof().GetMethod("set_Item", nullptr, true).Wrap();
        return s_Method.Call<void>(*this, BoxOnNeed(key), BoxOnNeed(val));
    }

    template <IsDictParam TKey, IsDictParam TValue>
    void Dictionary<TKey, TValue>::Add(const TKey& key, const TValue& val)
    {
        THROW_IF_NULL();

        static auto s_Add = GetType().GetMethod("Add", {}, true).Wrap();
        s_Add.Call<void>(*this, BoxOnNeed(key), BoxOnNeed(val));
    }

    template <IsDictParam TKey, IsDictParam TValue>
    int Dictionary<TKey, TValue>::Count() const
    {
        THROW_IF_NULL();

        static auto s_Add = GetType().GetMethod("get_Count", {}, true).Wrap();
        return s_Add.Call<int>(*this);
    }

    template <IsDictParam TKey, IsDictParam TValue>
    bool Dictionary<TKey, TValue>::ContainsKey(const TKey& key)
    {
        THROW_IF_NULL();

        static auto s_Method = typeof().GetMethod("ContainsKey", nullptr, true).Wrap();
        return s_Method.Call<bool>(*this, BoxOnNeed(key));
    }

    template <IsDictParam TKey, IsDictParam TValue>
    bool Dictionary<TKey, TValue>::ContainsValue(const TValue& val)
    {
        THROW_IF_NULL();

        static auto s_Method = typeof().GetMethod("ContainsValue", nullptr, true).Wrap();
        return s_Method.Call<bool>(*this, val);
    }

    template <IsDictParam TKey, IsDictParam TValue>
    bool Dictionary<TKey, TValue>::TryGetValue(const TKey& key, TValue* val)
    {
        THROW_IF_NULL();

        static auto s_Method = typeof().GetMethod("TryGetValue", nullptr, true).Wrap();
        return s_Method.Call<bool>(*this, key, val);
    }
}

template <System::IsRefType To, System::IsRefType From>
To As(From obj)
{
    if (obj == null)
        return null;

    if constexpr (std::derived_from<From, To>)
        return std::bit_cast<To>(obj);
    else
        return To::typeof().IsInstanceOfType(obj) ? std::bit_cast<To>(obj) : null;
}

template <System::IsRefType Exp, System::IsRefType Act>
bool Is(Act obj)
{
    if (obj == null)
        return false;

    if constexpr (std::derived_from<Act, Exp>)
        return true;
    else
        return Exp::typeof().IsInstanceOfType(obj);
}

namespace System
{
    TypeRequest& RequestType(const char* assembly, const char* namespaze, const char* name);
    MethodRequest& RequestMethod(System::Type& type, const char* name, const char* sig = nullptr);
    MethodWrapperRequest& RequestMethodWrapper(System::Type& type, const char* name, const char* sig = nullptr, int flags = System::MethodInfoWrapperFlags::Each);
    FieldOffsetRequest& RequestFieldOffset(System::Type& type, const char* name);
    StaticFieldRequest& RequestStaticField(System::Type& type, const char* name);
    EnumRequest& RequestEnum(System::Type& type, const char* name);
}
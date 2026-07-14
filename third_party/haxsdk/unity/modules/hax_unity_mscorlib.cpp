#define HAX_UNITY_INCLUDE_UVM
#include "hax_unity_mscorlib.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <bit>

namespace Hax::Unity
{
    extern Hax::LogFile* g_Logger;
    extern bool g_ICallsReturnHandle;
}

// Classes
namespace System
{
    Type Int32::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System", "Int32", true);
        return s_Type;
    }

    Type UInt32::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System", "UInt32", true);
        return s_Type;
    }

    Type Single::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System", "Single", true);
        return s_Type;
    }

    Type Double::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System", "Single", true);
        return s_Type;
    }

    Type Char::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System", "Char", true);
        return s_Type;
    }

    Type IntPtr::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System", "IntPtr", true);
        return s_Type;
    }

    GCHandle GCHandle::Alloc(Object obj, bool pinned)
    {
        void* handle = (obj == null) ? nullptr : uvm_gchandle_new(std::bit_cast<UvmObject*>(obj), pinned);
        return GCHandle{handle};
    }

    void GCHandle::Free()
    {
        if (Handle != nullptr)
            uvm_gchandle_free(Handle);

        Handle = nullptr;
    }

    Type Object::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System", "Object", true);
        return s_Type;
    }

    Type Object::GetType() const
    {
        THROW_IF_NULL();

        UvmClass* klass = uvm_object_get_class((UvmObject*)m_Ptr);
        UvmType* type = uvm_class_get_type(klass);
        UvmReflectionType* refType = uvm_type_get_object(type);

        return std::bit_cast<Type>(refType);
    }

    //! Crash on boxed objects
    String Object::ToString() const
    {
        static MethodInfo s_BaseMethod = typeof().GetMethod("ToString", nullptr, true);
        return s_BaseMethod.GetVirtualImpl(*this).Wrap().Call<String, Object>(*this);
    }

    bool Object::Equals(Object obj) const
    {
        static MethodInfo s_BaseMethod = typeof().GetMethod("Equals", "System.Boolean(System.Object)", true);
        return s_BaseMethod.GetVirtualImpl(*this).Wrap().Call<bool, Object, Object>(*this, obj);
    }

    Type Assembly::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System", "RuntimeAssembly", true);
        return s_Type;
    }

    Assembly Assembly::Load(const char* assemblyString)
    {
        static Hax::HashMap<const char*, UvmReflectionAssembly*> s_Cache;

        UvmReflectionAssembly*& cached = s_Cache.FindOrAdd(assemblyString);

        if (cached == nullptr)
        {
            using Func = void*(*)(void*, void*, MonoError*);
            static Func s_ICall = (Func)uvm_lookup_internal_call(uvm_class_get_method_from_name(uvm_assembly_get_class(), "load_with_partial_name", {}));
            HAX_ASSERT(s_ICall != nullptr);

            UvmString* str = uvm_string_new_len(assemblyString, (uint32_t)strlen(assemblyString));
            void* result = nullptr;

            if constexpr (HAX_UNITY_IS_MONO)
            {
                void* evidence = null;
                MonoError err{};
                result = s_ICall(&str, &evidence, &err);
            }
            else
            {
                result = s_ICall(str, nullptr, nullptr);
            }

            cached = Hax::Unity::g_ICallsReturnHandle ? *(UvmReflectionAssembly**)result : (UvmReflectionAssembly*)result;
        }

        return std::bit_cast<Assembly>(cached);
    }

    Type Assembly::GetType(const char* namespaze, const char* name, bool assertOnError) const
    {
        THROW_IF_NULL();

        UvmAssembly* uvmAssembly = (UvmAssembly*)m_UvmReflectionAssembly->m_UvmAssembly;
        UvmImage* uvmImage = uvm_assembly_get_image(uvmAssembly);
        UvmClass* klass = uvm_class_from_name(uvmImage, namespaze, name);
        if (klass == nullptr)
        {
            HAX_PANIC(!assertOnError, Hax::Unity::g_Logger, L"Class %hs.%hs not found", namespaze, name);
            return null;
        }

        UvmType* type = uvm_class_get_type(klass);
        return std::bit_cast<Type>(uvm_type_get_object(type));
    }

    Type Type::typeof()
    {
        static Type s_Type = GetType("mscorlib", "System", "RuntimeType", true);
        return s_Type;
    }

    Type Type::GetType(const char* assembly, const char* namespaze, const char* name, bool assertOnError)
    {
        UvmReflectionAssembly* refAssembly = std::bit_cast<UvmReflectionAssembly*>(Assembly::Load(assembly));
        if (refAssembly == null)
        {
            HAX_PANIC(!assertOnError, Hax::Unity::g_Logger, L"Assembly %hs not found", name);
            return null;
        }

        UvmImage* img = uvm_assembly_get_image(refAssembly->UvmAssembly);
        UvmClass* klass = uvm_class_from_name(img, namespaze, name);
        if (klass == nullptr)
        {
            HAX_PANIC(!assertOnError, Hax::Unity::g_Logger, L"Class %hs.%hs not found", namespaze, name);
            return null;
        }

        UvmType* type = uvm_class_get_type(klass);
        return std::bit_cast<Type>(uvm_type_get_object(type));
    }

    Assembly Type::Assembly() const
    {
        THROW_IF_NULL();

        using Func = void*(*)(const void*, void*);
        static Func s_ICall = (Func)uvm_lookup_internal_call(uvm_class_get_method_from_name(uvm_runtime_type_handle_get_class(), "GetAssembly", {}));

        void* result;
        if constexpr (HAX_UNITY_IS_MONO)
        {
            MonoError err{};
            result = s_ICall(&m_Ptr, &err);
        }
        else
            result = s_ICall(m_Ptr, nullptr);

        return Hax::Unity::g_ICallsReturnHandle ? *(System::Assembly*)result : std::bit_cast<System::Assembly>(result);
    }

    const char* Type::Name() const
    {
        THROW_IF_NULL();

        UvmClass* klass = uvm_class_from_system_type((UvmReflectionType*)m_Ptr);
        return uvm_class_get_name(klass);
    }

    const char* Type::Namespace() const
    {
        THROW_IF_NULL();

        UvmClass* klass = uvm_class_from_system_type((UvmReflectionType*)m_Ptr);
        return uvm_class_get_namespace(klass);
    }

    bool Type::IsInstanceOfType(Object o) const
    {
        THROW_IF_NULL();

        UvmClass* to = uvm_class_from_system_type((UvmReflectionType*)m_Ptr);
        UvmClass* from = uvm_object_get_class(std::bit_cast<UvmObject*>(o));
        return uvm_class_is_assignable_from(to, from);
    }

    bool Type::IsSubclassOf(Type type) const
    {
        THROW_IF_NULL();

        UvmClass* klass1 = uvm_class_from_system_type((UvmReflectionType*)m_Ptr);
        UvmClass* klass2 = uvm_class_from_system_type(std::bit_cast<UvmReflectionType*>(type));
        return uvm_class_is_subclass_of(klass1, klass2, false);
    }

    bool Type::IsValueType() const
    {
        THROW_IF_NULL();

        UvmClass* klass = uvm_class_from_system_type((UvmReflectionType*)m_Ptr);
        return uvm_class_is_valuetype(klass);
    }

    FieldInfo Type::GetField(const char* name, bool assertOnError) const
    {
        THROW_IF_NULL();

        UvmClass* klass = uvm_class_from_system_type((UvmReflectionType*)m_Ptr);
        UvmField* field = uvm_class_get_field_from_name(klass, name);
        UvmReflectionField* refField = uvm_field_get_object(field);

        HAX_PANIC(!assertOnError || refField != nullptr, Hax::Unity::g_Logger, L"Field %hs.%hs not found", uvm_class_get_name(klass), name);

        return std::bit_cast<FieldInfo>(refField);
    }

    MethodInfo Type::GetMethod(const char* name, const char* sig, bool assertOnError) const
    {
        THROW_IF_NULL();

        UvmClass* klass = uvm_class_from_system_type((UvmReflectionType*)m_Ptr);
        UvmMethod* method = uvm_class_get_method_from_name(klass, name, sig);
        UvmReflectionMethod* refMethod = uvm_method_get_object(method);

        HAX_PANIC(!assertOnError || refMethod != nullptr, Hax::Unity::g_Logger, L"Method %hs.%hs not found", uvm_class_get_name(klass), name);

        return std::bit_cast<MethodInfo>(refMethod);
    }

    Type Type::MakeArrayType() const
    {
        THROW_IF_NULL();

        UvmClass* klass = uvm_class_from_system_type((UvmReflectionType*)m_Ptr);
        UvmClass* arrClass = uvm_array_class_get(klass, 1);
        UvmType* type = uvm_class_get_type(arrClass);

        return std::bit_cast<Type>(uvm_type_get_object(type));
    }

    Type Type::MakeGenericType(Array<Type> types) const
    {
        THROW_IF_NULL();
        static auto s_Method = typeof().GetMethod("MakeGenericType", "System.Type(System.Type[])", true).Wrap();
        return s_Method.Call<Type>(*this, types);
    }

    Object Type::CreateInstance() const
    {
        THROW_IF_NULL();

        UvmClass* klass = uvm_class_from_system_type((UvmReflectionType*)m_Ptr);
        return std::bit_cast<Object>(uvm_object_new(klass));
    }

    Object Type::CreateInstanceDefaultCtor() const
    {
        THROW_IF_NULL();

        UvmClass* klass = uvm_class_from_system_type((UvmReflectionType*)m_Ptr);
        UvmObject* obj = uvm_object_new(klass);
        uvm_runtime_object_init(obj);

        return std::bit_cast<Object>(obj);
    }

    void ThrowHelper::ThrowNullRefException()
    {
        throw NullReferenceException::New();
    }

    void ThrowHelper::ThrowArgumentOutOfRangeException()
    {
        throw ArgumentOutOfRangeException::New();
    }

    String String::New(Hax::StringView str)
    {
        return std::bit_cast<String>(uvm_string_new_len(str.begin(), (uint32_t)str.Length()));
    }

    String String::New(Hax::WStringView str)
    {
        return std::bit_cast<String>(uvm_string_new_utf16(str.begin(), (uint32_t)str.Length()));
    }

    Type String::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System", "String", true);
        return s_Type;
    }

    bool String::operator==(const String& o) const
    {
        return (m_UvmString == o.m_UvmString) || (o.m_UvmString != nullptr && m_UvmString != nullptr && m_UvmString->Length == o.m_UvmString->Length && wcsncmp(m_UvmString->Chars, o.m_UvmString->Chars, (size_t)m_UvmString->Length) == 0);
    }

    bool String::operator==(Hax::WStringView o) const
    {
        return m_UvmString != nullptr && (m_UvmString->Length == (int)o.Length() && wcsncmp(m_UvmString->Chars, o.Data(), (size_t)m_UvmString->Length) == 0);
    }

    String String::Concat(String s1, String s2)
    {
        static auto s_Method = String::typeof().GetMethod("Concat", "System.String(System.String,System.String)", true).Wrap();
        return s_Method.Call<String, String, String>(s1, s2);
    }

    String String::Empty()
    {
        static String* s_Ptr = (String*)String::typeof().GetField("Empty", true).GetStaticAddress();
        return *s_Ptr;
    }

    bool String::StartsWith(Hax::WStringView prefix) const
    {
        return ToHaxView().StartsWith(prefix);
    }

    bool String::EndsWith(Hax::WStringView postfix) const
    {
        return ToHaxView().EndsWith(postfix);
    }

    bool String::Contains(const wchar_t* s) const
    {
        THROW_IF_NULL();

        return wcsstr(m_UvmString->Chars, s) != nullptr;
    }

    String String::Replace(String s1, String s2) const
    {
        THROW_IF_NULL();

        static auto s_Method = typeof().GetMethod("Replace", "System.String(System.String,System.String)", true).Wrap();
        return s_Method.Call<String, String, String, String>(*this, s1, s2);
    }

    String String::ToUpper() const
    {
        THROW_IF_NULL();

        static auto s_Method = typeof().GetMethod("ToUpper", "System.String()").Wrap();
        return s_Method.Call<String, String>(*this);
    }

    Type Exception::typeof()
    {
        static Type s_Type = std::bit_cast<Type>(uvm_type_get_object(uvm_class_get_type(uvm_exception_get_class())));
        return s_Type;
    }

    Exception Exception::New()
    {
        return Activator::CreateInstanceDefaultCtor<Exception>();
    }

    Exception Exception::New(String message)
    {
        static auto s_Ctor = typeof().GetMethod(".ctor", "System.Void(System.String)", true).Wrap();

        Exception ex = Activator::CreateInstance<Exception>();
        s_Ctor.Call<void, Exception, String>(ex, message);

        return ex;
    }

    String Exception::Message() const
    {
        THROW_IF_NULL();
        static MethodInfo s_Method = typeof().GetMethod("get_Message", nullptr, true);
        return s_Method.GetVirtualImpl(*this).Wrap().Call<String, Exception>(*this);
    }
    
    Type NullReferenceException::typeof()
    {
        static Type s_Type = std::bit_cast<Type>(uvm_type_get_object(uvm_class_get_type(uvm_null_reference_exception_get_class())));
        return s_Type;
    }

    NullReferenceException NullReferenceException::New()
    {
        return Activator::CreateInstanceDefaultCtor<NullReferenceException>();
    }

    NullReferenceException NullReferenceException::New(String message)
    {
        static auto s_Ctor = typeof().GetMethod(".ctor", "System.Void(System.String)", true).Wrap();

        NullReferenceException ex = Activator::CreateInstance<NullReferenceException>();
        s_Ctor.Call<void, NullReferenceException, String>(ex, message);

        return ex;
    }

    Type ArgumentOutOfRangeException::typeof()
    {
        static Type s_Type = std::bit_cast<Type>(uvm_type_get_object(uvm_class_get_type(uvm_argument_out_of_range_exception_get_class())));
        return s_Type;
    }

    ArgumentOutOfRangeException ArgumentOutOfRangeException::New()
    {
        return Activator::CreateInstanceDefaultCtor<ArgumentOutOfRangeException>();
    }

    Type Array<void>::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System", "Array", true);
        return s_Type;
    }

    Array<void> Array<void>::CreateInstance(Type elementType, size_t len)
    {
        UvmClass* klass = uvm_class_from_system_type(std::bit_cast<UvmReflectionType*>(elementType));
        return std::bit_cast<Array>(uvm_array_new(klass, len));
    }

    Type Enum::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System", "Enum", true); 
        return s_Type;
    }

    Array<UInt32> Enum::GetValues(Type enumType)
    {
        static auto s_Method = typeof().GetMethod("GetValues", "System.Array(System.Type)", true).Wrap();
        return s_Method.Call<Array<UInt32>>(enumType);
    }

    Array<String> Enum::GetNames(Type enumType)
    {
        static auto s_Method = typeof().GetMethod("GetNames", "System.String[](System.Type)", true).Wrap();
        return s_Method.Call<Array<String>>(enumType);
    }

    Type MethodInfo::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System.Reflection", "RuntimeMethodInfo", true);
        return s_Type;
    }

    MethodInfo MethodInfo::MakeGenericMethod(Array<Type> arr) const
    {
        THROW_IF_NULL();

        static auto s_Method = typeof().GetMethod("MakeGenericMethod", "System.Reflection.MethodInfo(System.Type[])", true).Wrap();
        return s_Method.Call<MethodInfo>(*this, arr);
    }

    MethodInfo MethodInfo::GetVirtualImpl(Object obj) const
    {
        THROW_IF_NULL();

        HAX_ASSERT(IsVirtual());

        if (obj == null)
            return null;

        UvmMethod* impl = uvm_object_get_virtual_method(std::bit_cast<UvmObject*>(obj), (UvmMethod*)m_UvmReflectionMethod->m_UvmMethod);
        return std::bit_cast<MethodInfo>(uvm_method_get_object(impl));
    }

    Type MethodInfo::DeclaringType() const
    {
        THROW_IF_NULL();

        UvmClass* klass = uvm_method_get_class((UvmMethod*)m_Ptr);
        UvmType* type = uvm_class_get_type(klass);

        return std::bit_cast<Type>(uvm_type_get_object(type));
    }

    uint32_t MethodInfo::Attributes() const
    {
        THROW_IF_NULL();

        return uvm_method_get_flags((UvmMethod*)m_UvmReflectionMethod->m_UvmMethod, nullptr);
    }

    uint32_t MethodInfo::MethodImplementationFlags() const
    {
        THROW_IF_NULL();

        uint32_t iflags = 0;
        uvm_method_get_flags((UvmMethod*)m_UvmReflectionMethod->m_UvmMethod, &iflags);
        return iflags;
    }

    void* MethodInfo::GetAddress() const
    {
        THROW_IF_NULL();

        return uvm_method_get_pointer((UvmMethod*)m_UvmReflectionMethod->m_UvmMethod);
    }

    void* MethodInfo::GetThunk() const
    {
        THROW_IF_NULL();

        return uvm_method_get_unmanaged_thunk((UvmMethod*)m_UvmReflectionMethod->m_UvmMethod);
    }

    void* MethodInfo::GetICall() const
    {
        THROW_IF_NULL();

        return uvm_lookup_internal_call((UvmMethod*)m_UvmReflectionMethod->m_UvmMethod);
    }

    MethodInfoWrapper MethodInfo::Wrap(int flags) const
    {
        MethodInfoWrapper wrap;
        wrap.Method = *this;
        wrap.Flags = Attributes();
        wrap.ImplFlags = MethodImplementationFlags();
        if ((flags & MethodInfoWrapperFlags::Address) != 0) wrap.Address = GetAddress();
        if ((flags & MethodInfoWrapperFlags::Thunk) != 0)   wrap.Thunk = GetThunk();
        if ((flags & MethodInfoWrapperFlags::ICall) != 0)   wrap.ICall = GetICall();

        return wrap;
    }

    Type FieldInfo::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System.Reflection", "RuntimeFieldInfo", true);
        return s_Type;
    }

    Type FieldInfo::FieldType() const
    {
        THROW_IF_NULL();

        UvmType* type = uvm_field_get_type((UvmField*)m_UvmReflectionField->m_UvmField);

        return std::bit_cast<Type>(uvm_type_get_object(type));
    }

    const char* FieldInfo::Name() const
    {
        THROW_IF_NULL();

        return uvm_field_get_name((UvmField*)m_UvmReflectionField->m_UvmField);
    }

    uint32_t FieldInfo::Attributes() const
    {
        THROW_IF_NULL();

        return uvm_field_get_flags((UvmField*)m_UvmReflectionField->m_UvmField);
    }

    uint32_t FieldInfo::GetFieldOffset() const
    {
        THROW_IF_NULL();

        return uvm_field_get_offset((UvmField*)m_UvmReflectionField->m_UvmField);
    }

    void* FieldInfo::GetStaticAddress() const
    {
        THROW_IF_NULL();

        UvmField* uvmField = (UvmField*)m_UvmReflectionField->m_UvmField;
        uint32_t offset = uvm_field_get_offset(uvmField);

        HAX_ASSERT(IsStatic());
        HAX_ASSERT(!IsLiteral());
        HAX_ASSERT(offset != (uint32_t)-1);

        UvmClass* klass = uvm_field_get_parent(uvmField);
        return (char*)uvm_class_get_static_field_data(klass) + offset;
    }

    void FieldInfo::GetStaticValue(void* out) const
    {
        THROW_IF_NULL();

        HAX_ASSERT(IsStatic());

        uvm_field_static_get_value((UvmField*)m_UvmReflectionField->m_UvmField, out);
    }

    Type List<void>::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System.Collections.Generic", "List`1", true);
        return s_Type;
    }

    Type Dictionary<void>::typeof()
    {
        static Type s_Type = Type::GetType("mscorlib", "System.Collections.Generic", "Dictionary`2", true);
        return s_Type;
    }
}

// Requests
namespace System
{
    struct TypeRequest 
    { 
        const char *Assembly;
        const char *Namespace;
        const char *Name; 
        Type* Out; 
    };

    struct MethodRequest 
    { 
        Type* Type; 
        const char *Name;
        const char *Sig; 
        MethodInfo* Out; 
    };

    struct MethodWrapperRequest 
    { 
        Type* Type; 
        const char *Name;
        const char *Sig; 
        int Flags; 
        MethodInfoWrapper* Out; 
    };

    struct FieldOffsetRequest 
    { 
        Type* Type; 
        const char* Name; 
        Int32* Out; 
    };

    struct StaticFieldRequest 
    { 
        Type* Type; 
        const char* Name; 
        IntPtr* Out; 
    };

    struct EnumRequest 
    { 
        Type* Type; 
        const char* Name; 
        UInt32* Out; 
    };

    static Hax::Vector<System::TypeRequest>& GetTypeRequests()
    {
        static Hax::Vector<System::TypeRequest> s_Vector;
        return s_Vector;
    }

    static Hax::Vector<System::MethodRequest>& GetMethodRequests()
    {
        static Hax::Vector<System::MethodRequest> s_Vector;
        return s_Vector;
    }

    static Hax::Vector<System::MethodWrapperRequest>& GetMethodWrapperRequests()
    {
        static Hax::Vector<System::MethodWrapperRequest> s_Vector;
        return s_Vector;
    }

    static Hax::Vector<System::FieldOffsetRequest>& GetFieldOffsetRequests()
    {
        static Hax::Vector<System::FieldOffsetRequest> s_Vector;
        return s_Vector;
    }

    static Hax::Vector<System::StaticFieldRequest>& GetStaticFieldRequests()
    {
        static Hax::Vector<System::StaticFieldRequest> s_Vector;
        return s_Vector;
    }

    static Hax::Vector<System::EnumRequest>& GetEnumRequests()
    {
        static Hax::Vector<System::EnumRequest> s_Vector;
        return s_Vector;
    }

    void HandleMetadataRequests()
    {
        for (TypeRequest& request : GetTypeRequests())
            *request.Out = System::Type::GetType(request.Assembly, request.Namespace, request.Name, true);
        for (MethodRequest& request : GetMethodRequests())
            *request.Out = request.Type->GetMethod(request.Name, request.Sig, true);
        for (MethodWrapperRequest& request : GetMethodWrapperRequests())
            *request.Out = request.Type->GetMethod(request.Name, request.Sig, true).Wrap(request.Flags);
        for (FieldOffsetRequest& request : GetFieldOffsetRequests())
            *request.Out = request.Type->GetField(request.Name, true).GetFieldOffset();
        for (StaticFieldRequest& request : GetStaticFieldRequests())
            *request.Out = request.Type->GetField(request.Name, true).GetStaticAddress();
        for (EnumRequest& request : GetEnumRequests())
        {
            System::FieldInfo field = request.Type->GetField(request.Name, true);
            HAX_ASSERT(field.IsLiteral());
            field.GetStaticValue(request.Out);
        }
    }

    TypeRequest& RequestType(const char* assembly, const char* namespaze, const char* name)
    {
        auto& requests = GetTypeRequests();
        requests.PushBack(TypeRequest{.Assembly = assembly,
            .Namespace = namespaze, 
            .Name = name});
        return requests.Last();
    }

    MethodRequest& RequestMethod(System::Type& type, const char* name, const char* sig)
    {
        auto& requests = GetMethodRequests();
        requests.PushBack(MethodRequest{.Type = &type,
            .Name = name,
            .Sig = sig});
        return requests.Last();
    }

    MethodWrapperRequest& RequestMethodWrapper(System::Type& type, const char* name, const char* sig, int flags)
    {
        auto& requests = GetMethodWrapperRequests();//!
        requests.PushBack(MethodWrapperRequest{.Type = &type,
            .Name = name,
            .Sig = sig,
            .Flags = flags});
        return requests.Last();
    }

    FieldOffsetRequest& RequestFieldOffset(System::Type& type, const char* name)
    {
        auto& requests = GetFieldOffsetRequests();
        requests.PushBack(FieldOffsetRequest{.Type = &type,
            .Name = name});
        return requests.Last();
    }

    StaticFieldRequest& RequestStaticField(System::Type& type, const char* name)
    {
        auto& requests = GetStaticFieldRequests();
        requests.PushBack(StaticFieldRequest{.Type = &type,
            .Name = name});
        return requests.Last();
    }

    EnumRequest& RequestEnum(System::Type& type, const char* name)
    {
        auto& requests = GetEnumRequests();
        requests.PushBack(EnumRequest{.Type = &type,
            .Name = name});
        return requests.Last();
    }


    Int32::Int32(FieldOffsetRequest& req)
    {
        req.Out = this;
    }

    UInt32::UInt32(EnumRequest& req) 
    { 
        req.Out = this; 
    }

    IntPtr::IntPtr(StaticFieldRequest& req) 
    { 
        req.Out = this;
    }

    Type::Type(System::TypeRequest& req) : Object(null) 
    { 
        req.Out = this;
    }

    MethodInfo::MethodInfo(MethodRequest& req) : Object(null) 
    { 
        req.Out = this; 
    }

    MethodInfoWrapper::MethodInfoWrapper(MethodWrapperRequest& req) 
    { 
        req.Out = this; 
    }
}

// Internal
namespace System::Internal
{
    Hax::Vector<void*> g_BoxesPool;

    void* UvmMethodGetThunk(void* uvmMethod)
    {
        return uvm_method_get_unmanaged_thunk((UvmMethod*)uvmMethod);
    }

    void* UvmMethodGetAddress(void* uvmMethod)
    {
        return uvm_method_get_pointer((UvmMethod*)uvmMethod);
    }

    void* Invoke(void* method, void* __this, void** args, void** ex)
    {
        return uvm_runtime_invoke((UvmMethod*)method, __this, args, ex);
    }

    void GCSetField(const System::Object& __this, const System::Object* field, const System::Object& value)
    {
        uvm_gc_wbarrier_set_field(std::bit_cast<const UvmObject*>(__this), std::bit_cast<UvmObject**>(field), std::bit_cast<UvmObject*>(value));
    }

    size_t CreateBoxPool(Type type)
    {
        g_BoxesPool.Reserve(HAX_UNITY_BOX_POOL_SIZE * 25);
        size_t poolOffset = g_BoxesPool.Size();

        UvmClass* klass = uvm_class_from_system_type(std::bit_cast<UvmReflectionType*>(type));

        for (size_t i = 0; i < HAX_UNITY_BOX_POOL_SIZE; ++i)
        {
            UvmObject* object = uvm_object_new(klass);
            uvm_gchandle_new(object, true);
            g_BoxesPool.PushBack(object);
        }

        return poolOffset;
    }

    void* GetBoxedObject(size_t index)
    {
        return g_BoxesPool[index];
    }
}
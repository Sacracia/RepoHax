#include "hax_unity.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

namespace Hax::Unity
{
    struct Context
    {
        HMODULE hVirtualMachine;
        LogFile* LogFile;

        HashMap<UVM::Type*, UVM::ReflectionType*> CachedRefTypes;
    };

    static Context* g_Context;

    struct OffsetRequest
    {
        int*                Out;
        ClassInfo           ClassInfo;
        const char*         FieldName;
    };

    struct StaticFieldRequest
    {
        void**              Out;
        ClassInfo           ClassInfo;
        const char*         FieldName;
    };

    struct EnumLiteralRequest
    {
        int*                Out;
        ClassInfo           EnumInfo;
        const char*         LiteralName;
    };

    struct MethodRequest
    {
        System::MethodInfo* Out;
        ClassInfo           ClassInfo;
        const char*         MethodName;
        const char*         MethodSig;
    };

    struct TypeRequest
    {
        System::Type*       Out;
        ClassInfo           ClassInfo;
    };

    static Hax::Vector<OffsetRequest>& GetOffsetRequests()
    {
        static Hax::Vector<OffsetRequest> s_Vec;
        return s_Vec;
    }

    static Hax::Vector<StaticFieldRequest>& GetStaticFieldRequests()
    {
        static Hax::Vector<StaticFieldRequest> s_Vec;
        return s_Vec;
    }

    static Hax::Vector<EnumLiteralRequest>& GetEnumLiteralRequests()
    {
        static Hax::Vector<EnumLiteralRequest> s_Vec;
        return s_Vec;
    }

    static Hax::Vector<MethodRequest>& GetMethodRequests()
    {
        static Hax::Vector<MethodRequest> s_Vec;
        return s_Vec;
    }

    static Hax::Vector<TypeRequest>& GetTypeRequests()
    {
        static Hax::Vector<TypeRequest> s_Vec;
        return s_Vec;
    }

    static void RequestOffset(int& out, const ClassInfo& classInfo, const char* fieldName)
    {
        if (!Hax::Unity::IsUnityProcess())
            return;

        if (g_Context == nullptr)
        {
            OffsetRequest request = {&out, classInfo, fieldName};
            GetOffsetRequests().PushBack(request);
            return;
        }

        out = System::AppDomain::GetCurrent().Load(classInfo.Assembly).GetType(classInfo.Namespace, classInfo.Name).GetField(fieldName).GetOffset();
    }

    static void RequestStaticField(void*& out, const ClassInfo& classInfo, const char* fieldName)
    {
        if (!Hax::Unity::IsUnityProcess())
            return;

        if (g_Context == nullptr)
        {
            StaticFieldRequest request = {&out, classInfo, fieldName};
            GetStaticFieldRequests().PushBack(request);
            return;
        }

        out = System::AppDomain::GetCurrent().Load(classInfo.Assembly).GetType(classInfo.Namespace, classInfo.Name).GetField(fieldName).GetValuePtr(nullptr);
    }

    static void RequestEnumLiteral(int& out, const ClassInfo& enumInfo, const char* literalName)
    {
        if (!Hax::Unity::IsUnityProcess())
            return;

        if (g_Context == nullptr)
        {
            EnumLiteralRequest request = {&out, enumInfo, literalName};
            GetEnumLiteralRequests().PushBack(request);
            return;
        }

        out = System::AppDomain::GetCurrent().Load(enumInfo.Assembly).GetType(enumInfo.Namespace, enumInfo.Name).GetField(literalName).GetEnumValue();
    }

    static void RequestMethod(System::MethodInfo& out, const ClassInfo& classInfo, const char* name, const char* sig)
    {
        if (!Hax::Unity::IsUnityProcess())
            return;

        if (g_Context == nullptr)
        {
            MethodRequest request = {&out, classInfo, name, sig};
            GetMethodRequests().PushBack(request);
            return;
        }

        UVM::Class& klass = UVM::ImageFindClass(UVM::AssemblyGetImage(UVM::FindAssembly(classInfo.Assembly)), classInfo.Namespace, classInfo.Name);
        out.m_Base = &UVM::ClassFindMethod(klass, name, sig);
        out.m_Pointer = UVM::MethodGetPointer(*out.m_Base);
        out.m_Thunk = UVM::MethodGetThunk(*out.m_Base);
    }

    static void RequestType(System::Type& out, const ClassInfo& typeInfo)
    {
        if (!Hax::Unity::IsUnityProcess())
            return;

        if (g_Context == nullptr)
        {
            TypeRequest request = {&out, typeInfo};
            GetTypeRequests().PushBack(request);
            return;
        }

        out = System::AppDomain::GetCurrent().Load(typeInfo.Assembly).GetType(typeInfo.Namespace, typeInfo.Name);
    }

    Offset::Offset(const ClassInfo& classInfo, const char* name)
    {
        RequestOffset(Value, classInfo, name);
    }

    StaticField::StaticField(const ClassInfo& classInfo, const char* name)
    {
        RequestStaticField(Address, classInfo, name);
    }

    EnumLiteral::EnumLiteral(const ClassInfo& classInfo, const char* name)
    {
        RequestEnumLiteral(Value, classInfo, name);
    }

    Handle GetUvmHandle()
    {
        if (g_Context != nullptr && g_Context->hVirtualMachine != 0)
            return (Handle)g_Context->hVirtualMachine;

        MODULEENTRY32W me = {};
        me.dwSize = sizeof(MODULEENTRY32W);

        HMODULE hVirtualMachine = 0;
        HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
        if (::Module32FirstW(hSnapshot, &me))
        {
            do
            {
                if (wcsstr(me.szModule, L"mono") != nullptr)
                {
                    hVirtualMachine = me.hModule;
                    break;
                }
            } while (::Module32NextW(hSnapshot, &me));
        }
        ::CloseHandle(hSnapshot);

        return (Handle)hVirtualMachine;
    }

    void Initialize(LogFile* logFile)
    {
        HAX_ASSERT(g_Context == nullptr);

        g_Context = New<Context>();
        g_Context->hVirtualMachine = (HMODULE)GetUvmHandle(); HAX_ASSERT(g_Context->hVirtualMachine != 0);
        g_Context->LogFile = logFile;

        if (logFile != nullptr)
            Hax::LogDebug(*logFile, L"Initializing predefined data...");

        UVM::Thread& thread = UVM::ThreadAttach();
        for (auto& request : GetOffsetRequests())       { RequestOffset(*request.Out, request.ClassInfo, request.FieldName); }
        for (auto& request : GetStaticFieldRequests())  { RequestStaticField(*request.Out, request.ClassInfo, request.FieldName); }
        for (auto& request : GetEnumLiteralRequests())  { RequestEnumLiteral(*request.Out, request.EnumInfo, request.LiteralName); }
        for (auto& request : GetMethodRequests())       { RequestMethod(*request.Out, request.ClassInfo, request.MethodName, request.MethodSig); }
        for (auto& request : GetTypeRequests())         { RequestType(*request.Out, request.ClassInfo); }
        UVM::ThreadDetach(thread);
    
        GetOffsetRequests().ClearFree();
        GetStaticFieldRequests().ClearFree();
        GetEnumLiteralRequests().ClearFree();
        GetMethodRequests().ClearFree();
        GetTypeRequests().ClearFree();
    }

    void Shutdown()
    {
        HAX_ASSERT(g_Context != nullptr);
        Delete(g_Context);
    }

    bool IsUnityProcess()
    {
        static int s_Cached = -1;
        if (s_Cached >= 0)
            return (bool)s_Cached;

        DWORD processId = ::GetProcessId(::GetCurrentProcess());
        HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

        MODULEENTRY32W me = {};
        me.dwSize = sizeof(MODULEENTRY32W);
        if (::Module32FirstW(hSnapshot, &me))
        {
            do
            {
                if (_wcsicmp(me.szModule, L"UnityPlayer.dll") == 0)
                {
                    ::CloseHandle(hSnapshot);
                    s_Cached = 1;
                    return true;
                }
            } while (::Module32NextW(hSnapshot, &me));
        }

        ::CloseHandle(hSnapshot);
        s_Cached = 0;
        return false;
    }

    void ThrowNullRef()
    {
        throw System::NullReferenceException::New();
    }

    void ThrowOutOfRange()
    {
        throw System::ArgumentOutOfRangeException::New();
    }
}

namespace UVM
{
    using GFunc = void(__cdecl*)(void* data, void* user_data);

    static FARPROC GetVMProc(const char* procName);
    static void _cdecl AssemblyEnumerator(void* iter, void* args);
    static Method* ClassGetMethods(Class& klass, void** iter);
    static void GetMethodSignature(Method& method, Hax::StringBuilder<256>& sb);

    struct EnumeratorParams
    {
        const char*         TargetName;
        Assembly*           Out;
    };

    struct Method
    {
        uint16_t            Flags;
        uint16_t            iFlags;
        uint32_t            Token;
        Class*              Class;
        MethodSignature*    Signature;
        const char*         Name;
        /* ... */
    };

    struct VTable
    {
        Class*              Class;
    };

    struct Type
    {
        union 
        {
            Class*          Class;
            Type*           Type;
            ArrayType*      Array;
            MethodSignature* Method;
            GenericParam*   GenericParam;
            GenericClass*   GenericClass;
        }                   Data;
        uint16_t            Attrs;
    };

    struct Field
    {
        Type*               Type;
        const char*         Name;
        Class*              Parent;
        int                 Offset;
    };

    struct Exception : Object
    {
        String*             ClassName;
        String*             Message;
        Object*             Data;
        Object*             InnerEx;
        String*             HelpLink;
        void**              TraceIps;
        String*             StackTrace;
        String*             RemoteStackTrace;
        int	                Remote_stack_index;
        Object*             Dynamic_methods;
        int	                HResult;
        String*             Source;
        Object*             SerializationManager;
        Object*             CapturedTraces;
        void**              NativeTraceIps;
        int                 CaughtInUnmanaged;
    };

    Image& AssemblyGetImage(Assembly& assembly)
    {
        static Image&(*s_ApiProc)(Assembly&) = (decltype(s_ApiProc))GetVMProc("mono_assembly_get_image");
        return s_ApiProc(assembly);
    }

    Domain& GetRootDomain()
    {
        static Domain&(*s_ApiProc)() = (decltype(s_ApiProc))GetVMProc("mono_get_root_domain");
        return s_ApiProc();
    }

    Assembly& FindAssembly(const char* name)
    {
        Assembly* ret = TryFindAssembly(name);
        HAX_PANIC(ret != nullptr, Hax::Unity::g_Context->LogFile, L"Assembly %hs not found", name);
        return *ret;
    }

    Assembly* TryFindAssembly(const char* name)
    {
        static void(*s_ApiProc)(GFunc, void*) = (decltype(s_ApiProc))GetVMProc("mono_assembly_foreach");

        EnumeratorParams params{};
        params.TargetName = name;

        s_ApiProc(AssemblyEnumerator, &params);
        return params.Out;
    }

    const char* ImageGetName(Image& image)
    {
        static const char*(*s_ApiProc)(Image&) = (decltype(s_ApiProc))GetVMProc("mono_image_get_name");
        return s_ApiProc(image);
    }

    Image& ImageGetCorlib()
    {
        static Image&(*s_ApiProc)() = (decltype(s_ApiProc))GetVMProc("mono_get_corlib");
        return s_ApiProc();
    }

    Class* ImageTryFindClass(Image& image, const char* nameSpace, const char* name)
    {
        static Class*(*s_ApiProc)(Image&,const char*,const char*) = (decltype(s_ApiProc))GetVMProc("mono_class_from_name");
        return s_ApiProc(image, nameSpace, name);
    }

    Class& ImageFindClass(Image& image, const char* nameSpace, const char* name)
    {
        Class* ret = ImageTryFindClass(image, nameSpace, name);
        HAX_PANIC(ret != nullptr, Hax::Unity::g_Context->LogFile, L"Class %hs.%hs not found", nameSpace, name);
        return *ret;
    }

    Thread& ThreadAttach()
    {
        static Thread&(*s_ApiProc)(Domain&) = (decltype(s_ApiProc))GetVMProc("mono_thread_attach");
        return s_ApiProc(GetRootDomain());
    }

    void ThreadDetach(Thread& thread)
    {
        static void(*s_ApiProc)(Thread&) = (decltype(s_ApiProc))GetVMProc("mono_thread_detach");
        return s_ApiProc(thread);
    }

    Field* ClassTryFindField(Class& klass, const char* name)
    {
        static Field*(*s_ApiProc)(Class&, const char*) = (decltype(s_ApiProc))GetVMProc("mono_class_get_field_from_name");
        return s_ApiProc(klass, name);
    }

    Field& ClassFindField(Class& klass, const char* name)
    {
        Field* ret = ClassTryFindField(klass, name);
        HAX_PANIC(ret != nullptr, Hax::Unity::g_Context->LogFile, L"Field %hs not found", name);
        return *ret;
    }

    Method& ClassFindMethod(Class& klass, const char* name, const char* sig)
    {
        Method* ret = ClassTryFindMethod(klass, name, sig);
        HAX_PANIC(ret != nullptr, Hax::Unity::g_Context->LogFile, L"Method %hs.%hs not found", ClassGetName(klass), name);
        return *ret;
    }

    Method* ClassTryFindMethod(Class& klass, const char* name, const char* sig)
    {
        Class* currentClass = &klass;
        do
        {
            void* iter = nullptr;
            while (Method* method = ClassGetMethods(*currentClass, &iter))
            {
                const char* methodName = MethodGetName(*method);
                if (strcmp(methodName, name) == 0)
                {
                    if (!sig || !sig[0])
                        return method;

                    Hax::StringBuilder<256> sb;
                    GetMethodSignature(*method, sb);
                    if (strcmp(sb.CStr(), sig) == 0)
                        return method;
                }
            }

            currentClass = ClassGetParent(*currentClass);
        } while (currentClass != nullptr);

        return nullptr;
    }

    Type& ClassGetType(Class& klass)
    {
        static Type&(*s_ApiProc)(Class&) = (decltype(s_ApiProc))GetVMProc("mono_class_get_type");
        return s_ApiProc(klass);
    }

    VTable& ClassGetVTable(Class& klass)
    {
        static VTable&(*s_ApiProc)(Domain&,Class&) = (decltype(s_ApiProc))GetVMProc("mono_class_vtable");
        return s_ApiProc(GetRootDomain(), klass);
    }

    const char* ClassGetName(Class& klass)
    {
        static const char*(*s_ApiProc)(Class&) = (decltype(s_ApiProc))GetVMProc("mono_class_get_name");
        return s_ApiProc(klass);
    }

    uint8_t* ClassGetStaticFieldData(Class& klass)
    {
        static uint8_t*(*s_ApiProc)(VTable&) = (decltype(s_ApiProc))GetVMProc("mono_vtable_get_static_field_data");
        return s_ApiProc(ClassGetVTable(klass));
    }

    Class* ClassGetParent(Class& klass)
    {
        static Class*(*s_ApiProc)(Class&) = (decltype(s_ApiProc))GetVMProc("mono_class_get_parent");
        return s_ApiProc(klass);
    }

    void* MethodGetPointer(Method& method)
    {
        static void*(*s_ApiProc)(Method&) = (decltype(s_ApiProc))GetVMProc("mono_compile_method");
        return s_ApiProc(method);
    }

    Object* MethodInvoke(Method& method, void* __this, void** args, Exception** ex)
    {
        static Object*(*s_ApiProc)(Method&,void*,void**,Exception**) = (decltype(s_ApiProc))GetVMProc("mono_runtime_invoke");
        return s_ApiProc(method, __this, args, ex);
    }

    void* MethodGetThunk(Method& method)
    {
        static void*(*s_ApiProc)(Method&) = (decltype(s_ApiProc))GetVMProc("mono_method_get_unmanaged_thunk");
        return s_ApiProc(method);
    }

    Class& MethodGetClass(Method& method)
    {
        return *method.Class;
    }

    const char* MethodGetName(Method& method)
    {
        return method.Name;
    }

    MethodSignature& MethodGetSignature(Method& method)
    {
        static auto s_ApiProc = (MethodSignature&(*)(Method&))GetVMProc("mono_method_signature");
        return s_ApiProc(method);
    }

    Type& SignatureGetReturnType(MethodSignature& sig)
    {
        static Type&(*s_ApiProc)(MethodSignature&) = (decltype(s_ApiProc))GetVMProc("mono_signature_get_return_type");
        return s_ApiProc(sig);
    }

    uint32_t SignatureGetParamCount(MethodSignature& sig)
    {
        static uint32_t(*s_ApiProc)(MethodSignature&) = (decltype(s_ApiProc))GetVMProc("mono_signature_get_param_count");
        return s_ApiProc(sig);
    }

    Type* SignatureGetParams(MethodSignature& sig, void** iter)
    {
        static Type*(*s_ApiProc)(MethodSignature&,void**) = (decltype(s_ApiProc))GetVMProc("mono_signature_get_params");
        return s_ApiProc(sig, iter);
    }

    const char* TypeGetName(Type& type)
    {
        static const char*(*s_ApiProc)(Type&) = (decltype(s_ApiProc))GetVMProc("mono_type_get_name");
        return s_ApiProc(type);
    }

    ReflectionType& TypeGetReflectionType(Type& type)
    {
        ReflectionType*& refType = Hax::Unity::g_Context->CachedRefTypes.FindOrAdd(&type);
        if (refType == nullptr)
        {
            refType = &TypeCreateReflectionType(type);
            GCHandleNew(*refType, true);
        }

        return *refType;
    }

    ReflectionType& TypeCreateReflectionType(Type& type)
    {
        static ReflectionType&(*s_ApiProc)(Domain&, Type&) = (decltype(s_ApiProc))GetVMProc("mono_type_get_object");
        return s_ApiProc(GetRootDomain(), type);
    }

    Class& TypeGetClass(Type& type)
    {
        static auto s_ApiProc = (Class&(__cdecl*)(Type&))GetVMProc("mono_class_from_mono_type");
        return s_ApiProc(type);
    }

    Object& ObjectNew(Class& klass)
    {
        static Object&(*s_ApiProc)(Domain&, Class&) = (decltype(s_ApiProc))GetVMProc("mono_object_new");
        return s_ApiProc(GetRootDomain(), klass);
    }

    void ObjectCtor(Object& obj)
    {
        static void(*s_ApiProc)(Object&) = (decltype(s_ApiProc))GetVMProc("mono_runtime_object_init");
        s_ApiProc(obj);
    }

    Class& ObjectGetClass(Object& obj)
    {
        return *obj.VTable->Class;
    }

    bool FieldIsStatic(Field& field)
    {
        return field.Type->Attrs & 0x0010;
    }

    int FieldGetOffset(Field& field)
    {
        return field.Offset;
    }

    const char* FieldGetName(Field& field)
    {
        return field.Name;
    }

    bool FieldIsLiteral(Field& field)
    {
        return field.Type->Attrs & 0x0040;
    }

    void* FieldGetValuePtr(Field& field, void* __this)
    {
        if (FieldIsStatic(field))
        {
            uint8_t* staticData = ClassGetStaticFieldData(*field.Parent);
            return staticData + field.Offset;
        }
        return (uint8_t*)__this + field.Offset;
    }

    void FieldGetStaticValue(Field& field, void* value)
    {
        static void(*s_ApiProc)(VTable&,Field&,void*) = (decltype(s_ApiProc))GetVMProc("mono_field_static_get_value");
        s_ApiProc(ClassGetVTable(*field.Parent), field, value);
    }

    String* ExceptionGetMessage(Exception& exc)
    {
        return exc.Message;
    }

    String* ExceptionGetStackTrace(Exception& exc)
    {
        return exc.StackTrace;
    }

    Exception& ExceptionGetNullReference()
    {
        static Exception* s_Instance;

        if (s_Instance == nullptr)
        {
            Object& obj = ObjectNew(ImageFindClass(ImageGetCorlib(), "System", "NullReferenceException"));
            ObjectCtor(obj);
            GCHandleNew(obj, true);
            s_Instance = (Exception*)&obj;
        }

        return *s_Instance;
    }

    Exception& ExceptionGetArgumentOutOfRange()
    {
        static Exception* s_Instance;

        if (s_Instance == nullptr)
        {
            Object& obj = ObjectNew(ImageFindClass(ImageGetCorlib(), "System", "ArgumentOutOfRangeException"));
            ObjectCtor(obj);
            GCHandleNew(obj, true);
            s_Instance = (Exception*)&obj;
        }

        return *s_Instance;
    }

    Exception& ExceptionGetTargetException(String* message)
    {
        static Exception* s_Instance;

        if (s_Instance == nullptr)
        {
            Object& obj = ObjectNew(ImageFindClass(ImageGetCorlib(), "System", "TargetException"));
            ObjectCtor(obj);
            GCHandleNew(obj, true);
            s_Instance = (Exception*)&obj;
        }

        return *s_Instance;
    }

    uint32_t GCHandleNew(Object& obj, bool pinned)
    {
        static uint32_t(*s_ApiProc)(Object&,bool) = (decltype(s_ApiProc))GetVMProc("mono_gchandle_new");
        return s_ApiProc(obj, pinned);
    }

    uint32_t GCHandleNewWeak(Object& obj, bool trackResurrection)
    {
        static uint32_t(*s_ApiProc)(Object&,bool) = (decltype(s_ApiProc))GetVMProc("mono_gchandle_new_weakref");
        return s_ApiProc(obj, trackResurrection);
    }

    Object* GCHandleGetTarget(uint32_t handle)
    {
        if (handle == 0)
            return nullptr;

        static Object*(*s_ApiProc)(uint32_t) = (decltype(s_ApiProc))GetVMProc("mono_gchandle_get_target");
        return s_ApiProc(handle);
    }

    void GCHandleFree(uint32_t handle)
    {
        if (handle != 0)
        {
            static void(*s_ApiProc)(uint32_t) = (decltype(s_ApiProc))GetVMProc("mono_gchandle_free");
            s_ApiProc(handle);
        }
    }

    Object* ArrayNew(Class& klass, size_t size)
    {
        static Object*(*s_ApiProc)(Domain&,Class&,size_t) = (decltype(s_ApiProc))GetVMProc("mono_array_new");
        return s_ApiProc(GetRootDomain(), klass, size);
    }

    String* StringNew(const char* content)
    {
        static String*(*s_ApiProc)(Domain&,const char*) = (decltype(s_ApiProc))GetVMProc("mono_string_new");
        return s_ApiProc(GetRootDomain(), content);
    }

    ReflectionType* StringGetReflectionType()
    {
        Class& klass = ImageFindClass(ImageGetCorlib(), "System", "String");
        return &TypeGetReflectionType(ClassGetType(klass));
    }

    ReflectionType::ReflectionType(UVM::Type* type) : Object(nullptr)
    {
        static UVM::VTable& s_VTable = UVM::ClassGetVTable(UVM::ImageFindClass(UVM::ImageGetCorlib(), "System", "RuntimeType"));
        VTable = &s_VTable;
        Monitor = nullptr;
        Type = type;
    }

    //

    static FARPROC GetVMProc(const char* procName)
    {
        FARPROC ptr = ::GetProcAddress(Hax::Unity::g_Context->hVirtualMachine, procName);
        HAX_PANIC(ptr != nullptr, Hax::Unity::g_Context->LogFile, L"Proc %hs not found", procName);
        return ptr;
    }

    static void _cdecl AssemblyEnumerator(void* iter, void* args)
    {
        Assembly& assembly = *(Assembly*)iter;
        auto params = (EnumeratorParams*)args;
        const char* assemblyName = ImageGetName(AssemblyGetImage(assembly));
        if (strcmp(assemblyName, params->TargetName) == 0)
            params->Out = &assembly;
    }

    static Method* ClassGetMethods(Class& klass, void** iter)
    {
        static auto s_ApiProc = (Method*(*)(Class&, void**))GetVMProc("mono_class_get_methods");
        return s_ApiProc(klass, iter);
    }

    static void GetMethodSignature(Method& method, Hax::StringBuilder<256>& sb)
    {
        MethodSignature& signature = MethodGetSignature(method);
        Type& returnType = SignatureGetReturnType(signature);
        sb.Append(TypeGetName(returnType));
        sb.Append("(");

        uint32_t nParams = SignatureGetParamCount(signature);
        if (nParams > 0)
        {
            void* iter = nullptr;
            sb.Append(TypeGetName(*SignatureGetParams(signature, &iter)));
            while (Type* paramType = SignatureGetParams(signature, &iter))
            {
                sb.Append(",");
                sb.Append(TypeGetName(*paramType));
            }
        }
        sb.Append(")");
    }
}

namespace System
{
    Type Int32::typeof()
    {
        static Type s_Type = Assembly::Corlib().GetType("System", "Int32");
        return s_Type;
    }

    Type Single::typeof()
    {
        static Type s_Type = Assembly::Corlib().GetType("System", "Single");
        return s_Type;
    }

    Type Char::typeof()
    {
        static Type s_Type = Assembly::Corlib().GetType("System", "Single");
        return s_Type;
    }

    Type Boolean::typeof()
    {
        static Type s_Type = Assembly::Corlib().GetType("System", "Boolean");
        return s_Type;
    }

    Type IntPtr::typeof()
    {
        static Type s_Type = Assembly::Corlib().GetType("System", "IntPtr");
        return s_Type;
    }

    Type Object::GetType()
    {
        THROW_IF_NULL(); 
        UVM::Type& type = UVM::ClassGetType(UVM::ObjectGetClass(*this->GetPtr()));
        return Type(&UVM::TypeGetReflectionType(type));
    }

    Type Object::typeof()
    {
        static Type s_Type = Assembly::Corlib().GetType("System", "Object");
        return s_Type;
    }

    Type Assembly::TryGetType(const char* nameSpace, const char* name)
    {
        THROW_IF_NULL();

        UVM::Class* klass = UVM::ImageTryFindClass(*m_Ptr, nameSpace, name);
        if (klass != nullptr)
        {
            UVM::ReflectionType& refType = UVM::TypeGetReflectionType(UVM::ClassGetType(*klass));
            return Type(&refType);
        }

        return Type(nullptr);
    }

    Type Assembly::GetType(const char* nameSpace, const char* name)
    {
        THROW_IF_NULL();
        UVM::Type& type = UVM::ClassGetType(UVM::ImageFindClass(*m_Ptr, nameSpace, name));
        return Type(&UVM::TypeCreateReflectionType(type));
    }

    Type::Type(const Hax::Unity::ClassInfo& info)
    {
        Hax::Unity::RequestType(*this, info);
    }

    MethodInfo::MethodInfo(const Hax::Unity::ClassInfo& classInfo, const char* name, const char* sig)
    {
        Hax::Unity::RequestMethod(*this, classInfo, name, sig);
    }

    bool Type::operator==(const Type& other) const
    {
        return m_Ptr != nullptr && other.m_Ptr != nullptr && other.m_PtrType->Type == m_PtrType->Type;
    }

    FieldInfo Type::GetField(const char* name)
    {
        THROW_IF_NULL(); 
        return FieldInfo(&UVM::ClassFindField(UVM::TypeGetClass(*GetPtr()->Type), name));
    }

    bool Type::HasField(const char* name, FieldInfo* out)
    {
        THROW_IF_NULL(); 
        UVM::Field* field = UVM::ClassTryFindField(UVM::TypeGetClass(*GetPtr()->Type), name);
        if (out && field)
            out->m_Ptr = field;
        return field != nullptr;
    }

    MethodInfo Type::GetMethod(const char* name, const char* sig)
    { 
        THROW_IF_NULL();

        UVM::Class& klass = UVM::TypeGetClass(*GetPtr()->Type);

        MethodInfo methodInfo;
        methodInfo.m_Base = &UVM::ClassFindMethod(klass, name, sig);
        methodInfo.m_Pointer = UVM::MethodGetPointer(*methodInfo.m_Base);
        methodInfo.m_Thunk = UVM::MethodGetThunk(*methodInfo.m_Base);
        return methodInfo;
    }

    Object Type::CreateInstanceForAnotherGenericParameter(Type genericType, Type genericArgument)
    {
        static MethodInfo s_Method = Assembly::Corlib().GetType("System", "RuntimeType").GetMethod("CreateInstanceForAnotherGenericParameter");
        return s_Method.CallThunk<Object, Type, Type>(genericType, genericArgument);
    }

    String Path::Combine(String path1, String path2)
    {
        static MethodInfo s_Method = Assembly::Corlib().GetType("System.IO", "Path").GetMethod("Combine");
        return s_Method.CallThunk<String, String, String>(path1, path2);
    }

    Type String::typeof()
    {
        static Type s_Type = Assembly::Corlib().GetType("System", "String");
        return s_Type;
    }

    wchar_t& String::operator[](int i)
    {
        THROW_IF_NULL();
        THROW_IF_RANGE(i, 0, m_PtrStr->GetLength() - 1);

        return m_PtrStr->operator[](i);
    }

    const wchar_t& String::operator[](int i) const
    {
        THROW_IF_NULL();
        THROW_IF_RANGE(i, 0, m_PtrStr->GetLength() - 1);

        return m_PtrStr->operator[](i);
    }

    bool String::operator==(const String& o) const
    {
        if (this->null() || o.null())
        return false;
        if (o.m_PtrStr == m_PtrStr)
        return true;
        return o.GetLength() == this->GetLength() && wcscmp(o.GetRawStringData(), this->GetRawStringData()) == 0;
    }

    bool String::operator==(const wchar_t* wstr) const
    {
        THROW_IF_NULL();

        size_t len = (size_t)this->GetLength();
        return wcsncmp(this->GetRawStringData(), wstr, len) == 0 && wstr[len] == L'\0';
    }

    wchar_t* String::begin()
    {
        THROW_IF_NULL();

        return m_PtrStr->begin();
    }

    const wchar_t* String::begin() const
    {
        THROW_IF_NULL();

        return m_PtrStr->begin();
    }

    wchar_t* String::end()
    {
        THROW_IF_NULL();

        return m_PtrStr->begin() + m_PtrStr->GetLength();
    }

    const wchar_t* String::end() const
    {
        THROW_IF_NULL();

        return m_PtrStr->begin() + m_PtrStr->GetLength();
    }

    String String::Concat(String s1, String s2)
    {
        static MethodInfo s_Method = Assembly::Corlib().GetType("System", "String").GetMethod("Concat", "System.String(System.String,System.String)");
        return s_Method.CallThunk<String, String, String>(s1, s2);
    }

    String String::Empty()
    {
        static auto s_Ptr = (UVM::Object**)Assembly::Corlib().GetType("System", "String").GetField("Empty").GetValuePtr(nullptr);
        return String(*s_Ptr);
    }

    int String::GetLength() const
    {
        THROW_IF_NULL();

        return m_PtrStr->GetLength();
    }

    wchar_t* String::GetRawStringData() const
    {
        THROW_IF_NULL();

        return m_PtrStr->GetRawStringData();
    }

    bool String::StartsWith(const wchar_t* prefix)
    {
        THROW_IF_NULL();

        wchar_t* iter = m_PtrStr->begin();
        while (*prefix && *prefix == *iter) { ++prefix; ++iter; }
        return *prefix == 0;
    }

    bool String::EndsWith(const wchar_t* postfix)
    {
        THROW_IF_NULL();

        size_t postfixLen = wcslen(postfix);
        return wcscmp(m_PtrStr->begin() + m_PtrStr->GetLength() - postfixLen, postfix) == 0;
    }

    bool String::Contains(const wchar_t* substr)
    {
        THROW_IF_NULL();

        return wcsstr(m_PtrStr->begin(), substr);
    }

    String String::Replace(String s1, String s2)
    {
        THROW_IF_NULL();

        static MethodInfo s_Method = Assembly::Corlib().GetType("System", "String").GetMethod("Replace", "System.String(System.String,System.String)");
        return s_Method.CallThunk<String, String, String, String>(*this, s1, s2);
    }

    String String::ToUpper()
    {
        THROW_IF_NULL();

        static MethodInfo s_Method = Assembly::Corlib().GetType("System", "String").GetMethod("ToUpper", "System.String()");
        return s_Method.CallThunk<String, String>(*this);
    }

    /*bool MethodInfo::Hook(void* detour)
    {
        THROW_IF_NULL();

        auto res = safetyhook::InlineHook::create(m_Pointer, detour);
        if (res)
        {
            m_Orig = std::move(*res);
        return true;
        }

        HAX_LOG_ERROR("Unable to hook %s.%s. Error = %d", m_Base->GetClass()->GetName(), m_Base->GetName(), (int)res.error().allocator_error);

        return true;
    }*/

    Assembly AppDomain::Load(const char* name) 
    { 
        THROW_IF_NULL();
        return Assembly(&UVM::AssemblyGetImage(UVM::FindAssembly(name)));
    }

    String Exception::ToString()
    {
        THROW_IF_NULL();
        static MethodInfo s_Method = Assembly::Corlib().GetType("System", "Exception").GetMethod("ToString", "System.String()");
        return s_Method.CallThunk<String, Exception>(*this);
    }

    void* FieldInfo::GetValuePtr(Object __this)
    {
        THROW_IF_NULL();
        if (!this->IsStatic() && !__this)
            throw TargetException::New(String::New("Non-static field requires a target"));

        return UVM::FieldGetValuePtr(*m_Ptr, __this.GetPtr());
    }
}

namespace UnityEngine
{
    System::Type AsyncOperation::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "AsyncOperation");
        return s_Type;
    }

    bool AsyncOperation::GetIsDone()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(AsyncOperation::typeof().GetMethod("get_isDone"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<bool, AsyncOperation>(*this);
        #else
        return s_Method.CallThunk<bool, AsyncOperation>(*this);
        #endif
    }

    float AsyncOperation::GetProgress()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(AsyncOperation::typeof().GetMethod("get_progress"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<float, AsyncOperation>(*this);
        #else
        return s_Method.CallThunk<float, AsyncOperation>(*this);
        #endif
    }

    System::Type Behaviour::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Behaviour");
        return s_Type;
    }

    bool Behaviour::GetEnabled()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Behaviour::typeof().GetMethod("get_enabled"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<bool, Behaviour>(*this);
        #else
        return s_Method.CallThunk<bool, Behaviour>(*this);
        #endif
    }

    bool Behaviour::GetIsActiveAndEnabled()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Behaviour::typeof().GetMethod("get_isActiveAndEnabled"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<bool, Behaviour>(*this);
        #else
        return s_Method.CallThunk<bool, Behaviour>(*this);
        #endif
    }

    //
    // MonoBehaviour
    //

    System::Type MonoBehaviour::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "MonoBehaviour");
        return s_Type;
    }

    //
    // Time
    //

    System::Type Time::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Time");
        return s_Type;
    }

    float Time::GetTimeScale()
    {
        static System::MethodInfo s_Method(Time::typeof().GetMethod("get_timeScale"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<float>();
        #else
        return s_Method.CallThunk<float>();
        #endif
    }

    void Time::SetTimeScale(float scale)
    {
        static System::MethodInfo s_Method(Time::typeof().GetMethod("set_timeScale"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, float>(scale);
        #else
        s_Method.CallThunk<void, float>(scale);
        #endif
    }

    void Behaviour::SetEnabled(bool value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Behaviour::typeof().GetMethod("set_enabled"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Behaviour, bool>(*this, value);
        #else
        s_Method.CallThunk<void, Behaviour, bool>(*this, value);
        #endif
    }

    CommandBuffer CommandBuffer::New()
    {
        UVM::Object& obj = UVM::ObjectNew(UVM::TypeGetClass(*CommandBuffer::typeof().GetPtr()->Type));
        UVM::ObjectCtor(obj);
        return CommandBuffer(&obj);
    }

    System::Type CommandBuffer::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine.Rendering", "CommandBuffer");
        return s_Type;
    }

    void CommandBuffer::DrawRenderer(Renderer renderer, Material material)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(CommandBuffer::typeof().GetMethod("DrawRenderer", "System.Void(UnityEngine.Renderer,UnityEngine.Material)"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, CommandBuffer, Renderer, Material>(*this, renderer, material);
        #else
        s_Method.CallThunk<void, CommandBuffer, Renderer, Material>(*this, renderer, material);
        #endif
    }

    void CommandBuffer::DrawRenderer(Renderer renderer, Material material, int submeshIndex, int shaderPass)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(CommandBuffer::typeof().GetMethod("DrawRenderer", "System.Void(UnityEngine.Renderer,UnityEngine.Material,System.Int32,System.Int32)"));

        //#ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.CallThunk<void, CommandBuffer, Renderer, Material, int, int>(*this, renderer, material, submeshIndex, shaderPass);
        //#else
        //s_Method.Address<void, CommandBuffer, Renderer, Material, int, int>(*this, renderer, material, submeshIndex, shaderPass);
        //#endif
    }

    void CommandBuffer::Clear()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(CommandBuffer::typeof().GetMethod("Clear"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, CommandBuffer>(*this);
        #else
        s_Method.CallThunk<void, CommandBuffer>(*this);
        #endif
    }

    //
    // Camera
    //
    System::Type Camera::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Camera");
        return s_Type;
    }

    Camera Camera::GetMain()
    {
        static System::MethodInfo s_Method(Camera::typeof().GetMethod("get_main"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Camera>();
        #else
        return s_Method.CallThunk<Camera>();
        #endif
    }

    Vector3 Camera::WorldToScreenPoint(const Vector3& worldPos, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix, float screenWidth, float screenHeight)
    {
        UnityEngine::Vector4 worldPos4(worldPos.x, worldPos.y, worldPos.z, 1.0f);

        UnityEngine::Vector4 viewSpace = viewMatrix * worldPos4;
        UnityEngine::Vector4 clipSpace = projectionMatrix * viewSpace;

        if (clipSpace.w < 0.1f)
        return UnityEngine::Vector3(-9999, -9999, -9999);

        UnityEngine::Vector3 ndc;
        ndc.x = clipSpace.x / clipSpace.w;
        ndc.y = clipSpace.y / clipSpace.w;
        ndc.z = clipSpace.z / clipSpace.w;

        UnityEngine::Vector3 screenPos;
        screenPos.x = (ndc.x * 0.5f + 0.5f) * screenWidth;
        screenPos.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * screenHeight;
        screenPos.z = ndc.z;

        return screenPos;
    }

    Vector3 Camera::WorldToScreenPoint(const Vector3& pos)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("WorldToScreenPoint", "UnityEngine.Vector3(UnityEngine.Vector3)"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Vector3, Camera, Vector3>(*this, pos);
        #else
        System::Boxed<Vector3> boxed{pos};
        return s_Method.CallThunk<System::Boxed<Vector3>*, Camera, System::Boxed<Vector3>*>(*this, &boxed)->m_Value;
        #endif
    }

    void Camera::RemoveCommandBuffer(CameraEvent event, CommandBuffer cb)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("RemoveCommandBuffer"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<void, Camera, CameraEvent, CommandBuffer>(*this, event, cb);
        #else
        return s_Method.CallThunk<void, Camera, CameraEvent, CommandBuffer>(*this, event, cb);
        #endif
    }

    void Camera::AddCommandBuffer(CameraEvent event, CommandBuffer cb)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("AddCommandBuffer"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<void, Camera, CameraEvent, CommandBuffer>(*this, event, cb);
        #else
        return s_Method.CallThunk<void, Camera, CameraEvent, CommandBuffer>(*this, event, cb);
        #endif
    }

    float Camera::GetOrthographicSize()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("get_orthographicSize"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<float, Camera>(*this);
        #else
        return s_Method.CallThunk<float, Camera>(*this);
        #endif
    }

    void Camera::SetOrthographicSize(float value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("set_orthographicSize"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<void, Camera, float>(*this, value);
        #else
        return s_Method.CallThunk<void, Camera, float>(*this, value);
        #endif
    }

    int Camera::GetPixelWidth()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("get_pixelWidth"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<int, Camera>(*this);
        #else
        return s_Method.CallThunk<int, Camera>(*this);
        #endif
    }

    int Camera::GetPixelHeight()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("get_pixelHeight"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<int, Camera>(*this);
        #else
        return s_Method.CallThunk<int, Camera>(*this);
        #endif
    }

    Rect Camera::GetPixelRect()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("get_pixelRect"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Rect, Camera>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Rect>*, Camera>(*this)->m_Value;
        #endif
    }

    Matrix4x4 Camera::GetProjectionMatrix()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("get_projectionMatrix"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Matrix4x4, Camera>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Matrix4x4>*, Camera>(*this)->m_Value;
        #endif
    }

    void Camera::GetProjectionMatrix_Injected(Matrix4x4* ret)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("get_projectionMatrix_Injected"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Camera, Matrix4x4*>(*this, ret);
        #else
        System::Boxed<Matrix4x4> boxed{*ret};
        s_Method.CallThunk<void, Camera, System::Boxed<Matrix4x4>*>(*this, &boxed);
        *ret = boxed.m_Value;
        #endif
    }

    Matrix4x4 Camera::GetWorldToCameraMatrix()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("get_worldToCameraMatrix"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Matrix4x4, Camera>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Matrix4x4>*, Camera>(*this)->m_Value;
        #endif
    }

    void Camera::GetWorldToCameraMatrix_Injected(Matrix4x4* ret)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("get_worldToCameraMatrix_Injected"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Camera, Matrix4x4*>(*this, ret);
        #else
        System::Boxed<Matrix4x4> boxed{*ret};
        s_Method.CallThunk<void, Camera, System::Boxed<Matrix4x4>*>(*this, &boxed);
        *ret = boxed.m_Value;
        #endif
    }

    float Camera::GetFarClipPlane()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("get_farClipPlane"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<float, Camera>(*this);
        #else
        return s_Method.CallThunk<float, Camera>(*this);
        #endif
    }

    void Camera::SetFarClipPlane(float value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("set_farClipPlane"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Camera, float>(*this, value);
        #else
        s_Method.CallThunk<void, Camera, float>(*this, value);
        #endif
    }

    float Camera::GetNearClipPlane()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("get_nearClipPlane"));
       
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<float, Camera>(*this);
        #else
        return s_Method.CallThunk<float, Camera>(*this);
        #endif
    }

    float Camera::GetFieldOfView()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("get_fieldOfView"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<float, Camera>(*this);
        #else
        return s_Method.CallThunk<float, Camera>(*this);
        #endif
    }

    void Camera::SetFieldOfView(float value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Camera::typeof().GetMethod("set_fieldOfView"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Camera, float>(*this, value);
        #else
        s_Method.CallThunk<void, Camera, float>(*this, value);
        #endif
    }

    System::Type Cursor::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Cursor");
        return s_Type;
    }

    void Cursor::SetVisible(bool value)
    {
        static System::MethodInfo s_Method(Cursor::typeof().GetMethod("set_visible"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, bool>(value);
        #else
        s_Method.CallThunk<void, bool>(value);
        #endif
    }

    //
    // Component
    //

    System::Type Component::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Component");
        return s_Type;
    }

    Transform Component::GetTransform()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Component::typeof().GetMethod("get_transform"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Transform, Component>(*this);
        #else
        return s_Method.CallThunk<Transform, Component>(*this);
        #endif
    }

    GameObject Component::GetGameObject()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method = Component::typeof().GetMethod("get_gameObject");
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<GameObject, Component>(*this);
        #else
        return s_Method.CallThunk<GameObject, Component>(*this);
        #endif
    }

    Component Component::GetComponentInChildren(System::Type type)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method = Component::typeof().GetMethod("GetComponentInChildren", "UnityEngine.Component(System.Type)");

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Component, Component, System::Type>(*this, type);
        #else
        return s_Method.CallThunk<Component, Component, System::Type>(*this, type);
        #endif
    }

    System::Array<Component> Component::GetComponentsInChildren(System::Type type)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method = Component::typeof().GetMethod("GetComponentsInChildren", "UnityEngine.Component[](System.Type)");

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<System::Array<Component>, Component, System::Type>(*this, type);
        #else
        return s_Method.CallThunk<System::Array<Component>, Component, System::Type>(*this, type);
        #endif
    }

    System::Array<Component> Component::GetComponentsInChildren(System::Type type, bool includeInactive)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method = Component::typeof().GetMethod("GetComponentsInChildren", "UnityEngine.Component[](System.Type,System.Boolean)");

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<System::Array<Component>, Component, System::Type, bool>(*this, type, includeInactive);
        #else
        return s_Method.CallThunk<System::Array<Component>, Component, System::Type, bool>(*this, type, includeInactive);
        #endif
    }

    Component Component::GetComponent(System::Type type)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method = Component::typeof().GetMethod("GetComponent", "UnityEngine.Component(System.Type)");
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Component, Component, System::Type>(*this, type);
        #else
        return s_Method.CallThunk<Component, Component, System::Type>(*this, type);
        #endif
    }

    //
    // GameObject
    //
    GameObject GameObject::New()
    {
        return GameObject(&UVM::ObjectNew(UVM::TypeGetClass(*GameObject::typeof().GetPtr()->Type)));
    }

    GameObject GameObject::New(const char* name)
    {
        static System::MethodInfo s_Method(GameObject::typeof().GetMethod(".ctor", "System.Void(System.String)"));
        GameObject go = GameObject::New();

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, GameObject, System::String>(go, System::String::New(name));
        #else
        s_Method.CallThunk<void, GameObject, System::String>(go, System::String::New(name));
        #endif
        return go;
    }

    System::Type GameObject::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "GameObject");
        return s_Type;
    }

    Transform GameObject::GetTransform()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(GameObject::typeof().GetMethod("get_transform"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Transform, GameObject>(*this);
        #else
        return s_Method.CallThunk<Transform, GameObject>(*this);
        #endif
    }

    int GameObject::GetLayer()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(GameObject::typeof().GetMethod("get_layer"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<int, GameObject>(*this);
        #else
        return s_Method.CallThunk<int, GameObject>(*this);
        #endif
    }

    void GameObject::SetLayer(int value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(GameObject::typeof().GetMethod("set_layer"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, GameObject, int>(*this, value);
        #else
        s_Method.CallThunk<void, GameObject, int>(*this, value);
        #endif
    }

    void GameObject::SetTag(System::String tag)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(GameObject::typeof().GetMethod("set_tag"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, GameObject, System::String>(*this, tag);
        #else
        s_Method.CallThunk<void, GameObject, System::String>(*this, tag);
        #endif
    }

    bool GameObject::CompareTag(System::String tag)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(GameObject::typeof().GetMethod("CompareTag"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<bool, GameObject, System::String>(*this, tag);
        #else
        return s_Method.CallThunk<bool, GameObject, System::String>(*this, tag);
        #endif
    }

    bool GameObject::GetActiveSelf()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(GameObject::typeof().GetMethod("get_activeSelf"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<bool, GameObject>(*this);
        #else
        return s_Method.CallThunk<bool, GameObject>(*this);
        #endif
    }

    Component GameObject::GetComponent(System::Type type)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(GameObject::typeof().GetMethod("GetComponent", "UnityEngine.Component(System.Type)"));
       
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Component, GameObject, System::Type>(*this, type);
        #else
        return s_Method.CallThunk<Component, GameObject, System::Type>(*this, type);
        #endif
    }

    Component GameObject::AddComponent(System::Type componentType)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(GameObject::typeof().GetMethod("AddComponent", "UnityEngine.Component(System.Type)"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Component, GameObject, System::Type>(*this, componentType);
        #else
        return s_Method.CallThunk<Component, GameObject, System::Type>(*this, componentType);
        #endif
    }

    System::Array<Component> GameObject::GetComponentsInChildren(System::Type type, bool includeInactive)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(GameObject::typeof().GetMethod("GetComponentsInChildren", "Component[](System.Type,System.Boolean)"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<System::Array<Component>, GameObject, System::Type, bool>(*this, type, includeInactive);
        #else
        return s_Method.CallThunk<System::Array<Component>, GameObject, System::Type, bool>(*this, type, includeInactive);
        #endif
    }

    System::Type LayerMask::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "LayerMask");
        return s_Type;
    }

    int LayerMask::NameToLayer(System::String name)
    {
        static System::MethodInfo s_Method(LayerMask::typeof().GetMethod("NameToLayer"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<int, System::String>(name);
        #else
        return s_Method.CallThunk<int, System::String>(name);
        #endif
    }

    void GameObject::SetActive(bool value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(GameObject::typeof().GetMethod("set_active"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, GameObject, bool>(*this, value);
        #else
        s_Method.CallThunk<void, GameObject, bool>(*this, value);
        #endif
    }

    bool GameObject::GetActive()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(GameObject::typeof().GetMethod("get_active"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<bool, GameObject>(*this);
        #else
        return s_Method.CallThunk<bool, GameObject>(*this);
        #endif
    }

    bool GameObject::GetActiveInHierarchy()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(GameObject::typeof().GetMethod("get_activeInHierarchy"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<bool, GameObject>(*this);
        #else
        return s_Method.CallThunk<bool, GameObject>(*this);
        #endif
    }

    //
    // Light
    //

    System::Type Light::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Light");
        return s_Type;
    }

    void Light::SetIntensity(float value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Light::typeof().GetMethod("set_intensity"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Light, float>(*this, value);
        #else
        s_Method.CallThunk<void, Light, float>(*this, value);
        #endif
    }

    float Light::GetIntensity()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Light::typeof().GetMethod("get_intensity"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<float, Light>(*this);
        #else
        return s_Method.CallThunk<float, Light>(*this);
        #endif
    }

    void Light::SetRange(float value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Light::typeof().GetMethod("set_range"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Light, float>(*this, value);
        #else
        s_Method.CallThunk<void, Light, float>(*this, value);
        #endif
    }

    float Light::GetRange()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Light::typeof().GetMethod("get_range"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<float, Light>(*this);
        #else
        return s_Method.CallThunk<float, Light>(*this);
        #endif
    }

    void Light::SetSpotAngle(float value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Light::typeof().GetMethod("set_spotAngle"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Light, float>(*this, value);
        #else
        s_Method.CallThunk<void, Light, float>(*this, value);
        #endif
    }

    float Light::GetSpotAngle()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Light::typeof().GetMethod("get_spotAngle"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<float, Light>(*this);
        #else
        return s_Method.CallThunk<float, Light>(*this);
        #endif
    }

    //
    // LineRenderer
    //

    System::Type LineRenderer::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "LineRenderer");
        return s_Type;
    }

    void LineRenderer::SetStartWidth(float value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(LineRenderer::typeof().GetMethod("set_startWidth"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, LineRenderer, float>(*this, value);
        #else
        s_Method.CallThunk<void, LineRenderer, float>(*this, value);
        #endif
    }

    void LineRenderer::SetEndWidth(float value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(LineRenderer::typeof().GetMethod("set_endWidth"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<void, LineRenderer, float>(*this, value);
        #else
        return s_Method.CallThunk<void, LineRenderer, float>(*this, value);
        #endif
    }

    void LineRenderer::SetPositionCount(int value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(LineRenderer::typeof().GetMethod("set_positionCount"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, LineRenderer, int>(*this, value);
        #else
        s_Method.CallThunk<void, LineRenderer, int>(*this, value);
        #endif
    }

    void LineRenderer::SetPosition(int index, const Vector3& position)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(LineRenderer::typeof().GetMethod("SetPosition"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, LineRenderer, int, Vector3>(*this, index, position);
        #else
        s_Method.CallThunk<void, LineRenderer, int, const System::Boxed<Vector3>&>(*this, index, System::Boxed<Vector3>(position));
        #endif
    }

    //
    // SkinnedMeshRenderer
    //

    System::Type SkinnedMeshRenderer::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "SkinnedMeshRenderer");
        return s_Type;
    }

    //
    // AssetBundle
    //

    System::Type AssetBundle::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.AssetBundleModule").GetType("UnityEngine", "AssetBundle");
        return s_Type;
    }

    AssetBundle AssetBundle::LoadFromMemory(System::Array<char> binary)
    {
        static System::MethodInfo s_Method(AssetBundle::typeof().GetMethod("LoadFromMemory", "UnityEngine.AssetBundle(System.Byte[])"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<AssetBundle, System::Array<char>>(binary);
        #else
        return s_Method.CallThunk<AssetBundle, System::Array<char>>(binary);
        #endif
    }

    AssetBundle AssetBundle::LoadFromFile(System::String path)
    {
        static System::MethodInfo s_Method(AssetBundle::typeof().GetMethod("LoadFromFile", "UnityEngine.AssetBundle(System.String)"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<AssetBundle, System::String>(path);
        #else
        return s_Method.CallThunk<AssetBundle, System::String>(path);
        #endif
    }

    Object AssetBundle::LoadAsset(System::String name, System::Type type)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(AssetBundle::typeof().GetMethod("LoadAsset", "UnityEngine.Object(System.String,System.Type)"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<UnityEngine::Object, AssetBundle, System::String, System::Type>(*this, name, type);
        #else
        return s_Method.Invoke<UnityEngine::Object, AssetBundle, System::String, System::Type>(*this, name, type);
        #endif
    }

    //
    // Application
    //

    System::Type Application::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Application");
        return s_Type;
    }

    System::String Application::GetDataPath()
    {
        static System::MethodInfo s_Method(Application::typeof().GetMethod("get_dataPath"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<System::String>();
        #else
        return s_Method.CallThunk<System::String>();
        #endif
    }

    //
    // Material
    //

    System::Type Material::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Material");
        return s_Type;
    }

    Material Material::New(Shader shader)
    {
        static System::MethodInfo s_Method(Material::typeof().GetMethod(".ctor", "System.Void(UnityEngine.Shader)"));

        Material mat = Material(&UVM::ObjectNew(UVM::TypeGetClass(*Material::typeof().GetPtr()->Type)));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Material, Shader>(mat, shader);
        #else
        s_Method.CallThunk<void, Material, Shader>(mat, shader);
        #endif
        return mat;
    }

    void Material::SetInt(System::String name, int value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method = Material::typeof().GetMethod("SetInt", "System.Void(System.String,System.Int32)");

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Material, System::String, int>(*this, name, value);
        #else
        s_Method.CallThunk<void, Material, System::String, int>(*this, name, value);
        #endif
    }

    void Material::SetColor(Color color)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method = Material::typeof().GetMethod("set_color");

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Material, Color>(*this, color);
        #else
        System::Boxed<Color> boxed{color};
        s_Method.CallThunk<void, Material, System::Boxed<Color>*>(*this, &boxed);
        #endif
    }

    void Material::SetColor(System::String name, Color color)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method = Material::typeof().GetMethod("SetColor", "System.Void(System.String,UnityEngine.Color)");

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Material, System::String, Color>(*this, name, color);
        #else
        System::Boxed<Color> boxed{color};
        s_Method.CallThunk<void, Material, System::String, System::Boxed<Color>*>(*this, name, &boxed);
        #endif
    }

    int Material::GetPassCount()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method = Material::typeof().GetMethod("get_passCount");

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<int, Material>(*this);
        #else
        return s_Method.CallThunk<int, Material>(*this);
        #endif
    }

    //
    // Matrix4x4
    //

    System::Type Matrix4x4::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Matrix4x4");
        return s_Type;
    }

    Vector4 Matrix4x4::operator*(const UnityEngine::Vector4& v) const
    {
        UnityEngine::Vector4 result;
        result.x = m00 * v.x + m01 * v.y + m02 * v.z + m03 * v.w;
        result.y = m10 * v.x + m11 * v.y + m12 * v.z + m13 * v.w;
        result.z = m20 * v.x + m21 * v.y + m22 * v.z + m23 * v.w;
        result.w = m30 * v.x + m31 * v.y + m32 * v.z + m33 * v.w;
        return result;
    }

    UnityEngine::Vector3 UnityEngine::Matrix4x4::MultiplyPoint(UnityEngine::Vector3& point)
    {
        Vector3 result;
        result.x = m00 * point.x + m01 * point.y + m02 * point.z + m03;
        result.y = m10 * point.x + m11 * point.y + m12 * point.z + m13;
        result.z = m20 * point.x + m21 * point.y + m22 * point.z + m23;
        float num = m30 * point.x + m31 * point.y + m32 * point.z + m33;
        num = 1.f / num;
        result.x *= num;
        result.y *= num;
        result.z *= num;
        return result;
    }

    //
    // Object
    //

    System::Type Object::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Object");
        return s_Type;
    }

    Object Object::Instantiate(Object original)
    {
        static System::MethodInfo s_Method = Object::typeof().GetMethod("Instantiate", "UnityEngine.Object(UnityEngine.Object)");

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Object, Object>(original);
        #else
        return s_Method.CallThunk<Object, Object>(original);
        #endif
    }

    Object Object::Instantiate(Object original, const Vector3& position, const Quaternion& rotation)
    {
        static System::MethodInfo s_Method = Object::typeof().GetMethod("Instantiate", "UnityEngine.Object(UnityEngine.Object,UnityEngine.Vector3,UnityEngine.Quaternion)");
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Object, Object, Vector3, Quaternion>(original, position, rotation);
        #else
        return s_Method.CallThunk<Object, Object, const System::Boxed<Vector3>&, const System::Boxed<Quaternion>&>(original, System::Boxed<Vector3>(position), System::Boxed<Quaternion>(rotation));
        #endif
    }

    System::Array<Object> Object::FindObjectsOfType(System::Type type)
    {
        static System::MethodInfo s_Method = Object::typeof().GetMethod("FindObjectsOfType", "UnityEngine.Object[](System.Type)");
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<System::Array<Object>, System::Type>(type);
        #else
        return s_Method.CallThunk<System::Array<Object>, System::Type>(type);
        #endif
    }

    System::Array<Object> Object::FindObjectsOfTypeAll(System::Type type)
    {
        static System::MethodInfo s_Method = Object::typeof().GetMethod("FindObjectsOfTypeAll", "UnityEngine.Object[](System.Type)");

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<System::Array<Object>, System::Type>(type);
        #else
        return s_Method.CallThunk<System::Array<Object>, System::Type>(type);
        #endif
    }

    Object Object::FindObjectOfType(System::Type type)
    {
        static System::MethodInfo s_Method = Object::typeof().GetMethod("FindObjectOfType", "UnityEngine.Object(System.Type)");
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Object, System::Type>(type);
        #else
        return s_Method.CallThunk<Object, System::Type>(type);
        #endif
    }

    void Object::Destroy(Object obj, float t)
    {
        static System::MethodInfo s_Method = Object::typeof().GetMethod("Destroy");
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Object, float>(obj, t);
        #else
        s_Method.CallThunk<void, Object, float>(obj, t);
        #endif
    }

    System::String Object::GetName()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method = Object::typeof().GetMethod("get_name");

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<System::String, Object>(*this);
        #else
        return s_Method.CallThunk<System::String, Object>(*this);
        #endif
    }

    void Object::SetName(System::String name)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method = Object::typeof().GetMethod("set_name");
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Object, System::String>(*this, name);
        #else
        s_Method.CallThunk<void, Object, System::String>(*this, name);
        #endif
    }

    void Object::SetHideFlags(HideFlags flags)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method = Object::typeof().GetMethod("set_hideFlags");
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Object, HideFlags>(*this, flags);
        #else
        s_Method.CallThunk<void, Object, HideFlags>(*this, flags);
        #endif
    }

    int Object::GetInstanceID()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method = Object::typeof().GetMethod("GetInstanceID");

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<int, Object>(*this);
        #else
        return s_Method.CallThunk<int, Object>(*this);
        #endif
    }

    //
    // Renderer
    //

    System::Type Renderer::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Renderer");
        return s_Type;
    }

    Bounds Renderer::GetBounds()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Renderer::typeof().GetMethod("get_bounds"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Bounds, Renderer>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Bounds>*, Renderer>(*this)->m_Value;
        #endif
    }

    bool Renderer::GetEnabled()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Renderer::typeof().GetMethod("get_enabled"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<bool, Renderer>(*this);
        #else
        return s_Method.CallThunk<bool, Renderer>(*this);
        #endif
    }

    Material Renderer::GetSharedMaterial()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Renderer::typeof().GetMethod("get_sharedMaterial"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Material, Renderer>(*this);
        #else
        return s_Method.CallThunk<Material, Renderer>(*this);
        #endif
    }

    void Renderer::SetSharedMaterial(Material material)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Renderer::typeof().GetMethod("set_sharedMaterial"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Renderer, Material>(*this, material);
        #else
        s_Method.CallThunk<void, Renderer, Material>(*this, material);
        #endif
    }

    void Renderer::SetMaterial(Material material)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Renderer::typeof().GetMethod("set_material"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Renderer, Material>(*this, material);
        #else
        s_Method.CallThunk<void, Renderer, Material>(*this, material);
        #endif
    }

    void Renderer::SetEnabled(bool value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Renderer::typeof().GetMethod("set_enabled"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Renderer, bool>(*this, value);
        #else
        s_Method.CallThunk<void, Renderer, bool>(*this, value);
        #endif
    }

    void Renderer::SetSortingOrder(int value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Renderer::typeof().GetMethod("set_sortingOrder"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Renderer, int>(*this, value);
        #else
        s_Method.CallThunk<void, Renderer, int>(*this, value);
        #endif
    }

    bool Renderer::IsVisible()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Renderer::typeof().GetMethod("get_isVisible"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<bool, Renderer>(*this);
        #else
        return s_Method.CallThunk<bool, Renderer>(*this);
        #endif
    }

    //
    // SpriteRenderer
    //

    System::Type SpriteRenderer::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "SpriteRenderer");
        return s_Type;
    }

    Color SpriteRenderer::GetColor()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(SpriteRenderer::typeof().GetMethod("get_color"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Color, SpriteRenderer>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Color>*, SpriteRenderer>(*this)->m_Value;
        #endif
    }

    void SpriteRenderer::SetColor(const Color& color)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method = SpriteRenderer::typeof().GetMethod("set_color");

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, SpriteRenderer, Color>(*this, color);
        #else
        s_Method.CallThunk<void, SpriteRenderer, const System::Boxed<Color>&>(*this, System::Boxed<Color>(color));
        #endif
    }

    //
    // MeshRenderer
    //

    System::Type MeshRenderer::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "MeshRenderer");
        return s_Type;
    }

    //
    // RenderSettings
    //

    System::Type RenderSettings::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "RenderSettings");
        return s_Type;
    }

    bool RenderSettings::GetFog()
    {
        static System::MethodInfo s_Method(RenderSettings::typeof().GetMethod("get_fog"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<bool>();
        #else
        return s_Method.CallThunk<bool>();
        #endif
    }

    void RenderSettings::SetFog(bool value)
    {
        static System::MethodInfo s_Method(RenderSettings::typeof().GetMethod("set_fog"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, bool>(value);
        #else
        s_Method.CallThunk<void, bool>(value);
        #endif
    }

    //
    // Texture
    //

    System::Type Texture::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Texture");
        return s_Type;
    }

    void* Texture::GetNativeTexturePtr()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Texture::typeof().GetMethod("GetNativeTexturePtr"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<void*, Texture>(*this);
        #else
        return s_Method.CallThunk<void*, Texture>(*this);
        #endif
    }

    int Texture::GetDataWidth()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Texture::typeof().GetMethod("GetDataWidth"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<int, Texture>(*this);
        #else
        return s_Method.CallThunk<int, Texture>(*this);
        #endif
    }

    int Texture::GetDataHeight()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Texture::typeof().GetMethod("GetDataHeight"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<int, Texture>(*this);
        #else
        return s_Method.CallThunk<int, Texture>(*this);
        #endif
    }

    //
    // PlayerLoop
    //

    System::Type LowLevel::PlayerLoop::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine.LowLevel", "PlayerLoop");
        return s_Type;
    }

    System::Array<PlayerLoopSystemInternal> LowLevel::PlayerLoop::GetDefaultPlayerLoopInternal()
    {
        static System::MethodInfo s_Method(LowLevel::PlayerLoop::typeof().GetMethod("GetDefaultPlayerLoopInternal"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<System::Array<PlayerLoopSystemInternal>>();
        #else
        return s_Method.CallThunk<System::Array<PlayerLoopSystemInternal>>();
        #endif
    }

    PlayerLoopSystem LowLevel::PlayerLoop::GetDefaultPlayerLoop()
    {
        static System::MethodInfo s_Method(LowLevel::PlayerLoop::typeof().GetMethod("GetDefaultPlayerLoop"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<PlayerLoopSystem>();
        #else
        return s_Method.CallThunk<System::Boxed<PlayerLoopSystem>*>()->m_Value;
        #endif
    }

    System::Type ResourceRequest::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "ResourceRequest");
        return s_Type;
    }

    UnityEngine::Object ResourceRequest::GetAsset()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(typeof().GetMethod("get_asset"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<UnityEngine::Object, Unity::ResourceRequest>(*this);
        #else
        return s_Method.CallThunk<UnityEngine::Object, ResourceRequest>(*this);
        #endif
    }

    System::Type Resources::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Resources");
        return s_Type;
    }

    ResourceRequest Resources::LoadAsync(System::String path)
    {
        static System::MethodInfo s_Method(typeof().GetMethod("LoadAsync", "UnityEngine.ResourceRequest(System.String)"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<ResourceRequest, System::String>(path);
        #else
        return s_Method.CallThunk<ResourceRequest, System::String>(path);
        #endif
    }

    System::Type PlayerLoop::PostLateUpdate::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine.PlayerLoop", "PostLateUpdate");
        return s_Type;
    }

    System::Type PlayerLoop::PlayerSendFrameStarted::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine.PlayerLoop", "PostLateUpdate/PlayerSendFrameStarted");
        return s_Type;
    }

    System::Type PlayerLoop::PresentAfterDraw::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine.PlayerLoop", "PostLateUpdate/PresentAfterDraw");
        return s_Type;
    }

    System::Type PlayerLoop::Update::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine.PlayerLoop", "Update");
        return s_Type;
    }

    System::Type PlayerLoop::ScriptRunBehaviourUpdate::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine.PlayerLoop", "Update/ScriptRunBehaviourUpdate");
        return s_Type;
    }

    //
    // Screen
    //

    System::Type Screen::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Screen");
        return s_Type;
    }

    int Screen::GetHeight()
    {
        static System::MethodInfo s_Method(Screen::typeof().GetMethod("get_height"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<int>();
        #else
        return s_Method.CallThunk<int>();
        #endif
    }

    int Screen::GetWidth()
    {
        static System::MethodInfo s_Method(Screen::typeof().GetMethod("get_width"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<int>();
        #else
        return s_Method.CallThunk<int>();
        #endif
    }


    //
    // Sprite
    //

    System::Type Sprite::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Sprite");
        return s_Type;
    }

    System::Array<Vector2> Sprite::GetUV()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Sprite::typeof().GetMethod("get_uv"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<System::Array<Vector2>, Sprite>(*this);
        #else
        return s_Method.CallThunk<System::Array<Vector2>, Sprite>(*this);
        #endif
    }

    Vector4 Sprite::GetOuterUVs_Injected()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Sprite::typeof().GetMethod("GetOuterUVs_Injected"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        Vector4 ret{};
        s_Method.Address<void, Sprite, Vector4*>(*this, &ret);
        return ret;
        #else
        System::Boxed<Vector4> boxed{};
        s_Method.CallThunk<void, Sprite, System::Boxed<Vector4>*>(*this, &boxed);
        return boxed.m_Value;
        #endif
    }

    Rect Sprite::GetRect_Injected()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Sprite::typeof().GetMethod("get_rect_Injected"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        Rect rect{};
        s_Method.Address<void, Sprite, Rect*>(*this, &rect);
        return rect;
        #else
        System::Boxed<Rect> ret{};
        s_Method.CallThunk<void, Sprite, System::Boxed<Rect>*>(*this, &ret);
        return ret.m_Value;
        #endif
    }

    Rect Sprite::GetTextureRect_Injected()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Sprite::typeof().GetMethod("GetTextureRect_Injected"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        Rect rect{};
        s_Method.Address<void, Sprite, Rect*>(*this, &rect);
        return rect;
        #else
        System::Boxed<Rect> ret{};
        s_Method.CallThunk<void, Sprite, System::Boxed<Rect>*>(*this, &ret);
        return ret.m_Value;
        #endif
    }

    Rect Sprite::GetTextureRect()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Sprite::typeof().GetMethod("GetTextureRect"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Rect, Sprite>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Rect>*, Sprite>(*this)->m_Value;
        #endif
    }

    Texture2D Sprite::GetTexture()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Sprite::typeof().GetMethod("get_texture"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Texture2D, Sprite>(*this);
        #else
        return s_Method.CallThunk<Texture2D, Sprite>(*this);
        #endif
    }

    //
    // Shader
    //

    System::Type Shader::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Shader");
        return s_Type;
    }

    Shader Shader::Find(System::String name)
    {
        static System::MethodInfo s_Method(Shader::typeof().GetMethod("Find"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Shader, System::String>(name);
        #else
        return s_Method.CallThunk<Shader, System::String>(name);
        #endif
    }

    void Shader::WarmupAllShaders()
    {
        static System::MethodInfo s_Method(Shader::typeof().GetMethod("WarmupAllShaders"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<void>();
        #else
        return s_Method.CallThunk<void>();
        #endif
    }

    bool Shader::IsSupported()
    {
        static System::MethodInfo s_Method(Shader::typeof().GetMethod("get_isSupported"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<bool, Shader>(*this);
        #else
        return s_Method.CallThunk<bool, Shader>(*this);
        #endif
    }

    //
    // Transform
    //

    System::Type Transform::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Transform");
        return s_Type;
    }

    Vector3 Transform::GetPosition()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Transform::typeof().GetMethod("get_position"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Vector3, Transform>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Vector3>*, Transform>(*this)->m_Value;
        #endif
    }

    void Transform::SetPosition(const UnityEngine::Vector3& value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Transform::typeof().GetMethod("set_position"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<void, Transform, Vector3>(*this, value);
        #else
        s_Method.CallThunk<void, Transform, const System::Boxed<Vector3>&>(*this, System::Boxed<Vector3>(value));
        #endif
    }

    Vector3 Transform::GetLocalPosition()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Transform::typeof().GetMethod("get_localPosition"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Vector3, Transform>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Vector3>*, Transform>(*this)->m_Value;
        #endif
    }

    void Transform::SetLocalPosition(const UnityEngine::Vector3& value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Transform::typeof().GetMethod("set_localPosition"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Transform, Vector3>(*this, value);
        #else
        s_Method.CallThunk<void, Transform, const System::Boxed<Vector3>&>(*this, System::Boxed<Vector3>(value));
        #endif
    }

    Quaternion Transform::GetRotation()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Transform::typeof().GetMethod("get_rotation"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Quaternion, Transform>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Quaternion>*, Transform>(*this)->m_Value;
        #endif
    }

    void Transform::SetRotation(const UnityEngine::Quaternion& value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Transform::typeof().GetMethod("set_rotation"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Transform, Quaternion>(*this, value);
        #else
        s_Method.CallThunk<void, Transform, const System::Boxed<Quaternion>&>(*this, System::Boxed<Quaternion>(value));
        #endif
    }

    Transform Transform::GetParent()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Transform::typeof().GetMethod("get_parent"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Transform, Transform>(*this);
        #else
        return s_Method.CallThunk<Transform, Transform>(*this);
        #endif
    }

    void Transform::SetParent(UnityEngine::Transform value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Transform::typeof().GetMethod("set_parent"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Transform, Transform>(*this, value);
        #else
        s_Method.CallThunk<void, Transform, Transform>(*this, value);
        #endif
    }

    Vector3 Transform::GetForward()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Transform::typeof().GetMethod("get_forward"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Vector3, Transform>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Vector3>*, Transform>(*this)->m_Value;
        #endif
    }

    Vector3 Transform::GetUp()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Transform::typeof().GetMethod("get_up"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Vector3, Transform>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Vector3>*, Transform>(*this)->m_Value;
        #endif
    }

    Vector3 Transform::GetLocalScale()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Transform::typeof().GetMethod("get_localScale"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Vector3, Transform>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Vector3>*, Transform>(*this)->m_Value;
        #endif
    }

    void Transform::SetLocalScale(const Vector3& value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Transform::typeof().GetMethod("set_localScale"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Transform, Vector3>(*this, value);
        #else
        s_Method.CallThunk<void, Transform, const System::Boxed<Vector3>&>(*this, System::Boxed<Vector3>(value));
        #endif
    }

    Transform Transform::Find(System::String n)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Transform::typeof().GetMethod("Find", "UnityEngine.Transform(System.String)"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Transform, Transform, System::String>(*this, n);
        #else
        return s_Method.CallThunk<Transform, Transform, System::String>(*this, n);
        #endif
    }

    Transform Transform::GetChild(int index)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Transform::typeof().GetMethod("GetChild"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Transform, Transform, int>(*this, index);
        #else
        return s_Method.CallThunk<Transform, Transform, int>(*this, index);
        #endif
    }

    //
    // Vector3
    //

    System::Type Vector3::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Vector3");
        return s_Type;
    }

    float Vector3::Distance(const UnityEngine::Vector3& a, const UnityEngine::Vector3& b)
    {
        Vector3 vector(a.x - b.x, a.y - b.y, a.z - b.z);
        return std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
    }

    float Vector3::Distance(const UnityEngine::Vector3& other)
    {
        Vector3 vector(x - other.x, y - other.y, z - other.z);
        return std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
    }

    Vector3 Vector3::Min(const Vector3& a, const Vector3& b)
    {
        return Vector3((Hax::Min)(a.x, b.x), (Hax::Min)(a.y, b.y), (Hax::Min)(a.z, b.z));
    }

    Vector3 Vector3::Max(const Vector3& a, const Vector3& b)
    {
        return Vector3(Hax::Max(a.x, b.x), Hax::Max(a.y, b.y), Hax::Max(a.z, b.z));
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
        return Vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
    }

    float Vector3::Dot(const Vector3& v1, const Vector3& v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    System::Type Vector4::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Vector4");
        return s_Type;
    }

    //
    // NavMeshHit
    //

    System::Type NavMeshHit::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.AIModule").GetType("UnityEngine.AI", "NavMeshHit");
        return s_Type;
    }

    //
    // NavMesh
    //

    System::Type NavMesh::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.AIModule").GetType("UnityEngine.AI", "NavMesh");
        return s_Type;
    }

    bool NavMesh::SamplePosition(const Vector3& pos, NavMeshHit* hit, float maxDist, int areaMask)
    {
        static System::MethodInfo s_Method(NavMesh::typeof().GetMethod("SamplePosition", "System.Boolean(UnityEngine.Vector3,UnityEngine.AI.NavMeshHit&,System.Single,System.Int32)"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<bool, Vector3, NavMeshHit*, float, int>(pos, hit, maxDist, areaMask);
        #else
        System::Boxed<NavMeshHit> boxedHit{};
        bool flag = s_Method.CallThunk<bool, const System::Boxed<Vector3>&, const System::Boxed<NavMeshHit>&, float, int>(System::Boxed<Vector3>(pos), boxedHit, maxDist, areaMask);
        *hit = boxedHit.m_Value;
        return flag;
        #endif
    }

    //
    // Quaternion
    //

    System::Type Quaternion::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Quaternion");
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

        return Quaternion(
            sx * cy * cz + cx * sy * sz,
            cx * sy * cz - sx * cy * sz,
            cx * cy * sz - sx * sy * cz,
            cx * cy * cz + sx * sy * sz);
    }

    Vector3 Quaternion::operator*(const Vector3& point) const
    {
        float num = x * 2.f;
        float num2 = y * 2.f;
        float num3 = z * 2.f;
        float num4 = x * num;
        float num5 = y * num2;
        float num6 = z * num3;
        float num7 = x * num2;
        float num8 = x * num3;
        float num9 = y * num3;
        float num10 = w * num;
        float num11 = w * num2;
        float num12 = w * num3;
        Vector3 result;
        result.x = (1.f - (num5 + num6)) * point.x + (num7 - num12) * point.y + (num8 + num11) * point.z;
        result.y = (num7 + num12) * point.x + (1.f - (num4 + num6)) * point.y + (num9 - num10) * point.z;
        result.z = (num8 - num11) * point.x + (num9 + num10) * point.y + (1.f - (num4 + num5)) * point.z;
        return result;
    }

    //
    // Color
    //

    System::Type Color::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Color");
        return s_Type;
    }

    //
    // Rect
    //

    System::Type Rect::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.CoreModule").GetType("UnityEngine", "Rect");
        return s_Type;
    }
}

namespace UnityEngine
{
    System::Type Collider2D::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.Physics2DModule").GetType("UnityEngine", "Collider2D");
        return s_Type;
    }

    Bounds Collider2D::GetBounds()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Collider2D::typeof().GetMethod("get_bounds"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Bounds, Collider2D>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Bounds>*, Collider2D>(*this)->m_Value;
        #endif
    }

    void Collider2D::SetIsTrigger(bool value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Collider2D::typeof().GetMethod("set_isTrigger"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Collider2D, bool>(*this, value);
        #else
        s_Method.CallThunk<void, Collider2D, bool>(*this, value);
        #endif
    }

    //
    // BoxCollider2D
    //

    System::Type BoxCollider2D::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.Physics2DModule").GetType("UnityEngine", "Collider2D");
        return s_Type;
    }

    //
    // Collider
    //

    System::Type Collider::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.PhysicsModule").GetType("UnityEngine", "Collider");
        return s_Type;
    }

    bool Collider::GetEnabled()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Collider::typeof().GetMethod("get_enabled"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<bool, Collider>(*this);
        #else
        return s_Method.CallThunk<bool, Collider>(*this);
        #endif
    }

    void Collider::SetEnabled(bool value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Collider::typeof().GetMethod("set_enabled"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Collider, bool>(*this, value);
        #else
        s_Method.CallThunk<void, Collider, bool>(*this, value);
        #endif
    }

    Bounds Collider::GetBounds()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Collider::typeof().GetMethod("get_bounds"));
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Bounds, Collider>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Bounds>*, Collider>(*this)->m_Value;
        #endif
    }

    //
    // BoxCollider
    //

    System::Type BoxCollider::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.Physics2DModule").GetType("UnityEngine", "BoxCollider");
        return s_Type;
    }

    Vector3 BoxCollider::GetCenter()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(BoxCollider::typeof().GetMethod("get_center"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Vector3, BoxCollider>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Vector3>*, BoxCollider>(*this)->m_Value;
        #endif
    }

    Vector3 BoxCollider::GetSize()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(BoxCollider::typeof().GetMethod("get_size"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Vector3, BoxCollider>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Vector3>*, BoxCollider>(*this)->m_Value;
        #endif
    }

    //
    // PolygonCollider2D
    //

    System::Type PolygonCollider2D::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.Physics2DModule").GetType("UnityEngine", "PolygonCollider2D");
        return s_Type;
    }

    System::Array<Vector2> PolygonCollider2D::GetPoints()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(PolygonCollider2D::typeof().GetMethod("get_points"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<System::Array<Vector2>, PolygonCollider2D>(*this);
        #else
        return s_Method.CallThunk<System::Array<Vector2>, PolygonCollider2D>(*this);
        #endif
    }

    void PolygonCollider2D::SetPoints(System::Array<Vector2> value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(PolygonCollider2D::typeof().GetMethod("set_points"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, PolygonCollider2D, System::Array<Vector2>>(*this, value);
        #else
        s_Method.CallThunk<void, PolygonCollider2D, System::Array<Vector2>>(*this, value);
        #endif
    }

    //
    // Rigidbody
    //

    System::Type Rigidbody::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.PhysicsModule").GetType("UnityEngine", "Rigidbody");
        return s_Type;
    }

    void Rigidbody::AddForce(const Vector3& force)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Rigidbody::typeof().GetMethod("AddForce", "System.Void(UnityEngine.Vector3)"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Rigidbody, Vector3>(*this, force);
        #else
        s_Method.CallThunk<void, Rigidbody, const System::Boxed<Vector3>&>(*this, System::Boxed<Vector3>(force));
        #endif
    }

    Vector3 Rigidbody::GetVelocity()
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Rigidbody::typeof().GetMethod("get_velocity"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Vector3, Rigidbody>(*this);
        #else
        return s_Method.CallThunk<System::Boxed<Vector3>*, Rigidbody>(*this)->m_Value;
        #endif
    }

    void Rigidbody::SetVelocity(const Vector3& value)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Rigidbody::typeof().GetMethod("set_velocity"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        s_Method.Address<void, Rigidbody, Vector3>(*this, value);
        #else
        s_Method.CallThunk<void, Rigidbody, const System::Boxed<Vector3>&>(*this, System::Boxed<Vector3>(value));
        #endif
    }

    //
    // Rigidbody2D
    //

    System::Type Rigidbody2D::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.Physics2DModule").GetType("UnityEngine", "Rigidbody2D");
        return s_Type;
    }

    int Rigidbody2D::GetAttachedCollidersList_Internal(System::List<Collider2D> results, bool findTriggers)
    {
        THROW_IF_NULL();

        static System::MethodInfo s_Method(Rigidbody2D::typeof().GetMethod("GetAttachedCollidersList_Internal"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<int, Rigidbody2D, System::List<Collider2D>, bool>(*this, results, findTriggers);
        #else
        return s_Method.CallThunk<int, Rigidbody2D, System::List<Collider2D>, bool>(*this, results, findTriggers);
        #endif
    }

    //
    // RaycastHit
    //

    System::Type RaycastHit::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.PhysicsModule").GetType("UnityEngine", "RaycastHit");
        return s_Type;
    }

    Transform RaycastHit::GetTransform()
    {
        static System::MethodInfo s_Method(RaycastHit::typeof().GetMethod("get_transform"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Transform, RaycastHit>(*this);
        #else
        return s_Method.CallThunk<Transform, const System::Boxed<RaycastHit>&>(System::Boxed<RaycastHit>(*this));
        #endif
    }

    //
    // Physics
    //

    System::Type Physics::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.PhysicsModule").GetType("UnityEngine", "Physics");
        return s_Type;
    }

    System::Array<RaycastHit> Physics::SphereCastAll(const Vector3& origin, float radius, const Vector3& direction, float maxDistance, int layerMask)
    {
        static System::MethodInfo s_Method(Physics::typeof().GetMethod("SphereCastAll", "UnityEngine.RaycastHit[](UnityEngine.Vector3,System.Single,UnityEngine.Vector3,System.Single,System.Int32)"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<System::Array<RaycastHit>, Vector3, float, Vector3, float, int>(origin, radius, direction, maxDistance, layerMask);
        #else
        return s_Method.CallThunk<System::Array<RaycastHit>, const System::Boxed<Vector3>&, float, const System::Boxed<Vector3>&, float, int>(System::Boxed<Vector3>(origin), radius, System::Boxed<Vector3>(direction), maxDistance, layerMask);
        #endif
    }
}

namespace UnityEngine
{
    //
    // EventSystem
    //

    System::Type EventSystem::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("UnityEngine.UI").GetType("UnityEngine.EventSystems", "EventSystem");
        return s_Type;
    }

    EventSystem EventSystem::GetCurrent()
    {
        static System::MethodInfo s_Method(EventSystem::typeof().GetMethod("get_current"));

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<EventSystem>();
        #else
        return s_Method.CallThunk<EventSystem>();
        #endif
    }

}

namespace UnityEngine
{
    System::Type PostProcessLayer::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("Unity.Postprocessing.Runtime").GetType("UnityEngine.Rendering.PostProcessing", "PostProcessLayer");
        return s_Type;
    }

    CommandBuffer PostProcessLayer::m_LegacyCmdBufferBeforeReflections()
    {
        static System::FieldInfo field{PostProcessLayer::typeof().GetField("m_LegacyCmdBufferBeforeReflections")};
        return *(CommandBuffer*)((char*)GetPtr() + field.GetOffset());
    }

    CommandBuffer PostProcessLayer::m_LegacyCmdBufferBeforeLighting()
    {
        static System::FieldInfo field{PostProcessLayer::typeof().GetField("m_LegacyCmdBufferBeforeLighting")};
        return *(CommandBuffer*)((char*)GetPtr() + field.GetOffset());
    }

    CommandBuffer PostProcessLayer::m_LegacyCmdBufferOpaque()
    {
        static System::FieldInfo field{PostProcessLayer::typeof().GetField("m_LegacyCmdBufferOpaque")};
        return *(CommandBuffer*)((char*)GetPtr() + field.GetOffset());
    }

    CommandBuffer PostProcessLayer::m_LegacyCmdBuffer()
    {
        static System::FieldInfo field{PostProcessLayer::typeof().GetField("m_LegacyCmdBuffer")};
        return *(CommandBuffer*)((char*)GetPtr() + field.GetOffset());
    }
}

namespace Photon
{
    System::Type PhotonView::typeof()
    {
        static System::Type s_Type = System::AppDomain::GetCurrent().Load("PhotonUnityNetworking").GetType("Photon.Pun", "PhotonView");
        return s_Type;
    }

    Player PhotonView::GetOwner()
    {
        THROW_IF_NULL();
        static System::MethodInfo s_Method = PhotonView::typeof().GetMethod("get_Owner");

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Player, PhotonView>(*this);
        #else
        return s_Method.CallThunk<Player, PhotonView>(*this);
        #endif
    }

    void PhotonView::RPC(System::String methodName, RpcTarget target, System::Array<System::Object> parameters)
    {
        THROW_IF_NULL();
        static System::MethodInfo s_Method = PhotonView::typeof().GetMethod("RPC", "System.Void(System.String,Photon.Pun.RpcTarget,System.Object[])");

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<void, PhotonView, System::String, int, System::Array<System::Object>>(*this, methodName, target, parameters);
        #else
        return s_Method.CallThunk<void, PhotonView, System::String, int, System::Array<System::Object>>(*this, methodName, target, parameters);
        #endif
    }

    bool& PhotonView::IsMine()
    {
        THROW_IF_NULL();
        static System::FieldInfo field = PhotonView::typeof().GetField("<IsMine>k__BackingField");
        return *(bool*)((char*)GetPtr() + field.GetOffset());
    }

    UnityEngine::GameObject PhotonNetwork::InstantiateRoomObject(System::String name, const UnityEngine::Vector3& position, const UnityEngine::Quaternion& rotation, char8_t group, void* data)
    {
        static System::MethodInfo s_Method = System::AppDomain::GetCurrent().Load("PhotonUnityNetworking").GetType("Photon.Pun", "PhotonNetwork").GetMethod("InstantiateRoomObject");
        
        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<UnityEngine::GameObject, 
                                System::String, 
                                Unity::Vector3, 
                                Unity::Quaternion, 
                                char8_t, 
                                void*>(name, position, rotation, group, data); 
        #else
        return s_Method.CallThunk<UnityEngine::GameObject, 
                              System::String, 
                              const System::Boxed<UnityEngine::Vector3>&,
                              const System::Boxed<UnityEngine::Quaternion>&,
                              char8_t, 
                              void*>(name, System::Boxed<UnityEngine::Vector3>(position), System::Boxed<UnityEngine::Quaternion>(rotation), group, data);
        #endif
    }

    Player PhotonNetwork::GetMasterClient()
    {
        static System::MethodInfo s_Method = System::AppDomain::GetCurrent().Load("PhotonUnityNetworking").GetType("Photon.Pun", "PhotonNetwork").GetMethod("get_MasterClient");

        #ifdef HAX_UNITY_BACKEND_IL2CPP
        return s_Method.Address<Player>();
        #else
        return s_Method.CallThunk<Player>();
        #endif
    }
}
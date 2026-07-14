#define HAX_UNITY_INCLUDE_UVM
#include "hax_unity.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include "modules/hax_unity_mscorlib.h"

static bool GetInternalCallsReturnHandles()
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        UvmMethod* method = uvm_class_get_method_from_name(uvm_appdomain_get_class(), "getRootDomain", {});
        HAX_ASSERT(method != nullptr);

        using Func = void*(*)(MonoError*);
        Func icall = (Func)uvm_lookup_internal_call(method);
        HAX_ASSERT(icall != nullptr);

        MonoError err{};
        UvmAppDomain* variant1 = (UvmAppDomain*)icall(&err);
        UvmAppDomain** variant2 = (UvmAppDomain**)variant1;

        UvmDomain* domain = uvm_get_root_domain();
        if (variant1->UvmDomain == domain)
            return false;

        HAX_ASSERT((*variant2)->UvmDomain == domain);
        return true;
    }
    else
    {
        return false;
    }
}

namespace System
{
    extern void HandleMetadataRequests(); // hax_unity_mscorlib.cpp
}

namespace Hax::Unity
{
    bool g_ICallsReturnHandle;
    Hax::LogFile* g_Logger;

    void Initialize(LogFile* logFile)
    {
        HAX_ASSERT(GetUvmHandle() != 0 && "Not a unity game");
        g_Logger = logFile;

        if (logFile != nullptr)
            Hax::LogDebug(*logFile, L"Initializing predefined data...");

        UvmThread* thread = uvm_thread_attach(uvm_get_root_domain());
        g_ICallsReturnHandle = GetInternalCallsReturnHandles();
        System::HandleMetadataRequests();
        uvm_thread_detach(thread);
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

    Handle GetUvmHandle()
    {
        static Handle s_UvmHandle;
        if (s_UvmHandle != 0)
            return s_UvmHandle;

        MODULEENTRY32W me = {.dwSize = sizeof(MODULEENTRY32W)};
        HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
        if (::Module32FirstW(hSnapshot, &me))
        {
            do
            {
                if (me.hModule == 0)
                    continue;

                if (::GetProcAddress(me.hModule, "il2cpp_init") != nullptr)
                {
                    if constexpr (!HAX_UNITY_IS_MONO)
                    {
                        s_UvmHandle = (Handle)me.hModule;
                        break;
                    }
                    else
                        HAX_ASSERT(false); //! TODO: add msg
                }
                if (::GetProcAddress(me.hModule, "mono_init") != nullptr)
                {
                    if constexpr (HAX_UNITY_IS_MONO)
                    {
                        s_UvmHandle = (Handle)me.hModule;
                        break;
                    }
                    else
                        HAX_ASSERT(false); //! TODO: add msg
                }
            } while (::Module32NextW(hSnapshot, &me));
        }
        ::CloseHandle(hSnapshot);

        return s_UvmHandle;
    }

    Handle AttachThread()
    {
        return (Handle)uvm_thread_attach(uvm_get_root_domain());
    }

    void DetachThread(Handle hThread)
    {
        uvm_thread_detach((UvmThread*)hThread);
    }
}

#pragma region UVM

namespace Hax::Unity
{
    extern bool g_ICallsReturnHandle;
}

static FARPROC GetUvmProc(const char* procName)
{
    FARPROC ptr = ::GetProcAddress((HMODULE)Hax::Unity::GetUvmHandle(), procName);
    HAX_ASSERT(ptr != nullptr);
    return ptr;
}

void uvm_assembly_foreach(void(*func)(void*,void*), void* data)
{ 
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static void(*s_ApiProc)(decltype(func), void*) = (decltype(s_ApiProc))GetUvmProc("mono_assembly_foreach");
        return s_ApiProc(func, data);
    }
    else
    {
        HAX_ASSERT(false);
    }
}

const char* uvm_domain_get_friendly_name(UvmDomain* domain)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static const char*(*s_ApiProc)(UvmDomain*) = (decltype(s_ApiProc))GetUvmProc("mono_domain_get_friendly_name");
        return s_ApiProc(domain);
    }
    else
    {
        HAX_ASSERT(false);
        return nullptr;
    }
}

const char* uvm_image_get_name(UvmImage* image)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static const char* (*s_ApiProc)(UvmImage*) = (decltype(s_ApiProc))GetUvmProc("mono_image_get_name");
        return s_ApiProc(image);
    }
    else
    {
        static const char*(*s_ApiProc)(UvmImage*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_image_get_name");
        return s_ApiProc(image);
    }
}

UvmImage* uvm_assembly_get_image(UvmAssembly* assembly)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmImage* (*s_ApiProc)(UvmAssembly*) = (decltype(s_ApiProc))GetUvmProc("mono_assembly_get_image");
        return s_ApiProc(assembly);
    }
    else
    {
        static UvmImage*(*s_ApiProc)(UvmAssembly*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_assembly_get_image");
        return s_ApiProc(assembly);
    }
}

UvmAssembly** uvm_domain_get_assemblies(UvmDomain* domain, size_t* size)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        HAX_ASSERT(false);
        return nullptr;
    }
    else
    {
        static UvmAssembly** (*s_ApiProc)(UvmDomain*, size_t*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_domain_get_assemblies");
        return s_ApiProc(domain, size);
    }
}

const char* uvm_assembly_get_name(UvmAssembly* uvmAssembly)
{
    return uvm_image_get_name(uvm_assembly_get_image(uvmAssembly));
}

UvmImage* uvm_get_corlib()
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmImage* (*s_ApiProc)() = (decltype(s_ApiProc))GetUvmProc("mono_get_corlib");
        return s_ApiProc();
    }
    else
    {
        static UvmImage*(*s_ApiProc)() = (decltype(s_ApiProc))GetUvmProc("il2cpp_get_corlib");
        return s_ApiProc();
    }
}

UvmDomain* uvm_get_root_domain()
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmDomain* s_Domain = ((UvmDomain*(*)())GetUvmProc("mono_get_root_domain"))();
        return s_Domain;
    }
    else
    {
        static UvmDomain*(*s_ApiProc)() = (decltype(s_ApiProc))GetUvmProc("il2cpp_domain_get");
        return s_ApiProc();
    }
}

UvmDomain* uvm_domain_get()
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmDomain* s_Domain = ((UvmDomain*(*)())GetUvmProc("mono_domain_get"))();
        return s_Domain;
    }
    else
    {
        static UvmDomain*(*s_ApiProc)() = (decltype(s_ApiProc))GetUvmProc("il2cpp_domain_get");
        return s_ApiProc();
    }
}

UvmThread* uvm_thread_attach(UvmDomain* domain)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmThread*(*s_ApiProc)(UvmDomain*) = (decltype(s_ApiProc))GetUvmProc("mono_thread_attach");
        return s_ApiProc(domain);
    }
    else
    {
        static UvmThread*(*s_ApiProc)(UvmDomain*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_thread_attach");
        return s_ApiProc(domain);
    }
}

void uvm_thread_detach(UvmThread* thread)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static void(*s_ApiProc)(UvmThread*) = (decltype(s_ApiProc))GetUvmProc("mono_thread_detach");
        return s_ApiProc(thread);
    }
    else
    {
        static void(*s_ApiProc)(UvmThread*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_thread_detach");
        return s_ApiProc(thread);
    }
}

const char* uvm_method_get_name(UvmMethod* method)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static const char*(*s_ApiProc)(UvmMethod*) = (decltype(s_ApiProc))GetUvmProc("mono_method_get_name");
        return s_ApiProc(method);
    }
    else
    {
        static const char*(*s_ApiProc)(UvmMethod*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_method_get_name");
        return s_ApiProc(method);
    }
}

UvmClass* uvm_class_from_name(UvmImage* image, const char* namespaze, const char* name)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmClass*(*s_ApiProc)(UvmImage*, const char*, const char*) = (decltype(s_ApiProc))GetUvmProc("mono_class_from_name");
        return s_ApiProc(image, namespaze, name);
    }
    else
    {
        static UvmClass*(*s_ApiProc)(UvmImage*, const char*, const char*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_class_from_name");
        return s_ApiProc(image, namespaze, name);
    }
}

UvmType* uvm_class_get_type(UvmClass* klass)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmType*(*s_ApiProc)(UvmClass*) = (decltype(s_ApiProc))GetUvmProc("mono_class_get_type");
        return s_ApiProc(klass);
    }
    else
    {
        static UvmType*(*s_ApiProc)(UvmClass*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_class_get_type");
        return s_ApiProc(klass);
    }
}

UvmClass* uvm_object_get_class(UvmObject* obj)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmClass*(*s_ApiProc)(UvmObject*) = (decltype(s_ApiProc))GetUvmProc("mono_object_get_class");
        return s_ApiProc(obj);
    }
    else
    {
        static UvmClass*(*s_ApiProc)(UvmObject*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_object_get_class");
        return s_ApiProc(obj);
    }
}

UvmReflectionType* uvm_type_get_object(UvmType* type)
{
    static Hax::HashMap<UvmType*, UvmReflectionType*> s_Cache;

    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmReflectionType*(*s_ApiProc)(UvmDomain*,UvmType*) = (decltype(s_ApiProc))GetUvmProc("mono_type_get_object");

        UvmReflectionType*& ref = s_Cache.FindOrAdd(type);
        if (ref == nullptr)
            ref = s_ApiProc(uvm_domain_get(), type);
        return ref;
    }
    else
    {
        static UvmReflectionType*(*s_ApiProc)(UvmType*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_type_get_object");

        UvmReflectionType*& ref = s_Cache.FindOrAdd(type);
        if (ref == nullptr)
            ref = s_ApiProc(type);
        return ref;
    }

}

UvmClass* uvm_type_get_class(UvmType* type)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmClass*(*s_ApiProc)(UvmType*) = (decltype(s_ApiProc))GetUvmProc("mono_class_from_mono_type");
        return s_ApiProc(type);
    }
    else
    {
        static UvmClass*(*s_ApiProc)(UvmType*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_type_get_class_or_element_class");
        return s_ApiProc(type);
    }
}

UvmClass* uvm_class_from_system_type(UvmReflectionType* type)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmType*(*s_ApiProc)(UvmReflectionType*) = (decltype(s_ApiProc))GetUvmProc("mono_reflection_type_get_type");
        return uvm_type_get_class(s_ApiProc(type));
    }
    else
    {
        static UvmClass*(*s_ApiProc)(UvmReflectionType*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_class_from_system_type");
        return s_ApiProc(type);
    }
}

UvmImage* uvm_class_get_image(UvmClass* klass)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmImage*(*s_ApiProc)(UvmClass*) = (decltype(s_ApiProc))GetUvmProc("mono_class_get_image");
        return s_ApiProc(klass);
    }
    else
    {
        static UvmImage*(*s_ApiProc)(UvmClass*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_class_get_image");
        return s_ApiProc(klass);
    }
}

char* uvm_type_get_name(UvmType* type)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static char*(*s_ApiProc)(UvmType*) = (decltype(s_ApiProc))GetUvmProc("mono_type_get_name");
        return s_ApiProc(type);
    }
    else
    {
        static char*(*s_ApiProc)(UvmType*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_type_get_name");
        return s_ApiProc(type);
    }
}

const char* uvm_class_get_name(UvmClass* klass)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static const char*(*s_ApiProc)(UvmClass*) = (decltype(s_ApiProc))GetUvmProc("mono_class_get_name");
        return s_ApiProc(klass);
    }
    else
    {
        static const char*(*s_ApiProc)(UvmClass*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_class_get_name");
        return s_ApiProc(klass);
    }
}

const char* uvm_class_get_namespace(UvmClass* klass)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static const char*(*s_ApiProc)(UvmClass*) = (decltype(s_ApiProc))GetUvmProc("mono_class_get_namespace");
        return s_ApiProc(klass);
    }
    else
    {
        static const char*(*s_ApiProc)(UvmClass*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_class_get_namespace");
        return s_ApiProc(klass);
    }
}

UvmField* uvm_class_get_field_from_name(UvmClass* klass, const char* name)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmField*(*s_ApiProc)(UvmClass*,const char*) = (decltype(s_ApiProc))GetUvmProc("mono_class_get_field_from_name");
        return s_ApiProc(klass, name);
    }
    else
    {
        static UvmField*(*s_ApiProc)(UvmClass*,const char*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_class_get_field_from_name");
        return s_ApiProc(klass, name);
    }
}

uint32_t uvm_field_get_flags(UvmField* field)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static uint32_t(*s_ApiProc)(UvmField*) = (decltype(s_ApiProc))GetUvmProc("mono_field_get_flags");
        return s_ApiProc(field);
    }
    else
    {
        static uint32_t(*s_ApiProc)(UvmField*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_field_get_flags");
        return s_ApiProc(field);
    }
}

uint32_t uvm_field_get_offset(UvmField* field)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static uint32_t(*s_ApiProc)(UvmField*) = (decltype(s_ApiProc))GetUvmProc("mono_field_get_offset");
        return s_ApiProc(field);
    }
    else
    {
        static size_t(*s_ApiProc)(UvmField*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_field_get_offset");
        return (uint32_t)s_ApiProc(field);
    }
}

UvmVTable* uvm_class_vtable(UvmDomain* domain, UvmClass* klass)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmVTable*(*s_ApiProc)(UvmDomain*, UvmClass*) = (decltype(s_ApiProc))GetUvmProc("mono_class_vtable");
        return s_ApiProc(domain, klass);
    }
    else
    {
        HAX_ASSERT(false);
    }
}

void* uvm_vtable_get_static_field_data(UvmVTable* vt)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static void*(*s_ApiProc)(UvmVTable*) = (decltype(s_ApiProc))GetUvmProc("mono_vtable_get_static_field_data");
        return s_ApiProc(vt);
    }
    else
    {
        HAX_ASSERT(false);
    }
}

void* uvm_class_get_static_field_data(UvmClass* klass)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        UvmVTable* vtable = uvm_class_vtable(uvm_domain_get(), klass);
        return uvm_vtable_get_static_field_data(vtable);
    }
    else
    {
        static void*(*s_ApiProc)(UvmClass*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_class_get_static_field_data");
        return s_ApiProc(klass);
    }

}

const char* uvm_field_get_name(UvmField* field)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static const char*(*s_ApiProc)(UvmField*) = (decltype(s_ApiProc))GetUvmProc("mono_field_get_name");
        return s_ApiProc(field);
    }
    else
    {
        static const char*(*s_ApiProc)(UvmField*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_field_get_name");
        return s_ApiProc(field);
    }

}

UvmClass* uvm_field_get_parent(UvmField* field)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmClass*(*s_ApiProc)(UvmField*) = (decltype(s_ApiProc))GetUvmProc("mono_field_get_parent");
        return s_ApiProc(field);
    }
    else
    {
        static UvmClass*(*s_ApiProc)(UvmField*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_field_get_parent");
        return s_ApiProc(field);
    }

}

void uvm_field_static_get_value(UvmField* field, void* value)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static void(*s_ApiProc)(UvmVTable*, UvmField*, void*) = (decltype(s_ApiProc))GetUvmProc("mono_field_static_get_value");

        UvmVTable* vtable = uvm_class_vtable(uvm_domain_get(), uvm_field_get_parent(field));
        s_ApiProc(vtable, field, value);
    }
    else
    {
        static void(*s_ApiProc)(UvmField*, void*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_field_static_get_value");
        if (s_ApiProc) s_ApiProc(field, value);
    }

}

UvmType* uvm_field_get_type(UvmField* field)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmType*(*s_ApiProc)(UvmField*) = (decltype(s_ApiProc))GetUvmProc("mono_field_get_type");
        return s_ApiProc(field);
    }
    else
    {
        static UvmType*(*s_ApiProc)(UvmField*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_field_get_type");
        return s_ApiProc(field);
    }

}

bool uvm_class_is_assignable_from(UvmClass* klass, UvmClass* oklass)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static int(*s_ApiProc)(UvmClass*, UvmClass*) = (decltype(s_ApiProc))GetUvmProc("mono_class_is_assignable_from");
        return s_ApiProc(klass, oklass);
    }
    else
    {
        static bool(*s_ApiProc)(UvmClass*, UvmClass*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_class_is_assignable_from");
        return s_ApiProc(klass, oklass);
    }

}

UvmMethod* uvm_class_get_methods(UvmClass* klass, void** iter)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmMethod*(*s_ApiProc)(UvmClass*, void**) = (decltype(s_ApiProc))GetUvmProc("mono_class_get_methods");
        return s_ApiProc(klass, iter);
    }
    else
    {
        static UvmMethod*(*s_ApiProc)(UvmClass*, void**) = (decltype(s_ApiProc))GetUvmProc("il2cpp_class_get_methods");
        return s_ApiProc(klass, iter);
    }

}

UvmMethodSignature* uvm_method_signature(UvmMethod* method)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmMethodSignature*(*s_ApiProc)(UvmMethod*) = (decltype(s_ApiProc))GetUvmProc("mono_method_signature");
        return s_ApiProc(method);
    }
    else
    {
        HAX_ASSERT(FALSE);
    }

}

UvmType* uvm_signature_get_return_type(UvmMethodSignature* sig)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmType*(*s_ApiProc)(UvmMethodSignature*) = (decltype(s_ApiProc))GetUvmProc("mono_signature_get_return_type");
        return s_ApiProc(sig);
    }
    else
    {
        HAX_ASSERT(FALSE);
    }

}

UvmType* uvm_method_get_return_type(UvmMethod* method)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        HAX_ASSERT(FALSE);
        return nullptr;
    }
    else
    {
        static UvmType*(*s_ApiProc)(UvmMethod*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_method_get_return_type");
        return s_ApiProc(method);
    }
}

uint32_t uvm_signature_get_param_count(UvmMethodSignature* sig)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static uint32_t(*s_ApiProc)(UvmMethodSignature*) = (decltype(s_ApiProc))GetUvmProc("mono_signature_get_param_count");
        return s_ApiProc(sig);
    }
    else
    {
        HAX_ASSERT(FALSE);
    }
}

uint32_t uvm_method_get_param_count(UvmMethod* method)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        HAX_ASSERT(FALSE);
        return 0;
    }
    else
    {
        static uint32_t(*s_ApiProc)(UvmMethod*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_method_get_param_count");
        return s_ApiProc(method);
    }
}

UvmType* uvm_method_get_param(UvmMethod* method, uint32_t index)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        HAX_ASSERT(FALSE);
        return nullptr;
    }
    else
    {
        static UvmType*(*s_ApiProc)(UvmMethod*, uint32_t) = (decltype(s_ApiProc))GetUvmProc("il2cpp_method_get_param");
        return s_ApiProc(method, index);
    }

}

UvmType* uvm_signature_get_params(UvmMethodSignature* sig, void** iter)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmType*(*s_ApiProc)(UvmMethodSignature*, void**) = (decltype(s_ApiProc))GetUvmProc("mono_signature_get_params");
        return s_ApiProc(sig, iter);
    }
    else
    {
        HAX_ASSERT(FALSE);
        return nullptr;
    }
}

void uvm_method_signature_str(UvmMethod* method, Hax::StringBuilder<256>& sb)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        UvmMethodSignature* monoSig = uvm_method_signature(method);
        UvmType* retType = uvm_signature_get_return_type(monoSig);
        sb.Append(uvm_type_get_name(retType));
        sb.Append("(");

        uint32_t nParams = uvm_signature_get_param_count(monoSig);
        if (nParams > 0)
        {
            void* iter = nullptr;
            sb.Append(uvm_type_get_name(uvm_signature_get_params(monoSig, &iter)));
            while (UvmType* paramType = uvm_signature_get_params(monoSig, &iter))
            {
                sb.Append(",");
                sb.Append(uvm_type_get_name(paramType));
            }
        }
        sb.Append(")");
    }
    else
    {
        UvmType* retType = uvm_method_get_return_type(method);
        sb.Append(uvm_type_get_name(retType));
        sb.Append("(");

        uint32_t nParams = uvm_method_get_param_count(method);
        if (nParams > 0) 
        {
            UvmType* param1 = uvm_method_get_param(method, 0);
            sb.Append(uvm_type_get_name(param1));

            for (uint32_t i = 1; i < nParams; ++i) 
            {
                UvmType* param = uvm_method_get_param(method, i);
                sb.Append(",");
                sb.Append(uvm_type_get_name(param));
            }
        }
        sb.Append(")");
    }

}

UvmClass* uvm_class_get_parent(UvmClass* klass)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmClass*(*s_ApiProc)(UvmClass*) = (decltype(s_ApiProc))GetUvmProc("mono_class_get_parent");
        return s_ApiProc(klass);
    }
    else
    {
        static UvmClass*(*s_ApiProc)(UvmClass*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_class_get_parent");
        return s_ApiProc(klass);
    }
}

UvmMethod* uvm_class_get_method_from_name(UvmClass* klass, Hax::StringView name, Hax::StringView sig)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        do
        {
            void* iter = nullptr;
            while (UvmMethod* method = uvm_class_get_methods(klass, &iter))
            {
                const char* methodName = uvm_method_get_name(method);
                if (name == methodName)
                {
                    if (sig.Empty())
                        return method;

                    Hax::StringBuilder<256> sb;
                    uvm_method_signature_str(method, sb);

                    if (sb.View().StartsWith(sig))
                        return method;
                }
            }
        } while (klass = uvm_class_get_parent(klass));

        return nullptr;
    }
    else
    {
        void* iter = nullptr;
        while (UvmMethod* method = uvm_class_get_methods(klass, &iter)) 
        {
            const char* methodName = uvm_method_get_name(method);
            if (name == methodName) 
            {
                if (sig.Empty())
                    return method;

                Hax::StringBuilder<256> sb;
                uvm_method_signature_str(method, sb);

                if (sb.View().StartsWith(sig))
                    return method;
            }
        }

        return nullptr;
    }

}

UvmMethod* uvm_object_get_virtual_method(UvmObject* obj, UvmMethod* method)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmMethod*(*s_ApiProc)(UvmObject*, UvmMethod*) = (decltype(s_ApiProc))GetUvmProc("mono_object_get_virtual_method");
        return s_ApiProc(obj, method);
    }
    else
    {
        static UvmMethod*(*s_ApiProc)(UvmObject*, UvmMethod*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_object_get_virtual_method");
        return s_ApiProc(obj, method);
    }

}

void* uvm_method_get_pointer(UvmMethod* method)
{
    static Hax::HashMap<UvmMethod*, void*> s_Cache;

    if constexpr (HAX_UNITY_IS_MONO)
    {
        static void*(*s_ApiProc)(UvmMethod*) = (decltype(s_ApiProc))GetUvmProc("mono_compile_method");

        void*& addr = s_Cache.FindOrAdd(method);
        if (addr == nullptr)
            addr = s_ApiProc(method);
        return addr;
    }
    else
    {
        return *(void**)method;
    }
}

void* uvm_method_get_unmanaged_thunk(UvmMethod* method)
{
    if (method == nullptr)
        return nullptr;

    static Hax::HashMap<UvmMethod*, void*> s_Cache;

    if constexpr (HAX_UNITY_IS_MONO)
    {
        static void*(*s_ApiProc)(UvmMethod*) = (decltype(s_ApiProc))GetUvmProc("mono_method_get_unmanaged_thunk");

        void*& thunk = s_Cache.FindOrAdd(method);
        if (thunk == nullptr)
            thunk = s_ApiProc(method);

        return thunk;
    }
    else
    {
        return *(void**)method;
    }
}

UvmString* uvm_string_new_len(const char* str, uint32_t length)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmString*(*s_ApiProc)(UvmDomain*, const char*, uint32_t) = (decltype(s_ApiProc))GetUvmProc("mono_string_new_len");
        return s_ApiProc(uvm_domain_get(), str, length);
    }
    else
    {
        static UvmString*(*s_ApiProc)(const char*, uint32_t) = (decltype(s_ApiProc))GetUvmProc("il2cpp_string_new_len");
        return s_ApiProc(str, length);
    }

}

UvmString* uvm_string_new_utf16(const wchar_t* str, uint32_t length)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmString*(*s_ApiProc)(UvmDomain*, const wchar_t*, uint32_t) = (decltype(s_ApiProc))GetUvmProc("mono_string_new_utf16");
        return s_ApiProc(uvm_domain_get(), str, length);
    }
    else
    {
        static UvmString*(*s_ApiProc)(const wchar_t*, uint32_t) = (decltype(s_ApiProc))GetUvmProc("il2cpp_string_new_utf16");
        return s_ApiProc(str, length);
    }

}

uint32_t uvm_method_get_flags(UvmMethod* method, uint32_t* iflags)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static uint32_t(*s_ApiProc)(UvmMethod*, uint32_t*) = (decltype(s_ApiProc))GetUvmProc("mono_method_get_flags");
        return s_ApiProc(method, iflags);
    }
    else
    {
        static uint32_t(*s_ApiProc)(UvmMethod*, uint32_t*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_method_get_flags");
        return s_ApiProc(method, iflags);
    }

}

UvmObject* uvm_object_new(UvmClass* klass)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmObject*(*s_ApiProc)(UvmDomain*, UvmClass*) = (decltype(s_ApiProc))GetUvmProc("mono_object_new");
        return s_ApiProc(uvm_domain_get(), klass);
    }
    else
    {
        static UvmObject*(*s_ApiProc)(UvmClass*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_object_new");
        return s_ApiProc(klass);
    }

}

void* uvm_gchandle_new(UvmObject* obj, bool pinned)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static void*(*s_ApiProc)(UvmObject*, bool) = (decltype(s_ApiProc))GetUvmProc("mono_gchandle_new");
        return s_ApiProc(obj, pinned);
    }
    else
    {
        static void*(*s_ApiProc)(UvmObject*, bool) = (decltype(s_ApiProc))GetUvmProc("il2cpp_gchandle_new");
        return s_ApiProc(obj, pinned);
    }

}

void uvm_runtime_object_init(UvmObject* obj)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static void(*s_ApiProc)(UvmObject*) = (decltype(s_ApiProc))GetUvmProc("mono_runtime_object_init");
        return s_ApiProc(obj);
    }
    else
    {
        static void(*s_ApiProc)(UvmObject*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_runtime_object_init");
        return s_ApiProc(obj);
    }

}

void* uvm_array_new(UvmClass* eklass, size_t count)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static void*(*s_ApiProc)(UvmDomain*, UvmClass*, size_t) = (decltype(s_ApiProc))GetUvmProc("mono_array_new");
        return s_ApiProc(uvm_domain_get(), eklass, count);
    }
    else
    {
        static void*(*s_ApiProc)(UvmClass*, size_t) = (decltype(s_ApiProc))GetUvmProc("il2cpp_array_new");
        return s_ApiProc(eklass, count);
    }

}

UvmClass* uvm_array_class_get(UvmClass* eklass, uint32_t rank)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmClass*(*s_ApiProc)(UvmClass*, uint32_t) = (decltype(s_ApiProc))GetUvmProc("mono_array_class_get");
        return s_ApiProc(eklass, rank);
    }
    else
    {
        static UvmClass*(*s_ApiProc)(UvmClass*, uint32_t) = (decltype(s_ApiProc))GetUvmProc("il2cpp_array_class_get");
        return s_ApiProc(eklass, rank);
    }

}

UvmObject* uvm_runtime_invoke(UvmMethod* method, void* obj, void** params, void** exc)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmObject*(*s_ApiProc)(UvmMethod*, void*, void**, void**) = (decltype(s_ApiProc))GetUvmProc("mono_runtime_invoke");
        return s_ApiProc(method, obj, params, exc);
    }
    else
    {
        HAX_ASSERT(false);
        return nullptr;
    }
}

bool uvm_class_is_subclass_of(UvmClass* klass, UvmClass* klassc, bool check_interfaces)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static bool(*s_ApiProc)(UvmClass*, UvmClass*, bool) = (decltype(s_ApiProc))GetUvmProc("mono_class_is_subclass_of");
        return s_ApiProc(klass, klassc, check_interfaces);
    }
    else
    {
        static bool(*s_ApiProc)(UvmClass*, UvmClass*, bool) = (decltype(s_ApiProc))GetUvmProc("il2cpp_class_is_subclass_of");
        return s_ApiProc(klass, klassc, check_interfaces);
    }

}

bool uvm_class_is_valuetype(UvmClass* klass)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static bool(*s_ApiProc)(UvmClass*) = (decltype(s_ApiProc))GetUvmProc("mono_class_is_valuetype");
        return s_ApiProc(klass);
    }
    else
    {
        static bool(*s_ApiProc)(UvmClass*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_class_is_valuetype");
        return s_ApiProc(klass);
    }

}

UvmClass* uvm_method_get_class(UvmMethod* method)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static UvmClass*(*s_ApiProc)(UvmMethod*) = (decltype(s_ApiProc))GetUvmProc("mono_method_get_class");
        return s_ApiProc(method);
    }
    else
    {
        static UvmClass*(*s_ApiProc)(UvmMethod*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_method_get_declaring_type");
        return s_ApiProc(method);
    }

}

void uvm_gc_wbarrier_set_field(const UvmObject* obj, UvmObject** field, UvmObject* value)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static void(*s_ApiProc)(const UvmObject*, UvmObject**, UvmObject*) = (decltype(s_ApiProc))GetUvmProc("mono_gc_wbarrier_set_field");
        return s_ApiProc(obj, field, value);
    }
    else
    {
        static void(*s_ApiProc)(const UvmObject*, UvmObject**, UvmObject*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_gc_wbarrier_set_field");
        return s_ApiProc(obj, field, value);
    }
}

void* uvm_lookup_internal_call(UvmMethod* method)
{
    if (method == nullptr)
        return nullptr;

    if constexpr (HAX_UNITY_IS_MONO)
    {
        static void*(*s_ApiProc)(UvmMethod*) = (decltype(s_ApiProc))GetUvmProc("mono_lookup_internal_call");
        return s_ApiProc(method);
    }
    else
    {
        return *(void**)method;
    }
}

void* uvm_resolve_icall(const char* name)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        HAX_ASSERT(false);
        return nullptr;
    }
    else
    {
        static void* (*s_ApiProc)(const char*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_resolve_icall");
        return s_ApiProc(name);
    }
}

void uvm_gchandle_free(void* handle)
{
    if constexpr (HAX_UNITY_IS_MONO)
    {
        static void(*s_ApiProc)(void*) = (decltype(s_ApiProc))GetUvmProc("mono_gchandle_free");
        s_ApiProc(handle);
    }
    else
    {
        static void(*s_ApiProc)(void*) = (decltype(s_ApiProc))GetUvmProc("il2cpp_gchandle_free");
        s_ApiProc(handle);
    }
}

UvmClass* uvm_methodinfo_get_class()
{
    static UvmClass* s_Class = uvm_class_from_name(uvm_get_corlib(), "System.Reflection", "RuntimeMethodInfo");
    return s_Class;
}

UvmReflectionMethod* uvm_method_get_object(UvmMethod* method)
{
    if (method == nullptr)
        return nullptr;

    static Hax::HashMap<UvmMethod*, UvmReflectionMethod*> s_Cache;

    UvmReflectionMethod*& refMethod = s_Cache.FindOrAdd(method);
    if (refMethod == nullptr)
    {
        static UvmMethod* s_Method = uvm_class_get_method_from_name(uvm_methodinfo_get_class(), "GetMethodFromHandleInternalType_native", {});
        HAX_ASSERT(s_Method != nullptr);

        using Func = void*(*)(UvmMethod*,UvmType*,bool,MonoError*);
        static Func s_ICall = (Func)uvm_lookup_internal_call(s_Method);
        HAX_ASSERT(s_ICall != nullptr);

        void* result = nullptr;
        UvmType* type = uvm_class_get_type(uvm_method_get_class(method));

        if constexpr (HAX_UNITY_IS_MONO)
        {
            MonoError err{};
            result = s_ICall(method, type, false, &err);
        }
        else
            result = s_ICall(method, type, false, nullptr);

        refMethod = Hax::Unity::g_ICallsReturnHandle ? *(UvmReflectionMethod**)result : (UvmReflectionMethod*)result;
    }

    return refMethod;
}

UvmClass* uvm_fieldinfo_get_class()
{
    static UvmClass* s_Class = uvm_class_from_name(uvm_get_corlib(), "System.Reflection", "FieldInfo");
    return s_Class;
}

UvmReflectionField* uvm_field_get_object(UvmField* uvmField)
{
    if (uvmField == nullptr)
        return nullptr;

    static Hax::HashMap<UvmField*, UvmReflectionField*> s_Cache;

    UvmReflectionField*& uvmRefField = s_Cache.FindOrAdd(uvmField);
    if (uvmRefField == nullptr)
    {
        static UvmMethod* s_Method = uvm_class_get_method_from_name(uvm_fieldinfo_get_class(), "internal_from_handle_type", {});
        HAX_ASSERT(s_Method != nullptr);

        using Func = void*(*)(UvmField*,UvmType*,MonoError*);
        static Func s_ICall = (Func)uvm_lookup_internal_call(s_Method);
        HAX_ASSERT(s_ICall != nullptr);

        void* result = nullptr;
        if constexpr (HAX_UNITY_IS_MONO)
        {
            MonoError err{};
            result = s_ICall(uvmField, nullptr, &err);
        }
        else
            result = s_ICall(uvmField, nullptr, nullptr);

        uvmRefField = Hax::Unity::g_ICallsReturnHandle ? *(UvmReflectionField**)result : (UvmReflectionField*)result;
    }

    return uvmRefField;
}

UvmClass* uvm_appdomain_get_class()
{
    static UvmClass* s_Class = uvm_class_from_name(uvm_get_corlib(), "System", "AppDomain");
    return s_Class;
}

UvmClass* uvm_assembly_get_class()
{
    static UvmClass* s_Class = uvm_class_from_name(uvm_get_corlib(), "System.Reflection", "Assembly");
    return s_Class;
}

UvmClass* uvm_runtime_type_handle_get_class()
{
    static UvmClass* s_Class = uvm_class_from_name(uvm_get_corlib(), "System", "RuntimeTypeHandle");
    return s_Class;
}

UvmClass* uvm_exception_get_class()
{
    static UvmClass* s_Class = uvm_class_from_name(uvm_get_corlib(), "System", "Exception");
    return s_Class;
}

UvmClass* uvm_argument_out_of_range_exception_get_class()
{
    static UvmClass* s_Class = uvm_class_from_name(uvm_get_corlib(), "System", "ArgumentOutOfRangeException");
    return s_Class;
}

UvmClass* uvm_null_reference_exception_get_class()
{
    static UvmClass* s_Class = uvm_class_from_name(uvm_get_corlib(), "System", "NullReferenceException");
    return s_Class;
}

#pragma endregion

#pragma once

#include "../hax.h"

inline constexpr size_t HAX_UNITY_BOX_POOL_SIZE = 4;
inline constexpr bool HAX_UNITY_IS_MONO = true;

namespace Hax::Unity
{
    void    Initialize(LogFile* logFile);

    bool    IsUnityProcess();
    Handle  GetUvmHandle();
    Handle  AttachThread();
    void    DetachThread(Handle hThread);
}

#ifndef HAX_UNITY_SRC

#include "modules/hax_unity_mscorlib.h"
#include "modules/hax_unity_core.h"
#include "modules/hax_unity_ai.h"
#include "modules/hax_unity_photon.h"
#include "modules/hax_unity_physics.h"
#include "modules/hax_unity_physics2d.h"

#define META(a, ns, c)\
private: inline static System::Type s_Type = System::RequestType(a, ns, c);\
public: static System::Type typeof() { return s_Type; }

#define STATIC_FIELD(n, t)                            \
private: static inline System::IntPtr s_ ## n = System::RequestStaticField(s_Type, #n);\
public: static t& n() { return *(t*)s_ ## n ## .Value; }

#define FIELD(n, t)                                   \
private: static inline System::Int32 s_ ## n = System::RequestFieldOffset(s_Type, #n);     \
public: t& n() const { THROW_IF_NULL(); return *(t*)((char*)m_Ptr + s_ ## n ## .Value); }

#define FIELD_EX(f, n, t)                                   \
private: static inline System::Int32 s_ ## f = System::RequestFieldOffset(s_Type, n); \
public: t& f() const { THROW_IF_NULL(); return *(t*)((char*)m_Ptr + s_ ## f ## .Value); }

#define STATIC_PROPERTY(n, t)                            \
private: static inline System::IntPtr s_ ## n = System::RequestStaticField(s_Type, "<" ## #n ## ">k__BackingField");        \
public: static t& n() { return *(t*)s_ ## n ## .Value; }

#define PROPERTY(n, t) \
private: static inline System::Int32 s_ ## n = System::RequestFieldOffset(s_Type, "<" ## #n ## ">k__BackingField"); \
public: t& n() const { THROW_IF_NULL(); return *(t*)((char*)m_Ptr + s_ ## n ## .Value); }

#define ENUM_VALUE(n)                                 \
private: static inline System::UInt32 s_ ## n = System::RequestEnum(s_Type, #n); \
public: static uint32_t n() { return s_ ## n ## .Value; }

#define METHOD(n)\
static inline System::MethodInfo s_ ## n = System::RequestMethod(s_Type, #n, nullptr)

#define METHOD_EX(n, sig)\
static inline System::MethodInfo s_ ## n = System::RequestMethod(s_Type, #n, sig)

#define METHOD_WRAPPER(n)\
static inline System::MethodInfoWrapper s_ ## n = System::RequestMethodWrapper(s_Type, #n, nullptr, System::MethodInfoWrapperFlags::Thunk | System::MethodInfoWrapperFlags::Address)

#define METHOD_WRAPPER_EX(n, sig)\
static inline System::MethodInfoWrapper s_ ## n = System::RequestMethodWrapper(s_Type, #n, sig, System::MethodInfoWrapperFlags::Thunk | System::MethodInfoWrapperFlags::Address)

#define ICALL_WRAPPER(n)\
static inline System::MethodInfoWrapper s_ ## n = System::RequestMethodWrapper(s_Type, #n, nullptr, System::MethodInfoWrapperFlags::ICall)

#define ICALL_WRAPPER_EX(n, sig)\
static inline System::MethodInfoWrapper s_ ## n = System::RequestMethodWrapper(s_Type, #n, sig, System::MethodInfoWrapperFlags::ICall)

#define COMMA ,

#endif

#ifdef HAX_UNITY_INCLUDE_UVM
struct UvmAssembly;
struct UvmDomain;
struct UvmField;
struct UvmMethod;
struct UvmClass;
struct UvmObject { void* Fields[2]; };
struct UvmReflectionMethod : UvmObject { UvmMethod* UvmMethod; };
struct UvmReflectionAssembly : UvmObject { UvmAssembly* UvmAssembly; };
struct UvmReflectionField : UvmObject { UvmClass* UvmClass; UvmField* UvmField; };
struct UvmAppDomain : UvmObject { void* Identity; UvmDomain* UvmDomain; };
struct UvmMethodSignature;
struct UvmImage;
struct UvmThread;
struct UvmType;
struct UvmVTable;
struct UvmException;
struct UvmReflectionType;
struct UvmArrayType;
struct UvmGenericParam;
struct UvmGenericClass;
struct UvmString;
struct MonoError { union { uint32_t Init; struct { uint16_t ErrorCode, Flags; void* Hidden[30]; }; }; };

void uvm_assembly_foreach(void(*func)(void*,void*), void* data);
const char* uvm_domain_get_friendly_name(UvmDomain* domain);
const char* uvm_image_get_name(UvmImage* image);
UvmImage* uvm_assembly_get_image(UvmAssembly* assembly);
UvmAssembly** uvm_domain_get_assemblies(UvmDomain* domain, size_t* size);
const char* uvm_assembly_get_name(UvmAssembly* uvmAssembly);
UvmImage* uvm_get_corlib();
UvmDomain* uvm_get_root_domain();
UvmDomain* uvm_domain_get();
UvmThread* uvm_thread_attach(UvmDomain* domain);
void uvm_thread_detach(UvmThread* thread);
const char* uvm_method_get_name(UvmMethod* method);
UvmClass* uvm_class_from_name(UvmImage* image, const char* namespaze, const char* name);
UvmType* uvm_class_get_type(UvmClass* klass);
UvmClass* uvm_object_get_class(UvmObject* obj);
UvmReflectionType* uvm_type_get_object(UvmType* type);
UvmClass* uvm_type_get_class(UvmType* type);
UvmClass* uvm_class_from_system_type(UvmReflectionType* type);
UvmImage* uvm_class_get_image(UvmClass* klass);
char* uvm_type_get_name(UvmType* type);
const char* uvm_class_get_name(UvmClass* klass);
const char* uvm_class_get_namespace(UvmClass* klass);
UvmField* uvm_class_get_field_from_name(UvmClass* klass, const char* name);
uint32_t uvm_field_get_flags(UvmField* field);
uint32_t uvm_field_get_offset(UvmField* field);
UvmVTable* uvm_class_vtable(UvmDomain* domain, UvmClass* klass);
void* uvm_vtable_get_static_field_data(UvmVTable* vt);
void* uvm_class_get_static_field_data(UvmClass* klass);
const char* uvm_field_get_name(UvmField* field);
UvmClass* uvm_field_get_parent(UvmField* field);
void uvm_field_static_get_value(UvmField* field, void* value);
UvmType* uvm_field_get_type(UvmField* field);
bool uvm_class_is_assignable_from(UvmClass* klass, UvmClass* oklass);
UvmMethod* uvm_class_get_methods(UvmClass* klass, void** iter);
UvmMethodSignature* uvm_method_signature(UvmMethod* method);
UvmType* uvm_signature_get_return_type(UvmMethodSignature* sig);
UvmType* uvm_method_get_return_type(UvmMethod* method);
uint32_t uvm_signature_get_param_count(UvmMethodSignature* sig);
uint32_t uvm_method_get_param_count(UvmMethod* method);
UvmType* uvm_method_get_param(UvmMethod* method, uint32_t index);
UvmType* uvm_signature_get_params(UvmMethodSignature* sig, void** iter);
void uvm_method_signature_str(UvmMethod* method, Hax::StringBuilder<256>& sb);
UvmClass* uvm_class_get_parent(UvmClass* klass);
UvmMethod* uvm_class_get_method_from_name(UvmClass* klass, Hax::StringView name, Hax::StringView sig);
UvmMethod* uvm_object_get_virtual_method(UvmObject* obj, UvmMethod* method);
void* uvm_method_get_pointer(UvmMethod* method);
void* uvm_method_get_unmanaged_thunk(UvmMethod* method);
UvmString* uvm_string_new_len(const char* str, uint32_t length);
UvmString* uvm_string_new_utf16(const wchar_t* str, uint32_t length);
uint32_t uvm_method_get_flags(UvmMethod* method, uint32_t* iflags);
UvmObject* uvm_object_new(UvmClass* klass);
void* uvm_gchandle_new(UvmObject* obj, bool pinned);
void uvm_runtime_object_init(UvmObject* obj);
void* uvm_array_new(UvmClass* eklass, size_t count);
UvmClass* uvm_array_class_get(UvmClass* eklass, uint32_t rank = 1);
UvmObject* uvm_runtime_invoke(UvmMethod* method, void* obj, void** params, void** exc);
bool uvm_class_is_subclass_of(UvmClass* klass, UvmClass* klassc, bool check_interfaces);
bool uvm_class_is_valuetype(UvmClass* klass);
UvmClass* uvm_method_get_class(UvmMethod* method);
void uvm_gc_wbarrier_set_field(const UvmObject* obj, UvmObject** field, UvmObject* value);
void* uvm_lookup_internal_call(UvmMethod* method);
void* uvm_resolve_icall(const char* name);
void uvm_gchandle_free(void* handle);
UvmClass* uvm_methodinfo_get_class();
UvmReflectionMethod* uvm_method_get_object(UvmMethod* method);
UvmClass* uvm_fieldinfo_get_class();
UvmReflectionField* uvm_field_get_object(UvmField* uvmField);
UvmClass* uvm_appdomain_get_class();
UvmClass* uvm_assembly_get_class();
UvmClass* uvm_runtime_type_handle_get_class();
UvmClass* uvm_exception_get_class();
UvmClass* uvm_argument_out_of_range_exception_get_class();
UvmClass* uvm_null_reference_exception_get_class();
#endif
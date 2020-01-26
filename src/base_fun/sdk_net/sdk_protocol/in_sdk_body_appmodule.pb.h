// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: in_sdk_body_appmodule.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_in_5fsdk_5fbody_5fappmodule_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_in_5fsdk_5fbody_5fappmodule_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3010000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3010000 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_in_5fsdk_5fbody_5fappmodule_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_in_5fsdk_5fbody_5fappmodule_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[3]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_in_5fsdk_5fbody_5fappmodule_2eproto;
namespace insider {
namespace sdk {
class AppModuleBaseInfo;
class AppModuleBaseInfoDefaultTypeInternal;
extern AppModuleBaseInfoDefaultTypeInternal _AppModuleBaseInfo_default_instance_;
class AppModuleCoutInfo;
class AppModuleCoutInfoDefaultTypeInternal;
extern AppModuleCoutInfoDefaultTypeInternal _AppModuleCoutInfo_default_instance_;
class AppModuleCoutInfoList;
class AppModuleCoutInfoListDefaultTypeInternal;
extern AppModuleCoutInfoListDefaultTypeInternal _AppModuleCoutInfoList_default_instance_;
}  // namespace sdk
}  // namespace insider
PROTOBUF_NAMESPACE_OPEN
template<> ::insider::sdk::AppModuleBaseInfo* Arena::CreateMaybeMessage<::insider::sdk::AppModuleBaseInfo>(Arena*);
template<> ::insider::sdk::AppModuleCoutInfo* Arena::CreateMaybeMessage<::insider::sdk::AppModuleCoutInfo>(Arena*);
template<> ::insider::sdk::AppModuleCoutInfoList* Arena::CreateMaybeMessage<::insider::sdk::AppModuleCoutInfoList>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace insider {
namespace sdk {

enum AppModuleState : int {
  E_APP_MODULE_IDLE = 0,
  E_APP_MODULE_INIT = 1,
  E_APP_MODULE_INSTALLED = 2,
  E_APP_MODULE_BAD = 3,
  AppModuleState_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  AppModuleState_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool AppModuleState_IsValid(int value);
constexpr AppModuleState AppModuleState_MIN = E_APP_MODULE_IDLE;
constexpr AppModuleState AppModuleState_MAX = E_APP_MODULE_BAD;
constexpr int AppModuleState_ARRAYSIZE = AppModuleState_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* AppModuleState_descriptor();
template<typename T>
inline const std::string& AppModuleState_Name(T enum_t_value) {
  static_assert(::std::is_same<T, AppModuleState>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function AppModuleState_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    AppModuleState_descriptor(), enum_t_value);
}
inline bool AppModuleState_Parse(
    const std::string& name, AppModuleState* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<AppModuleState>(
    AppModuleState_descriptor(), name, value);
}
enum AppModuleRunState : int {
  E_APP_MODULE_RUN_ST_OK = 0,
  E_APP_MODULE_RUN_ST_ERR = 1,
  E_APP_MODULE_RUN_ST_UNKNOWN = 2,
  AppModuleRunState_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  AppModuleRunState_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool AppModuleRunState_IsValid(int value);
constexpr AppModuleRunState AppModuleRunState_MIN = E_APP_MODULE_RUN_ST_OK;
constexpr AppModuleRunState AppModuleRunState_MAX = E_APP_MODULE_RUN_ST_UNKNOWN;
constexpr int AppModuleRunState_ARRAYSIZE = AppModuleRunState_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* AppModuleRunState_descriptor();
template<typename T>
inline const std::string& AppModuleRunState_Name(T enum_t_value) {
  static_assert(::std::is_same<T, AppModuleRunState>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function AppModuleRunState_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    AppModuleRunState_descriptor(), enum_t_value);
}
inline bool AppModuleRunState_Parse(
    const std::string& name, AppModuleRunState* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<AppModuleRunState>(
    AppModuleRunState_descriptor(), name, value);
}
enum AppModuleCfgPermission : int {
  E_APP_MODULE_CFG_PERMISSION_ENABLE = 0,
  E_APP_MODULE_CFG_PERMISSION_DISENABLE = 1,
  AppModuleCfgPermission_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  AppModuleCfgPermission_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool AppModuleCfgPermission_IsValid(int value);
constexpr AppModuleCfgPermission AppModuleCfgPermission_MIN = E_APP_MODULE_CFG_PERMISSION_ENABLE;
constexpr AppModuleCfgPermission AppModuleCfgPermission_MAX = E_APP_MODULE_CFG_PERMISSION_DISENABLE;
constexpr int AppModuleCfgPermission_ARRAYSIZE = AppModuleCfgPermission_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* AppModuleCfgPermission_descriptor();
template<typename T>
inline const std::string& AppModuleCfgPermission_Name(T enum_t_value) {
  static_assert(::std::is_same<T, AppModuleCfgPermission>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function AppModuleCfgPermission_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    AppModuleCfgPermission_descriptor(), enum_t_value);
}
inline bool AppModuleCfgPermission_Parse(
    const std::string& name, AppModuleCfgPermission* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<AppModuleCfgPermission>(
    AppModuleCfgPermission_descriptor(), name, value);
}
// ===================================================================

class AppModuleBaseInfo :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:insider.sdk.AppModuleBaseInfo) */ {
 public:
  AppModuleBaseInfo();
  virtual ~AppModuleBaseInfo();

  AppModuleBaseInfo(const AppModuleBaseInfo& from);
  AppModuleBaseInfo(AppModuleBaseInfo&& from) noexcept
    : AppModuleBaseInfo() {
    *this = ::std::move(from);
  }

  inline AppModuleBaseInfo& operator=(const AppModuleBaseInfo& from) {
    CopyFrom(from);
    return *this;
  }
  inline AppModuleBaseInfo& operator=(AppModuleBaseInfo&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const AppModuleBaseInfo& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const AppModuleBaseInfo* internal_default_instance() {
    return reinterpret_cast<const AppModuleBaseInfo*>(
               &_AppModuleBaseInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(AppModuleBaseInfo& a, AppModuleBaseInfo& b) {
    a.Swap(&b);
  }
  inline void Swap(AppModuleBaseInfo* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline AppModuleBaseInfo* New() const final {
    return CreateMaybeMessage<AppModuleBaseInfo>(nullptr);
  }

  AppModuleBaseInfo* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<AppModuleBaseInfo>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const AppModuleBaseInfo& from);
  void MergeFrom(const AppModuleBaseInfo& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* InternalSerializeWithCachedSizesToArray(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(AppModuleBaseInfo* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "insider.sdk.AppModuleBaseInfo";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_in_5fsdk_5fbody_5fappmodule_2eproto);
    return ::descriptor_table_in_5fsdk_5fbody_5fappmodule_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kNameFieldNumber = 1,
    kEnableFieldNumber = 2,
    kStateFieldNumber = 3,
    kPermissionFieldNumber = 4,
  };
  // string name = 1;
  void clear_name();
  const std::string& name() const;
  void set_name(const std::string& value);
  void set_name(std::string&& value);
  void set_name(const char* value);
  void set_name(const char* value, size_t size);
  std::string* mutable_name();
  std::string* release_name();
  void set_allocated_name(std::string* name);
  private:
  const std::string& _internal_name() const;
  void _internal_set_name(const std::string& value);
  std::string* _internal_mutable_name();
  public:

  // bool enable = 2;
  void clear_enable();
  bool enable() const;
  void set_enable(bool value);
  private:
  bool _internal_enable() const;
  void _internal_set_enable(bool value);
  public:

  // .insider.sdk.AppModuleState state = 3;
  void clear_state();
  ::insider::sdk::AppModuleState state() const;
  void set_state(::insider::sdk::AppModuleState value);
  private:
  ::insider::sdk::AppModuleState _internal_state() const;
  void _internal_set_state(::insider::sdk::AppModuleState value);
  public:

  // .insider.sdk.AppModuleCfgPermission permission = 4;
  void clear_permission();
  ::insider::sdk::AppModuleCfgPermission permission() const;
  void set_permission(::insider::sdk::AppModuleCfgPermission value);
  private:
  ::insider::sdk::AppModuleCfgPermission _internal_permission() const;
  void _internal_set_permission(::insider::sdk::AppModuleCfgPermission value);
  public:

  // @@protoc_insertion_point(class_scope:insider.sdk.AppModuleBaseInfo)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr name_;
  bool enable_;
  int state_;
  int permission_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_in_5fsdk_5fbody_5fappmodule_2eproto;
};
// -------------------------------------------------------------------

class AppModuleCoutInfo :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:insider.sdk.AppModuleCoutInfo) */ {
 public:
  AppModuleCoutInfo();
  virtual ~AppModuleCoutInfo();

  AppModuleCoutInfo(const AppModuleCoutInfo& from);
  AppModuleCoutInfo(AppModuleCoutInfo&& from) noexcept
    : AppModuleCoutInfo() {
    *this = ::std::move(from);
  }

  inline AppModuleCoutInfo& operator=(const AppModuleCoutInfo& from) {
    CopyFrom(from);
    return *this;
  }
  inline AppModuleCoutInfo& operator=(AppModuleCoutInfo&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const AppModuleCoutInfo& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const AppModuleCoutInfo* internal_default_instance() {
    return reinterpret_cast<const AppModuleCoutInfo*>(
               &_AppModuleCoutInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(AppModuleCoutInfo& a, AppModuleCoutInfo& b) {
    a.Swap(&b);
  }
  inline void Swap(AppModuleCoutInfo* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline AppModuleCoutInfo* New() const final {
    return CreateMaybeMessage<AppModuleCoutInfo>(nullptr);
  }

  AppModuleCoutInfo* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<AppModuleCoutInfo>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const AppModuleCoutInfo& from);
  void MergeFrom(const AppModuleCoutInfo& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* InternalSerializeWithCachedSizesToArray(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(AppModuleCoutInfo* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "insider.sdk.AppModuleCoutInfo";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_in_5fsdk_5fbody_5fappmodule_2eproto);
    return ::descriptor_table_in_5fsdk_5fbody_5fappmodule_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kBaseInfoFieldNumber = 3,
    kIdFieldNumber = 1,
    kRunStateFieldNumber = 2,
  };
  // .insider.sdk.AppModuleBaseInfo base_info = 3;
  bool has_base_info() const;
  private:
  bool _internal_has_base_info() const;
  public:
  void clear_base_info();
  const ::insider::sdk::AppModuleBaseInfo& base_info() const;
  ::insider::sdk::AppModuleBaseInfo* release_base_info();
  ::insider::sdk::AppModuleBaseInfo* mutable_base_info();
  void set_allocated_base_info(::insider::sdk::AppModuleBaseInfo* base_info);
  private:
  const ::insider::sdk::AppModuleBaseInfo& _internal_base_info() const;
  ::insider::sdk::AppModuleBaseInfo* _internal_mutable_base_info();
  public:

  // uint32 id = 1;
  void clear_id();
  ::PROTOBUF_NAMESPACE_ID::uint32 id() const;
  void set_id(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_id() const;
  void _internal_set_id(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // .insider.sdk.AppModuleRunState run_state = 2;
  void clear_run_state();
  ::insider::sdk::AppModuleRunState run_state() const;
  void set_run_state(::insider::sdk::AppModuleRunState value);
  private:
  ::insider::sdk::AppModuleRunState _internal_run_state() const;
  void _internal_set_run_state(::insider::sdk::AppModuleRunState value);
  public:

  // @@protoc_insertion_point(class_scope:insider.sdk.AppModuleCoutInfo)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::insider::sdk::AppModuleBaseInfo* base_info_;
  ::PROTOBUF_NAMESPACE_ID::uint32 id_;
  int run_state_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_in_5fsdk_5fbody_5fappmodule_2eproto;
};
// -------------------------------------------------------------------

class AppModuleCoutInfoList :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:insider.sdk.AppModuleCoutInfoList) */ {
 public:
  AppModuleCoutInfoList();
  virtual ~AppModuleCoutInfoList();

  AppModuleCoutInfoList(const AppModuleCoutInfoList& from);
  AppModuleCoutInfoList(AppModuleCoutInfoList&& from) noexcept
    : AppModuleCoutInfoList() {
    *this = ::std::move(from);
  }

  inline AppModuleCoutInfoList& operator=(const AppModuleCoutInfoList& from) {
    CopyFrom(from);
    return *this;
  }
  inline AppModuleCoutInfoList& operator=(AppModuleCoutInfoList&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const AppModuleCoutInfoList& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const AppModuleCoutInfoList* internal_default_instance() {
    return reinterpret_cast<const AppModuleCoutInfoList*>(
               &_AppModuleCoutInfoList_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  friend void swap(AppModuleCoutInfoList& a, AppModuleCoutInfoList& b) {
    a.Swap(&b);
  }
  inline void Swap(AppModuleCoutInfoList* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline AppModuleCoutInfoList* New() const final {
    return CreateMaybeMessage<AppModuleCoutInfoList>(nullptr);
  }

  AppModuleCoutInfoList* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<AppModuleCoutInfoList>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const AppModuleCoutInfoList& from);
  void MergeFrom(const AppModuleCoutInfoList& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* InternalSerializeWithCachedSizesToArray(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(AppModuleCoutInfoList* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "insider.sdk.AppModuleCoutInfoList";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_in_5fsdk_5fbody_5fappmodule_2eproto);
    return ::descriptor_table_in_5fsdk_5fbody_5fappmodule_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kAppModulesFieldNumber = 1,
  };
  // repeated .insider.sdk.AppModuleCoutInfo app_modules = 1;
  int app_modules_size() const;
  private:
  int _internal_app_modules_size() const;
  public:
  void clear_app_modules();
  ::insider::sdk::AppModuleCoutInfo* mutable_app_modules(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::insider::sdk::AppModuleCoutInfo >*
      mutable_app_modules();
  private:
  const ::insider::sdk::AppModuleCoutInfo& _internal_app_modules(int index) const;
  ::insider::sdk::AppModuleCoutInfo* _internal_add_app_modules();
  public:
  const ::insider::sdk::AppModuleCoutInfo& app_modules(int index) const;
  ::insider::sdk::AppModuleCoutInfo* add_app_modules();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::insider::sdk::AppModuleCoutInfo >&
      app_modules() const;

  // @@protoc_insertion_point(class_scope:insider.sdk.AppModuleCoutInfoList)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::insider::sdk::AppModuleCoutInfo > app_modules_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_in_5fsdk_5fbody_5fappmodule_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// AppModuleBaseInfo

// string name = 1;
inline void AppModuleBaseInfo::clear_name() {
  name_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline const std::string& AppModuleBaseInfo::name() const {
  // @@protoc_insertion_point(field_get:insider.sdk.AppModuleBaseInfo.name)
  return _internal_name();
}
inline void AppModuleBaseInfo::set_name(const std::string& value) {
  _internal_set_name(value);
  // @@protoc_insertion_point(field_set:insider.sdk.AppModuleBaseInfo.name)
}
inline std::string* AppModuleBaseInfo::mutable_name() {
  // @@protoc_insertion_point(field_mutable:insider.sdk.AppModuleBaseInfo.name)
  return _internal_mutable_name();
}
inline const std::string& AppModuleBaseInfo::_internal_name() const {
  return name_.GetNoArena();
}
inline void AppModuleBaseInfo::_internal_set_name(const std::string& value) {
  
  name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
}
inline void AppModuleBaseInfo::set_name(std::string&& value) {
  
  name_.SetNoArena(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:insider.sdk.AppModuleBaseInfo.name)
}
inline void AppModuleBaseInfo::set_name(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:insider.sdk.AppModuleBaseInfo.name)
}
inline void AppModuleBaseInfo::set_name(const char* value, size_t size) {
  
  name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:insider.sdk.AppModuleBaseInfo.name)
}
inline std::string* AppModuleBaseInfo::_internal_mutable_name() {
  
  return name_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* AppModuleBaseInfo::release_name() {
  // @@protoc_insertion_point(field_release:insider.sdk.AppModuleBaseInfo.name)
  
  return name_.ReleaseNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void AppModuleBaseInfo::set_allocated_name(std::string* name) {
  if (name != nullptr) {
    
  } else {
    
  }
  name_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:insider.sdk.AppModuleBaseInfo.name)
}

// bool enable = 2;
inline void AppModuleBaseInfo::clear_enable() {
  enable_ = false;
}
inline bool AppModuleBaseInfo::_internal_enable() const {
  return enable_;
}
inline bool AppModuleBaseInfo::enable() const {
  // @@protoc_insertion_point(field_get:insider.sdk.AppModuleBaseInfo.enable)
  return _internal_enable();
}
inline void AppModuleBaseInfo::_internal_set_enable(bool value) {
  
  enable_ = value;
}
inline void AppModuleBaseInfo::set_enable(bool value) {
  _internal_set_enable(value);
  // @@protoc_insertion_point(field_set:insider.sdk.AppModuleBaseInfo.enable)
}

// .insider.sdk.AppModuleState state = 3;
inline void AppModuleBaseInfo::clear_state() {
  state_ = 0;
}
inline ::insider::sdk::AppModuleState AppModuleBaseInfo::_internal_state() const {
  return static_cast< ::insider::sdk::AppModuleState >(state_);
}
inline ::insider::sdk::AppModuleState AppModuleBaseInfo::state() const {
  // @@protoc_insertion_point(field_get:insider.sdk.AppModuleBaseInfo.state)
  return _internal_state();
}
inline void AppModuleBaseInfo::_internal_set_state(::insider::sdk::AppModuleState value) {
  
  state_ = value;
}
inline void AppModuleBaseInfo::set_state(::insider::sdk::AppModuleState value) {
  _internal_set_state(value);
  // @@protoc_insertion_point(field_set:insider.sdk.AppModuleBaseInfo.state)
}

// .insider.sdk.AppModuleCfgPermission permission = 4;
inline void AppModuleBaseInfo::clear_permission() {
  permission_ = 0;
}
inline ::insider::sdk::AppModuleCfgPermission AppModuleBaseInfo::_internal_permission() const {
  return static_cast< ::insider::sdk::AppModuleCfgPermission >(permission_);
}
inline ::insider::sdk::AppModuleCfgPermission AppModuleBaseInfo::permission() const {
  // @@protoc_insertion_point(field_get:insider.sdk.AppModuleBaseInfo.permission)
  return _internal_permission();
}
inline void AppModuleBaseInfo::_internal_set_permission(::insider::sdk::AppModuleCfgPermission value) {
  
  permission_ = value;
}
inline void AppModuleBaseInfo::set_permission(::insider::sdk::AppModuleCfgPermission value) {
  _internal_set_permission(value);
  // @@protoc_insertion_point(field_set:insider.sdk.AppModuleBaseInfo.permission)
}

// -------------------------------------------------------------------

// AppModuleCoutInfo

// uint32 id = 1;
inline void AppModuleCoutInfo::clear_id() {
  id_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 AppModuleCoutInfo::_internal_id() const {
  return id_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 AppModuleCoutInfo::id() const {
  // @@protoc_insertion_point(field_get:insider.sdk.AppModuleCoutInfo.id)
  return _internal_id();
}
inline void AppModuleCoutInfo::_internal_set_id(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  id_ = value;
}
inline void AppModuleCoutInfo::set_id(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_id(value);
  // @@protoc_insertion_point(field_set:insider.sdk.AppModuleCoutInfo.id)
}

// .insider.sdk.AppModuleRunState run_state = 2;
inline void AppModuleCoutInfo::clear_run_state() {
  run_state_ = 0;
}
inline ::insider::sdk::AppModuleRunState AppModuleCoutInfo::_internal_run_state() const {
  return static_cast< ::insider::sdk::AppModuleRunState >(run_state_);
}
inline ::insider::sdk::AppModuleRunState AppModuleCoutInfo::run_state() const {
  // @@protoc_insertion_point(field_get:insider.sdk.AppModuleCoutInfo.run_state)
  return _internal_run_state();
}
inline void AppModuleCoutInfo::_internal_set_run_state(::insider::sdk::AppModuleRunState value) {
  
  run_state_ = value;
}
inline void AppModuleCoutInfo::set_run_state(::insider::sdk::AppModuleRunState value) {
  _internal_set_run_state(value);
  // @@protoc_insertion_point(field_set:insider.sdk.AppModuleCoutInfo.run_state)
}

// .insider.sdk.AppModuleBaseInfo base_info = 3;
inline bool AppModuleCoutInfo::_internal_has_base_info() const {
  return this != internal_default_instance() && base_info_ != nullptr;
}
inline bool AppModuleCoutInfo::has_base_info() const {
  return _internal_has_base_info();
}
inline void AppModuleCoutInfo::clear_base_info() {
  if (GetArenaNoVirtual() == nullptr && base_info_ != nullptr) {
    delete base_info_;
  }
  base_info_ = nullptr;
}
inline const ::insider::sdk::AppModuleBaseInfo& AppModuleCoutInfo::_internal_base_info() const {
  const ::insider::sdk::AppModuleBaseInfo* p = base_info_;
  return p != nullptr ? *p : *reinterpret_cast<const ::insider::sdk::AppModuleBaseInfo*>(
      &::insider::sdk::_AppModuleBaseInfo_default_instance_);
}
inline const ::insider::sdk::AppModuleBaseInfo& AppModuleCoutInfo::base_info() const {
  // @@protoc_insertion_point(field_get:insider.sdk.AppModuleCoutInfo.base_info)
  return _internal_base_info();
}
inline ::insider::sdk::AppModuleBaseInfo* AppModuleCoutInfo::release_base_info() {
  // @@protoc_insertion_point(field_release:insider.sdk.AppModuleCoutInfo.base_info)
  
  ::insider::sdk::AppModuleBaseInfo* temp = base_info_;
  base_info_ = nullptr;
  return temp;
}
inline ::insider::sdk::AppModuleBaseInfo* AppModuleCoutInfo::_internal_mutable_base_info() {
  
  if (base_info_ == nullptr) {
    auto* p = CreateMaybeMessage<::insider::sdk::AppModuleBaseInfo>(GetArenaNoVirtual());
    base_info_ = p;
  }
  return base_info_;
}
inline ::insider::sdk::AppModuleBaseInfo* AppModuleCoutInfo::mutable_base_info() {
  // @@protoc_insertion_point(field_mutable:insider.sdk.AppModuleCoutInfo.base_info)
  return _internal_mutable_base_info();
}
inline void AppModuleCoutInfo::set_allocated_base_info(::insider::sdk::AppModuleBaseInfo* base_info) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaNoVirtual();
  if (message_arena == nullptr) {
    delete base_info_;
  }
  if (base_info) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena = nullptr;
    if (message_arena != submessage_arena) {
      base_info = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, base_info, submessage_arena);
    }
    
  } else {
    
  }
  base_info_ = base_info;
  // @@protoc_insertion_point(field_set_allocated:insider.sdk.AppModuleCoutInfo.base_info)
}

// -------------------------------------------------------------------

// AppModuleCoutInfoList

// repeated .insider.sdk.AppModuleCoutInfo app_modules = 1;
inline int AppModuleCoutInfoList::_internal_app_modules_size() const {
  return app_modules_.size();
}
inline int AppModuleCoutInfoList::app_modules_size() const {
  return _internal_app_modules_size();
}
inline void AppModuleCoutInfoList::clear_app_modules() {
  app_modules_.Clear();
}
inline ::insider::sdk::AppModuleCoutInfo* AppModuleCoutInfoList::mutable_app_modules(int index) {
  // @@protoc_insertion_point(field_mutable:insider.sdk.AppModuleCoutInfoList.app_modules)
  return app_modules_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::insider::sdk::AppModuleCoutInfo >*
AppModuleCoutInfoList::mutable_app_modules() {
  // @@protoc_insertion_point(field_mutable_list:insider.sdk.AppModuleCoutInfoList.app_modules)
  return &app_modules_;
}
inline const ::insider::sdk::AppModuleCoutInfo& AppModuleCoutInfoList::_internal_app_modules(int index) const {
  return app_modules_.Get(index);
}
inline const ::insider::sdk::AppModuleCoutInfo& AppModuleCoutInfoList::app_modules(int index) const {
  // @@protoc_insertion_point(field_get:insider.sdk.AppModuleCoutInfoList.app_modules)
  return _internal_app_modules(index);
}
inline ::insider::sdk::AppModuleCoutInfo* AppModuleCoutInfoList::_internal_add_app_modules() {
  return app_modules_.Add();
}
inline ::insider::sdk::AppModuleCoutInfo* AppModuleCoutInfoList::add_app_modules() {
  // @@protoc_insertion_point(field_add:insider.sdk.AppModuleCoutInfoList.app_modules)
  return _internal_add_app_modules();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::insider::sdk::AppModuleCoutInfo >&
AppModuleCoutInfoList::app_modules() const {
  // @@protoc_insertion_point(field_list:insider.sdk.AppModuleCoutInfoList.app_modules)
  return app_modules_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace sdk
}  // namespace insider

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::insider::sdk::AppModuleState> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::insider::sdk::AppModuleState>() {
  return ::insider::sdk::AppModuleState_descriptor();
}
template <> struct is_proto_enum< ::insider::sdk::AppModuleRunState> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::insider::sdk::AppModuleRunState>() {
  return ::insider::sdk::AppModuleRunState_descriptor();
}
template <> struct is_proto_enum< ::insider::sdk::AppModuleCfgPermission> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::insider::sdk::AppModuleCfgPermission>() {
  return ::insider::sdk::AppModuleCfgPermission_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_in_5fsdk_5fbody_5fappmodule_2eproto

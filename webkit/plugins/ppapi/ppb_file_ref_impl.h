// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBKIT_PLUGINS_PPAPI_PPB_FILE_REF_IMPL_H_
#define WEBKIT_PLUGINS_PPAPI_PPB_FILE_REF_IMPL_H_

#include <string>

#include "base/files/file_path.h"
#include "googleurl/src/gurl.h"
#include "ppapi/c/pp_file_info.h"
#include "ppapi/c/ppb_file_ref.h"
#include "ppapi/shared_impl/ppb_file_ref_shared.h"
#include "ppapi/shared_impl/var.h"
#include "webkit/glue/webkit_glue_export.h"

namespace webkit {
namespace ppapi {

using ::ppapi::StringVar;

class PPB_FileSystem_Impl;

class WEBKIT_GLUE_EXPORT PPB_FileRef_Impl
    : public ::ppapi::PPB_FileRef_Shared {
 public:
  PPB_FileRef_Impl(const ::ppapi::PPB_FileRef_CreateInfo& info,
                   PPB_FileSystem_Impl* file_system);
  PPB_FileRef_Impl(const ::ppapi::PPB_FileRef_CreateInfo& info,
                   const base::FilePath& external_file_path);
  virtual ~PPB_FileRef_Impl();

  // The returned object will have a refcount of 0 (just like "new").
  static PPB_FileRef_Impl* CreateInternal(PP_Resource pp_file_system,
                                          const std::string& path);

  // The returned object will have a refcount of 0 (just like "new").
  static PPB_FileRef_Impl* CreateExternal(PP_Instance instance,
                                          const base::FilePath& external_file_path,
                                          const std::string& display_name);

  // PPB_FileRef_API implementation (not provided by PPB_FileRef_Shared).
  virtual PP_Resource GetParent() OVERRIDE;
  virtual int32_t MakeDirectory(
      PP_Bool make_ancestors,
      scoped_refptr< ::ppapi::TrackedCallback> callback) OVERRIDE;
  virtual int32_t Touch(
      PP_Time last_access_time,
      PP_Time last_modified_time,
      scoped_refptr< ::ppapi::TrackedCallback> callback) OVERRIDE;
  virtual int32_t Delete(
      scoped_refptr< ::ppapi::TrackedCallback> callback) OVERRIDE;
  virtual int32_t Rename(
      PP_Resource new_file_ref,
      scoped_refptr< ::ppapi::TrackedCallback> callback) OVERRIDE;
  virtual int32_t Query(
      PP_FileInfo* info,
      scoped_refptr< ::ppapi::TrackedCallback> callback) OVERRIDE;
  virtual PP_Var GetAbsolutePath();

  PPB_FileSystem_Impl* file_system() const { return file_system_.get(); }

  // Returns the system path corresponding to this file. Valid only for
  // external filesystems.
  base::FilePath GetSystemPath() const;

  // Returns the FileSystem API URL corresponding to this file.
  GURL GetFileSystemURL() const;

  // Checks if file ref has file system instance and if the instance is opened.
  bool HasValidFileSystem() const;
 private:
  // Many mutation functions are allow only to non-external filesystems, This
  // function returns true if the filesystem is opened and isn't external as an
  // access check for these functions.
  bool IsValidNonExternalFileSystem() const;

  // Null for external filesystems.
  scoped_refptr<PPB_FileSystem_Impl> file_system_;

  // Used only for external filesystems.
  base::FilePath external_file_system_path_;

  // Lazily initialized var created from the external path. This is so we can
  // return the identical string object every time it is requested.
  scoped_refptr<StringVar> external_path_var_;

  DISALLOW_COPY_AND_ASSIGN(PPB_FileRef_Impl);
};

}  // namespace ppapi
}  // namespace webkit

#endif  // WEBKIT_PLUGINS_PPAPI_PPB_FILE_REF_IMPL_H_

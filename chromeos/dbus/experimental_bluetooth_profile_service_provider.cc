// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chromeos/dbus/experimental_bluetooth_profile_service_provider.h"

#include <string>

#include "base/bind.h"
#include "base/chromeos/chromeos_version.h"
#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "base/threading/platform_thread.h"
#include "dbus/bus.h"
#include "dbus/exported_object.h"
#include "dbus/message.h"
#include "dbus/object_path.h"
#include "third_party/cros_system_api/dbus/service_constants.h"

namespace chromeos {

// The ExperimentalBluetoothProfileServiceProvider implementation used in
// production.
class ExperimentalBluetoothProfileServiceProviderImpl
    : public ExperimentalBluetoothProfileServiceProvider {
 public:
  ExperimentalBluetoothProfileServiceProviderImpl(
      dbus::Bus* bus,
      const dbus::ObjectPath& object_path,
      Delegate* delegate)
      : origin_thread_id_(base::PlatformThread::CurrentId()),
        bus_(bus),
        delegate_(delegate),
        object_path_(object_path),
        weak_ptr_factory_(this) {
    VLOG(1) << "Creating Bluetooth Profile: " << object_path_.value();

    exported_object_ = bus_->GetExportedObject(object_path_);

    exported_object_->ExportMethod(
        bluetooth_profile::kExperimentalBluetoothProfileInterface,
        bluetooth_profile::kRelease,
        base::Bind(
            &ExperimentalBluetoothProfileServiceProviderImpl::Release,
            weak_ptr_factory_.GetWeakPtr()),
        base::Bind(
            &ExperimentalBluetoothProfileServiceProviderImpl::OnExported,
            weak_ptr_factory_.GetWeakPtr()));

    exported_object_->ExportMethod(
        bluetooth_profile::kExperimentalBluetoothProfileInterface,
        bluetooth_profile::kNewConnection,
        base::Bind(
            &ExperimentalBluetoothProfileServiceProviderImpl::NewConnection,
            weak_ptr_factory_.GetWeakPtr()),
        base::Bind(
            &ExperimentalBluetoothProfileServiceProviderImpl::OnExported,
            weak_ptr_factory_.GetWeakPtr()));

    exported_object_->ExportMethod(
        bluetooth_profile::kExperimentalBluetoothProfileInterface,
        bluetooth_profile::kRequestDisconnection,
        base::Bind(
         &ExperimentalBluetoothProfileServiceProviderImpl::RequestDisconnection,
            weak_ptr_factory_.GetWeakPtr()),
        base::Bind(
            &ExperimentalBluetoothProfileServiceProviderImpl::OnExported,
            weak_ptr_factory_.GetWeakPtr()));

    exported_object_->ExportMethod(
        bluetooth_profile::kExperimentalBluetoothProfileInterface,
        bluetooth_profile::kCancel,
        base::Bind(
            &ExperimentalBluetoothProfileServiceProviderImpl::Cancel,
            weak_ptr_factory_.GetWeakPtr()),
        base::Bind(
            &ExperimentalBluetoothProfileServiceProviderImpl::OnExported,
            weak_ptr_factory_.GetWeakPtr()));
  }

  virtual ~ExperimentalBluetoothProfileServiceProviderImpl() {
    VLOG(1) << "Cleaning up Bluetooth Profile: " << object_path_.value();

    // Unregister the object path so we can reuse with a new agent.
    bus_->UnregisterExportedObject(object_path_);
  }

 private:
  // Returns true if the current thread is on the origin thread.
  bool OnOriginThread() {
    return base::PlatformThread::CurrentId() == origin_thread_id_;
  }

  // Called by dbus:: when the profile is unregistered from the Bluetooth
  // daemon, generally by our request.
  void Release(dbus::MethodCall* method_call,
               dbus::ExportedObject::ResponseSender response_sender) {
    DCHECK(OnOriginThread());
    DCHECK(delegate_);

    delegate_->Release();

    response_sender.Run(dbus::Response::FromMethodCall(method_call));
  }

  // Called by dbus:: when the Bluetooth daemon establishes a new connection
  // to the profile.
  void NewConnection(dbus::MethodCall* method_call,
                     dbus::ExportedObject::ResponseSender response_sender) {
    DCHECK(OnOriginThread());
    DCHECK(delegate_);

    dbus::MessageReader reader(method_call);
    dbus::ObjectPath device_path;
    dbus::FileDescriptor fd;
    dbus::MessageReader array_reader(NULL);
    if (!reader.PopObjectPath(&device_path) ||
        !reader.PopFileDescriptor(&fd) ||
        !reader.PopArray(&array_reader)) {
      LOG(WARNING) << "NewConnection called with incorrect paramters: "
                   << method_call->ToString();
      return;
    }

    Delegate::Options options;
    while (array_reader.HasMoreData()) {
      dbus::MessageReader dict_entry_reader(NULL);
      std::string key;
      if (!array_reader.PopDictEntry(&dict_entry_reader) ||
          !dict_entry_reader.PopString(&key)) {
        LOG(WARNING) << "NewConnection called with incorrect paramters: "
                     << method_call->ToString();
      } else {
        if (key == bluetooth_profile::kVersionProperty)
          dict_entry_reader.PopVariantOfUint16(&options.version);
        else if (key == bluetooth_profile::kFeaturesProperty)
          dict_entry_reader.PopVariantOfUint16(&options.features);
      }
    }

    Delegate::ConfirmationCallback callback = base::Bind(
        &ExperimentalBluetoothProfileServiceProviderImpl::OnConfirmation,
        weak_ptr_factory_.GetWeakPtr(),
        method_call,
        response_sender);

    delegate_->NewConnection(device_path, &fd, options, callback);

    response_sender.Run(dbus::Response::FromMethodCall(method_call));
  }

  // Called by dbus:: when the Bluetooth daemon is about to disconnect the
  // profile.
  void RequestDisconnection(
      dbus::MethodCall* method_call,
      dbus::ExportedObject::ResponseSender response_sender) {
    DCHECK(OnOriginThread());
    DCHECK(delegate_);

    dbus::MessageReader reader(method_call);
    dbus::ObjectPath device_path;
    if (!reader.PopObjectPath(&device_path)) {
      LOG(WARNING) << "RequestDisconnection called with incorrect paramters: "
                   << method_call->ToString();
      return;
    }

    Delegate::ConfirmationCallback callback = base::Bind(
        &ExperimentalBluetoothProfileServiceProviderImpl::OnConfirmation,
        weak_ptr_factory_.GetWeakPtr(),
        method_call,
        response_sender);

    delegate_->RequestDisconnection(device_path, callback);
  }

  // Called by dbus:: when the request failed before a reply was returned
  // from the device.
  void Cancel(dbus::MethodCall* method_call,
              dbus::ExportedObject::ResponseSender response_sender) {
    DCHECK(OnOriginThread());
    DCHECK(delegate_);

    delegate_->Cancel();

    response_sender.Run(dbus::Response::FromMethodCall(method_call));
  }

  // Called by dbus:: when a method is exported.
  void OnExported(const std::string& interface_name,
                  const std::string& method_name,
                  bool success) {
    LOG_IF(WARNING, !success) << "Failed to export "
                              << interface_name << "." << method_name;
  }

  // Called by the Delegate in response to a method requiring confirmation.
  void OnConfirmation(dbus::MethodCall* method_call,
                      dbus::ExportedObject::ResponseSender response_sender,
                      Delegate::Status status) {
    DCHECK(OnOriginThread());

    switch (status) {
      case Delegate::SUCCESS: {
        response_sender.Run(dbus::Response::FromMethodCall(method_call));
        break;
      }
      case Delegate::REJECTED: {
        response_sender.Run(
            dbus::ErrorResponse::FromMethodCall(
                method_call, bluetooth_profile::kErrorRejected, "rejected")
            .PassAs<dbus::Response>());
        break;
      }
      case Delegate::CANCELLED: {
        response_sender.Run(
            dbus::ErrorResponse::FromMethodCall(
                method_call, bluetooth_profile::kErrorCanceled, "canceled")
            .PassAs<dbus::Response>());
        break;
      }
      default:
        NOTREACHED() << "Unexpected status code from delegate: " << status;
    }
  }

  // Origin thread (i.e. the UI thread in production).
  base::PlatformThreadId origin_thread_id_;

  // D-Bus bus object is exported on, not owned by this object and must
  // outlive it.
  dbus::Bus* bus_;

  // All incoming method calls are passed on to the Delegate and a callback
  // passed to generate the reply. |delegate_| is generally the object that
  // owns this one, and must outlive it.
  Delegate* delegate_;

  // D-Bus object path of object we are exporting, kept so we can unregister
  // again in our destructor.
  dbus::ObjectPath object_path_;

  // D-Bus object we are exporting, owned by this object.
  scoped_refptr<dbus::ExportedObject> exported_object_;

  // Weak pointer factory for generating 'this' pointers that might live longer
  // than we do.
  // Note: This should remain the last member so it'll be destroyed and
  // invalidate its weak pointers before any other members are destroyed.
  base::WeakPtrFactory<ExperimentalBluetoothProfileServiceProviderImpl>
      weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(ExperimentalBluetoothProfileServiceProviderImpl);
};

// The ExperimentalBluetoothProfileServiceProvider implementation used on Linux
// desktop, which does nothing.
class ExperimentalBluetoothProfileServiceProviderStubImpl
    : public ExperimentalBluetoothProfileServiceProvider {
 public:
  explicit ExperimentalBluetoothProfileServiceProviderStubImpl(
      Delegate* delegate) {
  }

  virtual ~ExperimentalBluetoothProfileServiceProviderStubImpl() {
  }
};

ExperimentalBluetoothProfileServiceProvider::
    ExperimentalBluetoothProfileServiceProvider() {
}

ExperimentalBluetoothProfileServiceProvider::
    ~ExperimentalBluetoothProfileServiceProvider() {
}

// static
ExperimentalBluetoothProfileServiceProvider*
    ExperimentalBluetoothProfileServiceProvider::Create(
        dbus::Bus* bus,
        const dbus::ObjectPath& object_path,
        Delegate* delegate) {
  if (base::chromeos::IsRunningOnChromeOS()) {
    return new ExperimentalBluetoothProfileServiceProviderImpl(
        bus, object_path, delegate);
  } else {
    return new ExperimentalBluetoothProfileServiceProviderStubImpl(delegate);
  }
}

}  // namespace chromeos

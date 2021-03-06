// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_EXTENSIONS_DEFAULT_APPS_H_
#define CHROME_BROWSER_EXTENSIONS_DEFAULT_APPS_H_

#include "base/basictypes.h"
#include "chrome/browser/extensions/external_provider_impl.h"
#include "chrome/common/extensions/manifest.h"

class PrefRegistrySyncable;
class Profile;

namespace base {
class DictionaryValue;
}

// Functions and types related to installing default apps.
namespace default_apps {

// These enum values are persisted in the user preferences, so they should not
// be changed.
enum InstallState {
  kUnknown,
  // Now unused, left for backward compatibility.
  kProvideLegacyDefaultApps,
  kNeverInstallDefaultApps,
  kAlreadyInstalledDefaultApps
};

// Register preference properties used by default apps to maintain
// install state.
void RegisterUserPrefs(PrefRegistrySyncable* registry);

// A specialization of the ExternalProviderImpl that conditionally installs apps
// from the chrome::DIR_DEFAULT_APPS location based on a preference in the
// profile.
class Provider : public extensions::ExternalProviderImpl {
 public:
  Provider(Profile* profile,
           VisitorInterface* service,
           extensions::ExternalLoader* loader,
           extensions::Manifest::Location crx_location,
           extensions::Manifest::Location download_location,
           int creation_flags);

  bool ShouldInstallInProfile();

  // ExternalProviderImpl overrides:
  virtual void VisitRegisteredExtension() OVERRIDE;
  virtual void SetPrefs(base::DictionaryValue* prefs) OVERRIDE;

 private:
  Profile* profile_;
  bool is_migration_;

  DISALLOW_COPY_AND_ASSIGN(Provider);
};

}  // namespace default_apps

#endif  // CHROME_BROWSER_EXTENSIONS_DEFAULT_APPS_H_

// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_TABS_PINNED_TAB_SERVICE_FACTORY_H_
#define CHROME_BROWSER_UI_TABS_PINNED_TAB_SERVICE_FACTORY_H_

#include "base/compiler_specific.h"
#include "base/memory/singleton.h"
#include "chrome/browser/profiles/profile_keyed_service_factory.h"

class PinnedTabService;
class Profile;

// Singleton that owns all PinnedTabServices and associates them with Profiles.
// Listens for the Profile's destruction notification and cleans up the
// associated PinnedTabService.
class PinnedTabServiceFactory : public ProfileKeyedServiceFactory {
 public:
  // Returns the PinnedTabService that tracks pinning changes for |profile|.
  static PinnedTabService* GetForProfile(Profile* profile);

  static PinnedTabServiceFactory* GetInstance();

 private:
  friend struct DefaultSingletonTraits<PinnedTabServiceFactory>;

  PinnedTabServiceFactory();
  virtual ~PinnedTabServiceFactory();

  // ProfileKeyedServiceFactory:
  virtual ProfileKeyedService* BuildServiceInstanceFor(
      Profile* profile) const OVERRIDE;
  virtual bool ServiceIsCreatedWithProfile() const OVERRIDE;
  virtual bool ServiceIsNULLWhileTesting() const OVERRIDE;
};

#endif  // CHROME_BROWSER_UI_TABS_PINNED_TAB_SERVICE_FACTORY_H_

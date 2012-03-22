// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ASH_SYSTEM_USER_TRAY_USER_H_
#define ASH_SYSTEM_USER_TRAY_USER_H_
#pragma once

#include "ash/system/tray/system_tray_item.h"
#include "ash/system/user/update_observer.h"
#include "ash/system/user/user_observer.h"

namespace views {
class ImageView;
}

namespace ash {
namespace internal {

namespace tray {
class UserView;
class RoundedImageView;
}

class TrayUser : public SystemTrayItem,
                 public UpdateObserver,
                 public UserObserver {
 public:
  TrayUser();
  virtual ~TrayUser();

 private:
  // Overridden from SystemTrayItem
  virtual views::View* CreateTrayView(user::LoginStatus status) OVERRIDE;
  virtual views::View* CreateDefaultView(user::LoginStatus status) OVERRIDE;
  virtual views::View* CreateDetailedView(user::LoginStatus status) OVERRIDE;
  virtual void DestroyTrayView() OVERRIDE;
  virtual void DestroyDefaultView() OVERRIDE;
  virtual void DestroyDetailedView() OVERRIDE;

  // Overridden from UpdateObserver.
  virtual void OnUpdateRecommended() OVERRIDE;

  // Overridden from UserObserver.
  virtual void OnUserUpdate() OVERRIDE;

  scoped_ptr<tray::UserView> user_;
  scoped_ptr<tray::RoundedImageView> avatar_;

  DISALLOW_COPY_AND_ASSIGN(TrayUser);
};

}  // namespace internal
}  // namespace ash

#endif  // ASH_SYSTEM_USER_TRAY_USER_H_

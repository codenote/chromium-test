// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_INDEXED_DB_INDEXED_DB_INTERNALS_UI_H_
#define CONTENT_BROWSER_INDEXED_DB_INDEXED_DB_INTERNALS_UI_H_

#include "content/public/browser/web_ui_controller.h"

namespace content {

// The implementation for the chrome://indexeddb-internals page.
class IndexedDBInternalsUI : public WebUIController {
 public:
  explicit IndexedDBInternalsUI(WebUI* web_ui);
  virtual ~IndexedDBInternalsUI();

 private:
  DISALLOW_COPY_AND_ASSIGN(IndexedDBInternalsUI);
};

}  // namespace content

#endif  // CONTENT_BROWSER_INDEXED_DB_INDEXED_DB_INTERNALS_UI_H_

// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/size_base.h"

#include "base/logging.h"

// This file provides the implementation for SizeBase template and
// used to instantiate the base class for Size and SizeF classes.
#if !defined(UI_IMPLEMENTATION)
#error "This file is intended for UI implementation only"
#endif

namespace gfx {

template<typename Class, typename Type>
void SizeBase<Class, Type>::set_width(Type width) {
  DCHECK(!(width < 0));
  width_ = width < 0 ? 0 : width;
}

template<typename Class, typename Type>
void SizeBase<Class, Type>::set_height(Type height) {
  DCHECK(!(height < 0));
  height_ = height < 0 ? 0 : height;
}

template<typename Class, typename Type>
SizeBase<Class, Type>::SizeBase(Type width, Type height)
    : width_(width < 0 ? 0 : width),
      height_(height < 0 ? 0 : height) {
  DCHECK(!(width < 0));
  DCHECK(!(height < 0));
}

}  // namespace gfx

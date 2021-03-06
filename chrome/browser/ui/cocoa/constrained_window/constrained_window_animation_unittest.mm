// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/memory/scoped_nsobject.h"
#include "base/message_pump_mac.h"
#import "chrome/browser/ui/cocoa/constrained_window/constrained_window_animation.h"
#import "chrome/browser/ui/cocoa/cocoa_test_helper.h"

// This class runs an animation for exactly two frames then end it.
@interface ConstrainedWindowAnimationTestDelegate : NSObject
                                                   <NSAnimationDelegate> {
 @private
  CGFloat frameCount_;
  scoped_refptr<base::MessagePumpNSRunLoop> message_pump_;
}

- (void)runAnimation:(NSAnimation*)animation;

@end

@implementation ConstrainedWindowAnimationTestDelegate

- (id)init {
  if ((self = [super init]))
    message_pump_ = new base::MessagePumpNSRunLoop;
  return self;
}

- (float)animation:(NSAnimation*)animation
    valueForProgress:(NSAnimationProgress)progress {
  ++frameCount_;
  if (frameCount_ >= 2)
    [animation setDuration:0.0];
  return frameCount_ == 1 ? 0.2 : 0.6;
}

- (void)animationDidEnd:(NSAnimation*)animation {
  EXPECT_EQ(2, frameCount_);
  message_pump_->Quit();
}

- (void)runAnimation:(NSAnimation*)animation {
  // This class will end the animation after 2 frames. Set a large duration to
  // ensure that both frames are processed.
  [animation setDuration:600];
  [animation setDelegate:self];
  [animation startAnimation];
  message_pump_->Run(NULL);
}

@end

class ConstrainedWindowAnimationTest : public CocoaTest {
 protected:
  ConstrainedWindowAnimationTest() : CocoaTest() {
    delegate_.reset([[ConstrainedWindowAnimationTestDelegate alloc] init]);
  }

  scoped_nsobject<ConstrainedWindowAnimationTestDelegate> delegate_;
};

// Test the show animation.
TEST_F(ConstrainedWindowAnimationTest, Show) {
  scoped_nsobject<NSAnimation> animation(
      [[ConstrainedWindowAnimationShow alloc] initWithWindow:test_window()]);
  [delegate_ runAnimation:animation];
}

// Test the hide animation.
TEST_F(ConstrainedWindowAnimationTest, Hide) {
  scoped_nsobject<NSAnimation> animation(
      [[ConstrainedWindowAnimationHide alloc] initWithWindow:test_window()]);
  [delegate_ runAnimation:animation];
}

// Test the pulse animation.
TEST_F(ConstrainedWindowAnimationTest, Pulse) {
  scoped_nsobject<NSAnimation> animation(
      [[ConstrainedWindowAnimationPulse alloc] initWithWindow:test_window()]);
  [delegate_ runAnimation:animation];
}

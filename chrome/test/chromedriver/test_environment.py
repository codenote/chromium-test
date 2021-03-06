# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""TestEnvironment classes.

These classes abstract away the various setups needed to run the WebDriver java
tests in various environments.
"""

import os
import sys

_THIS_DIR = os.path.abspath(os.path.dirname(__file__))

sys.path.insert(0, os.path.join(_THIS_DIR, os.pardir, 'pylib'))

from common import util

if util.IsLinux():
  sys.path.insert(0, os.path.join(_THIS_DIR, os.pardir, os.pardir, os.pardir,
                                  'build', 'android'))
  from pylib import android_commands
  from pylib import forwarder
  from pylib import valgrind_tools

ANDROID_TEST_HTTP_PORT = 2311
ANDROID_TEST_HTTPS_PORT = 2411

_EXPECTATIONS = {}
execfile(os.path.join(_THIS_DIR, 'test_expectations'), _EXPECTATIONS)


class BaseTestEnvironment(object):
  """Manages the environment java tests require to run."""

  def __init__(self, chrome_version='HEAD'):
    """Initializes a desktop test environment.

    Args:
      chrome_version: Optionally a chrome version to run the tests against.
    """
    self._chrome_version = chrome_version

  def GlobalSetUp(self):
    """Sets up the global test environment state."""
    pass

  def GlobalTearDown(self):
    """Tears down the global test environment state."""
    pass

  def GetPassedJavaTestFilter(self):
    """Returns the test filter for running all passing tests.

    Returns:
      Filter string, in Google Test (C++) format.
    """
    raise NotImplementedError


class DesktopTestEnvironment(BaseTestEnvironment):
  """Manages the environment java tests require to run on Desktop."""

  #override
  def GetPassedJavaTestFilter(self):
    return _EXPECTATIONS['GetPassedJavaTestFilter'](
        util.GetPlatformName(), self._chrome_version)


class AndroidTestEnvironment(DesktopTestEnvironment):
  """Manages the environment java tests require to run on Android."""

  def __init__(self, chrome_version='HEAD'):
    super(AndroidTestEnvironment, self).__init__(chrome_version)
    self._adb = None
    self._forwarder = None

  #override
  def GlobalSetUp(self):
    os.putenv('TEST_HTTP_PORT', str(ANDROID_TEST_HTTP_PORT))
    os.putenv('TEST_HTTPS_PORT', str(ANDROID_TEST_HTTPS_PORT))
    self._adb = android_commands.AndroidCommands()
    self._forwarder = forwarder.Forwarder(self._adb, 'Debug')
    self._forwarder.Run(
        [(ANDROID_TEST_HTTP_PORT, ANDROID_TEST_HTTP_PORT),
         (ANDROID_TEST_HTTPS_PORT, ANDROID_TEST_HTTPS_PORT)],
        valgrind_tools.BaseTool(), '127.0.0.1')

  #override
  def GlobalTearDown(self):
    if self._adb is not None:
      forwarder.Forwarder.KillDevice(self._adb, valgrind_tools.BaseTool())
    if self._forwarder is not None:
      self._forwarder.Close()

  #override
  def GetPassedJavaTestFilter(self):
    return _EXPECTATIONS['GetPassedJavaTestFilter'](
        'android', self._chrome_version)

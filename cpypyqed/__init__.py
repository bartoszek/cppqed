# Copyright Raimar Sandner 2012-2014. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt)

import sys
import cpypyqed.config
import traceback

debug='cpypyqed.debug'
if not debug in " ".join(traceback.format_stack()):
  if '--debug' in sys.argv:
    from .debug import *
    sys.argv.remove('--debug')
  else:
    try:
      from core import *
      from elements import *
      from io import *
      cpypyqed.config.build_type="release"
      cpypyqed.config.module_suffix=""
      from compilation.composite import *

    except ImportError as e:
      from .debug import *

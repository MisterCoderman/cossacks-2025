// On non-Windows, use the system stdarg.h instead of the bundled MSVC one.
// The compat/ directory comes first in the include path, so this
// shadows both the MSVC stdarg.h in Main executable/ and reaches
// the real system header via #include_next.
#pragma once
#include_next <stdarg.h>

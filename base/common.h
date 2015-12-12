#ifndef _BASE_COMMON_H_
#define _BASE_COMMON_H_

#include "basictypes.h"
#include "constructormagic.h"

#if defined(WIN32)
#include <windows.h>  // NOLINT: consider this a system header.
#elif defined(POSIX)
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#else
#error "Must define either WIN32 or POSIX."
#endif

#if defined(_MSC_VER)
// warning C4355: 'this' : used in base member initializer list
#pragma warning(disable:4355)
#endif

//////////////////////////////////////////////////////////////////////
// General Utilities
//////////////////////////////////////////////////////////////////////

// Note: UNUSED is also defined in basictypes.h
#ifndef UNUSED
#define UNUSED(x) Unused(static_cast<const void*>(&x))
#define UNUSED2(x, y) Unused(static_cast<const void*>(&x)); \
    Unused(static_cast<const void*>(&y))
#define UNUSED3(x, y, z) Unused(static_cast<const void*>(&x)); \
    Unused(static_cast<const void*>(&y)); \
    Unused(static_cast<const void*>(&z))
#define UNUSED4(x, y, z, a) Unused(static_cast<const void*>(&x)); \
    Unused(static_cast<const void*>(&y)); \
    Unused(static_cast<const void*>(&z)); \
    Unused(static_cast<const void*>(&a))
#define UNUSED5(x, y, z, a, b) Unused(static_cast<const void*>(&x)); \
    Unused(static_cast<const void*>(&y)); \
    Unused(static_cast<const void*>(&z)); \
    Unused(static_cast<const void*>(&a)); \
    Unused(static_cast<const void*>(&b))
inline void Unused(const void*) {}
#endif  // UNUSED

#ifndef WIN32
#define strnicmp(x, y, n) strncasecmp(x, y, n)
#define stricmp(x, y) strcasecmp(x, y)

// TODO: Remove this. std::max should be used everywhere in the code.
// NOMINMAX must be defined where we include <windows.h>.
#define stdmax(x, y) std::max(x, y)
#else
#define stdmax(x, y) base::_max(x, y)
#endif

#define ARRAY_SIZE(x) (static_cast<int>(sizeof(x) / sizeof(x[0])))

/////////////////////////////////////////////////////////////////////////////
// Assertions
/////////////////////////////////////////////////////////////////////////////

#ifndef ENABLE_DEBUG
#define ENABLE_DEBUG _DEBUG
#endif  // !defined(ENABLE_DEBUG)

// Even for release builds, allow for the override of LogAssert. Though no
// macro is provided, this can still be used for explicit runtime asserts
// and allow applications to override the assert behavior.

namespace base {

// LogAssert writes information about an assertion to the log. It's called by
// Assert (and from the ASSERT macro in debug mode) before any other action
// is taken (e.g. breaking the debugger, abort()ing, etc.).
void LogAssert(const char* function, const char* file, int line,
               const char* expression);

typedef void (*AssertLogger)(const char* function,
                             const char* file,
                             int line,
                             const char* expression);

// Sets a custom assert logger to be used instead of the default LogAssert
// behavior. To clear the custom assert logger, pass NULL for |logger| and the
// default behavior will be restored. Only one custom assert logger can be set
// at a time, so this should generally be set during application startup and
// only by one component.
void SetCustomAssertLogger(AssertLogger logger);

}  // namespace base


#if ENABLE_DEBUG

namespace base {

// If a debugger is attached, triggers a debugger breakpoint. If a debugger is
// not attached, forces program termination.
void Break();

inline bool Assert(bool result, const char* function, const char* file,
                   int line, const char* expression) {
  if (!result) {
    LogAssert(function, file, line, expression);
    Break();
    return false;
  }
  return true;
}

}  // namespace base

#if defined(_MSC_VER) && _MSC_VER < 1300
#define __FUNCTION__ ""
#endif

#ifndef ASSERT
#define ASSERT(x) \
    (void)base::Assert((x), __FUNCTION__, __FILE__, __LINE__, #x)
#endif

#ifndef VERIFY
#define VERIFY(x) base::Assert((x), __FUNCTION__, __FILE__, __LINE__, #x)
#endif

#else  // !ENABLE_DEBUG

namespace base {

inline bool ImplicitCastToBool(bool result) { return result; }

}  // namespace base

#ifndef ASSERT
#define ASSERT(x) (void)0
#endif

#ifndef VERIFY
#define VERIFY(x) base::ImplicitCastToBool(x)
#endif

#endif  // !ENABLE_DEBUG

#define COMPILE_TIME_ASSERT(expr)       char CTA_UNIQUE_NAME[expr]
#define CTA_UNIQUE_NAME                 CTA_MAKE_NAME(__LINE__)
#define CTA_MAKE_NAME(line)             CTA_MAKE_NAME2(line)
#define CTA_MAKE_NAME2(line)            constraint_ ## line

// Forces compiler to inline, even against its better judgement. Use wisely.
#if defined(__GNUC__)
#define FORCE_INLINE __attribute__((always_inline))
#elif defined(WIN32)
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE
#endif

// Borrowed from Chromium's base/compiler_specific.h.
// Annotate a virtual method indicating it must be overriding a virtual
// method in the parent class.
// Use like:
//   virtual void foo() OVERRIDE;
#if defined(WIN32)
#define OVERRIDE override
#elif defined(__clang__)
#define OVERRIDE override
#else
#define OVERRIDE
#endif

#endif  // _BASE_COMMON_H_

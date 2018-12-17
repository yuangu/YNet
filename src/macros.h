#ifndef macros_h
#define macros_h


#define YG_PLATFORM_UNKNOW				0
#define YG_PLATFORM_ANDROID			1
#define YG_PLATFORM_IOS				2
#define YG_PLATFORM_WIN32              3
#define YG_PLATFORM_LINUX              5
#define YG_PLATFORM_MAC                8

#define YG_PLATFORM YG_PLATFORM_UNKNOW


// Apple: Mac and iOS
#if defined(__APPLE__) && !defined(ANDROID) // exclude android for binding generator.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE // TARGET_OS_IPHONE includes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see TargetConditionals.h
#undef  YG_PLATFORM
#define YG_PLATFORM         YG_PLATFORM_IOS
#elif TARGET_OS_MAC
#undef  YG_PLATFORM
#define YG_PLATFORM         YG_PLATFORM_MAC
#endif
#endif

// android
#if defined(ANDROID)
#undef  YG_PLATFORM
#define YG_PLATFORM         YG_PLATFORM_ANDROID
#endif

// win32
#if defined(_WIN32) || defined(WIN32) || defined(_WINDOWS)
#undef  YG_PLATFORM
#define YG_PLATFORM         YG_PLATFORM_WIN32
#endif

// linux
#if defined(LINUX) && !defined(__APPLE__)
#undef  YG_PLATFORM
#define YG_PLATFORM         YG_PLATFORM_LINUX
#endif


#if defined (__GNUC__)
# define __FUNC__ ((const char*)(__PRETTY_FUNCTION__))
#elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 19901L 
# define __FUNC__ ((const char*)(__func__)) 
#else
# define __FUNC__ ((const char*)(__FUNCTION__))
#endif


#endif /* macros_h */

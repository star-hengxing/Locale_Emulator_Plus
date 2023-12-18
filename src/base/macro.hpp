#pragma once

#ifndef NAMESPACE_BEGIN
#define NAMESPACE_BEGIN(name) namespace name {
#endif

#ifndef NAMESPACE_END
#define NAMESPACE_END(name) }
#endif

#ifdef _MSC_VER
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE [[gnu::always_inline]]
#endif

#ifdef PROJECT_SET_SYMBOL
    #if defined(_WIN32) || defined(__CYGWIN__)
        #ifdef __GNUC__
            #define PROJECT_EXPORT __attribute__(dllexport)
            #define PROJECT_IMPORT __attribute__(dllimport)
        #else
            #define PROJECT_EXPORT __declspec(dllexport)
            #define PROJECT_IMPORT __declspec(dllimport)
        #endif
    #elif defined(__GNUC__) || defined(__clang__)
        #define PROJECT_EXPORT __attribute__((visibility ("default")))
        #define PROJECT_IMPORT __attribute__((visibility ("default")))
    #endif
#else
#define PROJECT_EXPORT
#define PROJECT_IMPORT
#endif

#if defined(DEBUG) || defined(_DEBUG)
#define PROJECT_DEBUG
#else
#define PROJECT_RELEASE
#endif

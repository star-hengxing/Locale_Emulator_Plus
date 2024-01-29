#pragma once

#ifndef NAMESPACE_BEGIN
#define NAMESPACE_BEGIN(name) namespace name {
#endif

#ifndef NAMESPACE_END
#define NAMESPACE_END(name) }
#endif

#ifndef ALWAYS_INLINE
    #ifdef _MSC_VER
        #define ALWAYS_INLINE __forceinline
    #else
        #define ALWAYS_INLINE [[gnu::always_inline]]
    #endif
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
    #ifndef PROJECT_EXPORT
        #define PROJECT_EXPORT
    #endif
    #ifndef PROJECT_IMPORT
        #define PROJECT_IMPORT
    #endif
#endif

#if !defined(PROJECT_DEBUG) || !(defined(NDEBUG)) || defined(DEBUG) || defined(_DEBUG)
#define PROJECT_DEBUG
#endif

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

#ifndef LIBRARY_API
    #ifdef LIBRARY_STATIC
        #define LIBRARY_API
    #else
        #if defined(_WIN32) || defined(__CYGWIN__)
            #ifdef __GNUC__
                #define LIBRARY_EXPORT __attribute__(dllexport)
                #define LIBRARY_IMPORT __attribute__(dllimport)
            #else
                #define LIBRARY_EXPORT __declspec(dllexport)
                #define LIBRARY_IMPORT __declspec(dllimport)
            #endif
        #elif defined(__GNUC__) || defined(__clang__)
            #define LIBRARY_EXPORT __attribute__((visibility ("default")))
            #define LIBRARY_IMPORT __attribute__((visibility ("default")))
        #endif

        #ifdef LIBRARY_BUILDING
            #define LIBRARY_API LIBRARY_EXPORT
        #else
            #define LIBRARY_API LIBRARY_IMPORT
        #endif
    #endif
#endif

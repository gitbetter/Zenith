
#ifndef CPPFS_API_H
#define CPPFS_API_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)

        #ifdef CPPFS_STATIC_DEFINE
        #  define CPPFS_API
        #  define CPPFS_NO_EXPORT
        #else
        #  ifndef CPPFS_API
        #    ifdef cppfs_EXPORTS
                /* We are building this library */
        #      define CPPFS_API __declspec(dllexport)
        #    else
                /* We are using this library */
        #      define CPPFS_API __declspec(dllimport)
        #    endif
        #  endif

        #  ifndef CPPFS_NO_EXPORT
        #    define CPPFS_NO_EXPORT 
        #  endif
        #endif

        #ifndef CPPFS_DEPRECATED
        #  define CPPFS_DEPRECATED __declspec(deprecated)
        #endif

        #ifndef CPPFS_DEPRECATED_EXPORT
        #  define CPPFS_DEPRECATED_EXPORT CPPFS_API CPPFS_DEPRECATED
        #endif

        #ifndef CPPFS_DEPRECATED_NO_EXPORT
        #  define CPPFS_DEPRECATED_NO_EXPORT CPPFS_NO_EXPORT CPPFS_DEPRECATED
        #endif

        #if 0 /* DEFINE_NO_DEPRECATED */
        #  ifndef CPPFS_NO_DEPRECATED
        #    define CPPFS_NO_DEPRECATED
        #  endif
        #endif

#else 

        #ifdef CPPFS_STATIC_DEFINE
        #  define CPPFS_API
        #  define CPPFS_NO_EXPORT
        #else
        #  ifndef CPPFS_API
        #    ifdef cppfs_EXPORTS
                /* We are building this library */
        #      define CPPFS_API __attribute__((visibility("default")))
        #    else
                /* We are using this library */
        #      define CPPFS_API __attribute__((visibility("default")))
        #    endif
        #  endif

        #  ifndef CPPFS_NO_EXPORT
        #    define CPPFS_NO_EXPORT __attribute__((visibility("hidden")))
        #  endif
        #endif

        #ifndef CPPFS_DEPRECATED
        #  define CPPFS_DEPRECATED __attribute__ ((__deprecated__))
        #endif

        #ifndef CPPFS_DEPRECATED_EXPORT
        #  define CPPFS_DEPRECATED_EXPORT CPPFS_API CPPFS_DEPRECATED
        #endif

        #ifndef CPPFS_DEPRECATED_NO_EXPORT
        #  define CPPFS_DEPRECATED_NO_EXPORT CPPFS_NO_EXPORT CPPFS_DEPRECATED
        #endif

        #if 0 /* DEFINE_NO_DEPRECATED */
        #  ifndef CPPFS_NO_DEPRECATED
        #    define CPPFS_NO_DEPRECATED
        #  endif
        #endif

#endif /* WIN32 */

#endif /* CPPFS_API_H */

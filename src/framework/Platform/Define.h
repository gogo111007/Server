#ifndef _SH_FRAMEWORK_DEFINE_H_
#define _SH_FRAMEWORK_DEFINE_H_

#include <CompilerDef.h>

#define NAMESPACE_SH_NAME SH
#define NAMESPACE_SH namespace NAMESPACE_SH_NAME {
#define NAMESPACE_SH_END };
#define NAMESPACE_SH_USE using namespace NAMESPACE_SH_NAME;
#define NAMESPACE_STD_USE using namespace std;

typedef signed   __int64  int64;
typedef          int      int32;
typedef          short    int16;
typedef signed   char     int8;
typedef unsigned __int64  uint64;
typedef unsigned int      uint32;
typedef unsigned short    uint16;
typedef unsigned char     uint8;

#if COMPILER != COMPILER_MICROSOFT
    typedef uint16      WORD;
    typedef uint32      DWORD;
#endif // compiler

#if COMPILER == COMPILER_INTEL
    #include <ext/hash_map>
    #include <ext/hash_set>
#elif COMPILER == COMPILER_GNU && (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 3)
    #include <tr1/unordered_map>
    #include <tr1/unordered_set>
#elif COMPILER == COMPILER_GNU && __GNUC__ >= 3
    #include <ext/hash_map>
    #include <ext/hash_set>
#elif COMPILER == COMPILER_MICROSOFT && (_MSC_VER > 1500 || _MSC_VER == 1500 && _HAS_TR1)
    #include <unordered_map>
    #include <unordered_set>
#else
    #include <hash_map>
    #include <hash_set>
#endif

#ifdef _STLPORT_VERSION
    #define UNORDERED_MAP std::hash_map
    #define UNORDERED_SET std::hash_set
    #define NAMESPACE_HASH_START namespace std {
    #define NAMESPACE_HASH_END }
    using std::hash_map;
    using std::hash_set;
#elif COMPILER == COMPILER_MICROSOFT && _MSC_VER >= 1600
    #define UNORDERED_MAP std::unordered_map
    #define UNORDERED_SET std::unordered_set
    #define NAMESPACE_HASH_START namespace std {
    #define NAMESPACE_HASH_END }
#elif COMPILER == COMPILER_MICROSOFT && _MSC_VER >= 1500 && _HAS_TR1
    #define UNORDERED_MAP std::tr1::unordered_map
    #define UNORDERED_SET std::tr1::unordered_set
    #define NAMESPACE_HASH_START namespace std { namespace tr1 {
    #define NAMESPACE_HASH_END } }
#elif COMPILER == COMPILER_MICROSOFT && _MSC_VER >= 1300
    #define UNORDERED_MAP stdext::hash_map
    #define UNORDERED_SET stdext::hash_set
    #define NAMESPACE_HASH_START namespace stdext {
    #define NAMESPACE_HASH_END }
    using stdext::hash_map;
    using stdext::hash_set;

    #if !_HAS_TRADITIONAL_STL
        HASH_NAMESPACE_START

            template<class K>
            class hash
            {
                public:
                    size_t operator()(K const&);
            };

        HASH_NAMESPACE_END
    #endif
#elif COMPILER == COMPILER_INTEL
    #define UNORDERED_MAP std::hash_map
    #define UNORDERED_SET std::hash_set
    #define HASH_NAMESPACE_START namespace std {
    #define HASH_NAMESPACE_END }
    using std::hash_map;
    using std::hash_set;
#elif COMPILER == COMPILER_GNU && (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 3)
    #define UNORDERED_MAP std::tr1::unordered_map
    #define UNORDERED_SET std::tr1::unordered_set
    #define HASH_NAMESPACE_START namespace std { namespace tr1 {
    #define HASH_NAMESPACE_END } }
#elif COMPILER == COMPILER_GNU && __GNUC__ >= 3
    #define UNORDERED_MAP __gnu_cxx::hash_map
    #define UNORDERED_SET __gnu_cxx::hash_set
    #define HASH_NAMESPACE_START namespace __gnu_cxx {
    #define HASH_NAMESPACE_END }

    HASH_NAMESPACE_START

        template<>
        class hash<unsigned long long>
        {
            public:
                size_t operator()(const unsigned long long& __x) const { return (size_t)__x; }
        };

        template<typename T>
        class hash<T*>
        {
            public:
                size_t operator()(T* const& __x) const { return (size_t)__x; }
        };

        template<> struct hash<std::string>
        {
            size_t operator()(const std::string& __x) const
            {
                return hash<const char*>()(__x.c_str());
            }
        };

    HASH_NAMESPACE_END

#else
    #define UNORDERED_MAP std::hash_map
    #define UNORDERED_SET std::hash_set
    #define NAMESPACE_HASH_START namespace std {
    #define NAMESPACE_HASH_END }
    using std::hash_map;
    using std::hash_set;
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p) = NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p) = NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p) = NULL; } }
#endif
#ifndef SAFE_RELEASE_DEL
#define SAFE_RELEASE_DEL(p)  { if (p) { (p)->Release(); SAFE_DELETE(p) } }
#endif

#ifndef SAFE_CALL
#define SAFE_CALL(p, f)      { if (p) { return (p)->f; } }
#endif
#ifndef SAFE_CALL_VOID
#define SAFE_CALL_VOID(p, f) { if (p) { (p)->f; } }
#endif

#endif // _H_

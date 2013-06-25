#ifndef _SH_FRAMEWORK_BYTECONVERTER_H_
#define _SH_FRAMEWORK_BYTECONVERTER_H_

#include <Platform\Define.h>
#include <algorithm>

NAMESPACE_SH

    namespace ByteConverter
    {
        template<size_t T>
        inline void convert(char *pVal)
        {
            std::swap(*pVal, *(pVal + T - 1));
            convert < T - 2 > (pVal + 1);
        }

        template<> inline void convert<0>(char*) {}
        template<> inline void convert<1>(char*) {}             // ignore central byte

        template<typename T>
        inline void apply(T *pVal)
        {
            convert<sizeof(T)>((char*)(pVal));
        }
    }

    template<typename T> inline void EndianConvert(T&) { }
    template<typename T> inline void EndianConvertReverse(T &val) { ByteConverter::apply<T>(&val); }

    template<typename T> void EndianConvert(T*);         // will generate link error
    template<typename T> void EndianConvertReverse(T*);  // will generate link error

    inline void EndianConvert(uint8&) { }
    inline void EndianConvert(int8&)  { }
    inline void EndianConvertReverse(uint8&) { }
    inline void EndianConvertReverse(int8&) { }

NAMESPACE_SH_END

#endif

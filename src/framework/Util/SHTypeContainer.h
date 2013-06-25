#ifndef _SH_FRAMEWORK_CLASS_SHTYPECONTAINER_H_
#define _SH_FRAMEWORK_CLASS_SHTYPECONTAINER_H_

#include <common.h>

NAMESPACE_SH

    NAMESPACE_STD_USE

    template<typename KEY, class T>
    class SHTypeContainer : public map<KEY, T>
    {
        public:
            SHTypeContainer() { }
            ~SHTypeContainer() { }
        private:
    };

NAMESPACE_SH_END

#endif
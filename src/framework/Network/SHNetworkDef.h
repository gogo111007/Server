#ifndef _SH_FRAMEWORK_NETWORKDEF_H_
#define _SH_FRAMEWORK_NETWORKDEF_H_

#include <common.h>

const size_t DEFAULT_LISTEN_QUEUE = 50;

NAMESPACE_SH

    using std::string;

    enum Protocol
    {
        TCP,
        UDP
    };

    enum IPVersion
    {
        IPv4,
        IPv6,
        ANY
    };

    enum NetType
    {
        CLIENT,
        SERVER
    };

NAMESPACE_SH_END

#include <SHException.h>
#include <SHUtil.h>
#include <SHReleaseMgr.h>

#endif

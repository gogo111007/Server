#ifndef _SH_SHARED_COMMOM_H_
#define _SH_SHARED_COMMOM_H_

#include <Define.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>

#include <chrono>
#include <mutex>
#include <thread>
#include <functional>

#include <set>
#include <list>
#include <vector>
#include <map>
#include <queue>
#include <iterator>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#if PLATFORM == PLATFORM_WINDOWS
    #include <WinSock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "wsock32.lib")
    #pragma comment(lib, "ws2_32.lib" )

    #define snprintf _snprintf_s
    #define strcpy   strcpy_s
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    #include <sys/socket.h>
    #include <sys/fcntl.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/unistd.h>
    #include <sys/time.h>
    #include <sys/ioctl.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <errno.h>
#endif // Platform

#endif // _H_
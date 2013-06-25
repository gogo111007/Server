#ifndef _SH_FRAMEWORK_CLASS_SOCKET_H_
#define _SH_FRAMEWORK_CLASS_SOCKET_H_

#include <SHNetworkDef.h>

NAMESPACE_SH

    class SHSocket
    {
        public:
            SHSocket(const string &host, uint32 port, Protocol protocol = TCP, IPVersion IPVersion = ANY);
            SHSocket(uint32 port, Protocol protocol = TCP, IPVersion IPVersion = IPv4, const string &host = "", uint32 listenQueue = DEFAULT_LISTEN_QUEUE);
            SHSocket(const string &host, uint32 localPort, uint32 remotePort, IPVersion IPVersion = ANY);

            ~SHSocket();

            SHSocket *Accept();

            int32           Read            (void *pBuffer, size_t bufferSize);
            void            Send            (const void *pBuffer, size_t size);
            int32           ReadFrom        (void *pBuffer, size_t bufferSize, string *plocalHost, uint32 *plocalPort = NULL);
            void            SendTo          (const void *pBuffer, size_t size, const string &remoteHost, uint32 remotePort);
            void            Blocking        (bool blocking);
            void            Parent          (void *pParent);
            void            Disconnect      ();
            bool            disconnected    () const;
            int32           nextReadSize    () const;
            const string   &remoteHost      () const;
            const string   &localHost       () const;
            uint32          remotePort      () const;
            uint32          localPort       () const;
            Protocol        protocol        () const;
            IPVersion       ipVersion       () const;
            NetType         type            () const;
            bool            blocking        () const;
            uint32          listenQueue     () const;
            int32           socketHandler   () const;
            void           *parent          () const;

        private:
            string      m_remoteHost;
            string      m_localHost;
            uint32      m_remotePort;
            uint32      m_localPort;
            Protocol    m_protocol;
            IPVersion   m_IPVersion;
            NetType     m_type;
            bool        m_blocking;
            uint32      m_listenQueue;
            int32       m_socketHandler;

            void       *m_pParent;

        protected:
            SHSocket();

            virtual void  InitSocket();
    };

    #include <Network\SHSocket.inl>

NAMESPACE_SH_END

#endif

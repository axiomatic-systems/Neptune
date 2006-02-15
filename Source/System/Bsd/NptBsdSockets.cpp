/*****************************************************************
|
|   Neptune - Sockets :: BSD/Winsock Implementation
|
|   (c) 2001-2002 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#if defined(WIN32)
#define __WIN32__
#endif
#if defined(__WIN32__)
#define STRICT
#define NPT_WIN32_USE_WINSOCK2
#ifdef NPT_WIN32_USE_WINSOCK2
/* it is important to include this in this order, because winsock.h and ws2tcpip.h */
/* have different definitions for the same preprocessor symbols, such as IP_ADD_MEMBERSHIP */
#include <winsock2.h>
#include <ws2tcpip.h> 
#else
#include <winsock.h>
#endif
#include <windows.h>

#elif defined(__TCS__)
#include <sockets.h>

#elif defined(__PSP__)
#include <psptypes.h>
#include <kernel.h>
#include <pspnet.h>
#include <pspnet_error.h>
#include <pspnet_inet.h>
#include <pspnet_resolver.h>
#include <pspnet_apctl.h>
#include <pspnet_ap_dialog_dummy.h>
#include <errno.h>
#include <wlan.h>
#include <pspnet/sys/socket.h>
#include <pspnet/sys/select.h>
#include <pspnet/netinet/in.h>

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#endif // defined(__WIN32__)

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "NptConfig.h"
#include "NptTypes.h"
#include "NptStreams.h"
#include "NptThreads.h"
#include "NptSockets.h"
#include "NptUtils.h"
#include "NptConstants.h"

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
const int NPT_TCP_SERVER_SOCKET_DEFAULT_LISTEN_COUNT = 5;

/*----------------------------------------------------------------------
|   WinSock adaptation layer
+---------------------------------------------------------------------*/
#if defined(__WIN32__)
#include "NptWin32Network.h"
static NPT_WinsockSystem& WinsockInitializer = NPT_WinsockSystem::Initializer; 

#define EWOULDBLOCK  WSAEWOULDBLOCK
#define EINPROGRESS  WSAEINPROGRESS
#define ECONNREFUSED WSAECONNREFUSED
#define ECONNRESET   WSAECONNRESET
#define ETIMEDOUT    WSAETIMEDOUT
#define ENETRESET    WSAENETRESET
#define EADDRINUSE   WSAEADDRINUSE
#define ENETDOWN     WSAENETDOWN
#define ENETUNREACH  WSAENETUNREACH

#define NPT_BSD_INVALID_SOCKET INVALID_SOCKET
#define NPT_BSD_SOCKET_ERROR SOCKET_ERROR
#define NPT_BSD_IOCTL_ERROR SOCKET_ERROR
#if defined(SetPort)
#undef SetPort
#endif
typedef int    ssize_t;
typedef int    socklen_t;
typedef char*  SocketBuffer;
typedef const char* SocketConstBuffer;
typedef char*  SocketOption;
typedef SOCKET SocketFd;
#define GetSocketError() WSAGetLastError()

#elif defined(__TCS__)  // trimedia PSOS w/ Target TCP
#define NPT_BSD_INVALID_SOCKET (-1)
#define NPT_BSD_SOCKET_ERROR   (-1)
typedef void*  SocketBuffer;
typedef const void* SocketConstBuffer;
typedef void*  SocketOption;
typedef int    SocketFd;
#define GetSocketError() errno

#elif defined(__PSP__)
typedef SceNetInetSocklen_t socklen_t;
#define timeval SceNetInetTimeval
#define inet_addr sceNetInetInetAddr
#define select sceNetInetSelect
#define socket sceNetInetSocket
#define connect sceNetInetConnect
#define bind sceNetInetBind
#define accept sceNetInetAccept
#define getpeername sceNetInetGetpeername
#define getsockopt sceNetInetGetsockopt
#define setsockopt sceNetInetSetsockopt
#define listen sceNetInetListen
#define getsockname sceNetInetGetsockname
#define sockaddr SceNetInetSockaddr
#define sockaddr_in SceNetInetSockaddrIn
#define in_addr SceNetInetInAddr
#define send  sceNetInetSend
#define sendto sceNetInetSendto
#define recv  sceNetInetRecv
#define recvfrom sceNetInetRecvfrom
#define closesocket sceNetInetClose
#define htonl sceNetHtonl
#define htons sceNetHtons
#define ntohl sceNetNtohl
#define ntohs sceNetNtohs
#define SOL_SOCKET SCE_NET_INET_SOL_SOCKET
#define AF_INET SCE_NET_INET_AF_INET
#define SOCK_STREAM SCE_NET_INET_SOCK_STREAM
#define SOCK_DGRAM SCE_NET_INET_SOCK_DGRAM
#define SO_BROADCAST SCE_NET_INET_SO_BROADCAST
#define SO_ERROR SCE_NET_INET_SO_ERROR
#define IPPROTO_IP SCE_NET_INET_IPPROTO_IP
#define IP_ADD_MEMBERSHIP SCE_NET_INET_IP_ADD_MEMBERSHIP
#define IP_MULTICAST_IF SCE_NET_INET_IP_MULTICAST_IF
#define IP_MULTICAST_TTL SCE_NET_INET_IP_MULTICAST_TTL
//#define SO_REUSEPORT SCE_NET_INET_SO_REUSEPORT
#define SO_REUSEADDR SCE_NET_INET_SO_REUSEADDR
//#define SO_REUSEADDR SCE_NET_INET_SO_REUSEPORT
#define INADDR_ANY SCE_NET_INET_INADDR_ANY
#define ip_mreq SceNetInetIpMreq
#ifdef fd_set
#undef fd_set
#endif
#define fd_set SceNetInetFdSet
#ifdef FD_ZERO
#undef FD_ZERO
#endif
#define FD_ZERO SceNetInetFD_ZERO
#ifdef FD_SET
#undef FD_SET
#endif
#define FD_SET SceNetInetFD_SET
#ifdef FD_CLR
#undef FD_CLR
#endif
#define FD_CLR SceNetInetFD_CLR
#ifdef FD_ISSET
#undef FD_ISSET
#endif
#define FD_ISSET SceNetInetFD_ISSET

#define NPT_BSD_INVALID_SOCKET (-1)
#define NPT_BSD_SOCKET_ERROR   (-1)
#define NPT_BSD_IOCTL_ERROR    (-1)
typedef void*  SocketBuffer;
typedef const void* SocketConstBuffer;
typedef void*  SocketOption;
typedef int    SocketFd;
#define GetSocketError() sceNetInetGetErrno()
#define RESOLVER_TIMEOUT (5 * 1000 * 1000)
#define RESOLVER_RETRY 5

#else  // unix-style BSD sockets
#define NPT_BSD_INVALID_SOCKET (-1)
#define NPT_BSD_SOCKET_ERROR   (-1)
#define NPT_BSD_IOCTL_ERROR    (-1)
typedef void*  SocketBuffer;
typedef const void* SocketConstBuffer;
typedef void*  SocketOption;
typedef int    SocketFd;
#define GetSocketError() errno
#define closesocket  close
#define ioctlsocket  ioctl
#endif

/*----------------------------------------------------------------------
|   SocketAddressToInetAddress
+---------------------------------------------------------------------*/
static void
SocketAddressToInetAddress(const NPT_SocketAddress& socket_address, 
                           struct sockaddr_in*      inet_address)
{
    // initialize the structure
    for (int i=0; i<8; i++) inet_address->sin_zero[i]=0;

    // setup the structure
    inet_address->sin_family = AF_INET;
    inet_address->sin_port = htons(socket_address.GetPort());
    inet_address->sin_addr.s_addr = htonl(socket_address.GetIpAddress().AsLong());
}

/*----------------------------------------------------------------------
|   InetAddressToSocketAddress
+---------------------------------------------------------------------*/
static void
InetAddressToSocketAddress(const struct sockaddr_in* inet_address,
                           NPT_SocketAddress&        socket_address)
{
    // read the fields
    socket_address.SetPort(ntohs(inet_address->sin_port));
    socket_address.SetIpAddress(NPT_IpAddress(ntohl(inet_address->sin_addr.s_addr)));
}

/*----------------------------------------------------------------------
|   MapErrorCode
+---------------------------------------------------------------------*/
static NPT_Result 
MapErrorCode(int error)
{
    switch (error) {
        case ECONNRESET:
        case ENETRESET:
            return NPT_ERROR_CONNECTION_RESET;

        case ECONNREFUSED:
            return NPT_ERROR_CONNECTION_REFUSED;

        case ETIMEDOUT:
            return NPT_ERROR_TIMEOUT;

        case EADDRINUSE:
            return NPT_ERROR_ADDRESS_IN_USE;

        case ENETDOWN:
            return NPT_ERROR_NETWORK_DOWN;

        case ENETUNREACH:
            return NPT_ERROR_NETWORK_UNREACHABLE;

        case EINPROGRESS:
        case EAGAIN:
#if defined(EWOULDBLOCK) && (EWOULDBLOCK != EAGAIN)
        case EWOULDBLOCK:
#endif
            return NPT_ERROR_WOULD_BLOCK;

        default:
            return NPT_FAILURE;
    }
}

/*----------------------------------------------------------------------
|   NPT_IpAddress::ResolveName
+---------------------------------------------------------------------*/
NPT_Result
NPT_IpAddress::ResolveName(const char* name, NPT_Timeout)
{
    // check parameters
    if (name == NULL || name[0] == '\0') return NPT_ERROR_HOST_UNKNOWN;

    // handle numerical addrs
    NPT_IpAddress numerical_address;
    if (NPT_SUCCEEDED(numerical_address.Parse(name))) {
        /* the name is a numerical IP addr */
        return Set(numerical_address.AsLong());
    }

#if defined(__TCS__)
    Set(getHostByName(name));
#elif defined(__PSP__)
    int rid;
    char buf[1024];
    int buflen = sizeof(buf);

    int ret = sceNetResolverCreate(&rid, buf, buflen);
    if(ret < 0){
        return NPT_FAILURE;
    }
    ret = sceNetResolverStartNtoA(rid, name, &address->sin_addr,
        RESOLVER_TIMEOUT, RESOLVER_RETRY);
    if(ret < 0){
        return NPT_ERROR_HOST_UNKNOWN;
    }
    sceNetResolverDelete(rid);
#else
    // do a name lookup
    struct hostent *host_entry = gethostbyname(name);
    if (host_entry == NULL ||
        host_entry->h_addrtype != AF_INET) {
        return NPT_ERROR_HOST_UNKNOWN;
    }
    NPT_CopyMemory(m_Address, host_entry->h_addr, 4);
#endif
	
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocketFd
+---------------------------------------------------------------------*/
class NPT_BsdSocketFd
{
public:
    // constructors and destructor
    NPT_BsdSocketFd(SocketFd fd) : m_SocketFd(fd) {}
    ~NPT_BsdSocketFd() {
        closesocket(m_SocketFd);
    }

    // methods
    SocketFd GetSocketFd() { return m_SocketFd; }
    NPT_Result SetBlockingMode(bool blocking);

private:
    // members
    SocketFd m_SocketFd;
};

typedef NPT_Reference<NPT_BsdSocketFd> NPT_BsdSocketFdReference;

#if defined(__WIN32__) || defined(__TCS__)
/*----------------------------------------------------------------------
|   NPT_BsdSocketFd::SetBlockingMode
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketFd::SetBlockingMode(bool blocking)
{
    unsigned long args = blocking?0:1;
    if (ioctlsocket(m_SocketFd, FIONBIO, &args) ==
        NPT_BSD_IOCTL_ERROR) {
            return NPT_FAILURE;
    }
    return NPT_SUCCESS;
}
#elif defined(__PSP__)
/*----------------------------------------------------------------------
|   NPT_BsdSocket::SetBlockingMode
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketFd::SetBlockingMode(bool blocking)
{
    unsigned long args = blocking?0:1;
    if (setsockopt(m_SocketFd, SOL_SOCKET, SCE_NET_INET_SO_NBIO, &args, sizeof(args))) {
        return NPT_FAILURE;
    }
    return NPT_SUCCESS;
}
#else
/*----------------------------------------------------------------------
|   NPT_BsdSocket::SetBlockingMode
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketFd::SetBlockingMode(bool blocking)
{
    int flags = fcntl(m_SocketFd, F_GETFL, 0);
    if (blocking) {
        flags ^= O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }
    if (fcntl(m_SocketFd, F_SETFL, flags)) {
        return NPT_FAILURE;
    }
    return NPT_SUCCESS;
}
#endif

/*----------------------------------------------------------------------
|   NPT_BsdSocketStream
+---------------------------------------------------------------------*/
class NPT_BsdSocketStream
{
 public:
    // methods
    NPT_BsdSocketStream(NPT_BsdSocketFdReference& socket_fd) :
       m_SocketFdReference(socket_fd) {}

    // NPT_InputStream and NPT_OutputStream methods
    NPT_Result Seek(NPT_Offset) { return NPT_FAILURE; }
    NPT_Result Tell(NPT_Offset& where) {
        where = 0;
        return NPT_SUCCESS;
    }

 protected:
    // constructors and destructors
     virtual ~NPT_BsdSocketStream() {}

    // members
    NPT_BsdSocketFdReference m_SocketFdReference;
};

/*----------------------------------------------------------------------
|   NPT_BsdSocketInputStream
+---------------------------------------------------------------------*/
class NPT_BsdSocketInputStream : public NPT_InputStream,
                                 private NPT_BsdSocketStream
{
public:
    // constructors and destructor
    NPT_BsdSocketInputStream(NPT_BsdSocketFdReference& socket_fd) :
      NPT_BsdSocketStream(socket_fd) {}

    // NPT_InputStream methods
    NPT_Result Read(void*     buffer, 
                    NPT_Size  bytes_to_read, 
                    NPT_Size* bytes_read);
    NPT_Result Seek(NPT_Offset offset) { 
        return NPT_BsdSocketStream::Seek(offset); }
    NPT_Result Tell(NPT_Offset& where) {
        return NPT_BsdSocketStream::Tell(where);
    }
    NPT_Result GetSize(NPT_Size& size);
    NPT_Result GetAvailable(NPT_Size& available);
};

/*----------------------------------------------------------------------
|   NPT_BsdSocketInputStream::Read
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketInputStream::Read(void*     buffer, 
                               NPT_Size  bytes_to_read, 
                               NPT_Size* bytes_read)
{
    ssize_t nb_read;

    nb_read = recv(m_SocketFdReference->GetSocketFd(), 
                   (SocketBuffer)buffer, 
                   bytes_to_read, 0);
    if (nb_read > 0) {
        if (bytes_read) *bytes_read = nb_read;
        return NPT_SUCCESS;
    } else {
        if (bytes_read) *bytes_read = 0;
        if (nb_read == 0) {
            return NPT_ERROR_EOS;
        } else {
            return MapErrorCode(GetSocketError());
        }
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocketInputStream::GetSize
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketInputStream::GetSize(NPT_Size& size)
{
    // generic socket streams have no size
    size = 0;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocketInputStream::GetAvailable
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketInputStream::GetAvailable(NPT_Size& available)
{
    unsigned long ready = 0;
    int io_result = ioctlsocket(m_SocketFdReference->GetSocketFd(), FIONREAD, &ready); 
    if (io_result == NPT_BSD_IOCTL_ERROR) {
        available = 0;
        return NPT_FAILURE;
    } else {
        available = ready;
        return NPT_SUCCESS;
    }
}

/*----------------------------------------------------------------------
|   NPT_BsdSocketOutputStream
+---------------------------------------------------------------------*/
class NPT_BsdSocketOutputStream : public NPT_OutputStream,
                                  private NPT_BsdSocketStream
{
public:
    // constructors and destructor
    NPT_BsdSocketOutputStream(NPT_BsdSocketFdReference& socket_fd) :
        NPT_BsdSocketStream(socket_fd) {}

    // NPT_OutputStream methods
    NPT_Result Write(const void* buffer, 
                     NPT_Size    bytes_to_write, 
                     NPT_Size*   bytes_written);
    NPT_Result Seek(NPT_Offset offset) { 
        return NPT_BsdSocketStream::Seek(offset); }
    NPT_Result Tell(NPT_Offset& where) {
        return NPT_BsdSocketStream::Tell(where);
    }
};

/*----------------------------------------------------------------------
|   NPT_BsdSocketOutputStream::Write
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketOutputStream::Write(const void*  buffer, 
                                 NPT_Size     bytes_to_write, 
                                 NPT_Size*    bytes_written)
{
    ssize_t nb_written;

	nb_written = send(m_SocketFdReference->GetSocketFd(), 
					  (SocketConstBuffer)buffer, 
					  bytes_to_write, 
					  0);

    if (nb_written > 0) {
        if (bytes_written) *bytes_written = nb_written;
        return NPT_SUCCESS;
    } else {
        if (bytes_written) *bytes_written = 0;
        if (nb_written == 0) {
            return NPT_ERROR_CONNECTION_RESET;
        } else {
            return MapErrorCode(GetSocketError());
        }
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket
+---------------------------------------------------------------------*/
class NPT_BsdSocket : public NPT_SocketInterface
{
 public:
    // constructors and destructor
             NPT_BsdSocket() {}
             NPT_BsdSocket(SocketFd fd);
    virtual ~NPT_BsdSocket();

    // methods
    NPT_Result RefreshInfo();

    // NPT_SocketInterface methods
	NPT_Result Bind(const NPT_SocketAddress& address, bool reuse_address = true);
	NPT_Result Connect(const NPT_SocketAddress& address, NPT_Timeout timeout);
    NPT_Result WaitForConnection(NPT_Timeout timeout);
    NPT_Result GetInputStream(NPT_InputStreamReference& stream);
    NPT_Result GetOutputStream(NPT_OutputStreamReference& stream);
    NPT_Result GetInfo(NPT_SocketInfo& info);
    NPT_Result SetBlockingMode(bool blocking);

 protected:
    // members
    NPT_BsdSocketFdReference m_SocketFdReference;
    NPT_SocketInfo           m_Info;
    bool                     m_Blocking;
};

/*----------------------------------------------------------------------
|   NPT_BsdSocket::NPT_BsdSocket
+---------------------------------------------------------------------*/
NPT_BsdSocket::NPT_BsdSocket(SocketFd fd) : 
    m_SocketFdReference(new NPT_BsdSocketFd(fd)),
    m_Blocking(true)
{
    RefreshInfo();
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::~NPT_BsdSocket
+---------------------------------------------------------------------*/
NPT_BsdSocket::~NPT_BsdSocket()
{
    // release the socket fd reference
    m_SocketFdReference = NULL;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::Bind
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocket::Bind(const NPT_SocketAddress& address, bool reuse_address)
{
    // set socket options
    if (reuse_address) {
        int option = 1;
        setsockopt(m_SocketFdReference->GetSocketFd(), 
                   SOL_SOCKET, 
                   SO_REUSEADDR, 
                   (SocketOption)&option, 
                   sizeof(option));
    }
    
    // convert the address
    struct sockaddr_in inet_address;
    SocketAddressToInetAddress(address, &inet_address);
    
    // bind the socket
    if (bind(m_SocketFdReference->GetSocketFd(), 
             (struct sockaddr*)&inet_address, 
             sizeof(inet_address)) < 0) {
        return NPT_ERROR_BIND_FAILED;
    }

    // refresh socket info
    RefreshInfo();

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::Connect
+---------------------------------------------------------------------*/
NPT_Result 
NPT_BsdSocket::Connect(const NPT_SocketAddress&, NPT_Timeout)
{
    // this is unsupported unless overridden in a derived class
    return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::WaitForConnection
+---------------------------------------------------------------------*/
NPT_Result 
NPT_BsdSocket::WaitForConnection(NPT_Timeout)
{
    // this is unsupported unless overridden in a derived class
    return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::GetInputStream
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocket::GetInputStream(NPT_InputStreamReference& stream)
{
    // default value
    stream = NULL;

    // check that we have a socket
    if (m_SocketFdReference.IsNull()) return NPT_ERROR_INVALID_STATE;

    // create a stream
    stream = new NPT_BsdSocketInputStream(m_SocketFdReference);

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::GetOutputStream
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocket::GetOutputStream(NPT_OutputStreamReference& stream)
{
    // default value
    stream = NULL;

    // check that the file is open
    if (m_SocketFdReference.IsNull()) return NPT_ERROR_INVALID_STATE;

    // create a stream
    stream = new NPT_BsdSocketOutputStream(m_SocketFdReference);

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::GetInfo
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocket::GetInfo(NPT_SocketInfo& info)
{
    // return the cached info
    info = m_Info;
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::RefreshInfo
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocket::RefreshInfo()
{
    // check that we have a socket
    if (m_SocketFdReference.IsNull()) return NPT_ERROR_INVALID_STATE;

    // get the local socket addr
    struct sockaddr_in inet_address;
    socklen_t          name_length = sizeof(inet_address);
    if (getsockname(m_SocketFdReference->GetSocketFd(), 
                    (struct sockaddr*)&inet_address, 
                    &name_length) == 0) {
        m_Info.local_address.SetIpAddress(ntohl(inet_address.sin_addr.s_addr));
        m_Info.local_address.SetPort(ntohs(inet_address.sin_port));
    }   

    // get the peer socket addr
    if (getpeername(m_SocketFdReference->GetSocketFd(),
                    (struct sockaddr*)&inet_address, 
                    &name_length) == 0) {
        m_Info.remote_address.SetIpAddress(ntohl(inet_address.sin_addr.s_addr));
        m_Info.remote_address.SetPort(ntohs(inet_address.sin_port));
    }   

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::SetBlockingMode
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocket::SetBlockingMode(bool blocking)
{
    if (m_Blocking != blocking) {
        m_SocketFdReference->SetBlockingMode(blocking);
        m_Blocking = blocking;
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_Socket::~NPT_Socket
+---------------------------------------------------------------------*/
NPT_Socket::~NPT_Socket()
{
    delete m_SocketDelegate;
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpSocket
+---------------------------------------------------------------------*/
class NPT_BsdUdpSocket : public    NPT_UdpSocketInterface,
                         protected NPT_BsdSocket
                         
{
 public:
    // constructor
     NPT_BsdUdpSocket();
    virtual ~NPT_BsdUdpSocket() {}

    // NPT_SocketInterface methods
    NPT_Result Connect(const NPT_SocketAddress& address,
                       NPT_Timeout              timeout);

    // NPT_UdpSocketInterface methods
    NPT_Result Send(const NPT_DataBuffer&    packet, 
                    const NPT_SocketAddress* address);
	NPT_Result Receive(NPT_DataBuffer&     packet, 
                       NPT_SocketAddress*  address);

    // friends
    friend class NPT_UdpSocket;
};

/*----------------------------------------------------------------------
|   NPT_BsdUdpSocket::NPT_BsdUdpSocket
+---------------------------------------------------------------------*/
NPT_BsdUdpSocket::NPT_BsdUdpSocket() : 
    NPT_BsdSocket(socket(AF_INET, SOCK_DGRAM, 0))
{
    // set default socket options
    int option = 1;
    setsockopt(m_SocketFdReference->GetSocketFd(), 
               SOL_SOCKET, 
               SO_BROADCAST, 
               (SocketOption)&option, 
               sizeof(option));
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpSocket::Connect
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdUdpSocket::Connect(const NPT_SocketAddress& address, 
                          NPT_Timeout /* ignored */)
{
    // setup an address structure
    struct sockaddr_in inet_address;
    SocketAddressToInetAddress(address, &inet_address);

    // connect so that we can have some addr bound to the socket
    int io_result = connect(m_SocketFdReference->GetSocketFd(), 
                            (struct sockaddr *)&inet_address, 
                            sizeof(inet_address));
    if (io_result == NPT_BSD_SOCKET_ERROR) { 
        return MapErrorCode(GetSocketError());
    }
    
    // refresh socket info
    RefreshInfo();

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpSocket::Send
+---------------------------------------------------------------------*/
NPT_Result 
NPT_BsdUdpSocket::Send(const NPT_DataBuffer&    packet, 
                       const NPT_SocketAddress* address) 
{
    // get the packet buffer
    const NPT_Byte* buffer        = packet.GetData();
    ssize_t         buffer_length = packet.GetDataSize();

    // send the packet buffer
    int io_result;
    if (address) {
        // send to the specified address

        // setup an address structure
        struct sockaddr_in inet_address;
        SocketAddressToInetAddress(*address, &inet_address);
        io_result = sendto(m_SocketFdReference->GetSocketFd(), 
                           (SocketConstBuffer)buffer, 
                           buffer_length, 
                           0, 
                           (struct sockaddr *)&inet_address, 
                           sizeof(inet_address));
    } else {
        // send to whichever addr the socket is connected
        io_result = send(m_SocketFdReference->GetSocketFd(), 
                         (SocketConstBuffer)buffer, 
                         buffer_length,
                         0);
    }

    // check result
    if (io_result == NPT_BSD_SOCKET_ERROR) {
        return MapErrorCode(GetSocketError());
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpSocket::Receive
+---------------------------------------------------------------------*/
NPT_Result 
NPT_BsdUdpSocket::Receive(NPT_DataBuffer&    packet, 
                          NPT_SocketAddress* address)
{
    // get the packet buffer
    NPT_Byte* buffer      = packet.UseData();
    ssize_t   buffer_size = packet.GetBufferSize();

    // check that we have some space to receive
    if (buffer_size == 0) {
        return NPT_ERROR_INVALID_PARAMETERS;
    }

    // receive a packet
    int io_result;
    if (address) {
        struct sockaddr_in inet_address;
        socklen_t          inet_address_length = sizeof(inet_address);
        io_result = recvfrom(m_SocketFdReference->GetSocketFd(), 
                             (SocketBuffer)buffer, 
                             buffer_size, 
                             0, 
                             (struct sockaddr *)&inet_address, 
                             &inet_address_length);

        // convert the address format
        if (io_result != NPT_BSD_SOCKET_ERROR) {
            if (inet_address_length == sizeof(inet_address)) {
                InetAddressToSocketAddress(&inet_address, *address);
            }
        }
    } else {
        io_result = recv(m_SocketFdReference->GetSocketFd(),
                         (SocketBuffer)buffer,
                         buffer_size,
                         0);
    }

    // check result
    if (io_result == NPT_BSD_SOCKET_ERROR) {
        packet.SetDataSize(0);
        return MapErrorCode(GetSocketError());
    } else {
        packet.SetDataSize(io_result);
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_UdpSocket::NPT_UdpSocket
+---------------------------------------------------------------------*/
NPT_UdpSocket::NPT_UdpSocket()
{
    NPT_BsdUdpSocket* delegate = new NPT_BsdUdpSocket();
    m_SocketDelegate    = delegate;
    m_UdpSocketDelegate = delegate;
}

/*----------------------------------------------------------------------
|   NPT_UdpSocket::NPT_UdpSocket
+---------------------------------------------------------------------*/
NPT_UdpSocket::NPT_UdpSocket(NPT_UdpSocketInterface* delegate) :
    m_UdpSocketDelegate(delegate)
{
}

/*----------------------------------------------------------------------
|   NPT_UdpSocket::~NPT_UdpSocket
+---------------------------------------------------------------------*/
NPT_UdpSocket::~NPT_UdpSocket()
{
    delete m_UdpSocketDelegate;

    // set the delegate pointers to NULL because it is shared by the
    // base classes, and we only want to delete the object once
    m_UdpSocketDelegate = NULL;
    m_SocketDelegate    = NULL;
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket
+---------------------------------------------------------------------*/
class NPT_BsdUdpMulticastSocket : public    NPT_UdpMulticastSocketInterface,
                                  protected NPT_BsdUdpSocket
                                  
{
 public:
    // methods
     NPT_BsdUdpMulticastSocket();
    ~NPT_BsdUdpMulticastSocket();

    // NPT_UdpMulticastSocketInterface methods
    NPT_Result JoinGroup(const NPT_IpAddress& group,
                         const NPT_IpAddress& iface);
    NPT_Result LeaveGroup(const NPT_IpAddress& group,
                          const NPT_IpAddress& iface);
    NPT_Result SetTimeToLive(unsigned char ttl);
    NPT_Result SetInterface(const NPT_IpAddress& iface);

    // friends 
    friend class NPT_UdpMulticastSocket;
};

/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::NPT_BsdUdpMulticastSocket
+---------------------------------------------------------------------*/
NPT_BsdUdpMulticastSocket::NPT_BsdUdpMulticastSocket()
{
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::~NPT_BsdUdpMulticastSocket
+---------------------------------------------------------------------*/
NPT_BsdUdpMulticastSocket::~NPT_BsdUdpMulticastSocket()
{
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::JoinGroup
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdUdpMulticastSocket::JoinGroup(const NPT_IpAddress& group,
                                     const NPT_IpAddress& iface)
{
    struct ip_mreq mreq;

    // set the interface address
    mreq.imr_interface.s_addr = htonl(iface.AsLong());

    // set the group address
    mreq.imr_multiaddr.s_addr = htonl(group.AsLong());

    // set socket option
    int io_result = setsockopt(m_SocketFdReference->GetSocketFd(), 
                               IPPROTO_IP, IP_ADD_MEMBERSHIP, 
                               (SocketOption)&mreq, sizeof(mreq));
    if (io_result == 0) {
        return NPT_SUCCESS;
    } else {
        return MapErrorCode(GetSocketError());
    }
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::LeaveGroup
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdUdpMulticastSocket::LeaveGroup(const NPT_IpAddress& group,
                                      const NPT_IpAddress& iface)
{
    struct ip_mreq mreq;

    // set the interface address
    mreq.imr_interface.s_addr = htonl(iface.AsLong());

    // set the group address
    mreq.imr_multiaddr.s_addr = htonl(group.AsLong());

    // set socket option
    int io_result = setsockopt(m_SocketFdReference->GetSocketFd(), 
                               IPPROTO_IP, IP_DROP_MEMBERSHIP, 
                               (SocketOption)&mreq, sizeof(mreq));
    if (io_result == 0) {
        return NPT_SUCCESS;
    } else {
        return MapErrorCode(GetSocketError());
    }
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::SetInterface
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdUdpMulticastSocket::SetInterface(const NPT_IpAddress& iface)
{
    struct in_addr iface_addr;
    // set the interface address
    iface_addr.s_addr = htonl(iface.AsLong());

    // set socket option
    int io_result = setsockopt(m_SocketFdReference->GetSocketFd(), 
                               IPPROTO_IP, IP_MULTICAST_IF, 
                               (char*)&iface_addr, sizeof(iface_addr));
    if (io_result == 0) {
        return NPT_SUCCESS;
    } else {
        return MapErrorCode(GetSocketError());
    }
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::SetTimeToLive
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdUdpMulticastSocket::SetTimeToLive(unsigned char ttl)
{
    unsigned char ttl_opt = ttl;

    // set socket option
    int io_result = setsockopt(m_SocketFdReference->GetSocketFd(), 
                               IPPROTO_IP, IP_MULTICAST_TTL, 
                               (SocketOption)&ttl_opt, sizeof(ttl_opt));
    if (io_result == 0) {
        return NPT_SUCCESS;
    } else {
        return MapErrorCode(GetSocketError());
    }
}

/*----------------------------------------------------------------------
|   NPT_UdpMulticastSocket::NPT_UdpMulticastSocket
+---------------------------------------------------------------------*/
NPT_UdpMulticastSocket::NPT_UdpMulticastSocket() :
    NPT_UdpSocket(NULL)
{
    NPT_BsdUdpMulticastSocket* delegate = new NPT_BsdUdpMulticastSocket();
    m_SocketDelegate             = delegate;
    m_UdpSocketDelegate          = delegate;
    m_UdpMulticastSocketDelegate = delegate;
}

/*----------------------------------------------------------------------
|   NPT_UdpMulticastSocket::~NPT_UdpMulticastSocket
+---------------------------------------------------------------------*/
NPT_UdpMulticastSocket::~NPT_UdpMulticastSocket()
{
    delete m_UdpMulticastSocketDelegate;

    // set the delegate pointers to NULL because it is shared by the
    // base classes, and we only want to delete the object once
    m_SocketDelegate             = NULL;
    m_UdpSocketDelegate          = NULL;
    m_UdpMulticastSocketDelegate = NULL;
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpClientSocket
+---------------------------------------------------------------------*/
class NPT_BsdTcpClientSocket : protected NPT_BsdSocket
{
 public:
    // methods
     NPT_BsdTcpClientSocket();
    ~NPT_BsdTcpClientSocket();

    // NPT_SocketInterface methods
    NPT_Result Connect(const NPT_SocketAddress& address,
                       NPT_Timeout              timeout);
    NPT_Result WaitForConnection(NPT_Timeout timeout);

protected:
    // methods
    NPT_Result DoWaitForConnection(NPT_Timeout timeout);

    // friends
    friend class NPT_TcpClientSocket;
};

/*----------------------------------------------------------------------
|   NPT_BsdTcpClientSocket::NPT_BsdTcpClientSocket
+---------------------------------------------------------------------*/
NPT_BsdTcpClientSocket::NPT_BsdTcpClientSocket() : 
    NPT_BsdSocket(socket(AF_INET, SOCK_STREAM, 0))
{
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpClientSocket::~NPT_BsdTcpClientSocket
+---------------------------------------------------------------------*/
NPT_BsdTcpClientSocket::~NPT_BsdTcpClientSocket()
{
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpClientSocket::Connect
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdTcpClientSocket::Connect(const NPT_SocketAddress& address, 
                                NPT_Timeout              timeout)
{
    SocketFd socket_fd = m_SocketFdReference->GetSocketFd();
    bool was_blocking = m_Blocking;

    // set the socket to nonblocking so that we can timeout on connect
    if (m_Blocking) {
        if (NPT_FAILED(m_SocketFdReference->SetBlockingMode(false))) {
            return NPT_FAILURE;
        }
    }

    // convert the address
    struct sockaddr_in inet_address;
    SocketAddressToInetAddress(address, &inet_address);

    // initiate connection
    int io_result;
    io_result = connect(socket_fd, 
                        (struct sockaddr *)&inet_address, 
                        sizeof(inet_address));
    if (io_result == 0) {
        // immediate connection

        // put the fd back in its original blocking mode
        if (was_blocking) m_SocketFdReference->SetBlockingMode(true);

        // get socket info
        RefreshInfo();

        return NPT_SUCCESS;
    }
    if (io_result == NPT_BSD_SOCKET_ERROR && 
        GetSocketError() != EINPROGRESS && 
        GetSocketError() != EWOULDBLOCK &&
        GetSocketError() != EAGAIN) {   
        // put the fd back in its original blocking mode
        if (was_blocking) m_SocketFdReference->SetBlockingMode(true);

        // error
        return MapErrorCode(GetSocketError());
    }

    return DoWaitForConnection(timeout);
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpClientSocket::WaitForConnection
+---------------------------------------------------------------------*/
NPT_Result 
NPT_BsdTcpClientSocket::WaitForConnection(NPT_Timeout timeout)
{
    // this function can only be called directly for non-blocking sockets
    if (m_Blocking) return NPT_ERROR_INVALID_STATE;

    return DoWaitForConnection(timeout);
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpClientSocket::DoWaitForConnection
+---------------------------------------------------------------------*/
NPT_Result 
NPT_BsdTcpClientSocket::DoWaitForConnection(NPT_Timeout timeout)
{
    SocketFd   socket_fd = m_SocketFdReference->GetSocketFd();
    NPT_Result result = NPT_SUCCESS;

    // wait for connection to succeed or fail
    fd_set read_set;
    fd_set write_set;
    fd_set except_set;
    FD_ZERO(&read_set);
    FD_SET(socket_fd, &read_set);
    FD_ZERO(&write_set);
    FD_SET(socket_fd, &write_set);
    FD_ZERO(&except_set);
    FD_SET(socket_fd, &except_set);

    struct timeval timeout_value;
    if (timeout != NPT_TIMEOUT_INFINITE) {
        timeout_value.tv_sec = timeout/1000;
        timeout_value.tv_usec = 1000*(timeout-1000*(timeout/1000));
    };

    int io_result = select((int)socket_fd+1, 
                           &read_set, &write_set, &except_set, 
                           timeout == NPT_TIMEOUT_INFINITE ? 
                           NULL : &timeout_value);

    if (io_result == 0) {
        if (timeout == 0) {
            // non-blocking call
            result = NPT_ERROR_WOULD_BLOCK;
            goto done;
        } else {
            // timeout
            result = NPT_ERROR_TIMEOUT;
            goto done;
        }
    } else if (io_result == NPT_BSD_SOCKET_ERROR) {
        result = MapErrorCode(GetSocketError());
        goto done;
    } else if (FD_ISSET(socket_fd, &read_set)    || 
               FD_ISSET(socket_fd, &write_set)   ||
               FD_ISSET(socket_fd, &except_set)) {
        int error = 0;
        socklen_t length = sizeof(error);
        // get error status from socket
        // (some systems return the error in errno, others
        //  return it in the buffer passed to getsockopt)
        io_result = getsockopt(socket_fd, 
                               SOL_SOCKET, 
                               SO_ERROR, 
                               (SocketOption)&error, 
                               &length);
        if (io_result == NPT_BSD_SOCKET_ERROR) {
            result = MapErrorCode(GetSocketError());
            goto done;
        } else if (error) {
            result = MapErrorCode(error);
            goto done;
        }
    }
    
    // get socket info
    RefreshInfo();

done:
    // put the fd back in its original blocking mode
    if (m_Blocking) m_SocketFdReference->SetBlockingMode(true);

    return result;
}

/*----------------------------------------------------------------------
|   NPT_TcpClientSocket::NPT_TcpClientSocket
+---------------------------------------------------------------------*/
NPT_TcpClientSocket::NPT_TcpClientSocket() :
    NPT_Socket(new NPT_BsdTcpClientSocket())
{
}

/*----------------------------------------------------------------------
|   NPT_TcpClientSocket::NPT_TcpClientSocket
+---------------------------------------------------------------------*/
NPT_TcpClientSocket::~NPT_TcpClientSocket()
{
    delete m_SocketDelegate;

    // set the delegate pointer to NULL because it is shared by the
    // base classes, and we only want to delete the object once
    m_SocketDelegate = NULL;
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpServerSocket
+---------------------------------------------------------------------*/
class NPT_BsdTcpServerSocket : public    NPT_TcpServerSocketInterface,
                               protected NPT_BsdSocket
                               
{
 public:
    // methods
     NPT_BsdTcpServerSocket();
    ~NPT_BsdTcpServerSocket();

    // NPT_SocketInterface methods
    NPT_Result Bind(const NPT_SocketAddress& address, bool reuse_address = true) {
        // inherit
        return NPT_BsdSocket::Bind(address, reuse_address);
    }
    NPT_Result Connect(const NPT_SocketAddress& address,
                       NPT_Timeout              timeout) {
        // inherit
        return NPT_BsdSocket::Connect(address, timeout);
    }
    NPT_Result GetInputStream(NPT_InputStreamReference& stream) {
        // no stream
        stream = NULL;
        return NPT_FAILURE;
    }
    NPT_Result GetOutputStream(NPT_OutputStreamReference& stream) {
        // no stream
        stream = NULL;
        return NPT_FAILURE;
    }
    NPT_Result GetInfo(NPT_SocketInfo& info) {
        // inherit
        return NPT_BsdSocket::GetInfo(info);
    }

    // NPT_TcpServerSocket methods
    NPT_Result Listen(unsigned int max_clients);
    NPT_Result WaitForNewClient(NPT_Socket*& client, NPT_Timeout timeout);

protected:
    // members
    unsigned int m_ListenMax;

    // friends
    friend class NPT_TcpServerSocket;
};

/*----------------------------------------------------------------------
|   NPT_BsdTcpServerSocket::NPT_BsdTcpServerSocket
+---------------------------------------------------------------------*/
NPT_BsdTcpServerSocket::NPT_BsdTcpServerSocket() : 
    NPT_BsdSocket(socket(AF_INET, SOCK_STREAM, 0)),
    m_ListenMax(0)
{
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpServerSocket::~NPT_BsdTcpServerSocket
+---------------------------------------------------------------------*/
NPT_BsdTcpServerSocket::~NPT_BsdTcpServerSocket()
{
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpServerSocket::Listen
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdTcpServerSocket::Listen(unsigned int max_clients)
{
    // listen for connections
    if (listen(m_SocketFdReference->GetSocketFd(), max_clients) < 0) {
        m_ListenMax = 0;
        return NPT_ERROR_LISTEN_FAILED;
    }   
    m_ListenMax = max_clients;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpServerSocket::WaitForNewClient
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdTcpServerSocket::WaitForNewClient(NPT_Socket*& client, 
                                         NPT_Timeout  /*timeout*/)
{
    // check that we are listening for clients
    if (m_ListenMax == 0) {
        Listen(NPT_TCP_SERVER_SOCKET_DEFAULT_LISTEN_COUNT);
    }

    // wait for incoming connection
    struct sockaddr_in inet_address;
    socklen_t namelen = sizeof(inet_address);
    SocketFd  socket_fd;
    socket_fd = accept(m_SocketFdReference->GetSocketFd(), 
                       (struct sockaddr*)&inet_address, 
                       &namelen); 
    if (socket_fd == NPT_BSD_INVALID_SOCKET) {
        client = NULL;
        return MapErrorCode(GetSocketError());
    }
    client = new NPT_Socket(new NPT_BsdSocket(socket_fd));

    // done
    return NPT_SUCCESS;    
}

/*----------------------------------------------------------------------
|   NPT_TcpServerSocket::NPT_TcpServerSocket
+---------------------------------------------------------------------*/
NPT_TcpServerSocket::NPT_TcpServerSocket()
{
    NPT_BsdTcpServerSocket* delegate = new NPT_BsdTcpServerSocket();
    m_SocketDelegate          = delegate;
    m_TcpServerSocketDelegate = delegate;
}

/*----------------------------------------------------------------------
|   NPT_TcpServerSocket::NPT_TcpServerSocket
+---------------------------------------------------------------------*/
NPT_TcpServerSocket::~NPT_TcpServerSocket()
{
    delete m_TcpServerSocketDelegate;

    // set the delegate pointers to NULL because it is shared by the
    // base classes, and we only want to delete the object once
    m_SocketDelegate          = NULL;
    m_TcpServerSocketDelegate = NULL;
}

/*****************************************************************
|
|   Neptune - Sockets :: Symbian OS Implementation
|
|   (c) 2001-2007 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "NptConfig.h"
#include "NptTypes.h"
#include "NptStreams.h"
#include "NptThreads.h"
#include "NptSockets.h"
#include "NptUtils.h"
#include "NptConstants.h"
#include "NptLogging.h"

#include <es_sock.h>
#include <in_sock.h>
#include <e32err.h>

/*----------------------------------------------------------------------
|   logging
+---------------------------------------------------------------------*/
NPT_SET_LOCAL_LOGGER("neptune.system.symbian.sockets")

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
const int NPT_TCP_SERVER_SOCKET_DEFAULT_LISTEN_COUNT = 20;

/*----------------------------------------------------------------------
|   SymbianNetwork
+---------------------------------------------------------------------*/
class SymbianNetwork {
public:
	// class members
	static SymbianNetwork Instance;

    // members
    RSocketServ m_SocketServer;
	
private:
	// methods
	SymbianNetwork();
	~SymbianNetwork();
};

/*----------------------------------------------------------------------
|   SymbianNetwork
+---------------------------------------------------------------------*/
SymbianNetwork::SymbianNetwork()
{
	TInt result = m_SocketServer.Connect();
}

/*----------------------------------------------------------------------
|   SymbianNetwork::SymbianNetwork
+---------------------------------------------------------------------*/
SymbianNetwork::~SymbianNetwork()
{
	m_SocketServer.Close();
}

/*----------------------------------------------------------------------
|   globals
+---------------------------------------------------------------------*/
SymbianNetwork SymbianNetwork::Instance;

/*----------------------------------------------------------------------
|   MapError
+---------------------------------------------------------------------*/
static NPT_Result
MapError(TInt err)
{
	switch (err) {
		case KErrNone:
	        return NPT_SUCCESS;
	        
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
        // the name is a numerical IP addr
        return Set(numerical_address.AsLong());
    }

    // create a resolver
    RHostResolver resolver;
	TInt result = resolver.Open(SymbianNetwork::Instance.m_SocketServer, KAfInet, KProtocolInetUdp);
    if (result != KErrNone) return MapError(result);

    // resolve the name
    TNameEntry entry;
    NPT_Size   name_length = NPT_StringLength(name);
    TUint16* name16 = new TUint16[name_length];
    for (unsigned int i=0; i<name_length; i++) {
    	name16[i] = name[i];
    }
    TPtr name_desc(name16, name_length);
	result = resolver.GetByName(name_desc, entry);
	
	// convert the result to a 32-bit address
	NPT_Result npt_result = NPT_FAILURE;
	if (result == KErrNone) {
		TNameRecord name_rec = entry();
		Set(TInetAddr::Cast(name_rec.iAddr).Address());
		npt_result = NPT_SUCCESS;
	}
	
	// cleanup
	delete[] name16;
    resolver.Close();
    
    return npt_result;
}

/*----------------------------------------------------------------------
|   NPT_SymbianSocket
+---------------------------------------------------------------------*/
class NPT_SymbianSocket : public NPT_SocketInterface
{
 public:
    // constructors and destructor
             NPT_SymbianSocket() {}
    virtual ~NPT_SymbianSocket();

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
    NPT_Result SetReadTimeout(NPT_Timeout timeout);
    NPT_Result SetWriteTimeout(NPT_Timeout timeout);

 protected:
    // members
    NPT_SocketInfo m_Info;
    bool           m_Blocking;
};

/*----------------------------------------------------------------------
|   NPT_SymbianSocket::~NPT_SymbianSocket
+---------------------------------------------------------------------*/
NPT_SymbianSocket::~NPT_SymbianSocket()
{
}

/*----------------------------------------------------------------------
|   NPT_SymbianSocket::Bind
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianSocket::Bind(const NPT_SocketAddress& /* address */, bool /* reuse_address */)
{
    // refresh socket info
    RefreshInfo();

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbianSocket::Connect
+---------------------------------------------------------------------*/
NPT_Result 
NPT_SymbianSocket::Connect(const NPT_SocketAddress&, NPT_Timeout)
{
    // this is unsupported unless overridden in a derived class
    return NPT_ERROR_NOT_SUPPORTED;
}

/*----------------------------------------------------------------------
|   NPT_SymbianSocket::WaitForConnection
+---------------------------------------------------------------------*/
NPT_Result 
NPT_SymbianSocket::WaitForConnection(NPT_Timeout)
{
    // this is unsupported unless overridden in a derived class
    return NPT_ERROR_NOT_SUPPORTED;
}

/*----------------------------------------------------------------------
|   NPT_SymbianSocket::GetInputStream
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianSocket::GetInputStream(NPT_InputStreamReference& stream)
{
    // default value
    stream = NULL;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbianSocket::GetOutputStream
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianSocket::GetOutputStream(NPT_OutputStreamReference& stream)
{
    // default value
    stream = NULL;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbianSocket::GetInfo
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianSocket::GetInfo(NPT_SocketInfo& info)
{
    // return the cached info
    info = m_Info;
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbianSocket::RefreshInfo
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianSocket::RefreshInfo()
{
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbianSocket::SetBlockingMode
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianSocket::SetBlockingMode(bool blocking)
{
    if (m_Blocking != blocking) {
        m_Blocking = blocking;
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbianSocket::SetReadTimeout
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianSocket::SetReadTimeout(NPT_Timeout /* timeout */)
{
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbianSocket::SetWriteTimeout
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianSocket::SetWriteTimeout(NPT_Timeout /* timeout */)
{
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
|   NPT_SymbianUdpSocket
+---------------------------------------------------------------------*/
class NPT_SymbianUdpSocket : public    NPT_UdpSocketInterface,
                             protected NPT_SymbianSocket
                         
{
 public:
    // constructor
     NPT_SymbianUdpSocket();
    virtual ~NPT_SymbianUdpSocket() {}

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
|   NPT_SymbianUdpSocket::NPT_SymbianUdpSocket
+---------------------------------------------------------------------*/
NPT_SymbianUdpSocket::NPT_SymbianUdpSocket()
{
}

/*----------------------------------------------------------------------
|   NPT_SymbianUdpSocket::Connect
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianUdpSocket::Connect(const NPT_SocketAddress& /* address */, 
                              NPT_Timeout              /* ignored */)
{
    // refresh socket info
    RefreshInfo();

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbianUdpSocket::Send
+---------------------------------------------------------------------*/
NPT_Result 
NPT_SymbianUdpSocket::Send(const NPT_DataBuffer&    /* packet  */, 
                           const NPT_SocketAddress* /* address */) 
{
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbianUdpSocket::Receive
+---------------------------------------------------------------------*/
NPT_Result 
NPT_SymbianUdpSocket::Receive(NPT_DataBuffer&    packet, 
                              NPT_SocketAddress* address)
{
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_UdpSocket::NPT_UdpSocket
+---------------------------------------------------------------------*/
NPT_UdpSocket::NPT_UdpSocket()
{
    NPT_SymbianUdpSocket* delegate = new NPT_SymbianUdpSocket();
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
|   NPT_SymbianUdpMulticastSocket
+---------------------------------------------------------------------*/
class NPT_SymbianUdpMulticastSocket : public    NPT_UdpMulticastSocketInterface,
                                      protected NPT_SymbianUdpSocket
                                  
{
 public:
    // methods
     NPT_SymbianUdpMulticastSocket();
    ~NPT_SymbianUdpMulticastSocket();

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
|   NPT_SymbianUdpMulticastSocket::NPT_SymbianUdpMulticastSocket
+---------------------------------------------------------------------*/
NPT_SymbianUdpMulticastSocket::NPT_SymbianUdpMulticastSocket()
{
}

/*----------------------------------------------------------------------
|   NPT_SymbianUdpMulticastSocket::~NPT_SymbianUdpMulticastSocket
+---------------------------------------------------------------------*/
NPT_SymbianUdpMulticastSocket::~NPT_SymbianUdpMulticastSocket()
{
}

/*----------------------------------------------------------------------
|   NPT_SymbianUdpMulticastSocket::JoinGroup
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianUdpMulticastSocket::JoinGroup(const NPT_IpAddress& group,
                                         const NPT_IpAddress& iface)
{
	return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbianUdpMulticastSocket::LeaveGroup
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianUdpMulticastSocket::LeaveGroup(const NPT_IpAddress& group,
                                          const NPT_IpAddress& iface)
{
	return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbianUdpMulticastSocket::SetInterface
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianUdpMulticastSocket::SetInterface(const NPT_IpAddress& iface)
{
	return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbianUdpMulticastSocket::SetTimeToLive
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianUdpMulticastSocket::SetTimeToLive(unsigned char ttl)
{
	return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_UdpMulticastSocket::NPT_UdpMulticastSocket
+---------------------------------------------------------------------*/
NPT_UdpMulticastSocket::NPT_UdpMulticastSocket() :
    NPT_UdpSocket(NULL)
{
    NPT_SymbianUdpMulticastSocket* delegate = new NPT_SymbianUdpMulticastSocket();
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
|   NPT_SymbianTcpClientSocket
+---------------------------------------------------------------------*/
class NPT_SymbianTcpClientSocket : protected NPT_SymbianSocket
{
 public:
    // methods
     NPT_SymbianTcpClientSocket();
    ~NPT_SymbianTcpClientSocket();

    // NPT_SocketInterface methods
    NPT_Result Connect(const NPT_SocketAddress& address,
                       NPT_Timeout              timeout);
    NPT_Result WaitForConnection(NPT_Timeout timeout);

protected:
    // friends
    friend class NPT_TcpClientSocket;
};

/*----------------------------------------------------------------------
|   NPT_SymbianTcpClientSocket::NPT_SymbianTcpClientSocket
+---------------------------------------------------------------------*/
NPT_SymbianTcpClientSocket::NPT_SymbianTcpClientSocket()
{
}

/*----------------------------------------------------------------------
|   NPT_SymbianTcpClientSocket::~NPT_SymbianTcpClientSocket
+---------------------------------------------------------------------*/
NPT_SymbianTcpClientSocket::~NPT_SymbianTcpClientSocket()
{
}

/*----------------------------------------------------------------------
|   NPT_SymbianTcpClientSocket::Connect
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianTcpClientSocket::Connect(const NPT_SocketAddress& address, 
                                    NPT_Timeout              timeout)
{
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbianTcpClientSocket::WaitForConnection
+---------------------------------------------------------------------*/
NPT_Result 
NPT_SymbianTcpClientSocket::WaitForConnection(NPT_Timeout /* timeout */)
{
    // this function can only be called directly for non-blocking sockets
    if (m_Blocking) return NPT_ERROR_INVALID_STATE;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_TcpClientSocket::NPT_TcpClientSocket
+---------------------------------------------------------------------*/
NPT_TcpClientSocket::NPT_TcpClientSocket() :
    NPT_Socket(new NPT_SymbianTcpClientSocket())
{
}

/*----------------------------------------------------------------------
|   NPT_TcpClientSocket::~NPT_TcpClientSocket
+---------------------------------------------------------------------*/
NPT_TcpClientSocket::~NPT_TcpClientSocket()
{
    delete m_SocketDelegate;

    // set the delegate pointer to NULL because it is shared by the
    // base classes, and we only want to delete the object once
    m_SocketDelegate = NULL;
}

/*----------------------------------------------------------------------
|   NPT_SymbianTcpServerSocket
+---------------------------------------------------------------------*/
class NPT_SymbianTcpServerSocket : public    NPT_TcpServerSocketInterface,
                                   protected NPT_SymbianSocket
                               
{
 public:
    // methods
     NPT_SymbianTcpServerSocket();
    ~NPT_SymbianTcpServerSocket();

    // NPT_SocketInterface methods
    NPT_Result Bind(const NPT_SocketAddress& address, bool reuse_address = true) {
        // inherit
        return NPT_SymbianSocket::Bind(address, reuse_address);
    }
    NPT_Result Connect(const NPT_SocketAddress& address,
                       NPT_Timeout              timeout) {
        // inherit
        return NPT_SymbianSocket::Connect(address, timeout);
    }
    NPT_Result GetInputStream(NPT_InputStreamReference& stream) {
        // no stream
        stream = NULL;
        return NPT_ERROR_NOT_SUPPORTED;
    }
    NPT_Result GetOutputStream(NPT_OutputStreamReference& stream) {
        // no stream
        stream = NULL;
        return NPT_ERROR_NOT_SUPPORTED;
    }
    NPT_Result GetInfo(NPT_SocketInfo& info) {
        // inherit
        return NPT_SymbianSocket::GetInfo(info);
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
|   NPT_SymbianTcpServerSocket::NPT_SymbianTcpServerSocket
+---------------------------------------------------------------------*/
NPT_SymbianTcpServerSocket::NPT_SymbianTcpServerSocket() :
    m_ListenMax(0)
{
}

/*----------------------------------------------------------------------
|   NPT_SymbianTcpServerSocket::~NPT_SymbianTcpServerSocket
+---------------------------------------------------------------------*/
NPT_SymbianTcpServerSocket::~NPT_SymbianTcpServerSocket()
{
}

/*----------------------------------------------------------------------
|   NPT_SymbianTcpServerSocket::Listen
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianTcpServerSocket::Listen(unsigned int max_clients)
{
    m_ListenMax = max_clients;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_SymbianTcpServerSocket::WaitForNewClient
+---------------------------------------------------------------------*/
NPT_Result
NPT_SymbianTcpServerSocket::WaitForNewClient(NPT_Socket*& client, 
                                         NPT_Timeout  timeout)
{
    // default value
    client = NULL;

    // check that we are listening for clients
    if (m_ListenMax == 0) {
        Listen(NPT_TCP_SERVER_SOCKET_DEFAULT_LISTEN_COUNT);
    }

    // done
    return NPT_SUCCESS;    
}

/*----------------------------------------------------------------------
|   NPT_TcpServerSocket::NPT_TcpServerSocket
+---------------------------------------------------------------------*/
NPT_TcpServerSocket::NPT_TcpServerSocket()
{
    NPT_SymbianTcpServerSocket* delegate = new NPT_SymbianTcpServerSocket();
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

/*****************************************************************
|
|      Sockets Test Program 1
|
|      (c) 2001-2010 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "NptDebug.h"

#if defined(WIN32) && defined(_DEBUG)
#include <crtdbg.h>
#endif

/*----------------------------------------------------------------------
|   TcpClientReadThread
+---------------------------------------------------------------------*/
class TcpClientReadThread : public NPT_Thread
{
public:
    TcpClientReadThread(NPT_Timeout timeout) : m_Timeout(timeout) {}
    
    void Run() {
        NPT_Console::Output("{00} connecting to www.google.com...\n");
        NPT_IpAddress     ip_address;
        NPT_Result result = ip_address.ResolveName("www.google.com");
        if (NPT_FAILED(result)) {
            NPT_Console::OutputF("failed to resolve name (%d) (%s)\n", result, NPT_ResultText(result));
            m_Ready.SetValue(1);
            return;
        }
        NPT_SocketAddress address(ip_address, 80);
        m_Socket.SetReadTimeout(m_Timeout);
        result = m_Socket.Connect(address);
        if (NPT_FAILED(result)) {
            NPT_Console::OutputF("connect returned (%d) (%s)\n", result, NPT_ResultText(result));
            m_Ready.SetValue(1);
            return;
        }
        NPT_Console::Output("{00} connected\n");
        m_Ready.SetValue(1);
        
        NPT_InputStreamReference input;
        m_Socket.GetInputStream(input);
        unsigned char buffer[4096];
        result = input->Read(buffer, 4096);
        NPT_Console::OutputF("{00} read returned %d (%s)\n", result, NPT_ResultText(result));
        NPT_Console::Output("{00} tcp client read thread done\n");
    }

    NPT_TcpClientSocket m_Socket;
    NPT_SharedVariable  m_Ready;
    NPT_Timeout         m_Timeout;
};

/*----------------------------------------------------------------------
|   TcpClientWriteThread
+---------------------------------------------------------------------*/
class TcpClientWriteThread : public NPT_Thread
{
public:
    TcpClientWriteThread(NPT_Timeout timeout) : m_Timeout(timeout) {}
    
    void Run() {
        NPT_Console::Output("{01} connecting to localhost...\n");
        NPT_SocketAddress address(NPT_IpAddress(127,0,0,1), 10000);
        m_Socket.SetWriteTimeout(m_Timeout);
        NPT_Result result = m_Socket.Connect(address);
        m_Ready.SetValue(1);
        if (NPT_FAILED(result)) {
            NPT_Console::OutputF("connect failed (%d) (%s)\n", result, NPT_ResultText(result));
            return;
        }
        NPT_Console::Output("{01} connected\n");
        
        NPT_OutputStreamReference output;
        m_Socket.GetOutputStream(output);
        unsigned char buffer[4096];
        NPT_SetMemory(buffer, 0, sizeof(buffer));
        NPT_Size total_written = 0;
        do {
            NPT_Size bytes_written = 0;
            result = output->Write(buffer, 4096, &bytes_written);
            if (NPT_SUCCEEDED(result)) {
                total_written += bytes_written;
            }
        } while (NPT_SUCCEEDED(result));
        output = NULL;
        NPT_Console::OutputF("{01} write returned %d (%s)\n", result, NPT_ResultText(result));
        NPT_Console::OutputF("{01} wrote %d bytes total\n", total_written);
        NPT_Console::Output("{01} tcp client write thread done\n");
    }

    NPT_TcpClientSocket m_Socket;
    NPT_SharedVariable  m_Ready;
    NPT_Timeout         m_Timeout;
};

/*----------------------------------------------------------------------
|   TcpServerThread
+---------------------------------------------------------------------*/
class TcpServerThread : public NPT_Thread
{
public:
    TcpServerThread() : m_Interrupted(false) {}
    
    void Run() {
        NPT_Console::Output("{02} waiting for connection on port 10000\n");
        NPT_SocketAddress address(NPT_IpAddress::Any, 10000);
        NPT_Result result = m_Socket.Bind(address, true);
        m_Ready.SetValue(1);
        if (NPT_FAILED(result)) {
            NPT_Console::OutputF("bind failed (%d) (%s)\n", result, NPT_ResultText(result));
            return;
        }
        NPT_Socket* client = NULL;
        result = m_Socket.WaitForNewClient(client);
        NPT_Console::Output("{02} client connected\n");
        for (;;) {
            NPT_System::Sleep(1.0);
            if (m_Interrupted) {
                NPT_Console::Output("{02} thread interrupted\n");
                break;
            }
        }
        delete client;
        NPT_Console::Output("{02} tcp server thread done\n");
    }

    NPT_TcpServerSocket m_Socket;
    NPT_SharedVariable  m_Ready;
    bool                m_Interrupted;
};

/*----------------------------------------------------------------------
|   TcpAcceptThread
+---------------------------------------------------------------------*/
class TcpAcceptThread : public NPT_Thread
{
public:
    TcpAcceptThread(NPT_Timeout timeout) : m_Timeout(timeout) {}
    
    void Run() {
        NPT_Console::Output("{03} waiting for connection on port 10000\n");
        NPT_SocketAddress address(NPT_IpAddress::Any, 10000);
        NPT_Result result = m_Socket.Bind(address, true);
        if (NPT_FAILED(result)) {
            NPT_Console::OutputF("bind failed (%d) (%s)\n", result, NPT_ResultText(result));
            return;
        }
        m_Ready.SetValue(1);
        m_Socket.SetReadTimeout(m_Timeout);
        m_Socket.SetWriteTimeout(m_Timeout);
        NPT_Socket* client = NULL;
        result = m_Socket.WaitForNewClient(client, m_Timeout);
        NPT_Console::OutputF("{03} WaitForNewClient returned %d (%s)\n", result, NPT_ResultText(result));
        NPT_Console::Output("{03} accept thread done\n");
    }

    NPT_TcpServerSocket m_Socket;
    NPT_SharedVariable  m_Ready;
    NPT_Timeout         m_Timeout;
};

/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int
main(int /*argc*/, char** /*argv*/)
{
    // setup debugging
#if defined(WIN32) && defined(_DEBUG)
    int flags = _crtDbgFlag       | 
        _CRTDBG_ALLOC_MEM_DF      |
        _CRTDBG_DELAY_FREE_MEM_DF |
        _CRTDBG_CHECK_ALWAYS_DF;

    _CrtSetDbgFlag(flags);
    //AllocConsole();
    //freopen("CONOUT$", "w", stdout);
#endif 

    NPT_Console::Output("--- read with no timeout\n");
    NPT_Console::Output("[00] starting read client thread\n");
    TcpClientReadThread* read_client = new TcpClientReadThread(NPT_TIMEOUT_INFINITE);
    read_client->Start();
    NPT_Console::Output("[00] waiting for client to be ready...\n");
    read_client->m_Ready.WaitUntilEquals(1);
    NPT_Console::Output("[00] client thread ready\n");
    NPT_Console::Output("[00] waiting a while...\n");
    NPT_System::Sleep(3.0);
    NPT_Console::Output("[00] interrupting client socket\n");
    read_client->m_Socket.Cancel();
    NPT_Console::Output("[00] waiting for client thread to finish\n");
    read_client->Wait();
    NPT_Console::Output("[00] client thread finished\n");
    delete read_client;

    NPT_Console::Output("\n--- read with 10s timeout\n");
    NPT_Console::Output("[00] starting read client thread\n");
    read_client = new TcpClientReadThread(10000);
    read_client->Start();
    NPT_Console::Output("[00] waiting for client to be ready...\n");
    read_client->m_Ready.WaitUntilEquals(1);
    NPT_Console::Output("[00] client thread ready\n");
    NPT_Console::Output("[00] waiting a while...\n");
    NPT_System::Sleep(3.0);
    NPT_Console::Output("[00] interrupting client socket\n");
    read_client->m_Socket.Cancel();
    NPT_Console::Output("[00] waiting for client thread to finish\n");
    read_client->Wait();
    NPT_Console::Output("[00] client thread finished\n");
    delete read_client;

    NPT_Console::Output("\n--- write with no timeout\n");
    NPT_Console::Output("[01] starting write server thread\n");
    TcpServerThread* server = new TcpServerThread();
    server->Start();
    NPT_Console::Output("[01] waiting for server to be ready...\n");
    server->m_Ready.WaitUntilEquals(1);
    NPT_Console::Output("[01] server thread ready\n");
    NPT_Console::Output("[01] waiting a while...\n");
    NPT_System::Sleep(3.0);
    NPT_Console::Output("[01] starting write client thread\n");
    TcpClientWriteThread* write_client = new TcpClientWriteThread(NPT_TIMEOUT_INFINITE);
    write_client->Start();
    NPT_Console::Output("[01] waiting for client to be ready...\n");
    write_client->m_Ready.WaitUntilEquals(1);
    NPT_Console::Output("[01] client thread ready\n");
    NPT_Console::Output("[01] waiting a while...\n");
    NPT_System::Sleep(3.0);
    NPT_Console::Output("[01] interrupting client socket\n");
    write_client->m_Socket.Cancel();
    NPT_Console::Output("[01] waiting for client thread to finish\n");
    write_client->Wait();
    NPT_Console::Output("[01] client thread finished\n");
    delete write_client;
    NPT_Console::Output("[01] terminating server\n");
    server->m_Interrupted = true;
    server->Wait();
    delete server;

    NPT_Console::Output("\n--- write with 10s timeout\n");
    NPT_Console::Output("[01] starting write server thread\n");
    server = new TcpServerThread();
    server->Start();
    NPT_Console::Output("[01] waiting for server to be ready...\n");
    server->m_Ready.WaitUntilEquals(1);
    NPT_Console::Output("[01] server thread ready\n");
    NPT_Console::Output("[01] waiting a while...\n");
    NPT_System::Sleep(3.0);
    NPT_Console::Output("[01] starting write client thread\n");
    write_client = new TcpClientWriteThread(10000);
    write_client->Start();
    NPT_Console::Output("[01] waiting for client to be ready...\n");
    write_client->m_Ready.WaitUntilEquals(1);
    NPT_Console::Output("[01] client thread ready\n");
    NPT_Console::Output("[01] waiting a while...\n");
    NPT_System::Sleep(3.0);
    NPT_Console::Output("[01] interrupting client socket\n");
    write_client->m_Socket.Cancel();
    NPT_Console::Output("[01] waiting for client thread to finish\n");
    write_client->Wait();
    NPT_Console::Output("[01] client thread finished\n");
    delete write_client;
    NPT_Console::Output("[01] terminating server\n");
    server->m_Interrupted = true;
    server->Wait();
    delete server;

    NPT_Console::Output("\n--- accept with no timeout\n");
    NPT_Console::Output("[02] starting accept server thread\n");
    TcpAcceptThread* accept_server = new TcpAcceptThread(NPT_TIMEOUT_INFINITE);
    accept_server->Start();
    NPT_Console::Output("[02] waiting for server to be ready...\n");
    accept_server->m_Ready.WaitUntilEquals(1);
    NPT_Console::Output("[02] server thread ready\n");
    NPT_Console::Output("[02] waiting a while...\n");
    NPT_System::Sleep(3.0);
    NPT_Console::Output("[02] interrupting server socket\n");
    accept_server->m_Socket.Cancel();
    NPT_Console::Output("[02] waiting for server thread to finish\n");
    accept_server->Wait();
    NPT_Console::Output("[02] server thread finished\n");
    delete accept_server;
    
    NPT_Console::Output("\n--- accept with 10s timeout\n");
    NPT_Console::Output("[02] starting accept server thread\n");
    accept_server = new TcpAcceptThread(10000);
    accept_server->Start();
    NPT_Console::Output("[02] waiting for server to be ready...\n");
    accept_server->m_Ready.WaitUntilEquals(1);
    NPT_Console::Output("[02] server thread ready\n");
    NPT_Console::Output("[02] waiting a while...\n");
    NPT_System::Sleep(3.0);
    NPT_Console::Output("[02] interrupting server socket\n");
    accept_server->m_Socket.Cancel();
    NPT_Console::Output("[02] waiting for server thread to finish\n");
    accept_server->Wait();
    NPT_Console::Output("[02] server thread finished\n");
    delete accept_server;

    NPT_Console::Output("------------\n");
    NPT_Console::Output("bye bye\n");
    
#if defined(WIN32) && defined(_DEBUG)
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}


#include "connection.hpp"

Connection::Connection()
: m_socket(INVALID_SOCKET),
  m_inSession(false),
  m_lastRecvTime(0),
  m_packetQueue(*this)
{
    memset(&m_localAddr, 0, sizeof(Address));
    memset(&m_remoteAddr, 0, sizeof(Address));
}

Connection::~Connection()
{
    close();
}

void Connection::init()
{
    struct addrinfo hints;
    struct addrinfo* remote;
    
    // Look up the login server IP
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_DGRAM;

    if (::getaddrinfo(REMOTE_HOST, REMOTE_PORT, &hints, &remote))
        throw 1;
    
    m_remoteAddr = *(Address*)remote->ai_addr;

    ::freeaddrinfo(remote);

    // Create our UDP socket
    m_socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket == INVALID_SOCKET)
        throw 2;
    
    Address addr;

    memset(&addr, 0, sizeof(Address));
    addr.sin_family         = AF_INET;
    addr.sin_port           = toNetworkShort(MIDDLEMAN_PORT);
    addr.sin_addr.s_addr    = toNetworkLong(INADDR_ANY);

    if (::bind(m_socket, (struct sockaddr*)&addr, sizeof(Address)))
        throw 3;
}

void Connection::close()
{
    if (m_socket != INVALID_SOCKET)
    {
        ::closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
}

void Connection::read()
{
    Address addr;
    socklen_t addrLen = sizeof(Address);
    
    int len = ::recvfrom(m_socket, (char*)m_buffer, BUFFER_SIZE, 0, (struct sockaddr*)&addr, &addrLen);
    
    if (len < 2)
    {
        if (len == -1)
        {
#ifdef _WIN32
            if (WSAGetLastError() != WSAECONNRESET)
#else
            if (errno != EWOULDBLOCK && errno != EAGAIN && errno != ESHUTDOWN)
#endif
                throw 4;
        }
        return;
    }
    
    time_t recvTime = ::time(NULL);
    
    // Is this packet from the remote login server?
    if (addr.sin_addr.s_addr == m_remoteAddr.sin_addr.s_addr && addr.sin_port == m_remoteAddr.sin_port)
    {
        m_packetQueue.add(m_buffer, len);
    }
    else
    {
        // If this isn't from the login server and we weren't in a session, record the address we received from
        if (!m_inSession || (recvTime - m_lastRecvTime) > SESSION_TIMEOUT_SECONDS)
            reset(addr);
        
        uint16_t opcode = toHostShort(*(uint16_t*)m_buffer);
        
        if (opcode == OP_SessionDisconnect)
            endSession();
        
        // Forward all local packets to the remote server
        ::sendto(m_socket, (char*)m_buffer, len, 0, (struct sockaddr*)&m_remoteAddr, sizeof(Address));
    }

    m_lastRecvTime = recvTime;
}

void Connection::sendToLocal(const void* data, int len)
{
    int sent = ::sendto(m_socket, (char*)data, len, 0, (struct sockaddr*)&m_localAddr, sizeof(Address));
    
    if (sent == -1)
        throw 6;
}

void Connection::reset(const Address& localAddr)
{
    m_localAddr = localAddr;
    m_inSession = true;
    m_packetQueue.reset();
}

void Connection::endSession()
{
    m_inSession = false;
    m_packetQueue.reset();
}

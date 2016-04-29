
#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

#include "define.hpp"
#include "packet_queue.hpp"
#include <time.h>

#ifndef MIDDLEMAN_PORT
#define MIDDLEMAN_PORT 5998
#endif

#ifndef REMOTE_HOST
#define REMOTE_HOST "login.eqemulator.net"
#endif

#ifndef REMOTE_PORT
#define REMOTE_PORT "5998"
#endif

typedef struct sockaddr_in Address;

class Connection
{
private:
    static const uint32_t BUFFER_SIZE = 2048;
    static const uint32_t SESSION_TIMEOUT_SECONDS = 60;

private:
    int     m_socket;
    bool    m_inSession;
    time_t  m_lastRecvTime;

    Address m_localAddr;
    Address m_remoteAddr;

    PacketQueue m_packetQueue;

    byte    m_buffer[BUFFER_SIZE];
    
public:
    Connection();
    ~Connection();

    void init();
    void close();
    void read();
    void sendToLocal(const void* data, int len);
    void reset(const Address& localAddr);
    void endSession();
};

#endif//_CONNECTION_HPP_

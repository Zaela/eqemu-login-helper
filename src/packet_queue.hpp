
#ifndef _PACKET_QUEUE_HPP_
#define _PACKET_QUEUE_HPP_

#include "define.hpp"

#define OP_Combined             0x03
#define OP_SessionDisconnect    0x05
#define OP_Packet               0x09
#define OP_Fragment             0x0d

class Connection;

class PacketQueue
{
private:
    struct Packet
    {
        byte*   data;
        int     len;
    };
    
private:
    static const uint32_t QUEUE_SIZE = 65536;
    
private:
    Connection& m_connection;

    uint16_t    m_nextSeq;
    Packet      m_queue[QUEUE_SIZE];

private:
    static uint16_t getProtocolOpcode(const void* data);
    static uint16_t getSequence(const void* data);

private:
    void sendQueued(uint16_t seq);
    void readCombined(const void* data, int len);
    
public:
    PacketQueue(Connection& con);
    ~PacketQueue();

    void add(const void* data, int len);
    void enqueue(const void* data, int len, uint16_t seq);

    void reset();
};

#endif//_PACKET_QUEUE_HPP_

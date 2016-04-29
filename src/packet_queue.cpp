
#include "packet_queue.hpp"
#include "connection.hpp"

PacketQueue::PacketQueue(Connection& con)
: m_connection(con),
  m_nextSeq(0)
{
    memset(m_queue, 0, sizeof(m_queue));
}

PacketQueue::~PacketQueue()
{
    reset();
}

void PacketQueue::add(const void* data, int len)
{
    switch (getProtocolOpcode(data))
    {
    case OP_Combined:
        readCombined(data, len);
        break;
    case OP_Packet:
    case OP_Fragment:
    {
        uint16_t seq = getSequence(data);
        
        if (seq == m_nextSeq)
        {
            m_connection.sendToLocal(data, len);
            sendQueued(seq + 1);
        }
        else
        {
            enqueue(data, len, seq);
        }
        
        break;
    }
    case OP_SessionDisconnect:
        m_connection.endSession();
        // Fallthrough
    default:
        // Forward anything that didn't need special handling
        m_connection.sendToLocal(data, len);
        break;
    }
}

void PacketQueue::enqueue(const void* data, int len, uint16_t seq)
{
    byte* copy = new byte[len];
    
    memcpy(copy, data, len);
    
    m_queue[seq].data   = copy;
    m_queue[seq].len    = len;
}

void PacketQueue::sendQueued(uint16_t seq)
{
    while (m_queue[seq].data)
    {
        Packet& p = m_queue[seq];
        m_connection.sendToLocal(p.data, p.len);
        delete[] p.data;
        p.data = NULL;
        seq++;
    }
    
    m_nextSeq = seq;
}

void PacketQueue::readCombined(const void* data, int len)
{
    const byte* d = (const byte*)data;
    
    d += 2;
    len -= 2;
    
    int read = 0;
    
    while (read < len)
    {
        int size = d[read];
        read++;
        
        if ((read + size) > len)
            break;
        
        add(d + read, size);
        
        read += size;
    }
}

uint16_t PacketQueue::getProtocolOpcode(const void* data)
{
    const byte* d = (const byte*)data;
    return uint16_t((d[0] << 8) | d[1]);
}

uint16_t PacketQueue::getSequence(const void* data)
{
    const byte* d = (const byte*)data;
    return uint16_t((d[2] << 8) | d[3]);
}

void PacketQueue::reset()
{
    m_nextSeq = 0;
    
    for (uint32_t i = 0; i < QUEUE_SIZE; i++)
    {
        if (m_queue[i].data)
        {
            delete[] m_queue[i].data;
            m_queue[i].data = NULL;
        }
    }
}

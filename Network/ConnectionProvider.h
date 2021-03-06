#ifndef CONNECTIONPROVIDER_H
#define CONNECTIONPROVIDER_H

#include <Network/Packet.h>

class ConnectionProvider {
public:
    virtual bool sendPacket(const Packet &packet) = 0;
    virtual bool recvPacket(Packet &packet) = 0;
};

#endif

#include <Network/Packet.h>
#include <Base/Assertion.h>
#include <Base/Log.h>
#include <Base/Timestamp.h>

Packet::Packet(): size(0), data(0) {
}

Packet::Packet(const Packet &other): size(0), data(0), clockStamp(0) {
    duplicate(other);
}

Packet::Packet(const NetAddress &a, const char *d, unsigned int s): addr(a), size(s) {
    data = (char*)calloc(s, sizeof(char));
    memcpy(data, d, s);
    clockStamp = GetClock();
}

Packet::~Packet() {
    if(data) {
        free(data);
        data = 0;
    }
}

const Packet& Packet::operator=(const Packet &rhs) {
    duplicate(rhs);
    return *this;
}

bool Packet::operator<(const Packet &rhs) const {
    // We want the packet with the oldest (lowest) timestamp to have the greatest (highest) priority
    return (clockStamp > rhs.clockStamp);
}

void Packet::duplicate(const Packet &other) {
    if(data) {
        free(data);
        data = 0;
    }
    clockStamp = other.clockStamp;
    addr = other.addr;
    size = other.size;
    data = (char*)calloc(size, sizeof(char));
    memcpy(data, other.data, size);
}

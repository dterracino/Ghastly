#include <Network/GhastlyServer.h>
#include <Base/Assertion.h>

GhastlyHostInfo::GhastlyHostInfo() {}
GhastlyHostInfo::GhastlyHostInfo(const GhastlyHostInfo &other) { copy(other); }
GhastlyHostInfo::GhastlyHostInfo(const NetAddress &a, HostID i): addr(a), id(i) {
    lastReceived = GetClock();
    latency = 0;
}

void GhastlyHostInfo::operator=(const GhastlyHostInfo &other) { copy(other); }
void GhastlyHostInfo::copy(const GhastlyHostInfo &other) {
    addr = other.addr;
    id = other.id;
    lastReceived = other.lastReceived;
    latency = other.latency;
}

GhastlyServer::GhastlyServer(unsigned int maxClients): GhastlyHost(ID_SERVER), _maxClients(maxClients) {
    _idPool = new IndexPool(maxClients);
}

GhastlyServer::~GhastlyServer() {
    // Send disconnect messages to all the clients before tearing down
    HostMap::iterator itr;
    for(itr = _hostMap.begin(); itr != _hostMap.end(); itr++) {
        Disconnect dc;
        sendPacket(Packet(itr->second.addr, (char*)&dc, sizeof(dc)));
    }

    delete _idPool;
}

void GhastlyServer::update(int elapsed) {
    Packet packet;
    while(recvPacket(packet)) {
        onPacketReceive(packet);
    }
}

void GhastlyServer::onPacketReceive(const Packet &packet) {
    Payload *payload = (Payload*)packet.data;
    switch(payload->type) {
    case IDRequestType: {
        HostID assignID = (HostID)_idPool->allocate();
        if(assignID == -1) {
            HostReject reject;

            Warn("All IDs allocated, client " << packet.addr << " will be rejected");
            sendPacket(Packet(packet.addr, (char*)&reject, sizeof(reject)));
        } else {
            IDAssign assign(assignID);

            Info("Client connecting, associated ID " << assignID << " with address " << packet.addr);
            sendPacket(Packet(packet.addr, (char*)&assign, sizeof(assign)));

            _idMap[packet.addr] = assignID;
            _hostMap[assignID] = GhastlyHostInfo(packet.addr, assignID);
        }

        break;
    }
    case DisconnectType: {
        HostID releasedID = _idMap[packet.addr];
        _idPool->free(releasedID);

        _idMap.erase(packet.addr);
        _hostMap.erase(releasedID);

        Info("Client disconnected, dissociating ID " << releasedID << " from address " << packet.addr);

        break;
    }
    }
}

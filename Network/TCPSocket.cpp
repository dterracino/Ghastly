#include <Network/TCPSocket.h>
#include <Base/Log.h>

TCPSocket::TCPSocket(unsigned short port, bool blocking) {
    // Not yet implemented
    Error("TCP Sockets are not implemented.");
    ASSERT(0);
}

TCPSocket::~TCPSocket() {
}

bool TCPSocket::openSocket() {
}

void TCPSocket::closeSocket() {
}

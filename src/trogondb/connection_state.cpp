#include "trogondb/connection_state.h"

namespace trogondb {

IConnectionState::IConnectionState(std::shared_ptr<Connection> connection)
    : m_connection(connection)
{}

void IConnectionState::doRead(const std::string &data)
{}

void IConnectionState::doWrite()
{}

void IConnectionState::doTimeout()
{}


void WaitingForArrayHeaderState::doRead(const std::string &data)
{

}

} // namespace trogondb

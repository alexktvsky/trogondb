#include "trogondb/connection_state.h"

namespace trogondb {

std::shared_ptr<IConnectionState> IConnectionState::handle()
{
    return shared_from_this();
}

} // namespace trogondb

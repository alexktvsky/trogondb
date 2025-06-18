#include "trogondb/proactor.h"

namespace trogondb {

void Proactor::run()
{
    m_executionContext.run();
}

void Proactor::stop()
{
    m_executionContext.stop();
}

bool Proactor::isRunning()
{
    return !m_executionContext.stopped();
}

} // namespace trogondb

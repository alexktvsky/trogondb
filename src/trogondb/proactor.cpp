#include "trogondb/proactor.h"

namespace trogondb {

void Proactor::run()
{
    m_context.run();
}

void Proactor::stop()
{
    m_context.stop();
}

bool Proactor::isRunning()
{
    !m_context.stopped();
}

} // namespace trogondb

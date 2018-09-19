#include "command_handler.h"

CommandHandler::CommandHandler(size_t commandPackSize)
:   m_reader(commandPackSize, std::cin)
{
    m_reader.subscribe(&m_processor);
    m_reader.subscribe(&m_log);

    m_reader.scan_input();
}

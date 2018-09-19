#pragma once

#include "command_reader.h"
#include "command_processors.h"

class CommandHandler {
public:
    explicit CommandHandler(size_t commandPackSize);

private:
    CommandReader           m_reader;
    CommandProcessor        m_processor;
    CommandLog              m_log;
};


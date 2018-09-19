#include "command_processors.h"

void CommandProcessor::process() {
    if (!m_commands.empty()) {
        std::cout << "bulk: ";
    }

    commandName name;
    while (!m_commands.empty()) {
        name = m_commands.front();
        m_commands.pop();
        std::cout << name << (m_commands.empty() ? "" : ", ");
    }

    std::cout << std::endl;
}

void CommandProcessor::add(const commandName& name) {
    m_commands.push(name);
}

void CommandProcessor::clear() {
    while (!m_commands.empty())
        m_commands.pop();
}

void CommandLog::process() {
    using std::to_string;
    using std::chrono::duration_cast;
    using std::chrono::seconds;

    if (!m_commands.empty()) {
        m_os.open("bulk" +
                  to_string(duration_cast<seconds>(m_time_point.time_since_epoch()).count()) +
                  ".log");
    }

    commandName name;
    while (!m_commands.empty()) {
        name = m_commands.front();
        m_commands.pop();
        m_os << name << std::endl;
    }

    m_os.close();
    m_begin_mark = false;
}

void CommandLog::add(const commandName& name) {
    if (!m_begin_mark) {
        m_time_point = std::chrono::system_clock::now();
        m_begin_mark = true;
    }

    m_commands.push(name);
}

void CommandLog::clear() {
    if (m_begin_mark) {
        m_begin_mark = false;
    }

    while (!m_commands.empty())
        m_commands.pop();
}

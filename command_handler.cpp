#include "command_handler.h"

CommandReader::CommandReader(size_t commandPackSize)
    : m_current_state(std::make_unique<NormalState>(commandPackSize)),
      m_other_state(std::make_unique<BracedState>())
{ }

void CommandReader::scan_input() {
    while (m_current_state->read_commands(this));
}

void CommandReader::subscribe(AbstractQueueObserver* obs) {
    m_observers.push_back(obs);
}

void CommandReader::push_string(const commandName& name) const {
    for (auto& obs : m_observers)
        obs->add(name);
}

void CommandReader::notify() {
    for (auto& obs : m_observers)
        obs->process();
}

void CommandReader::abort() {
    for (auto& obs : m_observers)
        obs->clear();
}

void CommandReader::switch_state() {
    std::swap(m_current_state, m_other_state);
}

NormalState::NormalState(size_t commandPackSize)
    : m_command_pack_size(commandPackSize)
{ }

void NormalState::open_brace(CommandReader* reader) {
    reader->notify();
    reader->switch_state();
}

void NormalState::close_brace(CommandReader*) { }

bool NormalState::read_commands(CommandReader* reader) {
    for (size_t i = 0; i < m_command_pack_size; i++) {
        commandName name;
        std::getline(std::cin, name);

        if (name.empty()) {
            reader->notify();
            return false;
        }

        if (name == "{") {
            open_brace(reader);
            return true;
        }

        if (name == "}") {
            continue;
        }

        reader->push_string(name);
    }

    reader->notify();
    return true;
}

BracedState::BracedState()
    : m_brace_counter(0)
{ }

void BracedState::open_brace(CommandReader*) { }

void BracedState::close_brace(CommandReader* reader) {
    reader->notify();
    reader->switch_state();
}

bool BracedState::read_commands(CommandReader* reader) {
    m_brace_counter++;

    commandName name;
    while(std::getline(std::cin, name)) {
        if (name.empty()) {
            reader->abort();
            return false;
        }

        if (name == "{") {
            m_brace_counter++;
            continue;
        }

        if (name == "}") {
            if (!--m_brace_counter) {
                close_brace(reader);
                return true;
            }

            continue;
        }

        reader->push_string(name);
    }

    return false;
}

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

CommandHandler::CommandHandler(size_t commandPackSize)
    : m_reader(commandPackSize)
{
    m_reader.subscribe(&m_processor);
    m_reader.subscribe(&m_log);

    m_reader.scan_input();
}

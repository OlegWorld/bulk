#include "command_handler.h"

CommandReader::CommandReader(size_t commandPackSize)
        :   m_current_state(new NormalState(commandPackSize)),
            m_other_state(new BracedState),
{ }

CommandReader::~CommandReader() {
    delete m_other_state;
    delete m_current_state;
}

void CommandReader::scan_input() {
    m_current_state->read_commands(this);
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

void NormalState::read_commands(CommandReader* reader) {
    for (size_t i = 0; i < m_command_pack_size; i++) {
        commandName name;
        std::getline(std::cin, name);

        if (name == "{") {
            open_brace(reader);
            break;
        }

        if (name == "}") {
            continue;
        }

        reader->push_string(name);
    }

    reader->notify();
}

void NormalState::commands_ended(CommandReader* reader) {

}

BracedState::BracedState()
    : m_brace_counter(0)
{ }

void BracedState::open_brace(CommandReader*) { }

void BracedState::close_brace(CommandReader* reader) {
    reader->notify();
    reader->switch_state();
}

void BracedState::read_commands(CommandReader* reader) {
    m_brace_counter++;

    while(true) {
        commandName name;
        std::getline(std::cin, name);

        if (name == "{") {
            m_brace_counter++;
            continue;
        }

        if (name == "}" && !(--m_brace_counter)) {
            break;
        }

        reader->push_string(name);
    }

    close_brace(reader);
}

void BracedState::commands_ended(CommandReader* reader) {

}

#include "command_reader.h"

CommandReader::CommandReader(size_t commandPackSize, std::istream& input)
:   m_current_state(std::make_unique<NormalState>(commandPackSize, input)),
    m_other_state(std::make_unique<BracedState>(input))
{ }

void CommandReader::scan_input() {
    while (m_current_state->read_commands(this));
}

void CommandReader::subscribe(AbstractObserver* obs) {
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

NormalState::NormalState(size_t commandPackSize, std::istream& input)
:   m_command_pack_size(commandPackSize),
    m_input(input)
{ }

void NormalState::open_brace(CommandReader* reader) {
    reader->notify();
    reader->switch_state();
}

void NormalState::close_brace(CommandReader*) { }

bool NormalState::read_commands(CommandReader* reader) {
    for (size_t i = 0; i < m_command_pack_size; i++) {
        commandName name;
        std::getline(m_input, name);

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

BracedState::BracedState(std::istream& input)
:   m_brace_counter(0),
    m_input(input)
{ }

void BracedState::open_brace(CommandReader*) { }

void BracedState::close_brace(CommandReader* reader) {
    reader->notify();
    reader->switch_state();
}

bool BracedState::read_commands(CommandReader* reader) {
    m_brace_counter++;

    commandName name;
    while(std::getline(m_input, name)) {
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


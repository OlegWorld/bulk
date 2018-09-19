#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include "abstract_observer.h"

class ReaderState;
class NormalState;
class BracedState;

class CommandReader {
public:
    explicit CommandReader(size_t commandPackSize, std::istream& input);

    ~CommandReader() = default;

    void scan_input();

    void subscribe(AbstractObserver* obs);

    void push_string(const commandName& name) const;

    void notify();

    void abort();

    void switch_state();

private:
    std::unique_ptr<ReaderState> m_current_state;
    std::unique_ptr<ReaderState> m_other_state;
    std::vector<AbstractObserver*> m_observers;
};

class ReaderState {
public:
    virtual ~ReaderState() = default;
    virtual void open_brace(CommandReader*) = 0;
    virtual void close_brace(CommandReader*) = 0;
    virtual bool read_commands(CommandReader*) = 0;
};

class NormalState : public ReaderState {
public:
    explicit NormalState(size_t commandPackSize, std::istream& input);
    ~NormalState() override = default;
    void open_brace(CommandReader* reader) override;
    void close_brace(CommandReader* reader) override;
    bool read_commands(CommandReader* reader) override;

private:
    const size_t m_command_pack_size;
    std::istream& m_input;
};

class BracedState : public ReaderState {
public:
    BracedState(std::istream& input);
    ~BracedState() override = default;
    void open_brace(CommandReader* reader) override;
    void close_brace(CommandReader* reader) override;
    bool read_commands(CommandReader* reader) override;

private:
    size_t m_brace_counter;
    std::istream& m_input;
};

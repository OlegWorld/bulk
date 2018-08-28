#pragma once

#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>
#include <memory>

using commandName = std::string;

class AbstractQueueObserver {
public:
    virtual ~AbstractQueueObserver() = default;
    virtual void process() = 0;
    virtual void add(const commandName& name) = 0;
    virtual void clear() = 0;
};

class ReaderState;
class NormalState;
class BracedState;

class CommandReader {
public:
    explicit CommandReader(size_t commandPackSize);

    ~CommandReader() = default;

    void scan_input();

    void subscribe(AbstractQueueObserver* obs);

    void push_string(const commandName& name) const;

    void notify();

    void abort();

    void switch_state();

private:
    std::unique_ptr<ReaderState> m_current_state;
    std::unique_ptr<ReaderState> m_other_state;
    std::vector<AbstractQueueObserver*> m_observers;
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
    explicit NormalState(size_t commandPackSize);
    ~NormalState() override = default;
    void open_brace(CommandReader* reader) override;
    void close_brace(CommandReader* reader) override;
    bool read_commands(CommandReader* reader) override;

private:
    const size_t m_command_pack_size;
};

class BracedState : public ReaderState {
public:
    BracedState();
    ~BracedState() override = default;
    void open_brace(CommandReader* reader) override;
    void close_brace(CommandReader* reader) override;
    bool read_commands(CommandReader* reader) override;

private:
    size_t m_brace_counter;
};

class CommandProcessor : public AbstractQueueObserver {
public:
    CommandProcessor() = default;
    ~CommandProcessor() override = default;

    void process() override;

    void add(const commandName& name) override;

    void clear() override;

private:
    std::queue<commandName> m_commands;
};

class CommandLog : public AbstractQueueObserver {
public:
    CommandLog() = default;
    ~CommandLog() override = default;

    void process() override;

    void add(const commandName& name) override;

    void clear() override;

private:
    std::queue<commandName> m_commands;
    std::ofstream m_os;
    std::chrono::time_point<std::chrono::system_clock> m_time_point;
    bool m_begin_mark = false;
};

class CommandHandler {
public:
    explicit CommandHandler(size_t commandPackSize);

private:
    CommandReader           m_reader;
    CommandProcessor        m_processor;
    CommandLog              m_log;
};


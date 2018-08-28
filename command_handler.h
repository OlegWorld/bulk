#pragma once

#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>

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

    ~CommandReader();

    void scan_input();

    void subscribe(AbstractQueueObserver* obs);

    void push_string(const commandName& name) const;

    void notify();

    void abort();

    void switch_state();

private:
    ReaderState* m_current_state;
    ReaderState* m_other_state;
    std::vector<AbstractQueueObserver*> m_observers;
};

class ReaderState {
public:
    virtual ~ReaderState() = default;
    virtual void open_brace(CommandReader*) = 0;
    virtual void close_brace(CommandReader*) = 0;
    virtual void read_commands(CommandReader*) = 0;
    virtual void commands_ended(CommandReader*) = 0;
};

class NormalState : public ReaderState {
public:
    explicit NormalState(size_t commandPackSize);
    ~NormalState() override = default;
    void open_brace(CommandReader* reader) override;
    void close_brace(CommandReader* reader) override;
    void read_commands(CommandReader* reader) override;
    void commands_ended(CommandReader* reader) override;

private:
    const size_t m_command_pack_size;
};

class BracedState : public ReaderState {
public:
    BracedState();
    ~BracedState() override = default;
    void open_brace(CommandReader* reader) override;
    void close_brace(CommandReader* reader) override;
    void read_commands(CommandReader* reader) override;
    void commands_ended(CommandReader* reader) override;

private:
    size_t m_brace_counter;
};

class CommandProcessor : public AbstractQueueObserver {
public:
    CommandProcessor() = default;
    ~CommandProcessor() override = default;

    void process() override {
        std::cout << "bulk: ";

        commandName name;
        while (!m_commands.empty()) {
            name = m_commands.front();
            m_commands.pop();
            std::cout << name << (m_commands.empty() ? "" : ", ");
        }

        std::cout << std::endl;
    }

    void add(const commandName& name) override {
        m_commands.push(name);
    }

    void clear() override {
        while (!m_commands.empty())
            m_commands.pop();
    }

private:
    std::queue<commandName> m_commands;
};

class CommandLog : public AbstractQueueObserver {
public:
    CommandLog() = default;
    ~CommandLog() override = default;

    void process() override {
        using std::to_string;
        using std::chrono::duration_cast;
        using std::chrono::seconds;

        m_os.open("bulk" +
                  to_string(duration_cast<seconds>(m_time_point.time_since_epoch()).count()) +
                  ".log");

        commandName name;
        while (!m_commands.empty()) {
            name = m_commands.front();
            m_commands.pop();
            m_os << name << std::endl;
        }

        m_os.close();
    }

    void add(const commandName& name) override {
        if (!m_begin_mark) {
            m_time_point = std::chrono::system_clock::now();
            m_begin_mark = true;
        }

        m_commands.push(name);
    }

    void clear() override {
        if (m_begin_mark) {
            m_begin_mark = false;
        }

        while (!m_commands.empty())
            m_commands.pop();
    }

private:
    std::queue<commandName> m_commands;
    std::ofstream m_os;
    std::chrono::time_point<std::chrono::system_clock> m_time_point;
    bool m_begin_mark;
};

class CommandHandler {
public:
    explicit CommandHandler(size_t commandPackSize)
    :   m_reader(commandPackSize)
    {
        m_reader.subscribe(&m_processor);
        m_reader.subscribe(&m_log);

        m_reader.scan_input();
    }

private:
    CommandReader           m_reader;
    CommandProcessor        m_processor;
    CommandLog              m_log;
};


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

    virtual void add(const commandName& name) {
        m_commands.push(name);
    }

    virtual void clear() {
        while (!m_commands.empty())
            m_commands.pop();
    }

    bool pop(commandName& name) {
        if (!m_commands.empty()) {
            name = m_commands.front();
            m_commands.pop();
            return true;
        }

        return false;
    }

private:
    std::queue<commandName> m_commands;
};

class CommandReader {
public:
    explicit CommandReader(size_t commandPackSize)
    :   m_command_pack_size(commandPackSize)
    { }

    void scan_input() {
        for (size_t i = 0; i < m_command_pack_size; i++) {
            commandName name;
            std::getline(std::cin, name);
            push_string(name);
        }

        notify();
    }

    void subscribe(AbstractQueueObserver* obs) {
        m_observers.push_back(obs);
    }

private:
    void push_string(const commandName& name) const {
        for (auto& obs : m_observers)
            obs->add(name);
    }

    void notify() {
        for (auto& obs : m_observers)
            obs->process();
    }

    void abort() {
        for (auto& obs : m_observers)
            obs->clear();
    }

private:
    std::vector<AbstractQueueObserver*> m_observers;
    size_t m_command_pack_size;
};

class CommandProcessor : public AbstractQueueObserver {
public:
    CommandProcessor() = default;
    ~CommandProcessor() override = default;

    void process() override {
        std::cout << "bulk: ";

        commandName name;
        while (pop(name)) {
            std::cout << name << ", ";
        }

        std::cout << std::endl;
    }
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
        while (pop(name)) {
            m_os << name << std::endl;
        }

        m_os.close();
    }

    void add(const commandName& name) override {
        if (!m_begin_mark) {
            m_time_point = std::chrono::system_clock::now();
            m_begin_mark = true;
        }

        AbstractQueueObserver::add(name);
    }

    void clear() override {
        if (m_begin_mark) {
            m_begin_mark = false;
        }

        AbstractQueueObserver::clear();
    }

private:
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


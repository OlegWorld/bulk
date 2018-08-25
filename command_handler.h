#pragma once

#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <ofstream>

using commandName = std::string;

class AbstractQueueObserver {
public:
    virtual void process() = 0;

    void add(const commandName& name) {
        m_commands.push(name);
    }

    bool pop(commandName& name) {
        name = m_commands.front();
        m_commands.pop();
    }

private:
    std::queue<commandName> m_commands;
};

class CommandReader {
public:
    explicit CommandReader(size_t commandPackSize)
    :   m_command_pack_size(commandPackSize)
    { }

    void subscribe(AbstractObserver& obs) {
        m_observers.push_back(obs);
    }

private:
    void push_string(const commandName& name) const {
        for (const auto& obs : m_observers)
            obs.add(name);
    }

    void notify() const {
        for (const auto& obs : m_observers)
            obs.process();
    }

private:
    std::vector<AbstractQueueObserver&> m_observers;
    size_t m_command_pack_size;
};

class CommandProcessor : public AbstractQueueObserver {
public:
    void process() override {
        std::cout << "bulk: ";

        commandName name;
        while (pop(name)) {
            std::cout << name << ", "
        }

        std::cout << std::endl;
    }
};

class CommandLog : public AbstractQueueObserver {
public:

private:
    std::ofstream m_os;
};

class CommandHandler {
public:
    CommandHandler(size_t commandPackSize);

private:
    CommandReader           m_reader;
    CommandProcessor        m_processor;
    CommandLog              m_log;
};


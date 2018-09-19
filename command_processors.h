#pragma once

#include <iostream>
#include <fstream>
#include <queue>
#include <chrono>
#include "abstract_observer.h"

class CommandProcessor : public AbstractObserver {
public:
    CommandProcessor() = default;
    ~CommandProcessor() override = default;

    void process() override;

    void add(const commandName& name) override;

    void clear() override;

private:
    std::queue<commandName> m_commands;
};

class CommandLog : public AbstractObserver {
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

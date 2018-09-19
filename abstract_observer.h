#pragma once

#include <string>

using commandName = std::string;

class AbstractObserver {
public:
    virtual ~AbstractObserver() = default;
    virtual void process() = 0;
    virtual void add(const commandName& name) = 0;
    virtual void clear() = 0;
};

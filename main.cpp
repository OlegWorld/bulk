#include <iostream>
#include <string>
#include "command_handler.h"


int main(int, char** argv) {
    CommandHandler(std::stoul(argv[1]));

    return 0;
}
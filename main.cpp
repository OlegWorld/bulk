#include <iostream>
#include <string>
#include "command_handler.h"


int main(int argc, char** argv) {
    CommandHandler(std::stoul(argv[1]));

    return 0;
}
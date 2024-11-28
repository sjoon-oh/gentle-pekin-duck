#include <iostream>

#include "utils/logger.hh"
#include "memory/buffer.hh"

int
main()
{
    pduck::utils::Logger::getInstance().getLogger()->info("start");
    return 0;
}
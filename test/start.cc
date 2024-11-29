#include <iostream>

#include "utils/Logger.hh"
#include "memory/Buffer.hh"

int
main()
{
    pduck::utils::Logger::getInstance().getLogger()->info("start");
    return 0;
}
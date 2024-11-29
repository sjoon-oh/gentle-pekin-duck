#include <iostream>
// #include <boost/program_options.hpp>

#include "utils/Logger.hh"
#include "utils/ArgParser.hh"

int main(int argc, char* argv[]) {
    
    pduck::utils::Logger::getInstance().getLogger()->info("arguments");
    pduck::utils::ArgumentParser argParser;

    argParser.addIntOption("argint", "Add integer argument");
    argParser.addDoubleOption("argdouble", "Add double argument");
    argParser.addStringOption("argstring", "Add string argument");

    argParser.parseArgs(argc, argv);

    pduck::utils::Logger::getInstance().getLogger()->info(
        "argint: {}", argParser.getIntArgument("argint")
    );

    pduck::utils::Logger::getInstance().getLogger()->info(
        "argdouble: {}", argParser.getDoubleArgument("argdouble")
    );

    pduck::utils::Logger::getInstance().getLogger()->info(
        "argstring: {}", argParser.getStringArgument("argstring")
    );


    return 0;
}
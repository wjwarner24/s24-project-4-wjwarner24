#include <iostream>
#include <string>

#include "utilities/flags/flags.hpp"
#include "simulation/simulation.hpp"

int main(int argc, char **argv)
{
    int error = 0;
    FlagOptions flags;

    error = parse_flags(argc, argv, flags);

    if (error != 0)
    {
        print_usage();
        return 1;
    }
try {
    Simulation simulation(flags);
    simulation.run();
    // std::cout << "ran" << std::endl;
    return error;
}
    catch (std::string const& e) {
        std::cerr << e << std::endl;
    }
}

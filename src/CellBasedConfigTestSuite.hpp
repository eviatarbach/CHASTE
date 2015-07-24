#ifndef CELLBASEDCONFIGTESTSUITE_HPP_
#define CELLBASEDCONFIGTESTSUITE_HPP_

#include "AbstractCellBasedTestSuite.hpp"
#include "CommandLineArguments.hpp"
#include <sstream>

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
                ( std::ostringstream() << std::dec << x ) ).str()

class CellBasedConfigTestSuite : public AbstractCellBasedTestSuite
{
    protected:

        std::map<std::string, double> config;
        unsigned simulation_id;

        void setUp()
        {
            AbstractCellBasedTestSuite::setUp();

            // TODO: Remove hardcoded path
            std::string configFile = "/nfs/zfs-dbhaskar92/chaste-source/projects/eviatarbach/test/config";

            simulation_id = UNSIGNED_UNSET;
            if (CommandLineArguments::Instance()->OptionExists("-sim_id"))
            {
                simulation_id = CommandLineArguments::Instance()->GetUnsignedCorrespondingToOption("-sim_id");
                configFile += SSTR("_" << simulation_id);
            }

            // some code from https://stackoverflow.com/questions/6892754/creating-a-simple-configuration-file-and-parser-in-c

            std::ifstream is_file(configFile.c_str(), std::ifstream::in);

            std::string line;
            while (std::getline(is_file, line))
            {
                std::istringstream is_line(line);
                std::string key;
                if (std::getline(is_line, key, '='))
                {
                    std::string value;
                    if (std::getline(is_line, value))
                        config[key] = std::atof(value.c_str());
                }
            }
        }
};

#endif /*CELLBASEDCONFIGTESTSUITE_HPP_*/

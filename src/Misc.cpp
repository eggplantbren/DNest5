#include "Misc.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

namespace DNest5
{

void clear_output_dir()
{
    static const std::vector<std::string> files = {
        "dnest5.db", "dnest5.db-shm", "dnest5.db-wal",
        "figure1.pdf", "figure2.pdf", "figure3.pdf",
        "posterior.csv", "posterior.db", "posterior.db-shm", "posterior.db-wal",
        "results.yaml"};

    std::cout << "Clearing output directory." << std::endl;
    for(const auto& file: files)
    {
        std::string path = "output/" + file;
        std::remove(path.c_str());
    }
}

} // namespace


#include <iostream>
#include <DNest5/Options.hpp>
#include <sqlite_modern_cpp/hdr/sqlite_modern_cpp.h>

using namespace DNest5;

int main()
{
    // Read-only database connection
    sqlite::database db(
                            Options::db_filename,
                            sqlite::sqlite_config
                            {
                                sqlite::OpenFlags::READONLY,
                                nullptr,
                                sqlite::Encoding::ANY
                            }
                        );
    return 0;
}


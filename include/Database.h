#ifndef DNest5_Database_h
#define DNest5_Database_h

#include <sqlite_modern_cpp/hdr/sqlite_modern_cpp.h>

namespace DNest5
{

/* Create and manage the output database */
class Database
{
    private:
        sqlite::database db;
        void pragmas();
        void create_tables();
        void create_indexes();
        void create_views();
        void clear_previous();

    public:
        Database();

        int num_full_particles(int sampler_id);

        // Friend
        template<typename T>
        friend class Sampler;
};

} // namespace

#endif


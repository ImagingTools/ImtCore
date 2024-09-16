#pragma once

// libmdbx includes
#include <mdbx.h++>


namespace imtmdbx
{


class IMdbxDatabaseEngine
{

public:
    virtual mdbx::env_managed& GetEnv() = 0;
};


}//namespace imtmdbx

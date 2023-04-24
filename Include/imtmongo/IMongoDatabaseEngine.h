#pragma once

// mongocxx includes
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtmongo
{


/**
	Interfaces and basic implementations for general mongo database management.
	This package is system independent.
*/
class IMongoDatabaseEngine: virtual public istd::IPolymorphic
{
public:
	virtual mongocxx::database GetDatabase() = 0;
};


} // namespace imtmongo



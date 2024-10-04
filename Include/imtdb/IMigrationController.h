#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TRange.h>


namespace imtdb
{


class IMigrationController: virtual public istd::IPolymorphic
{
public:
	virtual istd::CIntRange GetMigrationRange() const = 0;
	virtual bool DoMigration(int& resultRevision, const istd::CIntRange& subRange = istd::CIntRange()) const = 0;
};


} // namespace imtdb



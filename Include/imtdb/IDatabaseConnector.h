#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtdb/IDatabaseLoginSettings.h>


namespace imtdb
{


class IDatabaseConnector: virtual public istd::IPolymorphic
{
public:
	virtual bool ConnectToDatabase(const IDatabaseLoginSettings& loginSettings) const = 0;
	virtual bool DisconnectFromDatabase(const QString& connectionName) const = 0;
};


} // namespace imtdb



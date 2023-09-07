#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


class ISystemStatus: virtual public istd::IChangeable
{
public:
	enum SystemStatus
	{
		SS_UNKNOWN_ERROR = 0,
		SS_NO_ERROR,
		SS_DATABASE_CONNECTION_ERROR,
		SS_SERVER_CONNECTION_ERROR,
		SS_TRY_CONNECTING_SERVER
	};

	virtual SystemStatus GetSystemStatus(QString& errorMessage) const = 0;
};


} // namespace imtbase



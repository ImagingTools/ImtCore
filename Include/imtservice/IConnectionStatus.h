#pragma once


// Qt includes
#include <QtCore/QUrl>

// ACF includes
#include <istd/IChangeable.h>


namespace imtservice
{


/**
	Interface for describing an service connection status.
	\ingroup Service
*/
class IConnectionStatus: virtual public istd::IChangeable
{
public:
	enum ConnectionStatus
	{
		CS_OK,
		CS_REQUIRE_UPDATE
	};

	I_DECLARE_ENUM(ConnectionStatus, CS_OK, CS_REQUIRE_UPDATE);
	/**
		Get connection status.
	*/
	virtual ConnectionStatus GetConnectionStatus() const = 0;
};


} // namespace imtservice



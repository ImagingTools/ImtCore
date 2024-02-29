#pragma once


// ImtCore includes
#include <imtservice/IServiceConnectionInfo.h>


namespace imtservice
{


/**
	Interface for describing an service connections info.
	\ingroup Service
*/
class IServiceConnectionLinkParam: virtual public IServiceConnectionInfo
{
public:
	/**
		Get dependant service connection-Id.
	*/
	virtual QByteArray GetDependantServiceConnectionId() const = 0;
};


} // namespace imtservice



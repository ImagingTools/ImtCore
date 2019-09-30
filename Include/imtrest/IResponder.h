#pragma once


// Qt includes
#include <QtNetwork/QAbstractSocket>

// ACF includes
#include <istd/IChangeable.h>


namespace imtrest
{


class IResponse;


/**
	Interface describing transfer of the server response to the client.
*/
class IResponder: virtual public istd::IPolymorphic
{
public:
	/**
		Send a response to the output device (e.g a socket).
	*/
	virtual bool SendResponse(const IResponse& response) const = 0;
};


} // namespace imtrest




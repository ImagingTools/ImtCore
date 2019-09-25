#pragma once


// Qt includes
#include <QtCore/QIODevice>

// ACF includes
#include <istd/IChangeable.h>


namespace imtrest
{


class IResponse;


class IResponder: virtual public istd::IPolymorphic
{
public:
	/**
		Send a response to the output device (e.g a socket).
	*/
	virtual bool SendResponse(const IResponse& response, QIODevice& device) const = 0;
};


} // namespace imtrest




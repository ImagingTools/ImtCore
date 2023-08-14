#pragma once


// ACF includes
#include <ilog/IMessageContainer.h>

// ImtCore includes
#include <imtgql/IGqlResponseHandler.h>


namespace imtgql
{


class IGqlResponse:
			virtual public IGqlResponseHandler,
			virtual public ilog::IMessageContainer
{
public:
	virtual bool IsSuccessfull() const = 0;
};


} // namespace imtgql



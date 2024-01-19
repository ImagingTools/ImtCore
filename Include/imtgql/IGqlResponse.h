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
	virtual bool IsSuccessful() const = 0;
	virtual QVariant GetResult() const = 0;
};


} // namespace imtgql



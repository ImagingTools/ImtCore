#pragma once


// ImtCore includes
#include <imtbase/IMultiStatusProvider.h>


namespace imtbase
{


class IMultiStatusManager: virtual public IMultiStatusProvider
{
public:
	/**
		Set status data from a given information provider.
	*/
	virtual void SetStatus(const QByteArray& statusId, const istd::IInformationProvider& status);
};


} // namespace imtbase

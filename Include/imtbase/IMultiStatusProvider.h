#pragma once


// ACF includes
#include <imtbase/ICollectionInfo.h>


namespace istd
{
	class IInformationProvider;
};


namespace imtbase
{


/**
	Provider of list of status objects.
*/
class IMultiStatusProvider: virtual public istd::IChangeable
{
public:
	/**
		Get list of available status objects.
	*/
	virtual const imtbase::ICollectionInfo& GetStatusList() const = 0;

	/**
		Get status information with specified identifier.
	*/
	virtual const istd::IInformationProvider* GetStatus(const QByteArray& statusId) const = 0;
};


} // namespace imtbase



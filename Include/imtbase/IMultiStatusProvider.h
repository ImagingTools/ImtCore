#pragma once


// ACF includes
#include <istd/IInformationProvider.h>
#include <iprm/IOptionsList.h>


namespace imtbase
{


/**
	Multi status provider interface.
*/
class IMultiStatusProvider: virtual public istd::IInformationProvider
{
public:
	/**
		Get status information list.
	*/
	virtual const iprm::IOptionsList& GetStatusInfoList() const = 0;

	/**
		Get status information with specified identifier.
	*/
	virtual const istd::IInformationProvider* GetStatusInfo(const QByteArray& id) const = 0;
};


} // namespace imtbase

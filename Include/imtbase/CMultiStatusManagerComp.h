#pragma once


// ImtCore includes
#include <imtbase/IMultiStatusManager.h>
#include <imtbase/CMultiStatusProviderComp.h>


namespace imtbase
{


/**
	Multiple status provider component implementation.
*/
class CMultiStatusManagerComp: virtual public IMultiStatusManager, public CMultiStatusProviderComp
{
public:
	typedef CMultiStatusProviderComp BaseClass;

	I_BEGIN_COMPONENT(CMultiStatusManagerComp);
		I_REGISTER_INTERFACE(IMultiStatusManager);
	I_END_COMPONENT;

	// reimplemented (imtbase::IMultiStatusManager)
	virtual void SetStatus(const QByteArray& statusId, const istd::IInformationProvider& status) override;
};


} // namespace imtbase



#pragma once


// ACF includes
#include <istd/IInformationProvider.h>
#include <icomp/CComponentBase.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtbase/IMultiStatusProvider.h>


namespace imtbase
{


/**
	Multiple status provider component implementation.
*/
class CMultiStatusProviderComp:
			public icomp::CComponentBase,
			virtual public imtbase::IMultiStatusProvider,
			virtual public istd::IInformationProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CMultiStatusProviderComp);
		I_REGISTER_INTERFACE(IInformationProvider);
		I_REGISTER_INTERFACE(IMultiStatusProvider);
		I_ASSIGN_MULTI_0(m_informationProvidersCompPtr, "InformationProviders", "List of underlying information providers", true);
		I_ASSIGN_MULTI_0(m_statusIdsAttrPtr, "InformationStatusIds", "Information status identifiers", true);
		I_ASSIGN_MULTI_0(m_statusNamesAttrPtr, "InformationStatusNames", "Information status names", true);
		I_ASSIGN_MULTI_0(m_statusDescriptionsAttrPtr, "InformationStatusDescriptions", "Information status descriptions", true);
	I_END_COMPONENT;

	// reimplemented (imtbase::IMultiStatusProvider)
	virtual const iprm::IOptionsList& GetStatusInfoList() const override;
	virtual const istd::IInformationProvider* GetStatusInfo(const QByteArray& id) const override;

	// reimplemented (iinsp::IInformationProvider)
	virtual QDateTime GetInformationTimeStamp() const override;
	virtual InformationCategory GetInformationCategory() const override;
	virtual int GetInformationId() const override;
	virtual QString GetInformationDescription() const override;
	virtual QString GetInformationSource() const override;
	virtual int GetInformationFlags() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	I_MULTIREF(istd::IInformationProvider, m_informationProvidersCompPtr);
	I_MULTIATTR(QByteArray, m_statusIdsAttrPtr);
	I_MULTITEXTATTR(m_statusNamesAttrPtr);
	I_MULTITEXTATTR(m_statusDescriptionsAttrPtr);

	iprm::COptionsManager m_statusInfo;
};


} // namespace imtbase



#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IContactInfo.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtbase/IItemBasedRepresentationDataController.h>
#include <imod/TModelWrap.h>
#include <imtrest/ISuscriberEngine.h>
#include <imtqml/CPageDataEnumProviderComp.h>


namespace imtqml
{


/**
	Represenatation controller for contact info data model.
	\ingroup LicenseManagement
*/
class CJoinDataProviderComp:
		public icomp::CComponentBase,
		public imtbase::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtbase::IItemBasedRepresentationDataProvider BaseClass2;
	typedef imtqml::CPageDataEnumProviderComp PageEnum;

	I_BEGIN_COMPONENT(CJoinDataProviderComp);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_commandIdAttrPtr, "commandId", "Command Id", true, "");
		I_ASSIGN_MULTI_0(m_representationDataProviderCompPtr, "m_representationDataProviderCompPtr", "List of data providers for join", false);
	I_END_COMPONENT;

	CJoinDataProviderComp();

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields) override;


private:
	I_ATTR(QByteArray, m_commandIdAttrPtr);
	I_MULTIREF(imtbase::IItemBasedRepresentationDataProvider, m_representationDataProviderCompPtr);

};


} // namespace imtqml



#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>

#include <iprm/IParamsSet.h>


namespace imtguigql
{


class CSettingsControllerComp: public imtgql::CGqlRepresentationDataControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationDataControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSettingsControllerComp);
		I_ASSIGN(m_settingsParamsSetCompPtr, "SettingsParamsSet", "Settings parameters", true, "SettingsParamsSet");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(iprm::IParamsSet, m_settingsParamsSetCompPtr);
};


} // namespace imtguigql



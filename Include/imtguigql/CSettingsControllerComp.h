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

	enum OperationType
	{
		OT_UNKNOWN,
		OT_GET,
		OT_SAVE
	};

	// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
protected:
	virtual bool GetOperationFromRequest(const imtgql::CGqlRequest& gqlRequest, imtgql::CGqlObject& gqlObject, QString& errorMessage, int& operationType) const;
	virtual imtbase::CTreeItemModel* GetSettings(const QList<imtgql::CGqlObject>& inputParams, const imtgql::CGqlObject& gqlObject, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* SaveSettings(const QList<imtgql::CGqlObject>& inputParams, const imtgql::CGqlObject& gqlObject, QString& errorMessage) const;

private:
	I_REF(iprm::IParamsSet, m_settingsParamsSetCompPtr);
};


} // namespace imtguigql



#pragma once


// ACF includes
#include <iqt/ITranslationManager.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtgql/ICommandPermissionsProvider.h>
#include <imtauth/ICheckPermission.h>



namespace imtqml
{


class CCommandPermissionsFilterComp:
		public icomp::CComponentBase,
		public imtbase::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCommandPermissionsFilterComp);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_commandPermissionsProviderCompPtr, "CommandPermissions", "Command Permissions", true, "");
		I_ASSIGN(m_commandsProviderCompPtr, "CommandsDataProvider", "Commands Data Provider", true, "");
		I_ASSIGN(m_checkPermissionCompPtr, "CheckPermissions", "Check permissions", false, "CheckPermissions");
	I_END_COMPONENT;

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QList<imtgql::CGqlObject>& params, const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;

private:
	I_REF(imtgql::ICommandPermissionsProvider, m_commandPermissionsProviderCompPtr);
	I_REF(imtbase::IItemBasedRepresentationDataProvider, m_commandsProviderCompPtr);
	I_REF(imtauth::ICheckPermission, m_checkPermissionCompPtr);
};


} // namespace imtqml



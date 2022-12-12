#pragma once


// ACF includes
#include <iqt/ITranslationManager.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtgql/IItemBasedRepresentationDataProvider.h>
#include <imtgql/ICommandPermissionsProvider.h>
#include <imtauth/ICheckPermission.h>



namespace imtqml
{


class CCommandPermissionsFilterComp:
		public icomp::CComponentBase,
		public imtgql::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCommandPermissionsFilterComp);
		I_REGISTER_INTERFACE(imtgql::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_commandPermissionsProviderCompPtr, "CommandPermissions", "Command Permissions", true, "");
		I_ASSIGN(m_commandsProviderCompPtr, "CommandsDataProvider", "Commands Data Provider", true, "");
		I_ASSIGN(m_checkPermissionCompPtr, "CheckPermissions", "Check permissions", false, "CheckPermissions");
	I_END_COMPONENT;

	// reimplemented (imtgql::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetRepresentation(const QList<imtgql::CGqlObject>& params, const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;

private:
	I_REF(imtgql::ICommandPermissionsProvider, m_commandPermissionsProviderCompPtr);
	I_REF(imtgql::IItemBasedRepresentationDataProvider, m_commandsProviderCompPtr);
	I_REF(imtauth::ICheckPermission, m_checkPermissionCompPtr);
};


} // namespace imtqml



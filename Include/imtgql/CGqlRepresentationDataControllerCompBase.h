#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgql/IGqlRepresentationDataController.h>
#include <imtgql/ICommandPermissionsProvider.h>
#include <imtauth/ICheckPermission.h>


namespace imtgql
{


class CGqlRepresentationDataControllerCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtgql::IGqlRepresentationDataController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CGqlRepresentationDataControllerCompBase);
		I_REGISTER_INTERFACE(imtgql::IGqlRepresentationDataController);
		I_ASSIGN_MULTI_0(m_modelIdsCompPtr, "ModelIds", "List of model-IDs for GraphQL-response", true);
		I_ASSIGN_MULTI_0(m_contextIdsCompPtr, "ContextIds", "List of context-IDs for GraphQL-response", false);
		I_ASSIGN(m_commandPermissionsCompPtr, "CommandPermissions", "Command Permissions", false, "Command Permissions");
        I_ASSIGN(m_checkPermissionCompPtr, "CheckPermissions", "Check permissions", false, "CheckPermissions");
	I_END_COMPONENT;


    // reimplemented (imtgql::IGqlRepresentationDataController)
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual QByteArrayList GetModelIds() const override;
	virtual QByteArrayList GetContextIds() const override;

protected:
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
    virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

	I_MULTIATTR(QByteArray, m_modelIdsCompPtr);
    I_MULTIATTR(QByteArray, m_contextIdsCompPtr);
	I_REF(imtgql::ICommandPermissionsProvider, m_commandPermissionsCompPtr);
    I_REF(imtauth::ICheckPermission, m_checkPermissionCompPtr);


};


} // namespace imtgql



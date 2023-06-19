#include <imtgql/CGqlRequestHandlerCompBase.h>


namespace imtgql
{


// public methods

// reimplemented (imtgql::IGqlRequestHandler)

bool CGqlRequestHandlerCompBase::IsRequestSupported(const CGqlRequest& gqlRequest) const
{
	QByteArray commandId = gqlRequest.GetCommandId();

	return m_commandIdsAttrPtr.FindValue(commandId) != -1;
}


imtbase::CTreeItemModel* CGqlRequestHandlerCompBase::CreateResponse(const CGqlRequest& gqlRequest, QString& errorMessage) const
{
	Q_ASSERT(IsRequestSupported(gqlRequest));

	if (!IsRequestSupported(gqlRequest)){
		return nullptr;
	}

	if(CheckPermissions(gqlRequest, errorMessage)){
		return CreateInternalResponse(gqlRequest, errorMessage);
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	QString userName;
	if (gqlRequest.GetGqlContext() != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlRequest.GetGqlContext()->GetUserInfo();

		if (userInfoPtr != nullptr){
			userName = userInfoPtr->GetName();
		}
	}

	errorMessage = QT_TR_NOOP("Invalid permissions for " + userName);

	imtbase::CTreeItemModel* errorsItemModelPtr = rootModelPtr->AddTreeModel("errors");
	errorsItemModelPtr->SetData("message", errorMessage);

	return rootModelPtr.PopPtr();
}


// protected methods

iprm::IParamsSet* CGqlRequestHandlerCompBase::CreateContextParams(const imtgql::CGqlRequest& gqlRequest) const
{
	return nullptr;
}


bool CGqlRequestHandlerCompBase::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	bool result = true;
	if (gqlRequest.GetGqlContext() != nullptr){
		const QList<imtgql::CGqlObject> paramsPtr = gqlRequest.GetParams();
		QByteArray productId;
		if (!paramsPtr.empty()){
			productId = paramsPtr.at(0).GetFieldArgumentValue("ProductId").toByteArray();
		}

		QByteArray gqlCommand = gqlRequest.GetCommandId();

		const imtauth::IUserInfo* userInfoPtr = gqlRequest.GetGqlContext()->GetUserInfo();
		if (userInfoPtr != nullptr){
			QByteArray userId = userInfoPtr->GetId();
			if (!userInfoPtr->IsAdmin()){
				if(m_commandPermissionsCompPtr.IsValid()){
					imtauth::IUserInfo::FeatureIds permissions = userInfoPtr->GetPermissions(productId);
					QByteArray gqlCommand = gqlRequest.GetCommandId();
					QByteArrayList commandIds = m_commandPermissionsCompPtr->GetCommandIds();
					if(commandIds.contains(gqlCommand)){
						QByteArrayList permissionIds = m_commandPermissionsCompPtr->GetCommandPermissions(gqlCommand);
						if (m_checkPermissionCompPtr.IsValid()){
							result = m_checkPermissionCompPtr->CheckPermission(permissions, permissionIds);
						}
					}
				}
			}
		}
	}

	return result;
}


} // namespace imtgql



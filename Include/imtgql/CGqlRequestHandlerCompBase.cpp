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
		SendErrorMessage(0, QString("GQL handler is not supported GQL Request with command %1").arg(qPrintable(gqlRequest.GetCommandId())));

		return nullptr;
	}

	if(CheckPermissions(gqlRequest, errorMessage)){
		return CreateInternalResponse(gqlRequest, errorMessage);
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	QString userName;
	if (gqlRequest.GetRequestContext() != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlRequest.GetRequestContext()->GetUserInfo();

		if (userInfoPtr != nullptr){
			userName = userInfoPtr->GetName();
		}
	}

	SendErrorMessage(0, QString("Unable to create response for command %1 because invalid permissions for %1").arg(qPrintable(gqlRequest.GetCommandId())).arg(userName));
	errorMessage = QT_TR_NOOP("Invalid permissions for " + userName);

	imtbase::CTreeItemModel* errorsItemModelPtr = rootModelPtr->AddTreeModel("errors");
	errorsItemModelPtr->SetData("message", errorMessage);

	return rootModelPtr.PopPtr();
}


// protected methods

iprm::IParamsSet* CGqlRequestHandlerCompBase::CreateContextParams(const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	return nullptr;
}


bool CGqlRequestHandlerCompBase::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
{
	bool retVal = true;

	if (gqlRequest.GetRequestContext() != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlRequest.GetRequestContext()->GetUserInfo();
		if (userInfoPtr != nullptr){
			QByteArray userId = userInfoPtr->GetId();
			if (!userInfoPtr->IsAdmin()){
				if(m_commandPermissionsCompPtr.IsValid()){
					imtauth::IUserInfo::FeatureIds permissions = userInfoPtr->GetPermissions();
					QByteArray requestedCommandId = gqlRequest.GetCommandId();
					QByteArrayList commandIds = m_commandPermissionsCompPtr->GetCommandIds();
					if(commandIds.contains(requestedCommandId)){
						QByteArrayList permissionIds = m_commandPermissionsCompPtr->GetCommandPermissions(requestedCommandId);
						if (m_checkPermissionCompPtr.IsValid()){
							retVal = m_checkPermissionCompPtr->CheckPermission(permissions, permissionIds);
						}
					}
				}
			}
		}
	}

	return retVal;
}


} // namespace imtgql



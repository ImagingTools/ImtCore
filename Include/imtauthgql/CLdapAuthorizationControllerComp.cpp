#include <imtauthgql/CLdapAuthorizationControllerComp.h>


// Windows includes
#ifdef Q_OS_WIN
#include <windows.h>
#include <LMaccess.h>
#include <Lmapibuf.h>
#pragma comment(lib, "netapi32.lib")
#endif

// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>
#include <iprm/CEnableableParam.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>


namespace imtauthgql
{


// protected methods

QByteArray CLdapAuthorizationControllerComp::CheckExistsRole(const QByteArray& productId, RoleType roleType) const
{
	iprm::CParamsSet filterParam;
	iprm::CParamsSet paramsSet;

	iprm::CEnableableParam enableableParam;
	enableableParam.SetEnabled(true);

	switch (roleType) {
	case RT_DEFAULT:
		paramsSet.SetEditableParameter("IsDefault", &enableableParam);
		break;
	case RT_GUEST:
		paramsSet.SetEditableParameter("IsGuest", &enableableParam);
		break;
	}

	iprm::CTextParam productIdParam;
	productIdParam.SetText(productId);

	paramsSet.SetEditableParameter("ProductId", &productIdParam);

	filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

	imtbase::IObjectCollection::Ids roleIds = m_roleCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (roleIds.isEmpty()){
		return QByteArray();
	}

	return roleIds[0];
}


QByteArray CLdapAuthorizationControllerComp::InsertNewIdentifiableRoleInfo(
	const QByteArray& roleId,
	const QString& roleName,
	const QString& description,
	const QByteArray& productId,
	bool isDefault,
	bool isGuest) const
{
	istd::TDelPtr<imtauth::CIdentifiableRoleInfo> roleInfoPtr = new imtauth::CIdentifiableRoleInfo();
	if (!roleInfoPtr.IsValid()){
		return QByteArray();
	}

	QByteArray objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	roleInfoPtr->SetObjectUuid(objectId);
	roleInfoPtr->SetRoleId(roleId);
	roleInfoPtr->SetRoleName(roleName);
	roleInfoPtr->SetRoleDescription(description);
	roleInfoPtr->SetDefault(isDefault);
	roleInfoPtr->SetGuest(isGuest);
	roleInfoPtr->SetProductId(productId);

	QByteArray result = m_roleCollectionCompPtr->InsertNewObject(
		QByteArray("Role"),
		roleInfoPtr->GetRoleName(),
		roleInfoPtr->GetRoleDescription(),
		roleInfoPtr.GetPtr(),
		objectId);

	if (result.isEmpty()){
		return QByteArray();
	}

	return objectId;
}


const imtauth::CUserInfo* CLdapAuthorizationControllerComp::CreateUserInfoFromLdapUser(const QByteArray& ldapUserId) const
{
#ifdef Q_OS_WIN
	LPUSER_INFO_3 userInfo3BufPtr = NULL;

	QByteArray domain = ".";
	QByteArray username = ldapUserId;

	QByteArrayList data = ldapUserId.split('\\');
	if (data.size() >= 2){
		domain = data[0];
		username = data[1];
	}

	// Get the computer name of a DC for the specified domain.
	LPBYTE computerName;
	NetGetDCName(NULL, qUtf16Printable(domain), &computerName);

	NetUserGetInfo((LPWSTR)computerName, qUtf16Printable(ldapUserId), 3, (LPBYTE *)&userInfo3BufPtr);

	if (userInfo3BufPtr != nullptr){
		istd::TDelPtr<imtauth::CIdentifiableUserInfo> userInfoPtr;
		userInfoPtr.SetPtr(new imtauth::CIdentifiableUserInfo);

		userInfoPtr->SetObjectUuid(ldapUserId);

		imtauth::IUserInfo::SystemInfo systemInfo;
		systemInfo.systemId = *m_systemIdAttrPtr;
		systemInfo.systemName = "LDAP";

		userInfoPtr->AddToSystem(systemInfo);
		userInfoPtr->SetId(ldapUserId);

		QByteArray password = QString::fromWCharArray(userInfo3BufPtr->usri3_password).toUtf8();
		userInfoPtr->SetPasswordHash(password);

		QString name = QString::fromWCharArray(userInfo3BufPtr->usri3_full_name);
		if (!name.isEmpty()){
			userInfoPtr->SetName(name);
		}
		else{
			userInfoPtr->SetName(ldapUserId);
		}

		QString description = QString::fromWCharArray(userInfo3BufPtr->usri3_comment);
		userInfoPtr->SetDescription(description);

		NetApiBufferFree(userInfo3BufPtr);

		return userInfoPtr.PopPtr();
	}
#endif

	return nullptr;
}


sdl::imtauth::Authorization::CAuthorizationPayload::V1_0 CLdapAuthorizationControllerComp::OnAuthorization(
			const sdl::imtauth::Authorization::V1_0::CAuthorizationGqlRequest& authorizationRequest,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (m_enableableParamCompPtr.IsValid()){
		bool enabled = m_enableableParamCompPtr->IsEnabled();
		if (enabled){
			sdl::imtauth::Authorization::CAuthorizationInput::V1_0 inputArgument = authorizationRequest.GetRequestedArguments().input;

			QByteArray login;
			if (inputArgument.Login){
				login = inputArgument.Login->toUtf8();
			}
			QByteArray productId;
			if (inputArgument.ProductId){
				productId = *inputArgument.ProductId;
			}
			QByteArray password;
			if (inputArgument.Password){
				password = inputArgument.Password->toUtf8();
			}

			QByteArray userObjectId = GetUserObjectId(login);

			bool ok = CheckCredential(*m_systemIdAttrPtr, login, password);
			if (ok){
				QByteArray guestRoleId = CheckExistsRole(productId, RT_GUEST);
				if (guestRoleId.isEmpty()){
					InsertNewIdentifiableRoleInfo("Guest", "Guest", "Guest role", productId, false, true);
				}

				QByteArray defaultRoleId = CheckExistsRole(productId, RT_DEFAULT);
				if (defaultRoleId.isEmpty()){
					defaultRoleId = InsertNewIdentifiableRoleInfo("Default", "Default", "Default role", productId, true, false);
				}

				istd::TDelPtr<imtauth::CUserInfo> userInfoPtr;
				if (userObjectId.isEmpty()){
					userInfoPtr.SetCastedOrRemove(const_cast<imtauth::CUserInfo*>(CreateUserInfoFromLdapUser(login)));
					if (!userInfoPtr.IsValid()){
						userInfoPtr.SetPtr(new imtauth::CIdentifiableUserInfo);

						userInfoPtr->SetId(login);
					}

					imtauth::IUserInfo::SystemInfo systemInfo;
					systemInfo.systemId = *m_systemIdAttrPtr;
					systemInfo.systemName = "LDAP";
					userInfoPtr->AddToSystem(systemInfo);

					userInfoPtr->AddRole(productId, defaultRoleId);

					m_userCollectionCompPtr->InsertNewObject("User", "", "", userInfoPtr.GetPtr(), login);
				}
				else{
					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
						userInfoPtr.SetCastedOrRemove(dataPtr.GetPtr()->CloneMe());
						if (userInfoPtr.IsValid()){
							QByteArrayList products = userInfoPtr->GetProducts();
							if (!products.contains(productId)){
								userInfoPtr->AddRole(productId, defaultRoleId);
								if (!m_userCollectionCompPtr->SetObjectData(userObjectId, *userInfoPtr.GetPtr())){
									SendWarningMessage(
										0,
										QString("Unable to set product '%1' for user '%2'").arg(qPrintable(productId), qPrintable(userObjectId)),
										"CLdapAuthorizationControllerComp");
								}
							}
						}
					}
				}

				if (userInfoPtr.IsValid()){
					userInfoPtr->SetId(login);
					return CreateAuthorizationSuccessfulResponse(*userInfoPtr.GetPtr(), *m_systemIdAttrPtr, productId, errorMessage);
				}
				else{
					return CreateInvalidLoginOrPasswordResponse(login, errorMessage);
				}
			}
		}
	}

	return BaseClass::OnAuthorization(authorizationRequest, gqlRequest, errorMessage);
}


} // namespace imtauthgql



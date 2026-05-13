// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	if (m_roleCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::Ids roleIds = m_roleCollectionCompPtr->GetElementIds();
		for (const QByteArray& roleId : roleIds){
			imtbase::IObjectCollection::DataPtr roleDataPtr;
			if (m_roleCollectionCompPtr->GetObjectData(roleId, roleDataPtr)){
				const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(roleDataPtr.GetPtr());
				if (roleInfoPtr != nullptr){
					QByteArray roleProductId = roleInfoPtr->GetProductId();
					if (roleType == RT_DEFAULT && roleInfoPtr->IsDefault() && roleProductId == productId ||
						roleType == RT_GUEST && roleInfoPtr->IsGuest() && roleProductId == productId){
						return roleId;
					}
				}
			}
		}
	}

	return QByteArray();
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


istd::TUniqueInterfacePtr<imtauth::IUserInfo> CLdapAuthorizationControllerComp::CreateUserInfoFromLdapUser(const QByteArray& ldapUserId) const
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

		userInfoPtr->SetObjectUuid(QUuid::createUuid().toByteArray(QUuid::WithoutBraces));

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

#else
	Q_UNUSED(ldapUserId)
#endif

	return nullptr;
}


sdl::imtauth::Authorization::CAuthorizationPayload CLdapAuthorizationControllerComp::OnAuthorization(
			const sdl::imtauth::Authorization::CAuthorizationGqlRequest& authorizationRequest,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (m_enableableParamCompPtr.IsValid()){
		bool enabled = m_enableableParamCompPtr->IsEnabled();
		if (enabled){

			sdl::imtauth::Authorization::AuthorizationRequestArguments arguments = authorizationRequest.GetRequestedArguments();
			if (!arguments.input.Version_1_0.has_value()){
				Q_ASSERT(false);
				return sdl::imtauth::Authorization::CAuthorizationPayload();
			}

			sdl::imtauth::Authorization::CAuthorizationInput::V1_0 inputArgument = *arguments.input.Version_1_0;

			QByteArray login;
			if (inputArgument.login){
				login = inputArgument.login->toUtf8();
			}

			QByteArray productId;
			if (inputArgument.productId){
				productId = *inputArgument.productId;
			}

			QByteArray password;
			if (inputArgument.password){
				password = inputArgument.password->toUtf8();
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

				istd::TUniqueInterfacePtr<imtauth::CIdentifiableUserInfo> userInfoPtr;
				if (userObjectId.isEmpty()){
					userInfoPtr.MoveCastedPtr(CreateUserInfoFromLdapUser(login));
					if (!userInfoPtr.IsValid()){
						userInfoPtr.SetPtr(new imtauth::CIdentifiableUserInfo);
						userInfoPtr->SetObjectUuid(QUuid::createUuid().toByteArray(QUuid::WithoutBraces));
						userInfoPtr->SetId(login);
						userInfoPtr->SetName(login);
					}

					userObjectId = userInfoPtr->GetObjectUuid();

					imtauth::IUserInfo::SystemInfo systemInfo;
					systemInfo.systemId = *m_systemIdAttrPtr;
					systemInfo.systemName = "LDAP";
					userInfoPtr->AddToSystem(systemInfo);

					userInfoPtr->AddRole(productId, defaultRoleId);

					QByteArray retVal = m_userCollectionCompPtr->InsertNewObject("User", "", "", userInfoPtr.GetPtr(), userObjectId);
					if (retVal.isEmpty()){
						errorMessage = QString("Unable to insert LDAP user to the collection");
						return sdl::imtauth::Authorization::CAuthorizationPayload();
					}
				}
				else{
					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
						userInfoPtr.MoveCastedPtr(dataPtr.GetPtr()->CloneMe());
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
					sdl::imtauth::Authorization::CAuthorizationPayload retVal;
					retVal = CreateAuthorizationSuccessfulResponse(*userInfoPtr.GetPtr(), *m_systemIdAttrPtr, productId, errorMessage);

					return retVal;
				}

				sdl::imtauth::Authorization::CAuthorizationPayload retVal;
				retVal = CreateInvalidLoginOrPasswordResponse(login, errorMessage);

				return retVal;
			}
		}
	}

	return BaseClass::OnAuthorization(authorizationRequest, gqlRequest, errorMessage);
}


} // namespace imtauthgql



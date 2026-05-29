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
#include <imtauth/CLdapUserCollectionControllerComp.h>
#include <imtbase/CComplexCollectionFilter.h>


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

		QByteArray sid = imtauth::CLdapUserCollectionControllerComp::GetSidForUser(ldapUserId);
		userInfoPtr->SetSid(sid);

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

		QString email = imtauth::CLdapUserCollectionControllerComp::GetEmailForUser(ldapUserId);
		if (!email.isEmpty()){
			userInfoPtr->SetMail(email);
		}

		NetApiBufferFree(userInfo3BufPtr);

		return userInfoPtr.PopPtr();
	}

#else
	Q_UNUSED(ldapUserId)
#endif

	return nullptr;
}


QByteArray CLdapAuthorizationControllerComp::GetUserObjectIdBySid(const QByteArray& sid) const
{
	if (sid.isEmpty()){
		return QByteArray();
	}

	imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds();
	for (const QByteArray& userId : userIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
			const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
			if (userInfoPtr != nullptr && userInfoPtr->GetSid() == sid){
				return userId;
			}
		}
	}

	return QByteArray();
}


sdl::V1_0::imtauth::CAuthorizationPayload CLdapAuthorizationControllerComp::OnAuthorization(
			const sdl::V1_0::imtauth::CAuthorizationGqlRequest& authorizationRequest,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (m_enableableParamCompPtr.IsValid()){
		bool enabled = m_enableableParamCompPtr->IsEnabled();
		if (enabled){

			sdl::V1_0::imtauth::AuthorizationRequestArguments arguments = authorizationRequest.GetRequestedArguments();
			if (!arguments.input.Version_1_0.has_value()){
				Q_ASSERT(false);
				return sdl::V1_0::imtauth::CAuthorizationPayload();
			}

			sdl::V1_0::imtauth::CAuthorizationInput inputArgument = *arguments.input.Version_1_0;

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

			// If user not found by login name, try to find by SID
			// This handles the case where user logged in with a different login format
			if (userObjectId.isEmpty()){
				QByteArray sid = imtauth::CLdapUserCollectionControllerComp::GetSidForUser(login);
				if (!sid.isEmpty()){
					userObjectId = GetUserObjectIdBySid(sid);
				}
			}

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

						QByteArray sid = imtauth::CLdapUserCollectionControllerComp::GetSidForUser(login);
						userInfoPtr->SetSid(sid);

						QString mail = imtauth::CLdapUserCollectionControllerComp::GetEmailForUser(login);
						userInfoPtr->SetMail(mail);
					}

					userObjectId = userInfoPtr->GetObjectUuid();

					imtauth::IUserInfo::SystemInfo systemInfo;
					systemInfo.systemId = *m_systemIdAttrPtr;
					systemInfo.systemName = "LDAP";
					userInfoPtr->AddToSystem(systemInfo);

					userInfoPtr->AddRole(productId, defaultRoleId);

					QByteArray retVal = m_userCollectionCompPtr->InsertNewObject("User", userInfoPtr->GetName(), "", userInfoPtr.GetPtr(), userObjectId);
					if (retVal.isEmpty()){
						errorMessage = QString("Unable to insert LDAP user to the collection");
						return sdl::V1_0::imtauth::CAuthorizationPayload();
					}
				}
				else{
					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
						userInfoPtr.MoveCastedPtr(dataPtr.GetPtr()->CloneMe());
						if (userInfoPtr.IsValid()){
							bool needsUpdate = false;

							// Update login in case the user logged in with a different format
							if (userInfoPtr->GetId() != login){
								userInfoPtr->SetId(login);
								needsUpdate = true;
							}

							// Ensure SID is set (may be empty for users created before SID support)
							if (userInfoPtr->GetSid().isEmpty()){
								QByteArray sid = imtauth::CLdapUserCollectionControllerComp::GetSidForUser(login);
								userInfoPtr->SetSid(sid);
								needsUpdate = true;
							}

							// Ensure email is set (may be empty for users created before email lookup)
							if (userInfoPtr->GetMail().isEmpty()){
								QString email = imtauth::CLdapUserCollectionControllerComp::GetEmailForUser(login);
								if (!email.isEmpty()){
									userInfoPtr->SetMail(email);
									needsUpdate = true;
								}
							}

							QByteArrayList products = userInfoPtr->GetProducts();
							if (!products.contains(productId)){
								userInfoPtr->AddRole(productId, defaultRoleId);
								needsUpdate = true;
							}

							if (needsUpdate){
								if (!m_userCollectionCompPtr->SetObjectData(userObjectId, *userInfoPtr.GetPtr())){
									SendWarningMessage(
										0,
										QString("Unable to update user data for user '%1'").arg(qPrintable(userObjectId)),
										"CLdapAuthorizationControllerComp");
								}
							}
						}
					}
				}

				if (userInfoPtr.IsValid()){
					userInfoPtr->SetId(login);
					sdl::V1_0::imtauth::CAuthorizationPayload retVal;
					retVal = CreateAuthorizationSuccessfulResponse(*userInfoPtr.GetPtr(), *m_systemIdAttrPtr, productId, errorMessage);

					return retVal;
				}

				sdl::V1_0::imtauth::CAuthorizationPayload retVal;
				retVal = CreateInvalidLoginOrPasswordResponse(login, errorMessage);

				return retVal;
			}
		}
	}

	return BaseClass::OnAuthorization(authorizationRequest, gqlRequest, errorMessage);
}


} // namespace imtauthgql



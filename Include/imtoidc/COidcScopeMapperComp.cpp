// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoidc/COidcScopeMapperComp.h>


// Qt includes
#include <QtCore/QJsonArray>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtauth/IPersonBaseInfo.h>
#include <imtauth/IContactBaseInfo.h>
#include <imtauth/IUserRoleInfo.h>


namespace imtoidc
{


// protected methods

// reimplemented (imtauth::IOidcScopeMapper)

QJsonObject COidcScopeMapperComp::MapScopesToClaims(const QByteArray& userId, const QByteArrayList& scopes) const
{
	QJsonObject claims;

	// "sub" is always included when "openid" scope is present
	if (scopes.contains("openid")){
		claims["sub"] = QString::fromUtf8(userId);
	}

	if (!m_userCollectionCompPtr.IsValid()){
		return claims;
	}

	// Look up user data
	imtbase::IObjectCollection::DataPtr userDataPtr;
	if (!m_userCollectionCompPtr->GetObjectData(userId, userDataPtr)){
		SendWarningMessage(0, QString("User not found for OIDC claims: %1").arg(QString::fromUtf8(userId)),
						"COidcScopeMapperComp");
		return claims;
	}

	// Map "profile" scope
	if (scopes.contains("profile")){
		const imtauth::IPersonBaseInfo* personPtr =
			dynamic_cast<const imtauth::IPersonBaseInfo*>(userDataPtr.GetPtr());
		if (personPtr != nullptr){
			QString firstName = personPtr->GetFirstName();
			QString lastName = personPtr->GetLastName();

			if (!firstName.isEmpty()){
				claims["given_name"] = firstName;
			}
			if (!lastName.isEmpty()){
				claims["family_name"] = lastName;
			}
			if (!firstName.isEmpty() || !lastName.isEmpty()){
				claims["name"] = (firstName + " " + lastName).trimmed();
			}
		}
	}

	// Map "email" scope
	if (scopes.contains("email")){
		const imtauth::IContactBaseInfo* contactPtr =
			dynamic_cast<const imtauth::IContactBaseInfo*>(userDataPtr.GetPtr());
		if (contactPtr != nullptr){
			QString email = contactPtr->GetEmail();
			if (!email.isEmpty()){
				claims["email"] = email;
				claims["email_verified"] = true;
			}
		}
	}

	// Map "roles" scope
	if (scopes.contains("roles")){
		const imtauth::IUserRoleInfo* roleInfoPtr =
			dynamic_cast<const imtauth::IUserRoleInfo*>(userDataPtr.GetPtr());
		if (roleInfoPtr != nullptr){
			QByteArrayList roleIds = roleInfoPtr->GetRoleIds();
			QJsonArray rolesArray;
			for (const QByteArray& roleId : ::std::as_const(roleIds)){
				rolesArray.append(QString::fromUtf8(roleId));
			}
			claims["roles"] = rolesArray;
		}
	}

	// Map "permissions" scope
	if (scopes.contains("permissions")){
		const imtauth::IUserInfo* userInfoPtr =
			dynamic_cast<const imtauth::IUserInfo*>(userDataPtr.GetPtr());
		if (userInfoPtr != nullptr){
			imtauth::IUserInfo::FeatureIds permissions = userInfoPtr->GetPermissions();
			QJsonArray permissionsArray;
			for (const QByteArray& perm : ::std::as_const(permissions)){
				permissionsArray.append(QString::fromUtf8(perm));
			}
			claims["permissions"] = permissionsArray;
		}
	}

	return claims;
}


QByteArrayList COidcScopeMapperComp::GetSupportedScopes() const
{
	return QByteArrayList()
		<< "openid"
		<< "profile"
		<< "email"
		<< "roles"
		<< "permissions";
}


QByteArrayList COidcScopeMapperComp::GetSupportedClaims() const
{
	return QByteArrayList()
		<< "sub"
		<< "name"
		<< "given_name"
		<< "family_name"
		<< "email"
		<< "email_verified"
		<< "roles"
		<< "permissions";
}


} // namespace imtoidc



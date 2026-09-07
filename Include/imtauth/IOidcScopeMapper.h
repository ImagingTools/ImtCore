// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QByteArrayList>


namespace imtauth
{


/**
	Interface for mapping OIDC scopes to user claims.

	This interface provides the bridge between OIDC scopes and the
	Puma RBAC model. Implementations map standard OIDC scopes
	(openid, profile, email) and custom scopes to the appropriate
	user claims from IUserInfoProvider, IPersonBaseInfo, IContactInfo,
	and IPermissionsProvider/IRole.

	\ingroup Authorization
*/
class IOidcScopeMapper: virtual public istd::IPolymorphic
{
public:
	/**
		Map OIDC scopes to user claims for a given user.
		\param userId User identifier to retrieve claims for
		\param scopes List of granted scope identifiers
		\return JSON object containing the mapped claims
	*/
	virtual QJsonObject MapScopesToClaims(const QByteArray& userId, const QByteArrayList& scopes) const = 0;

	/**
		Get list of all supported scopes.
		\return List of supported scope identifiers
	*/
	virtual QByteArrayList GetSupportedScopes() const = 0;

	/**
		Get list of all supported claims.
		\return List of supported claim names
	*/
	virtual QByteArrayList GetSupportedClaims() const = 0;
};


} // namespace imtauth



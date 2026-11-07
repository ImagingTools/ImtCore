// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlContext.h>
#include <imtgql/CGqlRequest.h>

// Qt includes
#include <QtCore/QString>


namespace imtgql
{


class IGqlContextCreator: virtual public istd::IPolymorphic
{
public:
	enum ContextCreationStatus
	{
		CCS_OK,
		CCS_UNAUTHORIZED,
		CCS_FORBIDDEN,
		CCS_INTERNAL_ERROR
	};

	struct ContextCreationError
	{
		ContextCreationStatus status = CCS_OK;
		QString message;
	};

	virtual IGqlContextUniquePtr CreateGqlContext(
				const IGqlContext::Headers& headers,
				ContextCreationError& error) const = 0;

	/**
		Tell whether the token lives until it is revoked, so what it holds must be checked again later.
	*/
	virtual bool IsRevocableToken(const QByteArray& token) const = 0;

	/**
		Check the token only, without creating a context for its owner.
		\param scopes The permissions of a personal access token.
		\return CCS_INTERNAL_ERROR when the token could not be checked: that says nothing about the token.
	*/
	virtual ContextCreationStatus CheckToken(const QByteArray& token, QByteArrayList& scopes, QString& errorMessage) const = 0;

	/**
		CheckToken() for the token another server proves itself with (header x-server-token): it is checked where
		this server keeps the tokens it issued.
	*/
	virtual ContextCreationStatus CheckServerToken(const QByteArray& token, QByteArrayList& scopes, QString& errorMessage) const = 0;

	/**
		Check that the server of a product the request comes through may send it.
		A request of a client of this server passes.
		\param requiredPermission The permission of the server token the request needs, empty when it needs none.
	*/
	virtual bool CheckRequestAccess(const CGqlRequest& gqlRequest, QByteArray& requiredPermission, QString& errorMessage) const = 0;
};


} // namespace imtgql

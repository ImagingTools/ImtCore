// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlContext.h>
#include <imtgql/CGqlRequest.h>

// Qt includes
#include <QtCore/QDateTime>
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

	enum AuthorizationState
	{
		/// The credentials and the access were read afresh
		AS_CONFIRMED,
		/// The credentials are refused
		AS_DENIED,
		/// The credentials or the access could not be read: nothing is known about them
		AS_UNAVAILABLE,
		/// The access changed while it was read: the result is not used, the check is repeated
		AS_STALE
	};

	virtual IGqlContextUniquePtr CreateGqlContext(
				const IGqlContext::Headers& headers,
				ContextCreationError& error) const = 0;

	/**
		Create the context of a subscription afresh: the credentials, the user, its roles and its groups are read at their sources.
		\param isEstablished	A new subscription is refused with an expired access token, like a request; an established one
								is accepted with it while its session lives.
		\param error			Why there is no context; its status tells a refused token from a token to refresh.
		\param sessionExpiresAtPtr	[optional] When the session of the credentials ends, as it is known now; invalid when it is not
								known (a token without a session, an authorization server that does not tell it). A session may be
								prolonged, so it is a reason to check again, not the end of the access.
		\return The context when the state is AS_CONFIRMED, otherwise null.
	*/
	virtual IGqlContextUniquePtr RenewGqlContext(
				const IGqlContext::Headers& headers,
				bool isEstablished,
				AuthorizationState& state,
				ContextCreationError& error,
				QDateTime* sessionExpiresAtPtr = nullptr) const = 0;

	/**
		Get the generation of the access: it grows with every change of the access this server is told about.
		A context created at an older generation may have been built from changed data.
	*/
	virtual qint64 GetAuthorizationGeneration() const = 0;
};


} // namespace imtgql

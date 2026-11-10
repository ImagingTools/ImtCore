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
		Create afresh the context of an established subscription from the headers it was registered with, without any cache.
		Unlike a new request, an expired access token is accepted while its session lives.
		\return The context when the state is AS_CONFIRMED, otherwise null.
	*/
	virtual IGqlContextUniquePtr RenewGqlContext(
				const IGqlContext::Headers& headers,
				AuthorizationState& state,
				QString& errorMessage) const = 0;

	/**
		Get the generation of the access: it grows with every change of the access this server is told about.
		A context created at an older generation may have been built from changed data.
	*/
	virtual qint64 GetAuthorizationGeneration() const = 0;
};


} // namespace imtgql

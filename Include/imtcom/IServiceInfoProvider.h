// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtcom/IConnectionStatusProvider.h>


namespace imtcom
{


/**
	Information about a service the application depends on, e.g. the authorization server of a product,
	as it was reported by the service itself.
	The service reports the services it depends on as well, so the information is a tree.
	Implementations may be called from any thread.
*/
class IServiceInfoProvider: virtual public istd::IPolymorphic
{
public:
	/**
		State of the token this server uses at the service.
	*/
	enum CredentialStatus
	{
		/**
			This server does not use a token at the service.
		*/
		CRS_NOT_USED,
		CRS_VALID,

		/**
			The token is not configured.
		*/
		CRS_MISSING,

		/**
			The service refuses the token: it is wrong, revoked or issued for another product.
		*/
		CRS_REJECTED,

		/**
			The service does not answer, the token could not be checked.
		*/
		CRS_UNAVAILABLE
	};

	struct ServiceInfo
	{
		/**
			Role of the service for the application that depends on it, e.g. "AuthServer".
		*/
		QByteArray serviceId;

		/**
			Runtime status of the communication with the service.
			The other members are only known while the status is \c CS_CONNECTED, except the application name of a disconnected service.
		*/
		IConnectionStatusProvider::ConnectionStatus status = IConnectionStatusProvider::CS_UNKNOWN;
		QString applicationName;
		QString version;
		QString versionKind;
		QDateTime buildTime;
		CredentialStatus credentialStatus = CRS_NOT_USED;

		/**
			Services the service itself depends on.
		*/
		QList<ServiceInfo> dependencies;
	};

	/**
		Get the current information about the service, also while it is not reachable.
	*/
	virtual ServiceInfo GetServiceInfo() const = 0;
};


} // namespace imtcom



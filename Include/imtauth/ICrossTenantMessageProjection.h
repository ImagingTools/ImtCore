// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QByteArray>

// ImtCore includes
#include <imtauth/ICrossTenantMessage.h>


namespace imtauth
{


/**
	Projection layer of the cross-tenant messaging pipeline (RFC phase 2, §12).

	A projection transforms a source payload into the subset of data that is
	allowed to cross the tenant boundary. Internal, sensitive or commercially
	confidential fields must be removed before a message leaves the source
	tenant. The broker invokes the projection on every outgoing message.

	\ingroup Tenant
*/
class ICrossTenantMessageProjection: virtual public istd::IPolymorphic
{
public:
	/**
		Project a source payload for the given message type.

		Implementations are expected to validate, filter and transform the
		payload so that only fields allowed for cross-tenant exchange remain.
		The payload is treated as a JSON object by the default implementation,
		but other encodings are possible.

		\param messageType The domain type of the message being projected.
		\param customType Concrete type name when \a messageType is CTMT_CUSTOM.
		\param sourcePayload The raw payload produced by the source tenant.
		\param isValid Output flag set to false when the payload is rejected.
		\return The projected payload to be stored and delivered.
	*/
	virtual QByteArray ProjectPayload(
				CrossTenantMessageType messageType,
				const QString& customType,
				const QByteArray& sourcePayload,
				bool& isValid) const = 0;
};


} // namespace imtauth

// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/ICrossTenantMessageProjection.h>


namespace imtauth
{


/**
	Generic JSON field-whitelist/blacklist projection.

	Treats the payload as a JSON object and keeps only the configured allowed
	fields (when \c AllowedFields is non-empty), then removes any blocked fields.
	When no allowed fields are configured all fields except the blocked ones are
	kept. Non-object payloads are passed through unchanged.
*/
class CCrossTenantMessageProjectionComp:
			public icomp::CComponentBase,
			virtual public ICrossTenantMessageProjection
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCrossTenantMessageProjectionComp);
		I_REGISTER_INTERFACE(ICrossTenantMessageProjection);
		I_ASSIGN_MULTI_0(m_allowedFieldsAttrPtr, "AllowedFields", "Payload fields allowed to cross the tenant boundary (empty = all)", false);
		I_ASSIGN_MULTI_0(m_blockedFieldsAttrPtr, "BlockedFields", "Payload fields that must never cross the tenant boundary", false);
	I_END_COMPONENT;

	virtual QByteArray ProjectPayload(
				CrossTenantMessageType messageType,
				const QString& customType,
				const QByteArray& sourcePayload,
				bool& isValid) const override;

private:
	I_MULTIATTR(QString, m_allowedFieldsAttrPtr);
	I_MULTIATTR(QString, m_blockedFieldsAttrPtr);
};


} // namespace imtauth

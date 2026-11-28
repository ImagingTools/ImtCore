// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtassist
{


/**
	Decision whether the assistant is enabled for a tenant.
	Used in addition to the server-wide switch of the assistant service:
	the assistant answers a tenant only if both allow it.

	\ingroup imtassist
*/
class IAssistantTenantPolicy: virtual public istd::IPolymorphic
{
public:
	/**
		Check whether the assistant is enabled for the tenant.
		Must not block for long; called for every request.
	*/
	virtual bool IsAssistantEnabledForTenant(const QByteArray& tenantId) const = 0;
};


} // namespace imtassist



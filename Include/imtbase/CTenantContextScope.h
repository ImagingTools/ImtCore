// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>


namespace imtbase
{


/**
	RAII scope establishing the tenant context of the current thread.
	The scope is typically activated at the beginning of request processing
	and consumed by storage components (e.g. tenant-aware SQL delegates)
	to resolve the physical storage location of the current tenant.
	Scopes can be nested; the previous tenant ID is restored on destruction.
*/
class CTenantContextScope
{
public:
	explicit CTenantContextScope(const QByteArray& tenantId);
	~CTenantContextScope();

	CTenantContextScope(const CTenantContextScope&) = delete;
	CTenantContextScope& operator=(const CTenantContextScope&) = delete;

	/**
		Get the tenant ID of the innermost active scope on the current thread.
		Returns an empty byte array if no scope is active.
	*/
	static QByteArray GetCurrentTenantId();

	/**
		Check if a tenant context scope is active on the current thread.
	*/
	static bool IsActive();

private:
	QByteArray m_previousTenantId;
	bool m_wasActive;
};


} // namespace imtbase

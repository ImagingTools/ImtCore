// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>


namespace imtbase
{


/**
	Tenant security context of the current thread.

	The context describes on whose behalf the data access is executed:
	- undefined context (default): no tenant and no user, data access to tenant-owned data must be denied (fail-closed);
	- tenant context: tenant-ID and user-ID of the current request;
	- system context: trusted internal/administrative operation (migrations, maintenance jobs) that may bypass tenant isolation.

	The context is stored per thread. Use CTenantSecurityContextScope to set it for a limited code block.
	\sa CTenantSecurityContextScope
*/
class CTenantSecurityContext
{
public:
	CTenantSecurityContext();
	CTenantSecurityContext(const QByteArray& tenantId, const QByteArray& userId);

	/**
		Create the context for trusted system/administrative operations.
	*/
	static CTenantSecurityContext CreateSystemContext();

	/**
		Get the context of the current thread.
	*/
	static CTenantSecurityContext GetCurrentContext();

	QByteArray GetTenantId() const;
	QByteArray GetUserId() const;
	bool IsSystemContext() const;

	/**
		Check if the context is defined (system context or context with tenant or user).
	*/
	bool IsDefined() const;

	bool operator==(const CTenantSecurityContext& other) const;
	bool operator!=(const CTenantSecurityContext& other) const;

protected:
	friend class CTenantSecurityContextScope;

	static void SetCurrentContext(const CTenantSecurityContext& context);

private:
	QByteArray m_tenantId;
	QByteArray m_userId;
	bool m_isSystemContext;
};


} // namespace imtbase



// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


namespace imtbase
{


/**
	RAII scope declaring that the current thread deliberately operates on the shared storage,
	e.g. the database migrations of the shared schema.
	Tenant-resolved storage components deny access without a tenant context (fail-closed);
	inside this scope they use their shared storage instead. A tenant context opened inside
	the scope takes precedence. Scopes can be nested.
*/
class CSharedStorageScope
{
public:
	CSharedStorageScope();
	~CSharedStorageScope();

	CSharedStorageScope(const CSharedStorageScope&) = delete;
	CSharedStorageScope& operator=(const CSharedStorageScope&) = delete;

	/**
		Check if a shared storage scope is active on the current thread.
	*/
	static bool IsActive();

private:
	bool m_wasActive;
};


} // namespace imtbase



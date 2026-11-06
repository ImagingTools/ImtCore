// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CStorageResolverRegistry.h>


namespace imtbase
{


void CStorageResolverRegistry::RegisterResolver(IStorageResolver* resolverPtr, const QByteArray& storageId)
{
	QWriteLocker locker(&m_lock);

	if (resolverPtr != nullptr){
		m_resolvers[storageId] = resolverPtr;
	}
	else{
		m_resolvers.remove(storageId);
	}
}


bool CStorageResolverRegistry::UnregisterResolver(const QByteArray& storageId)
{
	QWriteLocker locker(&m_lock);

	return m_resolvers.remove(storageId) > 0;
}


// reimplemented (imtbase::IStorageResolverProvider)

IStorageResolver* CStorageResolverRegistry::GetStorageResolver(const QByteArray& storageId) const
{
	QReadLocker locker(&m_lock);

	return m_resolvers.value(storageId, nullptr);
}


} // namespace imtbase



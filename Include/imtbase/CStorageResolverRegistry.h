// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QReadWriteLock>

// ImtCore includes
#include <imtbase/IStorageResolverProvider.h>


namespace imtbase
{


/**
	Registry of the storage resolvers by the logical storage ID.
	The application (or its configuration) fills the registry at the start and so selects the storage for each logical storage ID.
	An empty storage ID addresses the default resolver. An unknown non-empty storage ID is not mapped to the default resolver.
	The registry doesn't own the resolvers.
	\ingroup Collection
*/
class CStorageResolverRegistry: virtual public IStorageResolverProvider
{
public:
	/**
		Register the resolver for the logical storage ID. An empty ID registers the default resolver.
		A previously registered resolver for this ID will be replaced. The resolver must outlive the registry.
	*/
	void RegisterResolver(IStorageResolver* resolverPtr, const QByteArray& storageId = QByteArray());

	/**
		Remove the resolver registration for the given logical storage ID.
	*/
	bool UnregisterResolver(const QByteArray& storageId = QByteArray());

	// reimplemented (imtbase::IStorageResolverProvider)
	virtual IStorageResolver* GetStorageResolver(const QByteArray& storageId = QByteArray()) const override;

private:
	mutable QReadWriteLock m_lock;
	QMap<QByteArray, IStorageResolver*> m_resolvers;
};


} // namespace imtbase



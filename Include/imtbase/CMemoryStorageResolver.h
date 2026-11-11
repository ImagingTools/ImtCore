// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QReadWriteLock>

// ImtCore includes
#include <imtbase/IStorageResolver.h>


namespace imtbase
{


/**
	Storage resolver keeping the objects in the memory.
	The container is a private detail of the resolver, the objects are accessed only via IDs.
	The loaded objects are live instances (no snapshot semantic): changes are visible for all links immediately.
	The container access is thread-safe, the access to the stored objects is not.
	\ingroup Collection
*/
class CMemoryStorageResolver: virtual public IStorageResolver
{
public:
	/**
		Get number of the stored objects.
	*/
	int GetObjectsCount() const;

	// reimplemented (imtbase::IStorageResolver)
	virtual istd::IChangeableSharedPtr LoadObject(const Id& objectId) const override;
	virtual Objects LoadObjects(const QSet<Id>& objectIds) const override;
	virtual bool SaveObject(const Id& objectId, const istd::IChangeable& object) override;
	virtual Id InsertObject(const istd::IChangeableSharedPtr& objectPtr, const QString& name = QString()) override;
	virtual bool RemoveObject(const Id& objectId) override;
	virtual QVariantMap GetStorageParams(const Id& objectId) const override;
	virtual bool IsSnapshotStorage() const override;
	virtual const iser::IVersionInfo* GetVersionInfo() const override;

private:
	mutable QReadWriteLock m_lock;
	QHash<Id, istd::IChangeableSharedPtr> m_objects;
};


} // namespace imtbase



// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QReadWriteLock>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtservergql/IGqlContextCache.h>


namespace imtservergql
{


class CGqlContextCacheComp:
			public icomp::CComponentBase,
			virtual public IGqlContextCache
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlContextCacheComp);
		I_REGISTER_INTERFACE(imtservergql::IGqlContextCache);
	I_END_COMPONENT;

	// reimplemented (imtservergql::IGqlContextCache)
	virtual bool TryCreateRequestContext(
				const QByteArray& token,
				const QByteArray& productId,
				const imtgql::IGqlContext::Headers& headers,
				imtgql::IGqlContextSharedPtr& gqlContextPtr) const override;

	virtual void Insert(
				const QByteArray& token,
				const imtgql::IGqlContext* gqlContextPtr) const override;

private:
	struct CachedContextItem
	{
		imtgql::IGqlContextSharedPtr prototypeContextPtr;
		qint64 expirationTimeMs = 0;
		qint64 createdAtMs = 0;
	};

	void Remove(const QByteArray& token) const;
	void RemoveExpiredItems(qint64 currentTimeMs) const;
	void EnsureSpaceForOneMoreItem() const;

private:
	mutable QMap<QByteArray, CachedContextItem> m_cacheItems;
	mutable QReadWriteLock m_lock = QReadWriteLock(QReadWriteLock::Recursive);
};


} // namespace imtservergql


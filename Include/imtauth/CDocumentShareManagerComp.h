// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <atomic>

// Qt includes
#include <QtCore/QHash>
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IDocumentShareManager.h>
#include <imtauth/ITenantRelationshipProvider.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	Document share manager on top of an object collection of IDocumentShareInfo.
	A share is accepted on creation when AutoAccept is set, otherwise it waits for the target tenant.
*/
class CDocumentShareManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public IDocumentShareManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDocumentShareManagerComp);
		I_REGISTER_INTERFACE(IDocumentShareManager);
		I_ASSIGN(m_shareCollectionCompPtr, "ShareCollection", "Collection of the document shares", true, "ShareCollection");
		I_ASSIGN(m_shareFactoryCompPtr, "ShareFactory", "Factory of the document shares", true, "DocumentShareInfo");
		I_ASSIGN(m_relationshipProviderCompPtr, "RelationshipProvider", "Provider of the tenant relationships a share requires", true, "RelationshipProvider");
		I_ASSIGN(m_autoAcceptAttrPtr, "AutoAccept", "Activate new shares without the confirmation of the target tenant", true, true);
		I_ASSIGN(m_relationshipCacheTimeoutAttrPtr, "RelationshipCacheTimeout", "Seconds the related organizations of a tenant are reused before they are requested again", true, 10);
	I_END_COMPONENT;

	// reimplemented (imtauth::IDocumentShareManager)
	virtual QByteArray ShareDocument(const IDocumentShareInfo& shareInfo, QString* errorMessagePtr = nullptr) override;
	virtual bool RemoveShares(const QByteArrayList& shareIds) override;
	virtual bool SetShareStatus(const QByteArrayList& shareIds, IDocumentShareInfo::ShareStatus status) override;
	virtual bool SetShareTargetGroups(const QByteArray& shareId, const QByteArrayList& groupIds) override;
	virtual IDocumentShareInfoUniquePtr GetShare(const QByteArray& shareId) const override;
	virtual DocumentShares GetDocumentShares(const QByteArray& entityId) const override;
	virtual DocumentShares GetIncomingShares(const QByteArray& targetTenantId) const override;
	virtual DocumentShares GetOutgoingShares(const QByteArray& ownerTenantId) const override;
	virtual QByteArrayList GetShareOwnerTenantIds(const QByteArray& targetTenantId) const override;
	virtual int GetSharesRevision() const override;

private:
	DocumentShares FindShares(const QByteArray& fieldId, const QByteArray& value) const;
	bool StoreShare(const IDocumentShareInfo& shareInfo, bool isNew);

private:
	I_REF(imtbase::IObjectCollection, m_shareCollectionCompPtr);
	I_FACT(IDocumentShareInfo, m_shareFactoryCompPtr);
	I_REF(ITenantRelationshipProvider, m_relationshipProviderCompPtr);
	I_ATTR(bool, m_autoAcceptAttrPtr);
	I_ATTR(int, m_relationshipCacheTimeoutAttrPtr);

	struct RelatedTenantsCacheItem
	{
		qint64 expiresAt = 0;
		QByteArrayList tenantIds;
	};

	std::atomic<int> m_sharesRevision{0};

	mutable QMutex m_relatedTenantsCacheMutex;
	mutable QHash<QByteArray, RelatedTenantsCacheItem> m_relatedTenantsCache;
};


} // namespace imtauth



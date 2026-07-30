// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// Qt includes
#include <functional>

// ImtCore includes
#include <imtauth/IDocumentShare.h>
#include <imtauth/IDocumentShareData.h>
#include <imtauth/ITenantManager.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	Manager of document shares.

	Implements IDocumentShareManager on top of an IObjectCollection holding
	IDocumentShareData elements. The manager is the single owner of the document
	share entity: it creates shares, changes the granted access mode, revokes
	them and resolves the access a tenant currently has on a document.

	When a tenant manager is wired, CreateShare additionally verifies that the
	relationship referenced by the share is known by the source tenant, so a
	share can never be issued across an unrelated tenant boundary.
*/
class CDocumentShareManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::IDocumentShareManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDocumentShareManagerComp);
		I_REGISTER_INTERFACE(imtauth::IDocumentShareManager);
		I_ASSIGN(m_shareCollectionCompPtr, "ShareCollection", "Document share collection", false, "ShareCollection");
		I_ASSIGN(m_shareFactoryCompPtr, "ShareFactory", "Document share factory", false, "DocumentShareInfo");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager used for relationship validation", false, "TenantManager");
	I_END_COMPONENT;

	// reimplemented (imtauth::IDocumentShareManager)
	virtual QByteArray CreateShare(const DocumentShareInfo& shareInfo) override;
	virtual DocumentShareInfo GetShare(const QByteArray& shareId) const override;
	virtual DocumentShares GetIncomingShares(const QByteArray& targetTenantId) const override;
	virtual DocumentShares GetOutgoingShares(const QByteArray& sourceTenantId) const override;
	virtual DocumentShares GetSharesOfDocument(const QByteArray& documentId) const override;
	virtual bool SetAccessMode(const QByteArray& shareId, DocumentShareAccessMode accessMode) override;
	virtual bool RevokeShare(const QByteArray& shareId) override;
	virtual bool GetDocumentAccess(
				const QByteArray& tenantId,
				const QByteArray& documentId,
				DocumentShareAccessMode* accessModePtr = nullptr) const override;

private:
	DocumentShares CollectShares(const std::function<bool(const DocumentShareInfo&)>& predicate) const;
	bool StoreShareInfo(const QByteArray& shareId, const DocumentShareInfo& info);
	bool ValidateRelationship(const QByteArray& sourceTenantId, const QByteArray& relationshipId) const;

private:
	I_REF(imtbase::IObjectCollection, m_shareCollectionCompPtr);
	I_FACT(imtauth::IDocumentShareData, m_shareFactoryCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
};


} // namespace imtauth

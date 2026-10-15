// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>

// ImtCore includes
#include <imtgql/IGqlContext.h>


namespace imtgql
{


class CGqlContext: virtual public IGqlContext
{
public:
	CGqlContext();
	~CGqlContext();

	// reimplemented (IGqlContext)
	virtual QByteArray GetProductId() const override;
	virtual void SetProductId(const QByteArray& productId) override;
	virtual QByteArray GetLanguageId() const override;
	virtual void SetLanguageId(const QByteArray& languageId) override;
	virtual QByteArray GetDesignScheme() const override;
	virtual void SetDesignScheme(const QByteArray& designScheme) override;
	virtual QByteArray GetToken() const override;
	virtual void SetToken(const QByteArray& token) override;
	virtual QByteArrayList GetScopes() const override;
	virtual void SetScopes(const QByteArrayList& scopes) override;
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual QByteArray GetTenantId() const override;
	virtual void SetTenantId(const QByteArray& tenantId) override;
	virtual const imtauth::IUserInfo* GetUserInfo() const override;
	virtual void SetUserInfo(const imtauth::IUserInfo* userInfoPtr) override;
	virtual imtlic::IProductInfo* GetProductInfo() const override;
	virtual void SetProductInfo(imtlic::IProductInfo* productInfoPtr) override;
	virtual imtauth::IUserInfo::FeatureIds GetImpliedPermissions() const override;
	virtual bool IsTenantOwner() const override;
	virtual void SetIsTenantOwner(bool isTenantOwner) override;
	virtual Headers GetHeaders() const override;
	virtual void SetHeaders(const Headers& headers) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	/**
		Drop the resolved implied permissions, so the next request for them
		answers from the user and product this context carries now.
	*/
	void ResetImpliedPermissions();

private:
	QByteArray m_userId;
	QByteArray m_tenantId;
	QByteArray m_productId;
	QByteArray m_languageId;
	QByteArray m_designScheme;
	QByteArray m_token;
	QByteArrayList m_scopes;
	istd::TSharedInterfacePtr<imtauth::IUserInfo> m_userInfoPtr;

	// The product is a component of the running application, shared by every
	// request and outliving all of them, so the context only points at it.
	imtlic::IProductInfo* m_productInfoPtr;
	bool m_isTenantOwner;
	Headers m_headers;

	// Resolving the implied permissions walks the whole feature tree of the
	// product, while a single request checks permissions many times over (every
	// command of a menu, every element of a page). It is done once per context
	// and kept here; a context outlives no request, so the answer cannot go
	// stale. Contexts are read from more than one thread, hence the lock.
	mutable QMutex m_impliedPermissionsMutex;
	mutable imtauth::IUserInfo::FeatureIds m_impliedPermissions;
	mutable bool m_areImpliedPermissionsResolved;
};


} // namespace imtgql


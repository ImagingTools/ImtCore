// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>

// Qt includes
#include <QtCore/QByteArrayList>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtlic
{
	class IProductInfo;
}


namespace imtgql
{


/**
	Common interface for a GraphQL context.
	\todo Move to imtrest. This is a general interface, rename it to IClientRequestContext
*/
class IGqlContext: virtual public iser::IObject
{
public:
	typedef QMap<QByteArray,QByteArray> Headers;

	virtual QByteArray GetProductId() const = 0;
	virtual void SetProductId(const QByteArray& productId) = 0;

	virtual QByteArray GetLanguageId() const = 0;
	virtual void SetLanguageId(const QByteArray& languageId) = 0;

	virtual QByteArray GetDesignScheme() const = 0;
	virtual void SetDesignScheme(const QByteArray& designScheme) = 0;

	virtual QByteArray GetToken() const = 0;
	virtual void SetToken(const QByteArray& token) = 0;

	virtual QByteArrayList GetScopes() const = 0;
	virtual void SetScopes(const QByteArrayList& scopes) = 0;

	virtual QByteArray GetUserId() const = 0;
	virtual void SetUserId(const QByteArray& userId) = 0;

	virtual QByteArray GetTenantId() const = 0;
	virtual void SetTenantId(const QByteArray& tenantId) = 0;

	virtual const imtauth::IUserInfo* GetUserInfo() const = 0;
	virtual void SetUserInfo(const imtauth::IUserInfo* userInfoPtr) = 0;

	/**
		Get information about the product this request runs against, if available.
		It carries the feature tree the permissions of the product are declared in,
		which is what a permission check needs to resolve requirements between them.
	*/
	virtual imtlic::IProductInfo* GetProductInfo() const = 0;
	virtual void SetProductInfo(imtlic::IProductInfo* productInfoPtr) = 0;

	/**
		Get the permissions the user of this request holds through the requirements
		of the permissions given to them, transitively: holding 'A' whose
		requirements name 'B' is holding 'B' as well.

		The permissions held directly are not part of the returned list, and the
		list is empty while the context carries no user or no product info to
		resolve the requirements in.
	*/
	virtual imtauth::IUserInfo::FeatureIds GetImpliedPermissions() const = 0;

	virtual bool IsTenantOwner() const = 0;
	virtual void SetIsTenantOwner(bool isTenantOwner) = 0;

	virtual Headers GetHeaders() const = 0;
	virtual void SetHeaders(const Headers& headers) = 0;
};


typedef istd::TUniqueInterfacePtr<IGqlContext> IGqlContextUniquePtr;
typedef istd::TSharedInterfacePtr<IGqlContext> IGqlContextSharedPtr;


} // namespace imtgql

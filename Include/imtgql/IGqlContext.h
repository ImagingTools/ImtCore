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

	enum PrincipalKind
	{
		PK_ANONYMOUS,
		PK_USER,
		PK_SERVICE
	};

	virtual PrincipalKind GetPrincipalKind() const = 0;
	virtual void SetPrincipalKind(PrincipalKind kind) = 0;

	// An explicit empty boundary grants nothing; an unset boundary does not restrict permissions.
	virtual bool HasPermissionBoundary() const = 0;
	virtual QByteArrayList GetPermissionBoundary() const = 0;
	virtual void SetPermissionBoundary(const QByteArrayList& permissions) = 0;

	/**
		Get what the request may give in the roles, groups and users of its product, when it differs from what it may do.
		The token of a server bounds what a request through it may do at this server; what its user may give in its own product
		is what this server holds for that user, within the personal token of the user.
		\param isAdmin		Receives whether the user, as this server reads it, is the superuser: it may give any permission of the product.
		\param permissions	Receives the permissions the user may give otherwise.
		\return False when nothing was set: the request gives no more than it holds.
	*/
	virtual bool GetGrantingAuthority(bool& isAdmin, QByteArrayList& permissions) const = 0;
	virtual void SetGrantingAuthority(bool isAdmin, const QByteArrayList& permissions) = 0;

	virtual QByteArray GetProductId() const = 0;
	virtual void SetProductId(const QByteArray& productId) = 0;

	/**
		Get the product whose server the request came through, empty for a request made here directly.
		Without a user the server itself sends it; the user of the request holds only what the token of the server allows.
	*/
	virtual QByteArray GetServerProductId() const = 0;
	virtual void SetServerProductId(const QByteArray& serverProductId) = 0;

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
		Get the permissions available to the organization of this request.
		An empty list sets no limit.
	*/
	virtual QByteArrayList GetTenantPermissions() const = 0;
	virtual void SetTenantPermissions(const QByteArrayList& permissions) = 0;

	/**
		Get the permissions the user of this request holds through the requirements
		of the permissions given to them in the product of the request, transitively:
		holding 'A' whose requirements name 'B' is holding 'B' as well.

		The permissions held directly are not part of the returned list, and the
		list is empty while the context carries no user or no product info to
		resolve the requirements in. \see imtgql::CollectImpliedPermissions
	*/
	virtual imtauth::IUserInfo::FeatureIds GetImpliedPermissions() const = 0;

	virtual bool IsTenantOwner() const = 0;
	virtual void SetIsTenantOwner(bool isTenantOwner) = 0;

	/**
		Get the permissions the user of this request holds inside its organization (\see imtauth::IsOrganizationInternalPermission).
		They come from the ownership or the membership, not from the roles of the user; empty outside of an organization.
	*/
	virtual QByteArrayList GetOrganizationPermissions() const = 0;
	virtual void SetOrganizationPermissions(const QByteArrayList& permissions) = 0;

	virtual Headers GetHeaders() const = 0;
	virtual void SetHeaders(const Headers& headers) = 0;
};


typedef istd::TUniqueInterfacePtr<IGqlContext> IGqlContextUniquePtr;
typedef istd::TSharedInterfacePtr<IGqlContext> IGqlContextSharedPtr;


} // namespace imtgql

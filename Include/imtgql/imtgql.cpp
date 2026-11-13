// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgql/imtgql.h>


// ImtCore includes
#include <iqt/iqt.h>
#include <imtauth/imtauth.h>
#include <imtauth/CUserInfo.h>
#include <QtCore/QSet>
#include <imtbase/imtbase.h>
#include <imtgql/CGqlContext.h>
#include <imtlic/IProductInfo.h>


namespace imtgql
{


// public methods

imtauth::IUserInfo::FeatureIds CollectImpliedPermissions(const IGqlContext& gqlContext, const imtauth::IUserInfo::FeatureIds& permissions)
{
	imtlic::IProductInfo* productInfoPtr = gqlContext.GetProductInfo();
	if (productInfoPtr == nullptr){
		return imtauth::IUserInfo::FeatureIds();
	}

	imtauth::IUserInfo::FeatureIds retVal = imtlic::CollectImpliedPermissions(*productInfoPtr, permissions);

	// Within an organization nothing outside its permission scope is granted; an empty scope grants nothing
	if (!gqlContext.GetTenantId().isEmpty()){
		const QByteArrayList tenantPermissions = gqlContext.GetTenantPermissions();
		retVal.removeIf([&tenantPermissions](const QByteArray& permission){
			return !imtauth::HasPermission(tenantPermissions, permission, false);
		});
	}

	return retVal;
}


QByteArray GetSenderServerProductId(const IGqlContext& gqlContext)
{
	const bool isServerItself = gqlContext.GetPrincipalKind() == IGqlContext::PK_SERVICE;

	return isServerItself ? gqlContext.GetServerProductId() : QByteArray();
}


bool RestrictUserPermissions(IGqlContext& gqlContext, const QByteArrayList& permissions)
{
	const imtauth::IUserInfo* sourcePtr = gqlContext.GetUserInfo();
	if (sourcePtr == nullptr){
		return false;
	}

	const auto principalKind = gqlContext.GetPrincipalKind();
	QByteArrayList boundary = permissions;
	if (gqlContext.HasPermissionBoundary()){
		const QByteArrayList previous = gqlContext.GetPermissionBoundary();
		const QSet<QByteArray> previousSet(previous.cbegin(), previous.cend());
		boundary.removeIf([&previousSet](const QByteArray& permission){ return !previousSet.contains(permission); });
	}
	const QSet<QByteArray> allowed(boundary.cbegin(), boundary.cend());
	const QByteArray productId = gqlContext.GetProductId();
	QByteArrayList effective = sourcePtr->IsAdmin() ? boundary
				: sourcePtr->GetPermissions(productId) + gqlContext.GetImpliedPermissions();
	effective.removeIf([&allowed](const QByteArray& permission){ return !allowed.contains(permission); });

	imtauth::CIdentifiableUserInfo user;
	if (!user.CopyFrom(*sourcePtr)){
		return false;
	}
	for (const QByteArray& product : user.GetProducts()){
		user.SetRoles(product, imtauth::IUserBaseInfo::RoleIds());
		user.SetLocalPermissions(product, QByteArrayList());
	}
	for (const QByteArray& group : user.GetGroups()){
		user.RemoveFromGroup(group);
	}
	user.SetLocalPermissions(productId, effective);
	gqlContext.SetUserInfo(&user);
	gqlContext.SetPrincipalKind(principalKind);
	gqlContext.SetPermissionBoundary(boundary);

	return true;
}


imtauth::IUserInfo::FeatureIds CollectTenantOwnerPermissions(const IGqlContext& gqlContext)
{
	if (!gqlContext.IsTenantOwner() || gqlContext.GetTenantId().isEmpty()){
		return imtauth::IUserInfo::FeatureIds();
	}

	// The owner holds the whole permission scope of the organization, and nothing beyond it
	const QByteArrayList tenantPermissions = gqlContext.GetTenantPermissions();
	imtlic::IProductInfo* productInfoPtr = gqlContext.GetProductInfo();
	if (productInfoPtr == nullptr){
		return tenantPermissions;
	}

	imtauth::IUserInfo::FeatureIds retVal = imtlic::CollectProductPermissions(*productInfoPtr);
	retVal.removeIf([&tenantPermissions](const QByteArray& permission){
		return !imtauth::HasPermission(tenantPermissions, permission, false);
	});

	return retVal;
}


imtauth::IUserInfo::FeatureIds CollectTenantScopedPermissions(const IGqlContext& gqlContext, const imtauth::IUserInfo::FeatureIds& permissions)
{
	if (gqlContext.GetTenantId().isEmpty()){
		return permissions;
	}

	// Within an organization nothing outside its permission scope is held; an empty scope holds nothing
	const QByteArrayList tenantPermissions = gqlContext.GetTenantPermissions();
	const QByteArrayList organizationAccessPermissions = imtauth::GetOrganizationAccessPermissions();

	imtauth::IUserInfo::FeatureIds retVal = permissions;
	retVal.removeIf([&tenantPermissions, &organizationAccessPermissions](const QByteArray& permission){
		return !organizationAccessPermissions.contains(permission) && !imtauth::HasPermission(tenantPermissions, permission, false);
	});

	return retVal;
}


imtauth::IUserInfo::FeatureIds CollectOrganizationAccessPermissions(const IGqlContext& gqlContext, const QByteArray& userId)
{
	const imtauth::IUserInfo* userInfoPtr = gqlContext.GetUserInfo();
	if (userInfoPtr == nullptr || gqlContext.GetUserId() != userId){
		return imtauth::IUserInfo::FeatureIds();
	}

	const QByteArray productId = gqlContext.GetProductId();
	const imtauth::IUserInfo::FeatureIds userPermissions = productId.isEmpty() ? userInfoPtr->GetPermissions() : userInfoPtr->GetPermissions(productId);

	imtauth::IUserInfo::FeatureIds retVal;
	for (const QByteArray& permission : imtauth::GetOrganizationAccessPermissions()){
		if (userPermissions.contains(permission)){
			retVal.append(permission);
		}
	}

	return retVal;
}


QByteArray GetLanguageIdFromRequest(const QList<CGqlParamObject>& inputParams)
{
	qsizetype count = inputParams.count();
	for (qsizetype i = 0; i < count; i++){
		if (inputParams.at(i).GetParamIds().contains("LanguageId")){
			return inputParams.at(i).GetParamArgumentValue("LanguageId").toByteArray();
		}
	}

	return QByteArray();
}


QByteArray GetDesignSchemeIdFromRequest(const QList<CGqlParamObject>& /*inputParams*/)
{
	return QByteArray();
}


QByteArray GetTranslation(const iqt::ITranslationManager* translationManagerPtr, const CGqlRequest& gqlRequest, const QByteArray& phrase, const QByteArray& context)
{
	if (translationManagerPtr == nullptr){
		return phrase;
	}

	const IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return phrase;
	}

	QByteArray languageId = gqlContextPtr->GetLanguageId();

	return iqt::GetTranslation(translationManagerPtr, phrase, languageId, context);
}


void SetServerRequestContext(CGqlRequest& gqlRequest, const QByteArray& serverToken)
{
	IGqlContextUniquePtr contextPtr(new CGqlContext);
	if (!serverToken.isEmpty()){
		IGqlContext::Headers headers;
		headers.insert(imtbase::s_authenticationTokenHeaderId, serverToken);

		contextPtr->SetToken(serverToken);
		contextPtr->SetHeaders(headers);
	}

	gqlRequest.SetGqlContext(IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr)));
}


} // namespace imtqml

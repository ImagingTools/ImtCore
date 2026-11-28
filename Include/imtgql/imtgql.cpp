// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgql/imtgql.h>


// ImtCore includes
#include <iqt/iqt.h>
#include <imtauth/imtauth.h>
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


QByteArray GetDataTenantId(const IGqlContext& gqlContext, imtbase::TenantType& tenantType)
{
	tenantType = imtbase::TT_ORGANIZATION;

	const QByteArray tenantId = gqlContext.GetTenantId();
	if (!tenantId.isEmpty() || gqlContext.GetPrincipalKind() != IGqlContext::PK_USER){
		return tenantId;
	}

	tenantType = imtbase::TT_USER;

	return gqlContext.GetUserId();
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


QByteArray GetRequestProductId(const CGqlRequest& gqlRequest)
{
	const IGqlContext* contextPtr = gqlRequest.GetRequestContext();

	return (contextPtr != nullptr) ? contextPtr->GetProductId() : QByteArray();
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


IGqlContext::Headers RemoveServerHeaders(const IGqlContext::Headers& headers)
{
	IGqlContext::Headers retVal = headers;
	for (IGqlContext::Headers::iterator iter = retVal.begin(); iter != retVal.end();){
		if (iter.key().compare(imtbase::s_documentActionHeaderId, Qt::CaseInsensitive) == 0){
			iter = retVal.erase(iter);
		}
		else{
			++iter;
		}
	}

	return retVal;
}


} // namespace imtqml

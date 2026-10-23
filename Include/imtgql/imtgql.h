// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>


namespace imtgql
{


/**
	Collect the permissions implied by the given ones in the product of the context,
	limited to the permissions available to its organization.
	\see imtlic::CollectImpliedPermissions
*/
imtauth::IUserInfo::FeatureIds CollectImpliedPermissions(const IGqlContext& gqlContext, const imtauth::IUserInfo::FeatureIds& permissions);

/**
	Every permission of the product of the context if the caller owns the organization of the context, otherwise none.
	The owner of an organization holds its whole permission scope, whatever the roles.
	\see imtlic::CollectProductPermissions
*/
imtauth::IUserInfo::FeatureIds CollectTenantOwnerPermissions(const IGqlContext& gqlContext);

/**
	The organization access permissions (\see imtauth::GetOrganizationAccessPermissions) the context grants to the given user.
*/
imtauth::IUserInfo::FeatureIds CollectOrganizationAccessPermissions(const IGqlContext& gqlContext, const QByteArray& userId);

QByteArray GetLanguageIdFromRequest(const QList<CGqlParamObject>& inputParams);
QByteArray GetDesignSchemeIdFromRequest(const QList<CGqlParamObject>& inputParams);
QByteArray GetTranslation(const iqt::ITranslationManager* translationManagerPtr, const CGqlRequest& gqlRequest, const QByteArray& phrase, const QByteArray& context);


} // namespace imtqml



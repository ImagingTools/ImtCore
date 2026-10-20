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

QByteArray GetLanguageIdFromRequest(const QList<CGqlParamObject>& inputParams);
QByteArray GetDesignSchemeIdFromRequest(const QList<CGqlParamObject>& inputParams);
QByteArray GetTranslation(const iqt::ITranslationManager* translationManagerPtr, const CGqlRequest& gqlRequest, const QByteArray& phrase, const QByteArray& context);


} // namespace imtqml



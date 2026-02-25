// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


QByteArray GetLanguageIdFromRequest(const QList<CGqlParamObject>& inputParams);
QByteArray GetDesignSchemeIdFromRequest(const QList<CGqlParamObject>& inputParams);
QByteArray GetTranslation(const iqt::ITranslationManager* translationManagerPtr, const CGqlRequest& gqlRequest, const QByteArray& phrase, const QByteArray& context);


} // namespace imtqml



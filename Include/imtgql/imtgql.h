#pragma once


// Qt includes
#include <QtCore/QList>

// ACF includes
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


QByteArray GetLanguageIdFromRequest(const QList<imtgql::CGqlParamObject>& inputParams);
QByteArray GetDesignSchemeIdFromRequest(const QList<imtgql::CGqlParamObject>& inputParams);
QByteArray GetTranslation(const iqt::ITranslationManager* translationManagerPtr, const imtgql::CGqlRequest& gqlRequest, const QByteArray& phrase, const QByteArray& context);


} // namespace imtqml



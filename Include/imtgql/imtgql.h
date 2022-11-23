#pragma once


// Qt includes
#include <QtCore/QList>

// ACF includes
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtgql/CGqlObject.h>


namespace imtgql
{


QByteArray GetLanguageIdFromRequest(const QList<imtgql::CGqlObject>& inputParams);

QByteArray GetDesignSchemeIdFromRequest(const QList<imtgql::CGqlObject>& inputParams);


} // namespace imtqml

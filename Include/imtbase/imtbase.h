#pragma once


// Qt includes
#include <QtCore/QList>

// Acf includes
#include <iqt/ITranslationManager.h>


/**
	Basic functionality and interfaces
	This package is system independent.
*/
namespace imtbase
{


/**
	Function to get phrase tranlate.
	Check all connect tranlation manager
	and return translate when finds it.
	If not found, returns the original phrase.
*/
QByteArray GetTranslation(const iqt::ITranslationManager* translationManagerPtr, const QByteArray& phrase, const QByteArray& languageId, const QByteArray& context);


} // namespace imtbase



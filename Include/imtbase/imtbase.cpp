// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/imtbase.h>


// ACF includes
#include <iqt/iqt.h>


namespace imtbase
{


QString TranslateAttributeText(const iqt::ITranslationManager* translationManagerPtr, const QString& text, const QByteArray& languageId)
{
	if (translationManagerPtr == nullptr || text.isEmpty()){
		return text;
	}

	return QString::fromUtf8(iqt::GetTranslation(translationManagerPtr, text.toUtf8(), languageId, QByteArrayLiteral("Attribute")));
}


} // namespace imtbase



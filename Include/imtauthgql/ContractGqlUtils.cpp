// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/ContractGqlUtils.h>


// ACF includes
#include <iqt/iqt.h>


namespace imtauthgql
{


QString ToSdlTime(const QDateTime& time)
{
	return time.isValid() ? time.toUTC().toString(Qt::ISODateWithMs) : QString();
}


QDateTime FromSdlTime(const QString& text)
{
	QDateTime retVal = QDateTime::fromString(text, Qt::ISODateWithMs);
	if (!retVal.isValid()){
		retVal = QDateTime::fromString(text, Qt::ISODate);
	}

	return retVal.isValid() ? retVal.toUTC() : QDateTime();
}


QDateTime FromSdlTime(const istd::TNullableValue<QString>& text)
{
	return FromSdlTime(text.value_or(QString()));
}


QString TranslateSystemText(const iqt::ITranslationManager* translationManagerPtr, const QString& text, const QByteArray& languageId)
{
	if (translationManagerPtr == nullptr || text.isEmpty()){
		return text;
	}

	return QString::fromUtf8(iqt::GetTranslation(translationManagerPtr, text.toUtf8(), languageId, QByteArrayLiteral("Attribute")));
}


} // namespace imtauthgql



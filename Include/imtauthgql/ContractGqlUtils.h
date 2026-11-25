// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <istd/TNullableValue.h>
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtsdl/TElementList.h>


namespace imtauthgql
{


/**
	A time of the schema: ISO text in UTC, empty for an unset time.
*/
QString ToSdlTime(const QDateTime& time);
QDateTime FromSdlTime(const QString& text);
QDateTime FromSdlTime(const istd::TNullableValue<QString>& text);

/**
	A text of an .acc file (a system role, a system type) in the language of the request.
*/
QString TranslateSystemText(const iqt::ITranslationManager* translationManagerPtr, const QString& text, const QByteArray& languageId);


template <typename T>
imtsdl::TElementList<T> ToElementList(const QList<T>& source)
{
	imtsdl::TElementList<T> retVal;
	retVal.FromList(source);

	return retVal;
}


template <typename T>
QList<T> FromElementList(const istd::TNullableValue<imtsdl::TElementList<T>>& source)
{
	return source ? source->ToList() : QList<T>();
}


/**
	The enumerations of the schema list their values in the order of the model.
*/
template <typename Target, typename Source>
Target ConvertEnum(Source value)
{
	return static_cast<Target>(static_cast<int>(value));
}


} // namespace imtauthgql



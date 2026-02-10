// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

// ImtCore includes
#include <imtgql/IGqlResponse.h>


namespace imtgql
{


template <class GqlResponseBase>
class TGraphQlJsonSerializerWrap: public virtual GqlResponseBase
{
public:
	TGraphQlJsonSerializerWrap(IGqlResponse::GqlRequestPtr originalRequestPtr);

	// reimplemented (IGqlResponse)
	virtual QByteArray GetData() const override;
	virtual typename GqlResponseBase::ErrorEntryList GetErrorList() const override;
	virtual QStringList GetErrorMessageList() const override;
};


template<class GqlResponseBase>
TGraphQlJsonSerializerWrap<GqlResponseBase>::TGraphQlJsonSerializerWrap(IGqlResponse::GqlRequestPtr originalRequestPtr)
	: GqlResponseBase(originalRequestPtr)
{
}


template <class GqlResponseBase>
QByteArray TGraphQlJsonSerializerWrap<GqlResponseBase>::GetData() const
{
	QJsonDocument rawPayload = QJsonDocument::fromJson(GqlResponseBase::GetResponseData());
	if (!rawPayload.isObject()){
		Q_ASSERT_X(false, "GetData", "Unexpected JSON");

		return QByteArray();
	}

	QJsonValue payloadDataValue = rawPayload.object().value(QStringLiteral("data"));

	if (payloadDataValue.isObject()){
		return QJsonDocument(payloadDataValue.toObject()).toJson();
	}
	if (payloadDataValue.isArray()){
		return QJsonDocument(payloadDataValue.toArray()).toJson();
	}

	Q_ASSERT_X(false, "GetData", "Unexpected 'data' JSON");

	return QByteArray();
}


template <class GqlResponseBase>
typename GqlResponseBase::ErrorEntryList TGraphQlJsonSerializerWrap<GqlResponseBase>::GetErrorList() const
{
	QJsonDocument rawPayload = QJsonDocument::fromJson(GqlResponseBase::GetResponseData());
	if (!rawPayload.isObject()){
		Q_ASSERT_X(false, "GetErrorList", "Unexpected JSON");

		return typename GqlResponseBase::ErrorEntryList();
	}

	QJsonValue payloadErrorsValue = rawPayload.object().value(QStringLiteral("errors"));

	Q_ASSERT_X(payloadErrorsValue.isArray(), "GetErrorList", "Unexpected 'errors' JSON");

	const QJsonArray payloadErrorsArray = payloadErrorsValue.toArray();
	typename GqlResponseBase::ErrorEntryList retVal;
	for(const QJsonValue& errorEntry: payloadErrorsArray){
		Q_ASSERT_X(errorEntry.isObject(), "GetErrorList", "Unexpected error entry");

		retVal << errorEntry.toObject().toVariantMap();
	}

	return retVal;
}


template <class GqlResponseBase>
QStringList TGraphQlJsonSerializerWrap<GqlResponseBase>::GetErrorMessageList() const
{
	QStringList retVal;
	const typename GqlResponseBase::ErrorEntryList errors = GetErrorList();
	for (const QVariantMap& errorEntry: errors){
		Q_ASSERT(errorEntry.contains(QStringLiteral("message")));

		retVal << errorEntry[QStringLiteral("message")].toString();
	}

	return retVal;
}


} // namespace imtqml



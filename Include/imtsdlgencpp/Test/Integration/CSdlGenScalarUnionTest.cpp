// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSdlGenIntegrationTest.h"


// STL includes
#include <variant>

// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QRegularExpression>
#include <QtTest/QTest>

// ImtCore includes
#include <GeneratedFiles/sdlgentestunionsdl/SDL/1.0/CPP/ScalarUnion.h>
#include <GeneratedFiles/sdlgentestunionsdl/SDL/1.0/CPP/ScalarUnionObjects.h>
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>


/*
	The union 'ScalarValue' of the schema 'ScalarUnion.sdl' contains all scalar types, that are allowed in unions.
	The QML test 'Tests/QmlTest/imtsdlgenqml/tst_ScalarUnion.qml' transfers the same values between the generated C++ objects
	and the generated QML types through JSON. Keep the values of both tests the same.
*/


namespace imtsdlgentest
{


namespace
{


using CScalarUnionPayload = sdl::V1_0::sdlgentestunion::CScalarUnionPayload;
using CScalarUnionPayloadObject = sdl::V1_0::sdlgentestunion::CScalarUnionPayloadObject;
using ScalarValue = sdl::V1_0::sdlgentestunion::ScalarValue;


QByteArray CanonicalJson(const QByteArray& data)
{
	QJsonParseError error;
	const QJsonDocument document = QJsonDocument::fromJson(data, &error);
	if (error.error != QJsonParseError::NoError || document.isNull()){
		return QByteArray();
	}

	return document.toJson(QJsonDocument::Compact);
}


QByteArray CreatePayloadJson(const QByteArray& jsonValue)
{
	return CanonicalJson(
		QByteArrayLiteral("{\"__typename\":\"ScalarUnionPayload\",\"requiredValue\":") + jsonValue +
		QByteArrayLiteral(",\"optionalValue\":") + jsonValue + '}');
}


ScalarValue CreateScalarValue(const QVariant& value)
{
	switch (value.typeId()){
	case QMetaType::Double:
		return ScalarValue(value.toDouble());
	case QMetaType::QString:
		return ScalarValue(value.toString());
	case QMetaType::Int:
		return ScalarValue(value.toInt());
	case QMetaType::Bool:
		return ScalarValue(value.toBool());
	}

	return ScalarValue();
}


QVariant ToVariant(const ScalarValue& value)
{
	return std::visit(
		[](const auto& alternative){ return QVariant::fromValue(alternative); },
		static_cast<const ScalarValue::BaseClass&>(value));
}


CScalarUnionPayload CreatePayload(const QVariant& value)
{
	CScalarUnionPayload payload;
	payload.requiredValue = CreateScalarValue(value);
	payload.optionalValue = CreateScalarValue(value);

	return payload;
}


QString DescribeVariant(const QVariant& value)
{
	return QString::fromLatin1(value.typeName()) + '(' + value.toString() + ')';
}


// The value must keep its type: an integer read as a double (or a numeric string read as a number) is an error
QString CheckScalarValue(const istd::TNullableValue<ScalarValue>& actual, const QVariant& expected, const QString& fieldName)
{
	if (!actual.HasValue()){
		return QStringLiteral("'%1' has no value").arg(fieldName);
	}

	const QVariant actualValue = ToVariant(*actual);
	if (actualValue.typeId() != expected.typeId() || actualValue != expected){
		return QStringLiteral("'%1' is %2, expected %3").arg(fieldName, DescribeVariant(actualValue), DescribeVariant(expected));
	}

	return QString();
}


QString CheckPayload(const CScalarUnionPayload& payload, const QVariant& expected)
{
	const QString requiredValueError = CheckScalarValue(payload.requiredValue, expected, QStringLiteral("requiredValue"));
	if (!requiredValueError.isEmpty()){
		return requiredValueError;
	}

	return CheckScalarValue(payload.optionalValue, expected, QStringLiteral("optionalValue"));
}


QByteArray WriteJson(const CScalarUnionPayload& payload, bool* isWrittenPtr)
{
	QJsonObject jsonObject;
	*isWrittenPtr = payload.WriteToJsonObject(jsonObject);

	return QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);
}


bool ReadJson(const QByteArray& json, CScalarUnionPayload& payload)
{
	const QJsonDocument document = QJsonDocument::fromJson(json);
	if (!document.isObject()){
		return false;
	}

	return payload.ReadFromJsonObject(document.object());
}


} // namespace


void CSdlGenIntegrationTest::TestScalarUnionRoundTrip_data()
{
	QTest::addColumn<QVariant>("value");
	QTest::addColumn<QByteArray>("jsonValue");

	QTest::newRow("Double") << QVariant(2.5) << QByteArrayLiteral("2.5");
	QTest::newRow("negative Double") << QVariant(-0.125) << QByteArrayLiteral("-0.125");
	QTest::newRow("String") << QVariant(QStringLiteral("text")) << QByteArrayLiteral("\"text\"");
	QTest::newRow("empty String") << QVariant(QString()) << QByteArrayLiteral("\"\"");
	QTest::newRow("numeric String") << QVariant(QStringLiteral("42")) << QByteArrayLiteral("\"42\"");
	QTest::newRow("boolean String") << QVariant(QStringLiteral("true")) << QByteArrayLiteral("\"true\"");
	QTest::newRow("Int") << QVariant(42) << QByteArrayLiteral("42");
	QTest::newRow("negative Int") << QVariant(-7) << QByteArrayLiteral("-7");
	QTest::newRow("zero Int") << QVariant(0) << QByteArrayLiteral("0");
	QTest::newRow("Bool true") << QVariant(true) << QByteArrayLiteral("true");
	QTest::newRow("Bool false") << QVariant(false) << QByteArrayLiteral("false");
}


void CSdlGenIntegrationTest::TestScalarUnionRoundTrip()
{
	QFETCH(QVariant, value);
	QFETCH(QByteArray, jsonValue);

	const CScalarUnionPayload source = CreatePayload(value);
	const QByteArray expectedJson = CreatePayloadJson(jsonValue);
	QVERIFY(!expectedJson.isEmpty());

	// C++ -> JSON: the data, that is sent to QML
	bool isWritten = false;
	const QByteArray actualJson = WriteJson(source, &isWritten);
	QVERIFY(isWritten);
	QCOMPARE(CanonicalJson(actualJson), expectedJson);

	// JSON -> C++: the data, that is received from QML
	CScalarUnionPayload jsonResult;
	QVERIFY(ReadJson(expectedJson, jsonResult));
	const QString jsonError = CheckPayload(jsonResult, value);
	QVERIFY2(jsonError.isEmpty(), qPrintable(jsonError));

	imtgql::CGqlParamObject gqlObject;
	QVERIFY(source.WriteToGraphQlObject(gqlObject));
	QCOMPARE(gqlObject.GetParamArgumentValue(QByteArrayLiteral("requiredValue")), value);
	CScalarUnionPayload gqlResult;
	QVERIFY(gqlResult.ReadFromGraphQlObject(gqlObject));
	const QString gqlError = CheckPayload(gqlResult, value);
	QVERIFY2(gqlError.isEmpty(), qPrintable(gqlError));

	imtbase::CTreeItemModel treeModel;
	QVERIFY(source.WriteToModel(treeModel));
	QCOMPARE(treeModel.GetData(QByteArrayLiteral("requiredValue")), value);
	CScalarUnionPayload treeResult;
	QVERIFY(treeResult.ReadFromModel(treeModel));
	const QString treeError = CheckPayload(treeResult, value);
	QVERIFY2(treeError.isEmpty(), qPrintable(treeError));
}


void CSdlGenIntegrationTest::TestScalarUnionQObjectBridge_data()
{
	TestScalarUnionRoundTrip_data();
}


// QML works with the C++ objects through their properties and the invokable methods
void CSdlGenIntegrationTest::TestScalarUnionQObjectBridge()
{
	QFETCH(QVariant, value);
	QFETCH(QByteArray, jsonValue);

	const QByteArray expectedJson = CreatePayloadJson(jsonValue);

	// C++ -> QML
	CScalarUnionPayloadObject sourceObject;
	static_cast<CScalarUnionPayload&>(sourceObject) = CreatePayload(value);
	const QVariant qmlValue = sourceObject.property("m_requiredValue");
	QCOMPARE(qmlValue.typeId(), value.typeId());
	QCOMPARE(qmlValue, value);
	QCOMPARE(CanonicalJson(sourceObject.toJson().toUtf8()), expectedJson);

	// QML -> C++
	CScalarUnionPayloadObject targetObject;
	QVERIFY(targetObject.setProperty("m_requiredValue", value));
	QVERIFY(targetObject.setProperty("m_optionalValue", value));
	const QString propertyError = CheckPayload(targetObject, value);
	QVERIFY2(propertyError.isEmpty(), qPrintable(propertyError));

	CScalarUnionPayloadObject parsedObject;
	QVERIFY(parsedObject.createFromJson(QString::fromUtf8(expectedJson)));
	const QString jsonError = CheckPayload(parsedObject, value);
	QVERIFY2(jsonError.isEmpty(), qPrintable(jsonError));
	QCOMPARE(parsedObject.property("m_optionalValue"), value);
}


// JSON has no separate integer type: a Double without a fractional part is read as Int
void CSdlGenIntegrationTest::TestScalarUnionIntegralDoubleIsReadAsInt()
{
	const CScalarUnionPayload source = CreatePayload(QVariant(5.0));

	bool isWritten = false;
	const QByteArray json = WriteJson(source, &isWritten);
	QVERIFY(isWritten);
	QCOMPARE(CanonicalJson(json), CreatePayloadJson(QByteArrayLiteral("5")));

	CScalarUnionPayload result;
	QVERIFY(ReadJson(json, result));
	const QString error = CheckPayload(result, QVariant(5));
	QVERIFY2(error.isEmpty(), qPrintable(error));
}


void CSdlGenIntegrationTest::TestScalarUnionOptionalValueMissing()
{
	CScalarUnionPayload result;
	QVERIFY(ReadJson(QByteArrayLiteral("{\"requiredValue\":\"text\"}"), result));
	const QString error = CheckScalarValue(result.requiredValue, QVariant(QStringLiteral("text")), QStringLiteral("requiredValue"));
	QVERIFY2(error.isEmpty(), qPrintable(error));
	QVERIFY(!result.optionalValue.HasValue());

	bool isWritten = false;
	const QByteArray json = WriteJson(result, &isWritten);
	QVERIFY(isWritten);
	CScalarUnionPayload rereadResult;
	QVERIFY(ReadJson(json, rereadResult));
	QVERIFY(!rereadResult.optionalValue.HasValue());

	CScalarUnionPayload missingRequiredResult;
	QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(".*Error: (Field: 'requiredValue' doesn't exist, but required|Field 'requiredValue' is missing, but required\\.?)")));
	QVERIFY(!ReadJson(QByteArrayLiteral("{\"optionalValue\":1}"), missingRequiredResult));
}


} // namespace imtsdlgentest

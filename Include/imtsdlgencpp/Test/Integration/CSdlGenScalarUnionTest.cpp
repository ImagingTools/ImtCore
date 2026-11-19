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
using CScalarUnionLink = sdl::V1_0::sdlgentestunion::CScalarUnionLink;
using CMixedUnionPayload = sdl::V1_0::sdlgentestunion::CMixedUnionPayload;
using CMixedUnionPayloadObject = sdl::V1_0::sdlgentestunion::CMixedUnionPayloadObject;
using MixedValue = sdl::V1_0::sdlgentestunion::MixedValue;
using CIntegerUnionPayload = sdl::V1_0::sdlgentestunion::CIntegerUnionPayload;
using CIntegerUnionPayloadObject = sdl::V1_0::sdlgentestunion::CIntegerUnionPayloadObject;
using IntegerValue = sdl::V1_0::sdlgentestunion::IntegerValue;


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


QJsonObject ReparseJson(const QJsonObject& jsonObject)
{
	return QJsonDocument::fromJson(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact)).object();
}


CScalarUnionLink CreateLink(const QString& url)
{
	CScalarUnionLink link;
	link.url = url;

	return link;
}


QString CheckMixedValue(const CMixedUnionPayload& payload, const MixedValue& expected, const QString& backendName)
{
	if (!payload.value.HasValue()){
		return QStringLiteral("%1: 'value' has no value").arg(backendName);
	}

	const MixedValue& actual = *payload.value;
	if (actual.index() != expected.index()){
		return QStringLiteral("%1: the alternative %2 is read, expected %3").arg(backendName).arg(actual.index()).arg(expected.index());
	}

	if (!(static_cast<const MixedValue::BaseClass&>(actual) == static_cast<const MixedValue::BaseClass&>(expected))){
		return QStringLiteral("%1: the value of the alternative %2 differs").arg(backendName).arg(actual.index());
	}

	return QString();
}


QString CheckMixedRoundTrip(const MixedValue& value)
{
	CMixedUnionPayload source;
	source.value = value;

	QJsonObject jsonObject;
	if (!source.WriteToJsonObject(jsonObject)){
		return QStringLiteral("JSON: the value is not written");
	}
	CMixedUnionPayload jsonResult;
	if (!jsonResult.ReadFromJsonObject(ReparseJson(jsonObject))){
		return QStringLiteral("JSON: the value is not read");
	}
	QString error = CheckMixedValue(jsonResult, value, QStringLiteral("JSON"));
	if (!error.isEmpty()){
		return error;
	}

	imtgql::CGqlParamObject gqlObject;
	if (!source.WriteToGraphQlObject(gqlObject)){
		return QStringLiteral("GraphQL: the value is not written");
	}
	CMixedUnionPayload gqlResult;
	if (!gqlResult.ReadFromGraphQlObject(gqlObject)){
		return QStringLiteral("GraphQL: the value is not read");
	}
	error = CheckMixedValue(gqlResult, value, QStringLiteral("GraphQL"));
	if (!error.isEmpty()){
		return error;
	}

	imtbase::CTreeItemModel treeModel;
	if (!source.WriteToModel(treeModel)){
		return QStringLiteral("Tree model: the value is not written");
	}
	CMixedUnionPayload treeResult;
	if (!treeResult.ReadFromModel(treeModel)){
		return QStringLiteral("Tree model: the value is not read");
	}

	return CheckMixedValue(treeResult, value, QStringLiteral("Tree model"));
}


QVariant ToVariant(const IntegerValue& value)
{
	return std::visit(
		[](const auto& alternative){ return QVariant::fromValue(alternative); },
		static_cast<const IntegerValue::BaseClass&>(value));
}


QString CheckIntegerValue(const CIntegerUnionPayload& payload, const QVariant& expected, const QString& backendName)
{
	if (!payload.value.HasValue()){
		return QStringLiteral("%1: 'value' has no value").arg(backendName);
	}

	const QVariant actualValue = ToVariant(*payload.value);
	if (actualValue.typeId() != expected.typeId() || actualValue != expected){
		return QStringLiteral("%1: 'value' is %2, expected %3").arg(backendName, DescribeVariant(actualValue), DescribeVariant(expected));
	}

	return QString();
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


void CSdlGenIntegrationTest::TestScalarUnionMixedRoundTrip()
{
	const QString linkError = CheckMixedRoundTrip(MixedValue(CreateLink(QStringLiteral("https://example.com/item"))));
	QVERIFY2(linkError.isEmpty(), qPrintable(linkError));

	const QString intError = CheckMixedRoundTrip(MixedValue(42));
	QVERIFY2(intError.isEmpty(), qPrintable(intError));

	const QString stringError = CheckMixedRoundTrip(MixedValue(QStringLiteral("text")));
	QVERIFY2(stringError.isEmpty(), qPrintable(stringError));

	// JSON numbers without fractional part are read as 'Int'
	CMixedUnionPayload jsonResult;
	QVERIFY(ReadJson(QByteArrayLiteral("{\"value\":7}"), jsonResult));
	const QString jsonError = CheckMixedValue(jsonResult, MixedValue(7), QStringLiteral("JSON"));
	QVERIFY2(jsonError.isEmpty(), qPrintable(jsonError));
}


// The scalar alternatives of a union with an object alternative are used by QML as plain values
void CSdlGenIntegrationTest::TestScalarUnionMixedQObjectBridge()
{
	CMixedUnionPayloadObject sourceObject;
	static_cast<CMixedUnionPayload&>(sourceObject).value = MixedValue(42);
	const QVariant intValue = sourceObject.property("m_value");
	QCOMPARE(intValue.typeId(), int(QMetaType::Int));
	QCOMPARE(intValue.toInt(), 42);

	static_cast<CMixedUnionPayload&>(sourceObject).value = MixedValue(QStringLiteral("text"));
	const QVariant stringValue = sourceObject.property("m_value");
	QCOMPARE(stringValue.typeId(), int(QMetaType::QString));
	QCOMPARE(stringValue.toString(), QStringLiteral("text"));

	CMixedUnionPayloadObject targetObject;
	QVERIFY(targetObject.setProperty("m_value", QVariant(42)));
	QString error = CheckMixedValue(targetObject, MixedValue(42), QStringLiteral("QObject"));
	QVERIFY2(error.isEmpty(), qPrintable(error));

	QVERIFY(targetObject.setProperty("m_value", QVariant(QStringLiteral("text"))));
	error = CheckMixedValue(targetObject, MixedValue(QStringLiteral("text")), QStringLiteral("QObject"));
	QVERIFY2(error.isEmpty(), qPrintable(error));
}


void CSdlGenIntegrationTest::TestScalarUnionIntegerTypesArePreserved_data()
{
	QTest::addColumn<QVariant>("value");

	QTest::newRow("Int") << QVariant(42);
	QTest::newRow("negative Int") << QVariant(-7);
	QTest::newRow("small LongLong") << QVariant(qlonglong(42));
	QTest::newRow("big LongLong") << QVariant(qlonglong(5000000000LL));
	QTest::newRow("negative big LongLong") << QVariant(qlonglong(-5000000000LL));
}


// A value of the native type of an integer alternative must not be read by another integer alternative (e.g. narrowed to 'Int')
void CSdlGenIntegrationTest::TestScalarUnionIntegerTypesArePreserved()
{
	QFETCH(QVariant, value);

	CIntegerUnionPayload source;
	if (value.typeId() == QMetaType::Int){
		source.value = IntegerValue(value.toInt());
	}
	else{
		source.value = IntegerValue(value.toLongLong());
	}

	imtgql::CGqlParamObject gqlObject;
	QVERIFY(source.WriteToGraphQlObject(gqlObject));
	CIntegerUnionPayload gqlResult;
	QVERIFY(gqlResult.ReadFromGraphQlObject(gqlObject));
	const QString gqlError = CheckIntegerValue(gqlResult, value, QStringLiteral("GraphQL"));
	QVERIFY2(gqlError.isEmpty(), qPrintable(gqlError));

	imtbase::CTreeItemModel treeModel;
	QVERIFY(source.WriteToModel(treeModel));
	CIntegerUnionPayload treeResult;
	QVERIFY(treeResult.ReadFromModel(treeModel));
	const QString treeError = CheckIntegerValue(treeResult, value, QStringLiteral("Tree model"));
	QVERIFY2(treeError.isEmpty(), qPrintable(treeError));

	CIntegerUnionPayloadObject sourceObject;
	static_cast<CIntegerUnionPayload&>(sourceObject) = source;
	const QVariant qmlValue = sourceObject.property("m_value");
	QCOMPARE(qmlValue.typeId(), value.typeId());
	QCOMPARE(qmlValue, value);

	CIntegerUnionPayloadObject targetObject;
	QVERIFY(targetObject.setProperty("m_value", value));
	const QString propertyError = CheckIntegerValue(targetObject, value, QStringLiteral("QObject"));
	QVERIFY2(propertyError.isEmpty(), qPrintable(propertyError));
}


void CSdlGenIntegrationTest::TestScalarUnionIntegerFallback_data()
{
	QTest::addColumn<QVariant>("value");
	QTest::addColumn<QVariant>("expected");

	QTest::newRow("Short") << QVariant::fromValue<short>(-7) << QVariant(-7);
	QTest::newRow("UShort") << QVariant::fromValue<ushort>(7) << QVariant(7);
	QTest::newRow("UChar") << QVariant::fromValue<uchar>(7) << QVariant(7);
	QTest::newRow("UInt in Int range") << QVariant(uint(7)) << QVariant(7);
	QTest::newRow("UInt out of Int range") << QVariant(uint(3000000000U)) << QVariant(qlonglong(3000000000LL));
	QTest::newRow("Long") << QVariant::fromValue<long>(7) << QVariant(7);
	QTest::newRow("ULongLong") << QVariant(qulonglong(5000000000ULL)) << QVariant(qlonglong(5000000000LL));
	QTest::newRow("integral Double") << QVariant(7.0) << QVariant(7);
	QTest::newRow("big integral Double") << QVariant(5000000000.0) << QVariant(qlonglong(5000000000LL));
}


// Values of the other numeric metatypes are read by the first integer alternative, which can hold the value
void CSdlGenIntegrationTest::TestScalarUnionIntegerFallback()
{
	QFETCH(QVariant, value);
	QFETCH(QVariant, expected);

	imtgql::CGqlParamObject gqlObject;
	gqlObject.InsertParam(QByteArrayLiteral("value"), value);
	CIntegerUnionPayload gqlResult;
	QVERIFY(gqlResult.ReadFromGraphQlObject(gqlObject));
	const QString gqlError = CheckIntegerValue(gqlResult, expected, QStringLiteral("GraphQL"));
	QVERIFY2(gqlError.isEmpty(), qPrintable(gqlError));

	imtbase::CTreeItemModel treeModel;
	QVERIFY(treeModel.SetData(QByteArrayLiteral("value"), value));
	CIntegerUnionPayload treeResult;
	QVERIFY(treeResult.ReadFromModel(treeModel));
	const QString treeError = CheckIntegerValue(treeResult, expected, QStringLiteral("Tree model"));
	QVERIFY2(treeError.isEmpty(), qPrintable(treeError));

	CIntegerUnionPayloadObject targetObject;
	QVERIFY(targetObject.setProperty("m_value", value));
	const QString propertyError = CheckIntegerValue(targetObject, expected, QStringLiteral("QObject"));
	QVERIFY2(propertyError.isEmpty(), qPrintable(propertyError));
}


} // namespace imtsdlgentest

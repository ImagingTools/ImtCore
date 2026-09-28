// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSdlGenTest.h"


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QRegularExpression>
#include <QtCore/QScopedPointer>
#include <QtTest/QTest>

// ImtCore includes
#include <GeneratedFiles/sdlgentestsdl/SDL/1.0/CPP/ArraySerialization.h>
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlRequest.h>


namespace imtsdlgentest
{


namespace
{


using CArrayPayload = sdl::V1_0::sdlgentest::CArrayPayload;
using CArrayPayloadObject = sdl::V1_0::sdlgentest::CArrayPayloadObject;
using CAttribute = sdl::V1_0::sdlgentest::CAttribute;
using CAttributeObject = sdl::V1_0::sdlgentest::CAttributeObject;
using CAttributeObjectList = sdl::V1_0::sdlgentest::CAttributeObjectList;
using CSavePayloadGqlRequest = sdl::V1_0::sdlgentest::CSavePayloadGqlRequest;
using SavePayloadRequestArguments = sdl::V1_0::sdlgentest::SavePayloadRequestArguments;


enum ValidPayloadCase
{
	VPC_FULL,
	VPC_ALL_EMPTY,
	VPC_NULLABLE_NULL
};


enum InvalidPayloadCase
{
	IPC_REQUIRED_OBJECTS_MISSING,
	IPC_REQUIRED_SCALARS_MISSING,
	IPC_REQUIRED_OBJECTS_NULL,
	IPC_REQUIRED_SCALARS_NULL,
	IPC_REQUIRED_OBJECT_ELEMENT_NULL,
	IPC_REQUIRED_SCALAR_ELEMENT_NULL,
	IPC_NULLABLE_REQUIRED_OBJECT_ELEMENT_NULL,
	IPC_NULLABLE_REQUIRED_SCALAR_ELEMENT_NULL
};


const QString s_rawDataDirectoryPath = qEnvironmentVariable("IMTCOREDIR") +
		QStringLiteral("/Include/imtsdlgencpp/Test/TestData/RawData");


CAttribute CreateAttribute(const QString& value)
{
	CAttribute attribute;
	attribute.value = value;

	return attribute;
}


CArrayPayload CreatePayload(ValidPayloadCase payloadCase)
{
	CArrayPayload payload;
	payload.requiredObjects.emplace();
	payload.requiredScalars.emplace();

	if (payloadCase == VPC_NULLABLE_NULL){
		payload.nullableObjectsWithRequiredElements.SetNull();
		payload.nullableObjects.SetNull();
		payload.nullableScalarsWithRequiredElements.SetNull();
		payload.nullableScalars.SetNull();

		return payload;
	}

	payload.nullableObjectsWithRequiredElements.emplace();
	payload.nullableObjects.emplace();
	payload.nullableScalarsWithRequiredElements.emplace();
	payload.nullableScalars.emplace();

	if (payloadCase == VPC_FULL){
		payload.requiredObjects->append(CreateAttribute(QStringLiteral("required")));
		payload.nullableObjectsWithRequiredElements->append(CreateAttribute(QStringLiteral("optional-required")));
		payload.nullableObjects->AppendNull();
		payload.nullableObjects->append(CreateAttribute(QStringLiteral("nullable")));
		payload.requiredScalars->append(QStringLiteral("alpha"));
		payload.nullableScalarsWithRequiredElements->append(QStringLiteral("beta"));
		payload.nullableScalars->AppendNull();
		payload.nullableScalars->append(QStringLiteral("omega"));
	}

	return payload;
}


template <class T, class Comparator>
bool NullableValuesEqual(
		const istd::TNullableValue<T>& left,
		const istd::TNullableValue<T>& right,
		Comparator comparator)
{
	if (left.IsValid() != right.IsValid() ||
		left.IsNull() != right.IsNull() ||
		left.HasValue() != right.HasValue())
	{
		return false;
	}
	if (!left.HasValue()){
		return true;
	}

	return comparator(*left, *right);
}


bool AttributesEqual(const CAttribute& left, const CAttribute& right)
{
	return NullableValuesEqual(left.value, right.value,
		[](const QString& leftValue, const QString& rightValue){ return leftValue == rightValue; });
}


template <class T, class Comparator>
bool ElementListsEqual(
		const imtsdl::TElementList<T>& left,
		const imtsdl::TElementList<T>& right,
		Comparator comparator)
{
	if (left.size() != right.size()){
		return false;
	}
	for (qsizetype index = 0; index < left.size(); ++index){
		if (!NullableValuesEqual(left[index], right[index], comparator)){
			return false;
		}
	}

	return true;
}


bool PayloadsEqual(const CArrayPayload& left, const CArrayPayload& right)
{
	const auto stringComparator = [](const QString& leftValue, const QString& rightValue){
		return leftValue == rightValue;
	};
	const auto attributeListComparator = [](const imtsdl::TElementList<CAttribute>& leftList,
			const imtsdl::TElementList<CAttribute>& rightList){
		return ElementListsEqual(leftList, rightList, AttributesEqual);
	};
	const auto stringListComparator = [stringComparator](const imtsdl::TElementList<QString>& leftList,
			const imtsdl::TElementList<QString>& rightList){
		return ElementListsEqual(leftList, rightList, stringComparator);
	};

	return NullableValuesEqual(left.requiredObjects, right.requiredObjects, attributeListComparator) &&
		NullableValuesEqual(left.nullableObjectsWithRequiredElements, right.nullableObjectsWithRequiredElements, attributeListComparator) &&
		NullableValuesEqual(left.nullableObjects, right.nullableObjects, attributeListComparator) &&
		NullableValuesEqual(left.requiredScalars, right.requiredScalars, stringListComparator) &&
		NullableValuesEqual(left.nullableScalarsWithRequiredElements, right.nullableScalarsWithRequiredElements, stringListComparator) &&
		NullableValuesEqual(left.nullableScalars, right.nullableScalars, stringListComparator);
}


CArrayPayload CreateInvalidPayload(InvalidPayloadCase invalidCase)
{
	CArrayPayload payload = CreatePayload(VPC_ALL_EMPTY);

	switch (invalidCase){
	case IPC_REQUIRED_OBJECTS_MISSING:
		payload.requiredObjects.Reset();
		break;
	case IPC_REQUIRED_SCALARS_MISSING:
		payload.requiredScalars.Reset();
		break;
	case IPC_REQUIRED_OBJECTS_NULL:
		payload.requiredObjects.SetNull();
		break;
	case IPC_REQUIRED_SCALARS_NULL:
		payload.requiredScalars.SetNull();
		break;
	case IPC_REQUIRED_OBJECT_ELEMENT_NULL:
		payload.requiredObjects->AppendNull();
		break;
	case IPC_REQUIRED_SCALAR_ELEMENT_NULL:
		payload.requiredScalars->AppendNull();
		break;
	case IPC_NULLABLE_REQUIRED_OBJECT_ELEMENT_NULL:
		payload.nullableObjectsWithRequiredElements->AppendNull();
		break;
	case IPC_NULLABLE_REQUIRED_SCALAR_ELEMENT_NULL:
		payload.nullableScalarsWithRequiredElements->AppendNull();
		break;
	}

	return payload;
}


QJsonObject CreateInvalidJsonObject(InvalidPayloadCase invalidCase)
{
	QJsonObject object;
	[[maybe_unused]] const bool isWritten = CreatePayload(VPC_ALL_EMPTY).WriteToJsonObject(object);

	switch (invalidCase){
	case IPC_REQUIRED_OBJECTS_MISSING:
		object.remove(QStringLiteral("requiredObjects"));
		break;
	case IPC_REQUIRED_SCALARS_MISSING:
		object.remove(QStringLiteral("requiredScalars"));
		break;
	case IPC_REQUIRED_OBJECTS_NULL:
		object.insert(QStringLiteral("requiredObjects"), QJsonValue::Null);
		break;
	case IPC_REQUIRED_SCALARS_NULL:
		object.insert(QStringLiteral("requiredScalars"), QJsonValue::Null);
		break;
	case IPC_REQUIRED_OBJECT_ELEMENT_NULL:
		object.insert(QStringLiteral("requiredObjects"), QJsonArray{QJsonValue::Null});
		break;
	case IPC_REQUIRED_SCALAR_ELEMENT_NULL:
		object.insert(QStringLiteral("requiredScalars"), QJsonArray{QJsonValue::Null});
		break;
	case IPC_NULLABLE_REQUIRED_OBJECT_ELEMENT_NULL:
		object.insert(QStringLiteral("nullableObjectsWithRequiredElements"), QJsonArray{QJsonValue::Null});
		break;
	case IPC_NULLABLE_REQUIRED_SCALAR_ELEMENT_NULL:
		object.insert(QStringLiteral("nullableScalarsWithRequiredElements"), QJsonArray{QJsonValue::Null});
		break;
	}

	return object;
}


imtgql::CGqlParamObject CreateInvalidGraphQlObject(InvalidPayloadCase invalidCase)
{
	imtgql::CGqlParamObject object;

	if (invalidCase == IPC_REQUIRED_OBJECTS_MISSING){
		object.InsertParam("requiredScalars", QVariantList());

		return object;
	}
	if (invalidCase == IPC_REQUIRED_SCALARS_MISSING){
		object.InsertParam("requiredObjects", QList<imtgql::CGqlParamObject>());

		return object;
	}

	[[maybe_unused]] const bool isWritten = CreatePayload(VPC_ALL_EMPTY).WriteToGraphQlObject(object);
	switch (invalidCase){
	case IPC_REQUIRED_OBJECTS_NULL:
		object.InsertParam("requiredObjects", QVariant());
		break;
	case IPC_REQUIRED_SCALARS_NULL:
		object.InsertParam("requiredScalars", QVariant());
		break;
	case IPC_REQUIRED_OBJECT_ELEMENT_NULL:
		object.InsertParam("requiredObjects", QList<imtgql::CGqlParamObject>{imtgql::CGqlParamObject::CreateNull()});
		break;
	case IPC_REQUIRED_SCALAR_ELEMENT_NULL:
		object.InsertParam("requiredScalars", QVariantList{QVariant()});
		break;
	case IPC_NULLABLE_REQUIRED_OBJECT_ELEMENT_NULL:
		object.InsertParam("nullableObjectsWithRequiredElements", QList<imtgql::CGqlParamObject>{imtgql::CGqlParamObject::CreateNull()});
		break;
	case IPC_NULLABLE_REQUIRED_SCALAR_ELEMENT_NULL:
		object.InsertParam("nullableScalarsWithRequiredElements", QVariantList{QVariant()});
		break;
	default:
		break;
	}

	return object;
}


bool ApplyInvalidTreeState(imtbase::CTreeItemModel& model, InvalidPayloadCase invalidCase)
{
	switch (invalidCase){
	case IPC_REQUIRED_OBJECTS_MISSING:
		return model.RemoveData("requiredObjects");
	case IPC_REQUIRED_SCALARS_MISSING:
		return model.RemoveData("requiredScalars");
	case IPC_REQUIRED_OBJECTS_NULL:
		return model.SetData("requiredObjects", QVariant());
	case IPC_REQUIRED_SCALARS_NULL:
		return model.SetData("requiredScalars", QVariant());
	case IPC_REQUIRED_OBJECT_ELEMENT_NULL:
	case IPC_NULLABLE_REQUIRED_OBJECT_ELEMENT_NULL:
	{
		const QByteArray key = invalidCase == IPC_REQUIRED_OBJECT_ELEMENT_NULL ?
			QByteArrayLiteral("requiredObjects") : QByteArrayLiteral("nullableObjectsWithRequiredElements");
		imtbase::CTreeItemModel* arrayModel = model.GetTreeItemModel(key);
		if (arrayModel == nullptr){
			return false;
		}
		arrayModel->InsertNewItem();

		return arrayModel->SetData(QByteArray(), QVariant(), 0);
	}
	case IPC_REQUIRED_SCALAR_ELEMENT_NULL:
	case IPC_NULLABLE_REQUIRED_SCALAR_ELEMENT_NULL:
	{
		const QByteArray key = invalidCase == IPC_REQUIRED_SCALAR_ELEMENT_NULL ?
			QByteArrayLiteral("requiredScalars") : QByteArrayLiteral("nullableScalarsWithRequiredElements");
		imtbase::CTreeItemModel* arrayModel = model.GetTreeItemModel(key);
		if (arrayModel == nullptr){
			return false;
		}
		arrayModel->InsertNewItem();

		return arrayModel->SetData(QByteArray(), QVariant(), 0);
	}
	}

	return false;
}


bool ReadFile(const QString& path, QByteArray& data)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly)){
		return false;
	}

	data = file.readAll();

	return file.error() == QFileDevice::NoError;
}


bool WriteFile(const QString& path, const QByteArray& data)
{
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
		return false;
	}

	return file.write(data) == data.size();
}


QByteArray CanonicalJson(const QByteArray& data)
{
	QJsonParseError error;
	const QJsonDocument document = QJsonDocument::fromJson(data, &error);
	if (error.error != QJsonParseError::NoError || document.isNull()){
		return QByteArray();
	}

	return document.toJson(QJsonDocument::Compact);
}


void AddInvalidPayloadRows()
{
	QTest::addColumn<int>("invalidCase");
	QTest::newRow("required objects missing") << int(IPC_REQUIRED_OBJECTS_MISSING);
	QTest::newRow("required scalars missing") << int(IPC_REQUIRED_SCALARS_MISSING);
	QTest::newRow("required objects null") << int(IPC_REQUIRED_OBJECTS_NULL);
	QTest::newRow("required scalars null") << int(IPC_REQUIRED_SCALARS_NULL);
	QTest::newRow("required object element null") << int(IPC_REQUIRED_OBJECT_ELEMENT_NULL);
	QTest::newRow("required scalar element null") << int(IPC_REQUIRED_SCALAR_ELEMENT_NULL);
	QTest::newRow("nullable required object element null") << int(IPC_NULLABLE_REQUIRED_OBJECT_ELEMENT_NULL);
	QTest::newRow("nullable required scalar element null") << int(IPC_NULLABLE_REQUIRED_SCALAR_ELEMENT_NULL);
}


// Negative tests intentionally violate ArraySerialization.sdl and require one validation warning per backend.
void ExpectInvalidArrayValueWarning()
{
	const QRegularExpression expectedError(QStringLiteral(
		".*Error: (Field: '[^']+' doesn't exist, but required|Field '[^']+' is missing, but required\\.?|Array field '[^']+' contains a null element)"));
	QTest::ignoreMessage(QtWarningMsg, expectedError);
}


} // namespace


void CSdlGenTest::TestArrayBackendsRoundTrip_data()
{
	QTest::addColumn<int>("payloadCase");
	QTest::newRow("populated and null elements") << int(VPC_FULL);
	QTest::newRow("all arrays empty") << int(VPC_ALL_EMPTY);
	QTest::newRow("nullable arrays null") << int(VPC_NULLABLE_NULL);
}


void CSdlGenTest::TestArrayBackendsRoundTrip()
{
	QFETCH(int, payloadCase);
	const CArrayPayload source = CreatePayload(ValidPayloadCase(payloadCase));

	QJsonObject jsonObject;
	QVERIFY(source.WriteToJsonObject(jsonObject));
	CArrayPayload jsonResult;
	QVERIFY(jsonResult.ReadFromJsonObject(jsonObject));
	QVERIFY(PayloadsEqual(jsonResult, source));

	imtgql::CGqlParamObject gqlObject;
	QVERIFY(source.WriteToGraphQlObject(gqlObject));
	CArrayPayload gqlResult;
	QVERIFY(gqlResult.ReadFromGraphQlObject(gqlObject));
	QVERIFY(PayloadsEqual(gqlResult, source));

	imtbase::CTreeItemModel treeModel;
	QVERIFY(source.WriteToModel(treeModel));
	CArrayPayload treeResult;
	QVERIFY(treeResult.ReadFromModel(treeModel));
	QVERIFY(PayloadsEqual(treeResult, source));
}


void CSdlGenTest::TestArrayReadersRejectInvalidValues_data()
{
	AddInvalidPayloadRows();
}


void CSdlGenTest::TestArrayReadersRejectInvalidValues()
{
	QFETCH(int, invalidCase);
	const auto typedCase = InvalidPayloadCase(invalidCase);

	CArrayPayload jsonResult;
	ExpectInvalidArrayValueWarning();
	QVERIFY(!jsonResult.ReadFromJsonObject(CreateInvalidJsonObject(typedCase)));

	CArrayPayload gqlResult;
	ExpectInvalidArrayValueWarning();
	QVERIFY(!gqlResult.ReadFromGraphQlObject(CreateInvalidGraphQlObject(typedCase)));

	imtbase::CTreeItemModel treeModel;
	QVERIFY(CreatePayload(VPC_ALL_EMPTY).WriteToModel(treeModel));
	QVERIFY(ApplyInvalidTreeState(treeModel, typedCase));
	CArrayPayload treeResult;
	ExpectInvalidArrayValueWarning();
	QVERIFY(!treeResult.ReadFromModel(treeModel));
}


void CSdlGenTest::TestArrayWritersRejectInvalidValues_data()
{
	AddInvalidPayloadRows();
}


void CSdlGenTest::TestArrayWritersRejectInvalidValues()
{
	QFETCH(int, invalidCase);
	const CArrayPayload payload = CreateInvalidPayload(InvalidPayloadCase(invalidCase));

	QJsonObject jsonObject;
	ExpectInvalidArrayValueWarning();
	QVERIFY(!payload.WriteToJsonObject(jsonObject));

	imtgql::CGqlParamObject gqlObject;
	ExpectInvalidArrayValueWarning();
	QVERIFY(!payload.WriteToGraphQlObject(gqlObject));

	imtbase::CTreeItemModel treeModel;
	ExpectInvalidArrayValueWarning();
	QVERIFY(!payload.WriteToModel(treeModel));
}


void CSdlGenTest::TestArrayOptionalReadersPreserveMissingFields()
{
	const CArrayPayload expected = CreatePayload(VPC_FULL);

	CArrayPayload jsonResult = CreatePayload(VPC_FULL);
	QVERIFY(jsonResult.OptReadFromJsonObject(QJsonObject()));
	QVERIFY(PayloadsEqual(jsonResult, expected));

	CArrayPayload gqlResult = CreatePayload(VPC_FULL);
	QVERIFY(gqlResult.OptReadFromGraphQlObject(imtgql::CGqlParamObject()));
	QVERIFY(PayloadsEqual(gqlResult, expected));

	CArrayPayload treeResult = CreatePayload(VPC_FULL);
	QVERIFY(treeResult.OptReadFromModel(imtbase::CTreeItemModel()));
	QVERIFY(PayloadsEqual(treeResult, expected));
}


void CSdlGenTest::TestArrayQObjectBridge()
{
	CArrayPayloadObject object;
	static_cast<CArrayPayload&>(object) = CreatePayload(VPC_FULL);

	auto* requiredObjects = object.GetRequiredObjects().value<CAttributeObjectList*>();
	QVERIFY(requiredObjects != nullptr);
	QCOMPARE(requiredObjects->getItemsCount(), 1);

	auto* nullableObjects = object.GetNullableObjects().value<CAttributeObjectList*>();
	QVERIFY(nullableObjects != nullptr);
	QCOMPARE(nullableObjects->getItemsCount(), 2);
	QVERIFY(!nullableObjects->getData(QStringLiteral("item"), 0).isValid());
	auto* nullableAttribute = nullableObjects->getData(QStringLiteral("item"), 1).value<CAttributeObject*>();
	QVERIFY(nullableAttribute != nullptr);
	QCOMPARE(nullableAttribute->GetValue().toString(), QStringLiteral("nullable"));

	const CArrayPayload beforeRejectedSetters = CreatePayload(VPC_FULL);
	CAttributeObjectList invalidObjectList;
	invalidObjectList.append(nullptr);
	object.SetRequiredObjects(QVariant::fromValue(&invalidObjectList));
	object.SetNullableObjectsWithRequiredElements(QVariant::fromValue(&invalidObjectList));
	object.SetRequiredScalars(QVariantList{QVariant()});
	object.SetNullableScalarsWithRequiredElements(QVariantList{QVariant()});
	QVERIFY(PayloadsEqual(static_cast<const CArrayPayload&>(object), beforeRejectedSetters));

	CAttributeObjectList nullableObjectList;
	nullableObjectList.append(nullptr);
	object.SetNullableObjects(QVariant::fromValue(&nullableObjectList));
	QVERIFY(object.nullableObjects.HasValue());
	QCOMPARE(object.nullableObjects->size(), 1);
	QVERIFY(object.nullableObjects->at(0).IsNull());

	object.SetNullableScalars(QVariantList{QVariant(), QStringLiteral("value")});
	QVERIFY(object.nullableScalars.HasValue());
	QCOMPARE(object.nullableScalars->size(), 2);
	QVERIFY(object.nullableScalars->at(0).IsNull());
	QCOMPARE(*object.nullableScalars->at(1), QStringLiteral("value"));

	object.SetNullableObjects(QVariant());
	object.SetNullableScalars(QVariant());
	QVERIFY(object.nullableObjects.IsNull());
	QVERIFY(object.nullableScalars.IsNull());

	CAttributeObjectList listModel;
	listModel.append(nullptr);
	CAttributeObject attributeObject;
	attributeObject.SetValue(QStringLiteral("copied"));
	listModel.append(&attributeObject);
	QScopedPointer<CAttributeObjectList> copiedList(listModel.copyMe());
	QVERIFY(copiedList != nullptr);
	QVERIFY(listModel.isEqualWithModel(copiedList.data()));
	QCOMPARE(CanonicalJson(copiedList->toJson().toUtf8()),
		QByteArrayLiteral("[null,{\"__typename\":\"Attribute\",\"value\":\"copied\"}]"));
}


void CSdlGenTest::TestArrayJsonFileRoundTrip()
{
	QVERIFY2(m_arraySerializationOutputDirectory.isValid(), "Unable to create a temporary output directory");
	QVERIFY2(QDir(s_rawDataDirectoryPath).exists(), qPrintable(s_rawDataDirectoryPath));

	QByteArray inputData;
	const QString inputPath = s_rawDataDirectoryPath + QStringLiteral("/Inputs/ArrayPayload.json");
	QVERIFY2(ReadFile(inputPath, inputData), qPrintable(inputPath));

	QJsonParseError inputError;
	const QJsonDocument inputDocument = QJsonDocument::fromJson(inputData, &inputError);
	QCOMPARE(inputError.error, QJsonParseError::NoError);
	QVERIFY(inputDocument.isObject());

	CArrayPayload payload;
	QVERIFY(payload.ReadFromJsonObject(inputDocument.object()));
	QVERIFY(PayloadsEqual(payload, CreatePayload(VPC_FULL)));

	QJsonObject actualObject;
	QVERIFY(payload.WriteToJsonObject(actualObject));
	const QByteArray actualData = QJsonDocument(actualObject).toJson(QJsonDocument::Compact);
	const QString actualPath = m_arraySerializationOutputDirectory.filePath(QStringLiteral("ArrayPayload.actual.json"));
	QVERIFY2(WriteFile(actualPath, actualData), qPrintable(actualPath));

	QByteArray actualFileData;
	QVERIFY(ReadFile(actualPath, actualFileData));
	QByteArray expectedData;
	const QString expectedPath = s_rawDataDirectoryPath + QStringLiteral("/ReferenceData/ArrayPayload.json");
	QVERIFY2(ReadFile(expectedPath, expectedData), qPrintable(expectedPath));
	QCOMPARE(actualFileData, CanonicalJson(expectedData));

	CArrayPayloadObject object;
	QVERIFY(object.createFromJson(QString::fromUtf8(inputData)));
	QCOMPARE(CanonicalJson(object.toJson().toUtf8()), CanonicalJson(expectedData));
}


void CSdlGenTest::TestArrayGraphQlRequestFileRoundTrip()
{
	QVERIFY2(m_arraySerializationOutputDirectory.isValid(), "Unable to create a temporary output directory");
	QVERIFY2(QDir(s_rawDataDirectoryPath).exists(), qPrintable(s_rawDataDirectoryPath));

	QByteArray inputData;
	const QString inputPath = s_rawDataDirectoryPath + QStringLiteral("/Inputs/SavePayload.gqlreq");
	QVERIFY2(ReadFile(inputPath, inputData), qPrintable(inputPath));

	imtgql::CGqlRequest parsedRequest;
	qsizetype errorPosition = -1;
	QVERIFY(parsedRequest.ParseQuery(inputData, errorPosition));
	QVERIFY(errorPosition < 0);
	QCOMPARE(parsedRequest.GetRequestType(), imtgql::IGqlRequest::RT_MUTATION);
	QCOMPARE(parsedRequest.GetCommandId(), QByteArrayLiteral("SavePayload"));

	CSavePayloadGqlRequest typedRequest(parsedRequest, false);
	QVERIFY(typedRequest.IsValid());
	QVERIFY(typedRequest.GetRequestedArguments().input.HasValue());
	const CArrayPayload& parsedPayload = *typedRequest.GetRequestedArguments().input;
	QJsonObject parsedPayloadObject;
	QVERIFY(parsedPayload.WriteToJsonObject(parsedPayloadObject));
	QByteArray expectedPayloadData;
	const QString expectedPayloadPath = s_rawDataDirectoryPath + QStringLiteral("/ReferenceData/ArrayPayload.json");
	QVERIFY2(ReadFile(expectedPayloadPath, expectedPayloadData), qPrintable(expectedPayloadPath));
	QCOMPARE(QJsonDocument(parsedPayloadObject).toJson(QJsonDocument::Compact), CanonicalJson(expectedPayloadData));
	QVERIFY(PayloadsEqual(parsedPayload, CreatePayload(VPC_FULL)));

	QByteArray expectedData;
	const QString expectedPath = s_rawDataDirectoryPath + QStringLiteral("/ReferenceData/SavePayload.gqlreq");
	QVERIFY2(ReadFile(expectedPath, expectedData), qPrintable(expectedPath));
	expectedData = expectedData.trimmed();
	QCOMPARE(parsedRequest.GetQuery(), expectedData);

	const QString actualPath = m_arraySerializationOutputDirectory.filePath(QStringLiteral("SavePayload.actual.gqlreq"));
	QVERIFY2(WriteFile(actualPath, parsedRequest.GetQuery()), qPrintable(actualPath));
	QByteArray actualFileData;
	QVERIFY(ReadFile(actualPath, actualFileData));
	QCOMPARE(actualFileData, expectedData);

	SavePayloadRequestArguments arguments;
	arguments.input = CreatePayload(VPC_FULL);
	imtgql::CGqlRequest createdRequest(imtgql::IGqlRequest::RT_MUTATION);
	QVERIFY(CSavePayloadGqlRequest::SetupGqlRequest(createdRequest, arguments));
	createdRequest.AddSimpleField("accepted");
	QCOMPARE(createdRequest.GetQuery(), expectedData);

	imtgql::CGqlRequest reparsedRequest;
	errorPosition = -1;
	QVERIFY(reparsedRequest.ParseQuery(actualFileData, errorPosition));
	QVERIFY(errorPosition < 0);
	CSavePayloadGqlRequest reparsedTypedRequest(reparsedRequest, false);
	QVERIFY(reparsedTypedRequest.IsValid());
	QVERIFY(PayloadsEqual(*reparsedTypedRequest.GetRequestedArguments().input, CreatePayload(VPC_FULL)));
}


} // namespace imtsdlgentest

#include "PrinterBase.h"


namespace sdl::V1_0::modsdl
{


QByteArray CPrinterSpecificationBase::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPrinterSpecificationBase::operator==(const CPrinterSpecificationBase& other) const
{
	return 
				name == other.name;
}


bool V1_0::modsdl::CPrinterSpecificationBase::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "PrinterSpecificationBase", modelIndex);

	if (name){
		model.SetData("name", *name, modelIndex);
	}


	return true;
}


bool V1_0::modsdl::CPrinterSpecificationBase::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	return true;
}


bool V1_0::modsdl::CPrinterSpecificationBase::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	return true;
}


bool V1_0::modsdl::CPrinterSpecificationBase::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (name){
		gqlObject.InsertParam("name", QVariant(*name));
	}

	gqlObject.InsertParam("__typename", QVariant("PrinterSpecificationBase"));

	return true;
}


bool V1_0::modsdl::CPrinterSpecificationBase::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	return true;
}


bool V1_0::modsdl::CPrinterSpecificationBase::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	return true;
}


bool V1_0::modsdl::CPrinterSpecificationBase::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (name){
		jsonObject["name"] = QJsonValue::fromVariant(*name);
	}

	jsonObject["__typename"] = "PrinterSpecificationBase";

	return true;
}


bool V1_0::modsdl::CPrinterSpecificationBase::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	return true;
}


bool V1_0::modsdl::CPrinterSpecificationBase::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	return true;
}




QByteArray CLink::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CLink::operator==(const CLink& other) const
{
	return 
				link == other.link;
}


bool V1_0::modsdl::CLink::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Link", modelIndex);

	if (!link){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("link", *link, modelIndex);


	return true;
}


bool V1_0::modsdl::CLink::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant linkData = model.GetData("link", modelIndex);
	if (linkData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	link = linkData.toByteArray();

	return true;
}


bool V1_0::modsdl::CLink::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant linkData = model.GetData("link", modelIndex);
	if (!linkData.isNull()){
		link = linkData.toByteArray();
	}

	return true;
}


bool V1_0::modsdl::CLink::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!link){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("link", QVariant(*link));

	gqlObject.InsertParam("__typename", QVariant("Link"));

	return true;
}


bool V1_0::modsdl::CLink::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("link") || (gqlObject["link"].userType() != QMetaType::QString && gqlObject["link"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	link = gqlObject["link"].toByteArray();

	return true;
}


bool V1_0::modsdl::CLink::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("link") && (gqlObject["link"].userType() == QMetaType::QString || gqlObject["link"].userType() == QMetaType::QByteArray)){
		link = gqlObject["link"].toByteArray();
	}

	return true;
}


bool V1_0::modsdl::CLink::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!link){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["link"] = QString::fromUtf8(*link);

	jsonObject["__typename"] = "Link";

	return true;
}


bool V1_0::modsdl::CLink::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("link") || ! jsonObject["link"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "link").toLocal8Bit().constData();)

		return false;
	}
	link = jsonObject["link"].toString().toUtf8();

	return true;
}


bool V1_0::modsdl::CLink::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("link") && jsonObject["link"].isString()){
		link = jsonObject["link"].toString().toUtf8();
	}

	return true;
}




QByteArray CPrinterBase::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPrinterBase::operator==(const CPrinterBase& other) const
{
	return 
				name == other.name &&
				specification == other.specification &&
				simpleTest == other.simpleTest &&
				mixedTest == other.mixedTest;
}


bool V1_0::modsdl::CPrinterBase::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "PrinterBase", modelIndex);

	if (name){
		model.SetData("name", *name, modelIndex);
	}

	if (!specification){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	QVariant specificationVariantValue;
		if (const V1_0::modsdl::CPrinterSpecificationBase* printerSpecificationBaseVal = std::get_if<V1_0::modsdl::CPrinterSpecificationBase>(specification.GetPtr())){
			if (!printerSpecificationBaseVal->WriteToModel(*(model.AddTreeModel("specification", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("specification", modelIndex) != nullptr){
				model.GetTreeItemModel("specification", modelIndex)->SetData("__typename", "PrinterSpecificationBase", 0);
			}
		}
		else if (const V1_0::modsdl::CLink* linkVal = std::get_if<V1_0::modsdl::CLink>(specification.GetPtr())){
			if (!linkVal->WriteToModel(*(model.AddTreeModel("specification", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("specification", modelIndex) != nullptr){
				model.GetTreeItemModel("specification", modelIndex)->SetData("__typename", "Link", 0);
			}
		}


	if (!simpleTest){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "simpleTest").toLocal8Bit().constData();)

		return false;
	}
	QVariant simpleTestVariantValue;
		if (const QString* stringVal = std::get_if<QString>(simpleTest.GetPtr())){
			model.SetData("", *stringVal);
		}
		else if (const double* doubleVal = std::get_if<double>(simpleTest.GetPtr())){
			model.SetData("", *doubleVal);
		}


	if (!mixedTest){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	QVariant mixedTestVariantValue;
		if (const QString* stringVal = std::get_if<QString>(mixedTest.GetPtr())){
			model.SetData("", *stringVal);
		}
		else if (const V1_0::modsdl::CLink* linkVal = std::get_if<V1_0::modsdl::CLink>(mixedTest.GetPtr())){
			if (!linkVal->WriteToModel(*(model.AddTreeModel("mixedTest", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("mixedTest", modelIndex) != nullptr){
				model.GetTreeItemModel("mixedTest", modelIndex)->SetData("__typename", "Link", 0);
			}
		}



	return true;
}


bool V1_0::modsdl::CPrinterBase::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant specificationData = model.GetData("specification", modelIndex);
	if (specificationData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	QString specificationTypename = model.GetTreeItemModel("specification", modelIndex)->GetData("__typename").toString();
	if (specificationTypename == "PrinterSpecificationBase") {
		V1_0::modsdl::CPrinterSpecificationBase specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
		if (!isspecificationRead){
			return false;
		}
		specification = PrinterSpecification(specificationConvert);
	}
	else if (specificationTypename == "Link") {
		V1_0::modsdl::CLink specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
		if (!isspecificationRead){
			return false;
		}
		specification = PrinterSpecification(specificationConvert);
	}

	QVariant simpleTestData = model.GetData("simpleTest", modelIndex);
	if (simpleTestData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "simpleTest").toLocal8Bit().constData();)

		return false;
	}
		if (simpleTestData.canConvert<QString>()){
		simpleTest = SimpleUnion(simpleTestData.value<QString>());
	}
	else if (simpleTestData.canConvert<double>()){
		simpleTest = SimpleUnion(simpleTestData.value<double>());
	}

	QVariant mixedTestData = model.GetData("mixedTest", modelIndex);
	if (mixedTestData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	QString mixedTestTypename = model.GetTreeItemModel("mixedTest", modelIndex)->GetData("__typename").toString();
	if (mixedTestData.canConvert<QString>()){
		mixedTest = MixedUnion(mixedTestData.value<QString>());
	}
	else if (mixedTestTypename == "Link") {
		V1_0::modsdl::CLink mixedTestConvert;
		const bool ismixedTestRead = mixedTestConvert.ReadFromModel(*model.GetTreeItemModel("mixedTest", modelIndex)); 
		if (!ismixedTestRead){
			return false;
		}
		mixedTest = MixedUnion(mixedTestConvert);
	}

	return true;
}


bool V1_0::modsdl::CPrinterBase::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant specificationData = model.GetData("specification", modelIndex);
	if (!specificationData.isNull()){
		QString specificationTypename = model.GetTreeItemModel("specification", modelIndex)->GetData("__typename").toString();
		if (specificationTypename == "PrinterSpecificationBase") {
			V1_0::modsdl::CPrinterSpecificationBase specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
			if (!isspecificationRead){
				return false;
			}
			specification = PrinterSpecification(specificationConvert);
		}
		else if (specificationTypename == "Link") {
			V1_0::modsdl::CLink specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
			if (!isspecificationRead){
				return false;
			}
			specification = PrinterSpecification(specificationConvert);
		}
	}

	QVariant simpleTestData = model.GetData("simpleTest", modelIndex);
	if (!simpleTestData.isNull()){
		if (simpleTestData.canConvert<QString>()){
			simpleTest = SimpleUnion(simpleTestData.value<QString>());
		}
		else if (simpleTestData.canConvert<double>()){
			simpleTest = SimpleUnion(simpleTestData.value<double>());
		}
	}

	QVariant mixedTestData = model.GetData("mixedTest", modelIndex);
	if (!mixedTestData.isNull()){
		QString mixedTestTypename = model.GetTreeItemModel("mixedTest", modelIndex)->GetData("__typename").toString();
		if (mixedTestData.canConvert<QString>()){
			mixedTest = MixedUnion(mixedTestData.value<QString>());
		}
		else if (mixedTestTypename == "Link") {
			V1_0::modsdl::CLink mixedTestConvert;
			const bool ismixedTestRead = mixedTestConvert.ReadFromModel(*model.GetTreeItemModel("mixedTest", modelIndex)); 
			if (!ismixedTestRead){
				return false;
			}
			mixedTest = MixedUnion(mixedTestConvert);
		}
	}

	return true;
}


bool V1_0::modsdl::CPrinterBase::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (name){
		gqlObject.InsertParam("name", QVariant(*name));
	}

	if (!specification){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	::imtgql::CGqlParamObject specificationDataObject;
	if (const V1_0::modsdl::CPrinterSpecificationBase* printerSpecificationBaseVal = std::get_if<V1_0::modsdl::CPrinterSpecificationBase>(specification.GetPtr())){
		if (!printerSpecificationBaseVal->WriteToGraphQlObject(specificationDataObject)){
			return false;
		}
		specificationDataObject.InsertParam("__typename", QVariant("PrinterSpecificationBase"));
	}
	else if (const V1_0::modsdl::CLink* linkVal = std::get_if<V1_0::modsdl::CLink>(specification.GetPtr())){
		if (!linkVal->WriteToGraphQlObject(specificationDataObject)){
			return false;
		}
		specificationDataObject.InsertParam("__typename", QVariant("Link"));
	}
	gqlObject.InsertParam("specification", specificationDataObject);

	if (!simpleTest){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "simpleTest").toLocal8Bit().constData();)

		return false;
	}
	::imtgql::CGqlParamObject simpleTestDataObject;
	if (const QString* stringVal = std::get_if<QString>(simpleTest.GetPtr())){
		simpleTestDataObject.InsertParam("simpleTest", *stringVal);
	}
	else if (const double* doubleVal = std::get_if<double>(simpleTest.GetPtr())){
		simpleTestDataObject.InsertParam("simpleTest", *doubleVal);
	}
	gqlObject.InsertParam("simpleTest", simpleTestDataObject);

	if (!mixedTest){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	::imtgql::CGqlParamObject mixedTestDataObject;
	if (const QString* stringVal = std::get_if<QString>(mixedTest.GetPtr())){
		mixedTestDataObject.InsertParam("mixedTest", *stringVal);
	}
	else if (const V1_0::modsdl::CLink* linkVal = std::get_if<V1_0::modsdl::CLink>(mixedTest.GetPtr())){
		if (!linkVal->WriteToGraphQlObject(mixedTestDataObject)){
			return false;
		}
		mixedTestDataObject.InsertParam("__typename", QVariant("Link"));
	}
	gqlObject.InsertParam("mixedTest", mixedTestDataObject);

	gqlObject.InsertParam("__typename", QVariant("PrinterBase"));

	return true;
}


bool V1_0::modsdl::CPrinterBase::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (!gqlObject.ContainsParam("specification")){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	const ::imtgql::CGqlParamObject* specificationDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
	if (!specificationDataObjectPtr->ContainsParam("__typename")){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	QString specificationTypename = specificationDataObjectPtr->GetParamArgumentValue("__typename").toString();
	if (specificationTypename == "PrinterSpecificationBase") {
		V1_0::modsdl::CPrinterSpecificationBase specificationConvert;
		const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
		if (!isSpecificationRead){
			return false;
		}
		specification = PrinterSpecification(specificationConvert);
	}
	else if (specificationTypename == "Link") {
		V1_0::modsdl::CLink specificationConvert;
		const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
		if (!isSpecificationRead){
			return false;
		}
		specification = PrinterSpecification(specificationConvert);
	}

	if (!gqlObject.ContainsParam("simpleTest")){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "simpleTest").toLocal8Bit().constData();)

		return false;
	}
	QVariant simpleTestVariantValue = gqlObject["simpleTest"];
	if (simpleTestVariantValue.canConvert<QString>()){
		simpleTest = SimpleUnion(simpleTestVariantValue.value<QString>());
	}
	else if (simpleTestVariantValue.canConvert<double>()){
		simpleTest = SimpleUnion(simpleTestVariantValue.value<double>());
	}

	if (!gqlObject.ContainsParam("mixedTest")){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	const ::imtgql::CGqlParamObject* mixedTestDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("mixedTest");
	if (!mixedTestDataObjectPtr->ContainsParam("__typename")){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	QString mixedTestTypename = mixedTestDataObjectPtr->GetParamArgumentValue("__typename").toString();
	QVariant mixedTestVariantValue = gqlObject["mixedTest"];
	if (mixedTestVariantValue.canConvert<QString>()){
		mixedTest = MixedUnion(mixedTestVariantValue.value<QString>());
	}
	else if (mixedTestTypename == "Link") {
		V1_0::modsdl::CLink mixedTestConvert;
		const bool isMixedTestRead = mixedTestConvert.ReadFromGraphQlObject(*mixedTestDataObjectPtr);
		if (!isMixedTestRead){
			return false;
		}
		mixedTest = MixedUnion(mixedTestConvert);
	}

	return true;
}


bool V1_0::modsdl::CPrinterBase::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (gqlObject.ContainsParam("specification")){
		const ::imtgql::CGqlParamObject* specificationDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
		if (!specificationDataObjectPtr->ContainsParam("__typename")){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

			return false;
		}
		QString specificationTypename = specificationDataObjectPtr->GetParamArgumentValue("__typename").toString();
		if (specificationTypename == "PrinterSpecificationBase") {
			V1_0::modsdl::CPrinterSpecificationBase specificationConvert;
			const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
			if (!isSpecificationRead){
				return false;
			}
			specification = PrinterSpecification(specificationConvert);
		}
		else if (specificationTypename == "Link") {
			V1_0::modsdl::CLink specificationConvert;
			const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
			if (!isSpecificationRead){
				return false;
			}
			specification = PrinterSpecification(specificationConvert);
		}
	}

	if (gqlObject.ContainsParam("simpleTest")){
		QVariant simpleTestVariantValue = gqlObject["simpleTest"];
		if (simpleTestVariantValue.canConvert<QString>()){
			simpleTest = SimpleUnion(simpleTestVariantValue.value<QString>());
		}
		else if (simpleTestVariantValue.canConvert<double>()){
			simpleTest = SimpleUnion(simpleTestVariantValue.value<double>());
		}
	}

	if (gqlObject.ContainsParam("mixedTest")){
		const ::imtgql::CGqlParamObject* mixedTestDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("mixedTest");
		if (!mixedTestDataObjectPtr->ContainsParam("__typename")){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

			return false;
		}
		QString mixedTestTypename = mixedTestDataObjectPtr->GetParamArgumentValue("__typename").toString();
		QVariant mixedTestVariantValue = gqlObject["mixedTest"];
		if (mixedTestVariantValue.canConvert<QString>()){
			mixedTest = MixedUnion(mixedTestVariantValue.value<QString>());
		}
		else if (mixedTestTypename == "Link") {
			V1_0::modsdl::CLink mixedTestConvert;
			const bool isMixedTestRead = mixedTestConvert.ReadFromGraphQlObject(*mixedTestDataObjectPtr);
			if (!isMixedTestRead){
				return false;
			}
			mixedTest = MixedUnion(mixedTestConvert);
		}
	}

	return true;
}


bool V1_0::modsdl::CPrinterBase::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (name){
		jsonObject["name"] = QJsonValue::fromVariant(*name);
	}

	if (!specification){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	if (const V1_0::modsdl::CPrinterSpecificationBase* printerSpecificationBaseVal = std::get_if<V1_0::modsdl::CPrinterSpecificationBase>(specification.GetPtr())){
		QJsonObject specificationJsonObject;
		const bool isspecificationAdded = printerSpecificationBaseVal->WriteToJsonObject(specificationJsonObject);
		if (!isspecificationAdded){
			return false;
		}
		specificationJsonObject["__typename"] = "PrinterSpecificationBase";
		jsonObject["specification"] = specificationJsonObject;
	}
	else if (const V1_0::modsdl::CLink* linkVal = std::get_if<V1_0::modsdl::CLink>(specification.GetPtr())){
		QJsonObject specificationJsonObject;
		const bool isspecificationAdded = linkVal->WriteToJsonObject(specificationJsonObject);
		if (!isspecificationAdded){
			return false;
		}
		specificationJsonObject["__typename"] = "Link";
		jsonObject["specification"] = specificationJsonObject;
	}


	if (!simpleTest){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "simpleTest").toLocal8Bit().constData();)

		return false;
	}
	if (const QString* stringVal = std::get_if<QString>(simpleTest.GetPtr())){
		if (!simpleTest){
			return false;
		}
		jsonObject["simpleTest"] = QJsonValue::fromVariant(*stringVal);
	}
	else if (const double* doubleVal = std::get_if<double>(simpleTest.GetPtr())){
		if (!simpleTest){
			return false;
		}
		jsonObject["simpleTest"] = QJsonValue::fromVariant(*doubleVal);
	}


	if (!mixedTest){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	if (const QString* stringVal = std::get_if<QString>(mixedTest.GetPtr())){
		if (!mixedTest){
			return false;
		}
		jsonObject["mixedTest"] = QJsonValue::fromVariant(*stringVal);
	}
	else if (const V1_0::modsdl::CLink* linkVal = std::get_if<V1_0::modsdl::CLink>(mixedTest.GetPtr())){
		QJsonObject mixedTestJsonObject;
		const bool ismixedTestAdded = linkVal->WriteToJsonObject(mixedTestJsonObject);
		if (!ismixedTestAdded){
			return false;
		}
		mixedTestJsonObject["__typename"] = "Link";
		jsonObject["mixedTest"] = mixedTestJsonObject;
	}


	jsonObject["__typename"] = "PrinterBase";

	return true;
}


bool V1_0::modsdl::CPrinterBase::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (!jsonObject.contains("specification")){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	const QJsonObject specificationObject = jsonObject.value("specification").toObject();
	if (!jsonObject.value("specification").isObject() || !specificationObject.contains("__typename")){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

		return false;
	}
	QString specificationTypename = specificationObject.value("__typename").toString();
	if (specificationTypename == "PrinterSpecificationBase") {
		V1_0::modsdl::CPrinterSpecificationBase specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
		if (!isspecificationRead){
			return false;
		}
		specification = PrinterSpecification(specificationConvert);
	}
	else if (specificationTypename == "Link") {
		V1_0::modsdl::CLink specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
		if (!isspecificationRead){
			return false;
		}
		specification = PrinterSpecification(specificationConvert);
	}

	if (!jsonObject.contains("simpleTest")){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "simpleTest").toLocal8Bit().constData();)

		return false;
	}
	QVariant simpleTestVariantValue = jsonObject["simpleTest"].toVariant();
	if (simpleTestVariantValue.canConvert<QString>()){
		simpleTest = SimpleUnion(simpleTestVariantValue.value<QString>());
	}
	else if (simpleTestVariantValue.canConvert<double>()){
		simpleTest = SimpleUnion(simpleTestVariantValue.value<double>());
	}

	if (!jsonObject.contains("mixedTest")){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	const QJsonObject mixedTestObject = jsonObject.value("mixedTest").toObject();
	if (!jsonObject.value("mixedTest").isObject() || !mixedTestObject.contains("__typename")){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

		return false;
	}
	QString mixedTestTypename = mixedTestObject.value("__typename").toString();
	QVariant mixedTestVariantValue = jsonObject["mixedTest"].toVariant();
	if (mixedTestVariantValue.canConvert<QString>()){
		mixedTest = MixedUnion(mixedTestVariantValue.value<QString>());
	}
	else if (mixedTestTypename == "Link") {
		V1_0::modsdl::CLink mixedTestConvert;
		const bool ismixedTestRead = mixedTestConvert.ReadFromJsonObject(jsonObject["mixedTest"].toObject());
		if (!ismixedTestRead){
			return false;
		}
		mixedTest = MixedUnion(mixedTestConvert);
	}

	return true;
}


bool V1_0::modsdl::CPrinterBase::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (jsonObject.contains("specification")){
		const QJsonObject specificationObject = jsonObject.value("specification").toObject();
		if (!jsonObject.value("specification").isObject() || !specificationObject.contains("__typename")){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "specification").toLocal8Bit().constData();)

			return false;
		}
		QString specificationTypename = specificationObject.value("__typename").toString();
		if (specificationTypename == "PrinterSpecificationBase") {
			V1_0::modsdl::CPrinterSpecificationBase specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
			if (!isspecificationRead){
				return false;
			}
			specification = PrinterSpecification(specificationConvert);
		}
		else if (specificationTypename == "Link") {
			V1_0::modsdl::CLink specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
			if (!isspecificationRead){
				return false;
			}
			specification = PrinterSpecification(specificationConvert);
		}
	}

	if (jsonObject.contains("simpleTest")){
		QVariant simpleTestVariantValue = jsonObject["simpleTest"].toVariant();
		if (simpleTestVariantValue.canConvert<QString>()){
			simpleTest = SimpleUnion(simpleTestVariantValue.value<QString>());
		}
		else if (simpleTestVariantValue.canConvert<double>()){
			simpleTest = SimpleUnion(simpleTestVariantValue.value<double>());
		}
	}

	if (jsonObject.contains("mixedTest")){
		const QJsonObject mixedTestObject = jsonObject.value("mixedTest").toObject();
		if (!jsonObject.value("mixedTest").isObject() || !mixedTestObject.contains("__typename")){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedTest").toLocal8Bit().constData();)

			return false;
		}
		QString mixedTestTypename = mixedTestObject.value("__typename").toString();
		QVariant mixedTestVariantValue = jsonObject["mixedTest"].toVariant();
		if (mixedTestVariantValue.canConvert<QString>()){
			mixedTest = MixedUnion(mixedTestVariantValue.value<QString>());
		}
		else if (mixedTestTypename == "Link") {
			V1_0::modsdl::CLink mixedTestConvert;
			const bool ismixedTestRead = mixedTestConvert.ReadFromJsonObject(jsonObject["mixedTest"].toObject());
			if (!ismixedTestRead){
				return false;
			}
			mixedTest = MixedUnion(mixedTestConvert);
		}
	}

	return true;
}




QByteArray CPrinterList::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPrinterList::operator==(const CPrinterList& other) const
{
	return 
				data == other.data;
}


bool V1_0::modsdl::CPrinterList::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "PrinterList", modelIndex);

	if (data){
		::imtbase::CTreeItemModel* newDataModelPtr = model.AddTreeModel("data", modelIndex);
		newDataModelPtr->setIsArray(true);
		for (qsizetype dataIndex = 0; dataIndex < data->size(); ++dataIndex){
			if (!data->at(dataIndex)){
				newDataModelPtr->InsertNewItem();
				newDataModelPtr->SetData(QByteArray(), QVariant(), dataIndex);
				continue;
			}
			newDataModelPtr->InsertNewItem();
			if (!(data->at(dataIndex)->WriteToModel(*newDataModelPtr, dataIndex))){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
		}
	}
	else if (data.IsNull()){
		model.SetData("data", QVariant(), modelIndex);
	}

	return true;
}


bool V1_0::modsdl::CPrinterList::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* dataModel = model.GetTreeItemModel("data", modelIndex);
	if (model.ContainsKey("data", modelIndex) && dataModel == nullptr){
		data.SetNull();
	}
	if (dataModel != nullptr){
		int dataCount = dataModel->GetItemsCount();
		imtsdl::TElementList<V1_0::modsdl::CPrinterBase> dataList;
		for (int dataIndex = 0; dataIndex < dataCount; ++dataIndex){
			if (dataModel->ContainsKey(QByteArray(), dataIndex) && !dataModel->GetData(QByteArray(), dataIndex).isValid()){
				dataList.AppendNull();
				continue;
			}
			V1_0::modsdl::CPrinterBase t_data;
			if (!t_data.ReadFromModel(*dataModel, dataIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			dataList << t_data;
		}
		data = dataList;

	}

	return true;
}


bool V1_0::modsdl::CPrinterList::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* dataModel = model.GetTreeItemModel("data", modelIndex);
	if (model.ContainsKey("data", modelIndex) && dataModel == nullptr){
		data.SetNull();
	}
	if (dataModel != nullptr){
		int dataCount = dataModel->GetItemsCount();
		imtsdl::TElementList<V1_0::modsdl::CPrinterBase> dataList;
		for (int dataIndex = 0; dataIndex < dataCount; ++dataIndex){
			if (dataModel->ContainsKey(QByteArray(), dataIndex) && !dataModel->GetData(QByteArray(), dataIndex).isValid()){
				dataList.AppendNull();
				continue;
			}
			V1_0::modsdl::CPrinterBase t_data;
			if (!t_data.OptReadFromModel(*dataModel, dataIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			dataList << t_data;
		}
		data = dataList;

	}

	return true;
}


bool V1_0::modsdl::CPrinterList::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (data){
		QList<::imtgql::CGqlParamObject> dataDataObjectList;
		for (qsizetype dataIndex = 0; dataIndex < data->size(); ++dataIndex){
			if (!data->at(dataIndex)){
				dataDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
				continue;
			}
			::imtgql::CGqlParamObject newDataGqlObject;
			if (!data->at(dataIndex)->WriteToGraphQlObject(newDataGqlObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			dataDataObjectList << newDataGqlObject;
		}
		gqlObject.InsertParam("data", dataDataObjectList);
	}
	else if (data.IsNull()){
		gqlObject.InsertParam("data", QVariant());
	}

	gqlObject.InsertParam("__typename", QVariant("PrinterList"));

	return true;
}


bool V1_0::modsdl::CPrinterList::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("data") && gqlObject["data"].isNull()){
		data.SetNull();
	}
	if (gqlObject.ContainsParam("data") && !(gqlObject["data"].isNull())){
		const qsizetype dataElementsCount = gqlObject.GetObjectsCount("data");
		data = imtsdl::TElementList<V1_0::modsdl::CPrinterBase>();
		for (qsizetype dataIndex = 0; dataIndex < dataElementsCount; ++dataIndex){
			const ::imtgql::CGqlParamObject* dataDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("data", dataIndex);
			if (dataDataObjectPtr == nullptr || dataDataObjectPtr->IsNull()){
				data->AppendNull();
				continue;
			}
			V1_0::modsdl::CPrinterBase tempData;
			if (!tempData.ReadFromGraphQlObject(*dataDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			data->append(tempData);
		}
	}

	return true;
}


bool V1_0::modsdl::CPrinterList::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("data") && gqlObject["data"].isNull()){
		data.SetNull();
	}
	if (gqlObject.ContainsParam("data") && !(gqlObject["data"].isNull())){
		const qsizetype dataElementsCount = gqlObject.GetObjectsCount("data");
		data = imtsdl::TElementList<V1_0::modsdl::CPrinterBase>();
		for (qsizetype dataIndex = 0; dataIndex < dataElementsCount; ++dataIndex){
			const ::imtgql::CGqlParamObject* dataDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("data", dataIndex);
			if (dataDataObjectPtr == nullptr || dataDataObjectPtr->IsNull()){
				data->AppendNull();
				continue;
			}
			V1_0::modsdl::CPrinterBase tempData;
			if (!tempData.OptReadFromGraphQlObject(*dataDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			data->append(tempData);
		}
	}

	return true;
}


bool V1_0::modsdl::CPrinterList::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (data){
		QJsonArray newDataArray;
		for (qsizetype dataIndex = 0; dataIndex < data->size(); ++dataIndex){
			if (!data->at(dataIndex)){
				newDataArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			QJsonObject newDataJsonObject;
			if (!data->at(dataIndex)->WriteToJsonObject(newDataJsonObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			newDataArray << newDataJsonObject;
		}
		jsonObject["data"] = newDataArray;
	}
	else if (data.IsNull()){
		jsonObject["data"] = QJsonValue(QJsonValue::Null);
	}

	jsonObject["__typename"] = "PrinterList";

	return true;
}


bool V1_0::modsdl::CPrinterList::ReadFromJsonObject(const QJsonObject& jsonObject)
{
if (jsonObject.contains("data") && jsonObject["data"].isNull()){
		data.SetNull();
	}
		if (jsonObject.contains("data") && jsonObject["data"].isArray()){
		const QJsonArray dataJsonArray = jsonObject["data"].toArray();
		const qsizetype dataArrayCount = dataJsonArray.size();
		data = imtsdl::TElementList<V1_0::modsdl::CPrinterBase>();
		for (qsizetype dataIndex = 0; dataIndex < dataArrayCount; ++dataIndex){
			if (dataJsonArray[dataIndex].isNull()){
				data->AppendNull();
				continue;
			}
			V1_0::modsdl::CPrinterBase tempData;
			if (!tempData.ReadFromJsonObject(dataJsonArray[dataIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			data->append(tempData);
		}
	}

	return true;
}


bool V1_0::modsdl::CPrinterList::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
if (jsonObject.contains("data") && jsonObject["data"].isNull()){
		data.SetNull();
	}
		if (jsonObject.contains("data") && jsonObject["data"].isArray()){
		const QJsonArray dataJsonArray = jsonObject["data"].toArray();
		const qsizetype dataArrayCount = dataJsonArray.size();
		data = imtsdl::TElementList<V1_0::modsdl::CPrinterBase>();
		for (qsizetype dataIndex = 0; dataIndex < dataArrayCount; ++dataIndex){
			if (dataJsonArray[dataIndex].isNull()){
				data->AppendNull();
				continue;
			}
			V1_0::modsdl::CPrinterBase tempData;
			if (!tempData.OptReadFromJsonObject(dataJsonArray[dataIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "data").toLocal8Bit().constData();)

				return false;
			}
			data->append(tempData);
		}
	}

	return true;
}






QByteArray CGetPrintersGqlRequest::GetCommandId()
{
	return QByteArrayLiteral("GetPrinters");
}


bool CGetPrintersGqlRequest::SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetPrintersRequestArguments& requestArguments, const GetPrintersRequestInfo& /*requestInfo*/)
{
	gqlRequest.SetCommandId(GetCommandId());

	// writting input arguments

	return true;
}


CGetPrintersGqlRequest::CGetPrintersGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead)
	: m_isValid(true)
{
	Q_UNUSED(optRead)
	m_isValid = true;
	m_gqlContextPtr = gqlRequest.GetRequestContext();

	// reading input arguments

	// reading requested fields
	const imtgql::CGqlFieldObject* requestedFieldsObjectPtr = nullptr;
	if (!gqlRequest.GetFields().GetFieldIds().isEmpty()){
		requestedFieldsObjectPtr = &gqlRequest.GetFields();
		if (requestedFieldsObjectPtr != nullptr){
			const QByteArrayList requestedIds = requestedFieldsObjectPtr->GetFieldIds();
			if (!requestedIds.isEmpty()){
				m_requestInfo.isDataRequested = requestedIds.contains("data");
				const imtgql::CGqlFieldObject* dataRequestedFieldsPtr = requestedFieldsObjectPtr->GetFieldArgumentObjectPtr("data");
				if (dataRequestedFieldsPtr != nullptr){
					const QByteArrayList dataRequestedIds = dataRequestedFieldsPtr->GetFieldIds();
					if (!dataRequestedIds.isEmpty()){
						m_requestInfo.data.isNameRequested = dataRequestedIds.contains("name");
						m_requestInfo.data.isSpecificationRequested = true;
						m_requestInfo.data.isSimpleTestRequested = true;
						m_requestInfo.data.isMixedTestRequested = true;
					}
				}
			}
		}
	}
}


bool CGetPrintersGqlRequest::IsValid() const
{
	return m_isValid;
}


const ::imtgql::IGqlContext* CGetPrintersGqlRequest::GetRequestContext() const
{
	return m_gqlContextPtr;
}


const GetPrintersRequestArguments& CGetPrintersGqlRequest::GetRequestedArguments() const
{
	return m_requestedArguments;
}


GetPrintersRequestInfo CGetPrintersGqlRequest::GetRequestInfo() const
{
	return m_requestInfo;
}
bool CPrinterBaseGqlHandlerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	if (commandId == CGetPrintersGqlRequest::GetCommandId()){
		return true;
	}

	return BaseClass::IsRequestSupported(gqlRequest);
}


QJsonObject CPrinterBaseGqlHandlerCompBase::CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	QJsonObject modelObj;
	QJsonObject dataModelObj;

	// GetPrinters
	if (commandId == CGetPrintersGqlRequest::GetCommandId()){
		CGetPrintersGqlRequest getPrintersGqlRequest(gqlRequest, false);
		if (!getPrintersGqlRequest.IsValid()){
			errorMessage = QStringLiteral("Bad request. Unexpected request for command-ID: '%1'").arg(commandId);
			SendErrorMessage(0, errorMessage);

			return QJsonObject();
		}

		V1_0::modsdl::CPrinterList replyPayload = OnGetPrinters(getPrintersGqlRequest, gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QStringLiteral("The derived call [OnGetPrinters] returned an error: %1").arg(errorMessage));

			return QJsonObject();
		}

		const bool isModelCreated = replyPayload.WriteToJsonObject(dataModelObj);
		if (!isModelCreated){
			errorMessage = QStringLiteral("Internal error. Unable to create response for command-ID: '%1'").arg(commandId);
			SendCriticalMessage(0, errorMessage);

			return QJsonObject();
		}

		modelObj.insert(QStringLiteral("data"), dataModelObj); return modelObj;
	}

	errorMessage = QStringLiteral("Bad request. Unexpected command-ID: '%1'").arg(commandId);
	SendErrorMessage(0, errorMessage);

	return QJsonObject();
}


} // namespace sdl::V1_0::modsdl

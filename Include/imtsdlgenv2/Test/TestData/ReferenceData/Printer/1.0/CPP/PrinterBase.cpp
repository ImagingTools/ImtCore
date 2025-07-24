#include "PrinterBase.h"


#include "PrinterBase.h"


#include "PrinterBase.h"


/// \file CPrinterSpecificationBase.cpp

namespace sdl::modsdl::PrinterBase
{


QByteArray CPrinterSpecificationBase::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPrinterSpecificationBase::V1_0::operator==(const V1_0& other) const
{
	return 
				name == other.name;
}


bool CPrinterSpecificationBase::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (name){
		model.SetData("name", *name, modelIndex);
	}


	return true;
}


bool CPrinterSpecificationBase::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	return true;
}


bool CPrinterSpecificationBase::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	return true;
}


bool CPrinterSpecificationBase::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (name){
		gqlObject.InsertParam("name", QVariant(*name));
	}

	return true;
}


bool CPrinterSpecificationBase::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	return true;
}


bool CPrinterSpecificationBase::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	return true;
}


bool CPrinterSpecificationBase::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (name){
		jsonObject["name"] = QJsonValue::fromVariant(*name);
	}

	return true;
}


bool CPrinterSpecificationBase::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	return true;
}


bool CPrinterSpecificationBase::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	return true;
}


// serialize methods

bool CPrinterSpecificationBase::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterSpecificationBase::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterSpecificationBase::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterSpecificationBase::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterSpecificationBase::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterSpecificationBase::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterSpecificationBase::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterSpecificationBase::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterSpecificationBase::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


} // namespace sdl::modsdl::PrinterBase




/// \file CLink.cpp

namespace sdl::modsdl::PrinterBase
{


QByteArray CLink::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CLink::V1_0::operator==(const V1_0& other) const
{
	return 
				link == other.link;
}


bool CLink::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!link){
		return false;
	}
	model.SetData("link", *link, modelIndex);


	return true;
}


bool CLink::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant linkData = model.GetData("link", modelIndex);
	if (linkData.isNull()){
		return false;
	}
	link = linkData.toByteArray();

	return true;
}


bool CLink::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant linkData = model.GetData("link", modelIndex);
	if (!linkData.isNull()){
		link = linkData.toByteArray();
	}

	return true;
}


bool CLink::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!link){
		return false;
	}
	gqlObject.InsertParam("link", QVariant(*link));

	return true;
}


bool CLink::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("link") || (gqlObject["link"].userType() != QMetaType::QString && gqlObject["link"].userType() != QMetaType::QByteArray)){
		return false;
	}
	link = gqlObject["link"].toByteArray();

	return true;
}


bool CLink::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("link") && (gqlObject["link"].userType() == QMetaType::QString || gqlObject["link"].userType() == QMetaType::QByteArray)){
		link = gqlObject["link"].toByteArray();
	}

	return true;
}


bool CLink::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!link){
		return false;
	}
	jsonObject["link"] = QJsonValue::fromVariant(*link);

	return true;
}


bool CLink::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("link") || ! jsonObject["link"].isString()){
		return false;
	}
	link = jsonObject["link"].toString().toUtf8();

	return true;
}


bool CLink::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("link") && jsonObject["link"].isString()){
		link = jsonObject["link"].toString().toUtf8();
	}

	return true;
}


// serialize methods

bool CLink::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLink::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLink::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLink::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLink::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLink::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLink::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLink::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLink::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


} // namespace sdl::modsdl::PrinterBase




/// \file CPrinterBase.cpp

namespace sdl::modsdl::PrinterBase
{


QByteArray CPrinterBase::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPrinterBase::V1_0::operator==(const V1_0& other) const
{
	return 
				name == other.name &&
				specification == other.specification;
}


bool CPrinterBase::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (name){
		model.SetData("name", *name, modelIndex);
	}

	if (!specification){
		return false;
	}
	QVariant specificationVariantValue;
		if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>((*specification).get())){
			if (!val->WriteToModel(*(model.AddTreeModel("specification", modelIndex)), 0)){
				return false;
			}
		}
		else if (const CLink* val = std::get_if<CLink>((*specification).get())){
			if (!val->WriteToModel(*(model.AddTreeModel("specification", modelIndex)), 0)){
				return false;
			}
		}



	return true;
}


bool CPrinterBase::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant specificationData = model.GetData("specification", modelIndex);
	if (specificationData.isNull()){
		return false;
	}
		if (specificationData.canConvert<CPrinterSpecificationBase>()){
		CPrinterSpecificationBase specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
		if (!isspecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}
	else if (specificationData.canConvert<CLink>()){
		CLink specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
		if (!isspecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}

	return true;
}


bool CPrinterBase::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant specificationData = model.GetData("specification", modelIndex);
	if (!specificationData.isNull()){
		if (specificationData.canConvert<CPrinterSpecificationBase>()){
			CPrinterSpecificationBase specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
			if (!isspecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
		}
		else if (specificationData.canConvert<CLink>()){
			CLink specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromModel(*model.GetTreeItemModel("specification", modelIndex)); 
			if (!isspecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
		}
	}

	return true;
}


bool CPrinterBase::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (name){
		gqlObject.InsertParam("name", QVariant(*name));
	}

	if (!specification){
		return false;
	}
	::imtgql::CGqlParamObject specificationDataObject;
	if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>((*specification).get())){
		if (!val->WriteToGraphQlObject(specificationDataObject)){
			return false;
		}
	}
	else if (const CLink* val = std::get_if<CLink>((*specification).get())){
		if (!val->WriteToGraphQlObject(specificationDataObject)){
			return false;
		}
	}
	gqlObject.InsertParam("specification", specificationDataObject);

	return true;
}


bool CPrinterBase::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (!gqlObject.ContainsParam("specification") || (gqlObject["specification"].isNull())){
		return false;
	}
	const QVariant specificationVariantValue = gqlObject["specification"];
	if (specificationVariantValue.canConvert<CPrinterSpecificationBase>()){
		CPrinterSpecificationBase specificationConvert;
		const ::imtgql::CGqlParamObject* specificationDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
		const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
		if (!isSpecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}
	else if (specificationVariantValue.canConvert<CLink>()){
		CLink specificationConvert;
		const ::imtgql::CGqlParamObject* specificationDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
		const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
		if (!isSpecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}

	return true;
}


bool CPrinterBase::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (gqlObject.ContainsParam("specification") && (!gqlObject["specification"].isNull())){
		const QVariant specificationVariantValue = gqlObject["specification"];
		if (specificationVariantValue.canConvert<CPrinterSpecificationBase>()){
			CPrinterSpecificationBase specificationConvert;
			const ::imtgql::CGqlParamObject* specificationDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
			const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
			if (!isSpecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
		}
		else if (specificationVariantValue.canConvert<CLink>()){
			CLink specificationConvert;
			const ::imtgql::CGqlParamObject* specificationDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("specification");
			const bool isSpecificationRead = specificationConvert.ReadFromGraphQlObject(*specificationDataObjectPtr);
			if (!isSpecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
		}
	}

	return true;
}


bool CPrinterBase::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (name){
		jsonObject["name"] = QJsonValue::fromVariant(*name);
	}

	if (!specification){
		return false;
	}
	if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>((*specification).get())){
		QJsonObject specificationJsonObject;
		const bool isspecificationAdded = val->WriteToJsonObject(specificationJsonObject);
		if (!isspecificationAdded){
			return false;
		}
		jsonObject["specification"] = specificationJsonObject;
	}
	else if (const CLink* val = std::get_if<CLink>((*specification).get())){
		QJsonObject specificationJsonObject;
		const bool isspecificationAdded = val->WriteToJsonObject(specificationJsonObject);
		if (!isspecificationAdded){
			return false;
		}
		jsonObject["specification"] = specificationJsonObject;
	}


	return true;
}


bool CPrinterBase::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (!jsonObject.contains("specification") || ! jsonObject["specification"].isObject()){
		return false;
	}
	const QVariant specificationVariantValue = jsonObject["specification"].toVariant();
	if (specificationVariantValue.canConvert<CPrinterSpecificationBase>()){
		CPrinterSpecificationBase specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
		if (!isspecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}
	else if (specificationVariantValue.canConvert<CLink>()){
		CLink specificationConvert;
		const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
		if (!isspecificationRead){
			return false;
		}
		specification = std::make_shared<PrinterSpecification>(specificationConvert);
	}

	return true;
}


bool CPrinterBase::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (jsonObject.contains("specification") && jsonObject["specification"].isObject()){
		const QVariant specificationVariantValue = jsonObject["specification"].toVariant();
		if (specificationVariantValue.canConvert<CPrinterSpecificationBase>()){
			CPrinterSpecificationBase specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
			if (!isspecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
		}
		else if (specificationVariantValue.canConvert<CLink>()){
			CLink specificationConvert;
			const bool isspecificationRead = specificationConvert.ReadFromJsonObject(jsonObject["specification"].toObject());
			if (!isspecificationRead){
				return false;
			}
			specification = std::make_shared<PrinterSpecification>(specificationConvert);
		}
	}

	return true;
}


// serialize methods

bool CPrinterBase::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterBase::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterBase::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterBase::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterBase::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterBase::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterBase::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterBase::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPrinterBase::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


} // namespace sdl::modsdl::PrinterBase





#include <imtgeo/CAddressElementDatabaseDelegateComp.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/TParamsPtr.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtgeo
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CAddressElementDatabaseDelegateComp::GetObjectTypeId(const QByteArray& /*objectId*/) const
{
	return "Address";
}


istd::IChangeable* CAddressElementDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_adrElementInfoFactCompPtr.IsValid()) {
		return nullptr;
	}

	istd::TDelPtr<imtgeo::CAddressElementInfo> adrElementInfoPtr = dynamic_cast<imtgeo::CAddressElementInfo*>(m_adrElementInfoFactCompPtr.CreateInstance());
	if (!adrElementInfoPtr.IsValid()) {
		return nullptr;
	}

	if (record.contains("Id")) {
		QByteArray id = record.value("Id").toByteArray();
		adrElementInfoPtr->SetId(id);
	}

	if (record.contains("ParentId")) {
		QByteArray parentId = record.value("ParentId").toByteArray();
		adrElementInfoPtr->SetParentId(parentId);
	}

	if (record.contains("Type")) {
		int type = record.value("Type").toInt();
		adrElementInfoPtr->SetType((imtgeo::IAddressElementInfo::AddressElementType)type);
	}

	if (record.contains("Name")) {
		QString name = record.value("Name").toByteArray();
		adrElementInfoPtr->SetName(name);
	}

	if (record.contains("Description")) {
		QString description = record.value("Description").toByteArray();
		adrElementInfoPtr->SetDescription(description);
	}

	if (record.contains("SerialId")) {
		QByteArray serialId = record.value("SerialId").toByteArray();
		adrElementInfoPtr->SetSerialId(serialId);
	}

	if (record.contains("FullAddress")) {
		QString address = record.value("FullAddress").toString();
		adrElementInfoPtr->SetAddress(address);
	}

	if (record.contains("Latitude")) {
		double lat = record.value("Latitude").toDouble();
		adrElementInfoPtr->SetLatitude(lat);
	}

	if (record.contains("Longitude")) {
		double lon = record.value("Longitude").toDouble();
		adrElementInfoPtr->SetLongitude(lon);
	}

	return adrElementInfoPtr.PopPtr();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CAddressElementDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& proposedObjectId,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr) const
{
	const imtgeo::IAddressElementInfo* adrInfoPtr = dynamic_cast<const imtgeo::IAddressElementInfo*>(valuePtr);
	if (adrInfoPtr == nullptr) {
		return NewObjectQuery();
	}

	QString name = adrInfoPtr->GetName();
	QString description = adrInfoPtr->GetDescription();
	QByteArray parentId = adrInfoPtr->GetParentId();
	int type = adrInfoPtr->GetType();
	QString address = adrInfoPtr->GetFullAddress();
	double lat = adrInfoPtr->GetLatitude();
	double lon = adrInfoPtr->GetLongitude();

	NewObjectQuery retVal;
	retVal.query = QString("INSERT INTO \"AddressElements\"(\"Id\", \"ParentId\", \"Type\", \"Name\", \"FullAddress\", \"Description\", \"Latitude\", \"Longitude\") SELECT '%1', '%2', '%3', '%4', '%5', '%6', '%7', '8' WHERE NOT EXISTS (SELECT * FROM \"AddressElements\" WHERE \"ParentId\" = '%2' AND \"Name\" = '%4');")
				.arg(qPrintable(proposedObjectId))
				.arg(qPrintable(parentId))
				.arg(type)
				.arg(name)
				.arg(address)
				.arg(description)
				.arg(lat)
				.arg(lon)
				.toLocal8Bit();
	retVal.objectName = name;

	return retVal;
}


QByteArray CAddressElementDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId) const
{
	QByteArray retVal = QString("DELETE FROM \"AddressElements\" WHERE \"Id\" = '%1';").arg(qPrintable(objectId)).toLocal8Bit();

	return retVal;
}


QByteArray CAddressElementDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const ContextDescription& /*description*/,
			bool /*useExternDelegate*/) const
{
	const imtgeo::IAddressElementInfo* adrInfoPtr = dynamic_cast<const imtgeo::IAddressElementInfo*>(&object);
	if (adrInfoPtr == nullptr || objectId.isEmpty()) {
		return QByteArray();
	}
	QByteArray adrId = adrInfoPtr->GetId();
	QByteArray parentId = adrInfoPtr->GetParentId();
	int type = adrInfoPtr->GetType();
	QString adrName = adrInfoPtr->GetName();
	QString description = adrInfoPtr->GetDescription();
	QString address = adrInfoPtr->GetFullAddress();
	double lat = adrInfoPtr->GetLatitude();
	double lon = adrInfoPtr->GetLongitude();

	if (adrId.isEmpty() && !objectId.isEmpty()) {
		adrId = objectId;
	}
	if (adrId.isEmpty()) {
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE \"AddressElements\" SET \"Id\" = '%1', \"ParentId\" = '%2', \"Type\" = '%3', \"Name\" = '%4', \"FullAddress\" = '%5', \"Description\" = '%6', \"Langitude\" = '%7', \"Latitude\" = '%8' WHERE \"Id\" ='%9';")
				.arg(qPrintable(adrId))
				.arg(qPrintable(parentId))
				.arg(type)
				.arg(adrName)
				.arg(address)
				.arg(description)
				.arg(lat)
				.arg(lon)
				.arg(qPrintable(objectId))
				.toLocal8Bit();

	return retVal;
}


QByteArray CAddressElementDatabaseDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/) const
{
	return QByteArray();
}


QByteArray CAddressElementDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/) const
{
	return QByteArray();
}


QByteArray CAddressElementDatabaseDelegateComp::GetSelectionQuery(const QByteArray &objectId, int offset, int count, const iprm::IParamsSet *paramsPtr) const
{
	if (!objectId.isEmpty()) {
		return QString("SELECT * FROM \"%1\" WHERE \"%2\" = '%3'")
			.arg(qPrintable(*m_tableNameAttrPtr))
			.arg(qPrintable(*m_objectIdColumnAttrPtr))
			.arg(qPrintable(objectId))
			.toLocal8Bit();
	}
	else {
		QString sortQuery;
		QString filterQuery;
		QString textFilter;
		if (count == 0) {
			return QByteArray();
		}
		if (paramsPtr != nullptr) {
			if (!CreateFilterQuery(*paramsPtr, filterQuery)) {
				//	return QByteArray();
			}

			iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(paramsPtr, "Filter");
			if (collectionFilterParamPtr.IsValid()) {
				textFilter = collectionFilterParamPtr->GetTextFilter();
				if (!CreateSortQuery(*collectionFilterParamPtr, sortQuery)) {
					return QByteArray();
				}
			}
		}

		QByteArray paginationQuery;
		if (!CreatePaginationQuery(offset, count, paginationQuery)) {
			return QByteArray();
		}
		QString baseSelelectionQuery = QString("SELECT * FROM \"%1\"").arg(qPrintable(m_tableNameAttrPtr->GetValue()));

		// Due to a bug in qt in the context of resolving of an expression like this: '%<SOME_NUMBER>%'
		QString retVal = "(" + baseSelelectionQuery;
		retVal += QString(" ") + filterQuery;
		retVal += QString(" ") + qPrintable(paginationQuery) + ")";
		retVal += QString(" ") + sortQuery;

		return retVal.toLocal8Bit();
	}

	return QByteArray();
}


bool CAddressElementDatabaseDelegateComp::CreateFilterQuery(const iprm::IParamsSet &filterParams, QString &filterQuery) const
{
	QString objectFilterQuery;
	bool retVal = CreateObjectFilterQuery(filterParams, objectFilterQuery);

	QString textFilterQuery;
	QString parentIdFilterQuery;
	QString typeIdFilterQuery;
	iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(&filterParams, "Filter");
	iprm::TParamsPtr<imtbase::ICollectionFilter> parentIdFilterParamPtr(&filterParams, "ParentId");
	iprm::TParamsPtr<imtbase::ICollectionFilter> typeIdFilterParamPtr(&filterParams, "Type");
	if (collectionFilterParamPtr.IsValid()) {
		retVal = CreateTextFilterQuery(*collectionFilterParamPtr, textFilterQuery);
	}
	if (parentIdFilterParamPtr.IsValid() && parentIdFilterParamPtr->GetTextFilter() != "") {
		parentIdFilterQuery = "\"ParentId\" = '" + parentIdFilterParamPtr->GetTextFilter() + "'";
	}
	if (typeIdFilterParamPtr.IsValid() && typeIdFilterParamPtr->GetTextFilter() != "") {
		typeIdFilterQuery = "\"Type\" = " + typeIdFilterParamPtr->GetTextFilter();
	}
	if (!objectFilterQuery.isEmpty() || !textFilterQuery.isEmpty() || !parentIdFilterQuery.isEmpty() || !typeIdFilterQuery.isEmpty()) {
		filterQuery = " WHERE ";
	}

	filterQuery += objectFilterQuery;
	if (!objectFilterQuery.isEmpty() && !textFilterQuery.isEmpty()) {
		filterQuery += " AND ";
	}

	if (!textFilterQuery.isEmpty()) {
		filterQuery += "(" + textFilterQuery + ")";
	}

	//levelFilter
	if (!textFilterQuery.isEmpty() && parentIdFilterQuery.isEmpty() && typeIdFilterQuery.isEmpty()) {

		QString text = collectionFilterParamPtr->GetTextFilter();
		QString columnId = collectionFilterParamPtr->GetFilteringInfoIds().first();
		int count = text.count(",");
		QString positive = "%";
		QString negative = "";
		for (int i = 0; i < count; i++) {
			positive += ",%";
			negative = positive + ",%";
		}
		QString levelQuery = QString(" AND (\"%1\" LIKE '%2') AND (\"%1\" NOT LIKE '%3')").arg(columnId).arg(positive).arg(negative);

		filterQuery += levelQuery;
	}


	if ((!objectFilterQuery.isEmpty() || !textFilterQuery.isEmpty()) && !parentIdFilterQuery.isEmpty()) {
		filterQuery += " AND ";
	}

	if (!parentIdFilterQuery.isEmpty()) {
		filterQuery += "(" + parentIdFilterQuery + ")";

	}

	if ((!objectFilterQuery.isEmpty() || !textFilterQuery.isEmpty() || !parentIdFilterQuery.isEmpty()) && !typeIdFilterQuery.isEmpty()) {
		filterQuery += " AND ";
	}

	if (!typeIdFilterQuery.isEmpty()) {
		filterQuery += "(" + typeIdFilterQuery + ")";

	}

	return true;
}


} // namespace imtgeo



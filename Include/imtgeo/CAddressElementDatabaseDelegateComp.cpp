// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgeo/CAddressElementDatabaseDelegateComp.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtdb/imtdb.h>
#include <imtcol/IDocumentCollectionFilter.h>


namespace imtgeo
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CAddressElementDatabaseDelegateComp::GetObjectTypeId(const QByteArray& /*objectId*/) const
{
	return QByteArrayLiteral("Address");
}


QByteArray CAddressElementDatabaseDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		const QString sql = GetBaseSelectionQuery() + QStringLiteral(R"sql(WHERE ae."State" = 'Active' AND ae."%1" = '%2')sql").arg(*m_objectIdColumnAttrPtr, objectId);

		return sql.toUtf8();
	}

	return BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);
}


istd::IChangeableUniquePtr CAddressElementDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record, const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid() || !m_adressFactoryCompPtr.IsValid()){
		Q_ASSERT_X(false, __func__, "Invalid component configuration");

		return nullptr;
	}

	IAddressElementInfoUniquePtr adrElementInfoPtr = m_adressFactoryCompPtr.CreateInstance();
	if (!adrElementInfoPtr.IsValid()){
		Q_ASSERT_X(false, __func__, "Failed to create address element info instance using factory");

		return nullptr;
	}

	if (record.contains(QStringLiteral("Id"))){
		if (auto adrElementIdentifiableInfoPtr = adrElementInfoPtr.GetPtr<CPositionIdentifiable>()){
			const QByteArray id = imtdb::VariantToByteArray(record.value(QStringLiteral("Id")));
			adrElementIdentifiableInfoPtr->SetObjectUuid(id);
		}
	}

	if (record.contains(QStringLiteral("ParentIds"))){
		const QByteArray parentIdBa = record.value(QStringLiteral("ParentIds")).toByteArray();
		const QJsonArray parentIdJsonArray = QJsonDocument::fromJson(parentIdBa).array();

		QByteArrayList parentIds;
		for (const QJsonValue& parentIdJsonVal : parentIdJsonArray){
			QByteArray parentId = parentIdJsonVal.toVariant().toByteArray();
			parentIds.append(parentId);
		}
		adrElementInfoPtr->SetParentIds(parentIds);
	}

	if (record.contains(QStringLiteral("Type"))){
		QByteArray typeId = record.value(QStringLiteral("Type")).toByteArray();
		adrElementInfoPtr->SetAddressTypeId(typeId);
	}

	if (record.contains(QStringLiteral("Name"))){
		QString name = record.value(QStringLiteral("Name")).toByteArray();
		adrElementInfoPtr->SetName(name);
	}

	if (record.contains(QStringLiteral("Description"))){
		QString description = record.value(QStringLiteral("Description")).toByteArray();
		adrElementInfoPtr->SetDescription(description);
	}

	if (record.contains(QStringLiteral("FullAddress"))){
		QString address = record.value(QStringLiteral("FullAddress")).toString();
		adrElementInfoPtr->SetFullAddress(address);
	}

	if (record.contains(QStringLiteral("Latitude"))){
		double lat = record.value(QStringLiteral("Latitude")).toDouble();
		adrElementInfoPtr->SetLatitude(lat);
	}

	if (record.contains(QStringLiteral("Longitude"))){
		double lon = record.value(QStringLiteral("Longitude")).toDouble();
		adrElementInfoPtr->SetLongitude(lon);
	}

	return adrElementInfoPtr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CAddressElementDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& proposedObjectId,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (!m_tableNameAttrPtr.IsValid()){
		Q_ASSERT_X(false, __func__, "Invalid component configuration: Table name is not set");

		return {};
	}

	auto adrInfoPtr = dynamic_cast<const IAddressElementInfo*>(valuePtr);
	if (adrInfoPtr == nullptr){
		return NewObjectQuery();
	}

	const QString name = adrInfoPtr->GetName();
	const QString description = adrInfoPtr->GetDescription();
	const QByteArrayList parentIdList = adrInfoPtr->GetParentIds();

	QJsonArray parenIdArray;
	for (const QByteArray& parentId : parentIdList){
		parenIdArray.append(QString(parentId));
	}

	const QByteArray parentIds = QJsonDocument(parenIdArray).toJson(QJsonDocument::Compact);

	const QByteArray typeId = adrInfoPtr->GetAddressTypeId();
	const double lat		= adrInfoPtr->GetLatitude();
	const double lon		= adrInfoPtr->GetLongitude();

	NewObjectQuery retVal;
	retVal.query = QStringLiteral(R"(
						INSERT INTO "%9" ("Id", "ParentIds", "Type", "Name", "Description", "Latitude", "Longitude", "LastModified")
						VALUES ('%1', '%2', '%3', %4, %5, %6, %7, '%8');)")
		.arg(
				/*1*/ proposedObjectId,
				/*2*/ parentIds,
				/*3*/ typeId,
				/*4*/ imtdb::SqlValue(name),
				/*5*/ imtdb::SqlValue(description),
				/*6*/ imtdb::SqlValue(lat),
				/*7*/ imtdb::SqlValue(lon),
				/*8*/ imtdb::UtcNow(),
				/*9*/ *m_tableNameAttrPtr
			)
		.toUtf8();

	retVal.objectName = name;

	return retVal;
}


QByteArray CAddressElementDatabaseDelegateComp::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Ids& objectIds,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (!m_tableNameAttrPtr.IsValid()){
		Q_ASSERT_X(false, __func__, "Invalid component configuration: Table name is not set");

		return QByteArray();
	}

	if (objectIds.isEmpty()){
		return QByteArray();
	}

	const QString quotedIds = objectIds.join(u8"','").prepend('\'').append('\'');

	QString query = QStringLiteral(R"(
								UPDATE "%1"
								SET "State" = 'Disabled'
									, "LastModified" = '%2'
								WHERE "State" = 'Active' AND "Id" IN (%3);)")
						.arg(*m_tableNameAttrPtr, imtdb::UtcNow(), quotedIds);

	return query.toUtf8();
}


QByteArray CAddressElementDatabaseDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	Q_ASSERT_X(false, __func__, "Delete object set operation is not supported for AddressElements collection");

	return QByteArray();
}


QByteArray CAddressElementDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	if (!m_tableNameAttrPtr.IsValid()){
		Q_ASSERT_X(false, __func__, "Invalid component configuration: Table name is not set");

		return QByteArray();
	}

	auto adrInfoPtr = dynamic_cast<const IAddressElementInfo*>(&object);
	if (adrInfoPtr == nullptr || objectId.isEmpty()){
		return QByteArray();
	}

	auto positionInfoPtr = dynamic_cast<const CPositionIdentifiable*>(&object);
	QByteArray adrId = (positionInfoPtr != nullptr) ? positionInfoPtr->GetObjectUuid() : QByteArray();
	if (adrId.isEmpty()){
		adrId = objectId;
	}

	if (adrId.isEmpty()){
		return QByteArray();
	}

	const QString adrName				= adrInfoPtr->GetName();
	const QString description			= adrInfoPtr->GetDescription();
	const QByteArrayList parentIdList	= adrInfoPtr->GetParentIds();

	QJsonArray parentIdJsonArray;
	for (const QByteArray& parentId : parentIdList){
		parentIdJsonArray.append(QString(parentId));
	}

	const QByteArray parents = QJsonDocument(parentIdJsonArray).toJson(QJsonDocument::Compact);

	const QByteArray typeId	= adrInfoPtr->GetAddressTypeId();
	const double lat		= adrInfoPtr->GetLatitude();
	const double lon		= adrInfoPtr->GetLongitude();

	QByteArray retVal = QStringLiteral(R"(
						UPDATE "%10"
						SET "Id" = '%1'
							, "ParentIds" = '%2'
							, "Type" = '%3'
							, "Name" = %4
							, "Description" = %5
							, "Latitude" = %6
							, "Longitude" = %7
							, "LastModified" = '%8'
						WHERE "Id" = '%9'
							AND "State" = 'Active';)")
		.arg(
				/*1*/ adrId,
				/*2*/ parents,
				/*3*/ typeId,
				/*4*/ imtdb::SqlValue(adrName),
				/*5*/ imtdb::SqlValue(description),
				/*6*/ imtdb::SqlValue(lat),
				/*7*/ imtdb::SqlValue(lon),
				/*8*/ imtdb::UtcNow(),
				/*9*/ objectId,
				/*10*/ *m_tableNameAttrPtr
			)
		.toUtf8();

	return retVal;
}


// unsupported operations

QByteArray CAddressElementDatabaseDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	Q_ASSERT_X(false, __func__, "Rename operation is not supported for AddressElements collection");

	return QByteArray();
}


QByteArray CAddressElementDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	Q_ASSERT_X(false, __func__, "Description update operation is not supported for AddressElements collection");

	return QByteArray();
}


// protected methods

// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

QString CAddressElementDatabaseDelegateComp::GetBaseSelectionQuery() const
{
	if (!m_tableNameAttrPtr.IsValid()){
		Q_ASSERT_X(false, __func__, "Invalid component configuration: Table name is not set");

		return QString();
	}

	return QStringLiteral(R"sql(SELECT *, (SELECT get_full_address(ae."ParentIds", ae."Name", ae."Type")) AS "FullAddress" FROM "%1" AS ae )sql")
				.arg(*m_tableNameAttrPtr);
}


QString CAddressElementDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	QString additionalFilterQuery;

	QString documentFilterQuery;
	iprm::TParamsPtr<imtcol::IDocumentCollectionFilter> documentFilterParamPtr(&filterParams, QByteArrayLiteral("State"));
	if (!documentFilterParamPtr.IsValid()){
		documentFilterQuery = QStringLiteral(R"("State" = 'Active')");
	}
	else{
		// Document States
		QStringList stateConditions;
		imtcol::IDocumentCollectionFilter::DocumentStates states = documentFilterParamPtr->GetDocumentStates();
		if (states.contains(imtcol::IDocumentCollectionFilter::DS_ACTIVE)){
			stateConditions << QStringLiteral(R"("State" = 'Active')");
		}

		if (states.contains(imtcol::IDocumentCollectionFilter::DS_INACTIVE)){
			stateConditions << QStringLiteral(R"("State" = 'InActive')");
		}

		if (states.contains(imtcol::IDocumentCollectionFilter::DS_DISABLED)){
			stateConditions << QStringLiteral(R"("State" = 'Disabled')");
		}

		if (!stateConditions.isEmpty()){
			documentFilterQuery = '(' + stateConditions.join(u" OR ") + ')';
		}
	}

	QString parentIdFilterQuery;
	iprm::TParamsPtr<iprm::ITextParam> parentIdFilterParamPtr(&filterParams, QByteArrayLiteral("ParentId"), false);
	if (parentIdFilterParamPtr.IsValid()){
		const QString parentId = imtdb::SqlEncode(parentIdFilterParamPtr->GetText());
		if(parentId.isEmpty()){
			parentIdFilterQuery = QStringLiteral(R"(AND jsonb_array_length("ParentIds") = 0)");
		}
		else{
			parentIdFilterQuery = QStringLiteral(R"(AND "ParentIds"->>(jsonb_array_length("ParentIds") - 1) = '%1')")
										.arg(parentId);
		}
	}

	additionalFilterQuery += documentFilterQuery;
	additionalFilterQuery += parentIdFilterQuery;

	return additionalFilterQuery;
}


} // namespace imtgeo



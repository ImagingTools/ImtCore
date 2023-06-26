#include <imtgeo/CAddressElementDatabaseDelegateComp.h>

// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/TParamsPtr.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtgeo
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CAddressElementDatabaseDelegateComp::GetObjectTypeId(const QByteArray& objectId) const
{
	return "Address";
}

istd::IChangeable* CAddressElementDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

    if (!m_adrElementInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<IAddressElementInfo> adrElementInfoPtr = m_adrElementInfoFactCompPtr.CreateInstance();
    if (!adrElementInfoPtr.IsValid()){
        return nullptr;
    }

    if(record.contains("Id")){
        QByteArray id = record.value("Id").toByteArray();
        adrElementInfoPtr->SetId(id);
    }

    if(record.contains("ParentIds")){
        QByteArray dataIndexes = record.value("ParentIds").toByteArray();
		QJsonDocument documentIndexes = QJsonDocument::fromJson(dataIndexes);
		QJsonArray jsonIndexes = documentIndexes.array();
        QList<QByteArray> parentIds;
		for(int index = 0; index < jsonIndexes.size(); index++){
			QByteArray parentId = jsonIndexes[index].toVariant().toByteArray();
            parentIds.append(parentId);
		}
        adrElementInfoPtr->SetParentIds(parentIds);
	}

	if(record.contains("AddressTypeId")){
		QByteArray typeId = record.value("AddressTypeId").toByteArray();
		adrElementInfoPtr->SetAddressTypeId(typeId);
	}

    if(record.contains("Name")){
        QString name = record.value("Name").toByteArray();
        adrElementInfoPtr->SetName(name);
    }

    if(record.contains("Description")){
        QString description = record.value("Description").toByteArray();
        adrElementInfoPtr->SetDescription(description);
    }

	if(record.contains("FullAddress")){
		QString address = record.value("FullAddress").toString();
		adrElementInfoPtr->SetAddress(address);
	}

	if(record.contains("Latitude")){
		double lat = record.value("Latitude").toDouble();
		adrElementInfoPtr->SetLatitude(lat);
	}

	if(record.contains("Longitude")){
		double lon = record.value("Longitude").toDouble();
		adrElementInfoPtr->SetLongitude(lon);
	}

    return adrElementInfoPtr.PopPtr();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CAddressElementDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& proposedObjectId,
			const QString& objectName,
			const QString& /*objectDescription*/,
            const istd::IChangeable* valuePtr,
            const imtbase::IOperationContext* operationContextPtr) const
{
	const IAddressElementInfo* adrInfoPtr = dynamic_cast<const IAddressElementInfo*>(valuePtr);
    if (adrInfoPtr == nullptr){
        return NewObjectQuery();
    }

    QString name = adrInfoPtr->GetName();
    QString description = adrInfoPtr->GetDescription();

    QList<QByteArray> parentIds = adrInfoPtr->GetParentIds();
	QJsonArray json;
    for (QByteArray parentId: parentIds){
		json.append(QJsonValue(QString(parentId)));
	}
	QJsonDocument document = QJsonDocument(json);
	QByteArray parents = document.toJson(QJsonDocument::Compact);

	QByteArray typeId = adrInfoPtr->GetAddressTypeId();
	QString address = adrInfoPtr->GetAddress();
	double lat = adrInfoPtr->GetLatitude();
	double lon = adrInfoPtr->GetLongitude();

    NewObjectQuery retVal;
    retVal.query = QString("INSERT INTO \"AddressElements\"(\"Id\", \"ParentIds\", \"AddressTypeId\", \"Name\", \"Description\", \"Latitude\", \"Longitude\")  VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7');")
                            .arg(qPrintable(proposedObjectId))
							.arg(qPrintable(parents))
							.arg(qPrintable(typeId))
                            .arg(name)
							.arg(description)
							.arg(lat)
							.arg(lon)
                            .toLocal8Bit();
    retVal.objectName = name;

    return retVal;
}


QByteArray CAddressElementDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& collection,
            const QByteArray& objectId,
            const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray retVal = QString("DELETE FROM \"AddressElements\" WHERE \"Id\" = '%1';").arg(qPrintable(objectId)).toLocal8Bit();

	return retVal;
}


QByteArray CAddressElementDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const istd::IChangeable& object,
            const imtbase::IOperationContext* operationContextPtr,
			bool /*useExternDelegate*/) const
{
	const IAddressElementInfo* adrInfoPtr = dynamic_cast<const IAddressElementInfo*>(&object);
    if (adrInfoPtr == nullptr || objectId.isEmpty()){
        return QByteArray();
    }
    QByteArray adrId = adrInfoPtr->GetId();
    QList<QByteArray> parentIds = adrInfoPtr->GetParentIds();
	QJsonArray json;
    for (QByteArray parentId: parentIds){
		json.append(QJsonValue(QString(parentId)));
	}
	QJsonDocument document = QJsonDocument(json);
	QByteArray parents = document.toJson(QJsonDocument::Compact);
	QByteArray typeId = adrInfoPtr->GetAddressTypeId();
    QString adrName = adrInfoPtr->GetName();
    QString description = adrInfoPtr->GetDescription();
	QString address = adrInfoPtr->GetAddress();
	double lat = adrInfoPtr->GetLatitude();
	double lon = adrInfoPtr->GetLongitude();

    if (adrId.isEmpty() && !objectId.isEmpty()){
        adrId = objectId;
    }
    if(adrId.isEmpty()){
        return QByteArray();
    }

    QByteArray retVal = QString("UPDATE \"AddressElements\" SET \"Id\" = '%1', \"ParentIds\" = '%2', \"TypeId\" = '%3', \"Name\" = '%4', \"FullAddress\" = '%5', \"Description\" = '%6', \"Langitude\" = '%7', \"Latitude\" = '%8' WHERE \"Id\" ='%9';")
                            .arg(qPrintable(adrId))
							.arg(qPrintable(parents))
							.arg(qPrintable(typeId))
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
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
            const QString& newObjectName,
            const imtbase::IOperationContext* operationContextPtr) const
{
	return QByteArray();
}


QByteArray CAddressElementDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
            const QString& description,
            const imtbase::IOperationContext* operationContextPtr) const
{
    return QByteArray();
}

QByteArray CAddressElementDatabaseDelegateComp::GetSelectionQuery(const QByteArray &objectId, int offset, int count, const iprm::IParamsSet *paramsPtr) const
{
	if (!objectId.isEmpty()){

//        return QString("SELECT * FROM \"%1\" WHERE \"%2\" = '%3'")
//                    .arg(qPrintable(*m_tableNameAttrPtr))
//                    .arg(qPrintable(*m_objectIdColumnAttrPtr))
//                    .arg(qPrintable(objectId))
//                    .toLocal8Bit();

        return QString(R"sql(SELECT "Id", "ParentId", "ParentIds", "Type", "Name", "Latitude", "Longitude",(SELECT get_full_address(ae."ParentIds", ae."Name", ae."Type")) AS "FullAddress" FROM "%1" AS ae WHERE ae."%2" = '%3')sql")
                .arg(qPrintable(*m_tableNameAttrPtr))
                .arg(qPrintable(*m_objectIdColumnAttrPtr))
                .arg(qPrintable(objectId))
                .toLocal8Bit();
    }
	else{
		QString sortQuery;
		QString filterQuery;
		QString textFilter;
		if (count == 0){
			return QByteArray();
		}
		if (paramsPtr != nullptr){
			if (!CreateFilterQuery(*paramsPtr, filterQuery)){
                return QByteArray();
			}

			iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(paramsPtr, "Filter");
			if (collectionFilterParamPtr.IsValid()){
				textFilter = collectionFilterParamPtr->GetTextFilter();                
                //qDebug() << "textFilter:::::: " << textFilter;
				if (!CreateSortQuery(*collectionFilterParamPtr, sortQuery)){
					return QByteArray();
				}
			}
		}

		QByteArray paginationQuery;
		if (!CreatePaginationQuery(offset, count, paginationQuery)){
			return QByteArray();
		}
        //QString baseSelelectionQuery = QString("SELECT * FROM \"%1\"").arg(qPrintable(m_tableNameAttrPtr->GetValue()));

        QString baseSelelectionQuery = QString(R"sql(SELECT "Id", "ParentId", "ParentIds", "Type", "Name", "Latitude", "Longitude",(SELECT get_full_address(ae."ParentIds", ae."Name", ae."Type")) AS "FullAddress" FROM "%1" AS ae )sql")
                        .arg(qPrintable(*m_tableNameAttrPtr));

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
    bool retVal = true;
    //bool retVal = CreateObjectFilterQuery(filterParams, objectFilterQuery);
    if (!retVal){
        return false;
    }

    QString textFilterQuery;
    //QString parentIdFilterQuery;
    QString parentIdsFilterQuery;
    QString typeIdFilterQuery;
    iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(&filterParams, "Filter");
    //iprm::TParamsPtr<imtbase::ICollectionFilter> parentIdFilterParamPtr(&filterParams, "ParentId");
    iprm::TParamsPtr<imtbase::ICollectionFilter> parentIdsFilterParamPtr(&filterParams, "ParentIds");
    iprm::TParamsPtr<imtbase::ICollectionFilter> typeIdFilterParamPtr(&filterParams, "TypeId");
    if (collectionFilterParamPtr.IsValid()){
        retVal = CreateTextFilterQuery(*collectionFilterParamPtr, textFilterQuery);
    }

    if (parentIdsFilterParamPtr.IsValid() && parentIdsFilterParamPtr->GetTextFilter() != ""){
        parentIdsFilterQuery = "'" + parentIdsFilterParamPtr->GetTextFilter() + "' <@ (\"ParentIds\")" ;
//        parentIdsFilterQuery = "\"ParentIds\" = ""'" + parentIdsFilterParamPtr->GetTextFilter() + "'";
    }

    if (typeIdFilterParamPtr.IsValid() && typeIdFilterParamPtr->GetTextFilter() != ""){
        typeIdFilterQuery = "\"AddressTypeId\" = " + typeIdFilterParamPtr->GetTextFilter();
    }

    if (!objectFilterQuery.isEmpty() || !textFilterQuery.isEmpty() || !parentIdsFilterQuery.isEmpty() /*|| !typeIdFilterQuery.isEmpty()*/){
        filterQuery = " WHERE ";
    }

    filterQuery += objectFilterQuery;
    if (!objectFilterQuery.isEmpty() && !textFilterQuery.isEmpty()){
        filterQuery += " AND ";
    }

    if (!textFilterQuery.isEmpty()){
        filterQuery += "(" + textFilterQuery + ")";
        //qDebug() << "textFilterQuery::: " << textFilterQuery;
    }


    if ((!objectFilterQuery.isEmpty() || !textFilterQuery.isEmpty()) && !parentIdsFilterQuery.isEmpty()){
        filterQuery += " AND ";
    }


    if (!parentIdsFilterQuery.isEmpty()){
        filterQuery += "(" + parentIdsFilterQuery + ")";

    }


    if ((!objectFilterQuery.isEmpty() || !textFilterQuery.isEmpty() || !parentIdsFilterQuery.isEmpty()) && !typeIdFilterQuery.isEmpty()){
        filterQuery += " AND ";
    }

    if (!typeIdFilterQuery.isEmpty()){
        filterQuery += "(" + typeIdFilterQuery + ")";

    }

    //qDebug() << "filterQuery::: " << filterQuery;
    return true;
}

} // namespace imtgeo



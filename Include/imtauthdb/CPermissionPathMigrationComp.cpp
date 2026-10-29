// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CPermissionPathMigrationComp.h>


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QSet>

// ImtCore includes
#include <imtauth/IRole.h>
#include <imtauth/IUserBaseInfo.h>
#include <imtbase/COperationDescription.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtlic/IFeatureInfo.h>


namespace imtauthdb
{


namespace
{


void CollectPermissionPaths(
			const imtlic::IFeatureInfo& featureInfo,
			QHash<QByteArray, QByteArray>& pathMap,
			QSet<QByteArray>& ambiguousIds)
{
	if (featureInfo.IsPermission()){
		const QByteArray featureId = featureInfo.GetFeatureId();
		const QByteArray path = imtlic::CalculateFeaturePath(featureInfo);
		if (pathMap.contains(featureId) && (pathMap.value(featureId) != path)){
			ambiguousIds.insert(featureId);
		}
		else{
			pathMap.insert(featureId, path);
		}
	}

	for (const imtlic::IFeatureInfo::FeatureInfoPtr& subFeaturePtr : featureInfo.GetSubFeatures()){
		if (subFeaturePtr.IsValid()){
			CollectPermissionPaths(*subFeaturePtr, pathMap, ambiguousIds);
		}
	}
}


} // namespace


// protected methods

bool CPermissionPathMigrationComp::Migrate() const
{
	if (!m_productInfoCompPtr.IsValid()){
		return false;
	}

	QByteArrayList ambiguousIds;
	const PathMap pathMap = CreatePathMap(ambiguousIds);
	if (!ambiguousIds.isEmpty()){
		SendWarningMessage(
					0,
					QStringLiteral("Permissions stored by these ids are left as they are, the ids are not unique in the feature tree: %1")
								.arg(QString::fromUtf8(ambiguousIds.join(", "))),
					"imtauthdb::CPermissionPathMigrationComp");
	}

	if (pathMap.isEmpty()){
		return true;
	}

	const QByteArray productId = m_productInfoCompPtr->GetProductId();
	bool retVal = true;

	if (m_roleCollectionCompPtr.IsValid()){
		retVal = MigrateCollection<imtauth::IRole>(*m_roleCollectionCompPtr, QStringLiteral("role"), [this, &pathMap, &productId](imtauth::IRole& role, imtbase::COperationContext& operationContext){
			if (role.GetProductId() != productId){
				return false;
			}

			QByteArrayList permissions = role.GetLocalPermissions();
			const bool permissionsChanged = ConvertIds(permissions, pathMap, "AddPermission", "RemovePermission", "Permission", QT_TRANSLATE_NOOP("Attribute", "Permission"), operationContext);
			if (permissionsChanged){
				role.SetLocalPermissions(permissions);
			}

			QByteArrayList prohibitions = role.GetProhibitions();
			const bool prohibitionsChanged = ConvertIds(prohibitions, pathMap, "AddProhibition", "RemoveProhibition", "Prohibition", QT_TRANSLATE_NOOP("Attribute", "Prohibition"), operationContext);
			if (prohibitionsChanged){
				role.SetProhibitions(prohibitions);
			}

			return permissionsChanged || prohibitionsChanged;
		});
	}

	auto convertUserBase = [this, &pathMap, &productId](imtauth::IUserBaseInfo& userBaseInfo, imtbase::COperationContext& operationContext){
		QByteArrayList permissions = userBaseInfo.GetLocalPermissions(productId);
		const bool permissionsChanged = ConvertIds(permissions, pathMap, "AddPermission", "RemovePermission", "Permission", QT_TRANSLATE_NOOP("Attribute", "Permission"), operationContext);
		if (permissionsChanged){
			userBaseInfo.SetLocalPermissions(productId, permissions);
		}

		QByteArrayList prohibitions = userBaseInfo.GetProhibitions();
		const bool prohibitionsChanged = ConvertIds(prohibitions, pathMap, "AddProhibition", "RemoveProhibition", "Prohibition", QT_TRANSLATE_NOOP("Attribute", "Prohibition"), operationContext);
		if (prohibitionsChanged){
			userBaseInfo.SetProhibitions(prohibitions);
		}

		return permissionsChanged || prohibitionsChanged;
	};

	if (m_userCollectionCompPtr.IsValid()){
		retVal = MigrateCollection<imtauth::IUserBaseInfo>(*m_userCollectionCompPtr, QStringLiteral("user"), convertUserBase) && retVal;
	}

	if (m_userGroupCollectionCompPtr.IsValid()){
		retVal = MigrateCollection<imtauth::IUserBaseInfo>(*m_userGroupCollectionCompPtr, QStringLiteral("group"), convertUserBase) && retVal;
	}

	return retVal;
}


CPermissionPathMigrationComp::PathMap CPermissionPathMigrationComp::CreatePathMap(QByteArrayList& ambiguousIds) const
{
	PathMap pathMap;
	QSet<QByteArray> ambiguousIdSet;

	imtbase::IObjectCollection* featureCollectionPtr = m_productInfoCompPtr->GetFeatures();
	if (featureCollectionPtr == nullptr){
		return pathMap;
	}

	for (const imtbase::ICollectionInfo::Id& elementId : featureCollectionPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!featureCollectionPtr->GetObjectData(elementId, dataPtr)){
			continue;
		}

		const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
		if (featureInfoPtr != nullptr){
			CollectPermissionPaths(*featureInfoPtr, pathMap, ambiguousIdSet);
		}
	}

	for (const QByteArray& featureId : std::as_const(ambiguousIdSet)){
		pathMap.remove(featureId);
		ambiguousIds << featureId;
	}

	return pathMap;
}


bool CPermissionPathMigrationComp::ConvertIds(
			QByteArrayList& ids,
			const PathMap& pathMap,
			const QByteArray& addOperationTypeId,
			const QByteArray& removeOperationTypeId,
			const QByteArray& key,
			const QString& keyName,
			imtbase::COperationContext& operationContext) const
{
	bool retVal = false;

	for (QByteArray& id : ids){
		if (id.startsWith('/') || !pathMap.contains(id)){
			continue;
		}

		const QByteArray path = pathMap.value(id);

		imtbase::COperationDescription removeDescription;
		removeDescription.SetOperationTypeId(removeOperationTypeId);
		removeDescription.SetKey(key);
		removeDescription.SetKeyName(keyName);
		removeDescription.SetOldValue(id);
		operationContext.GetChangesCollection()->InsertNewObject(QByteArrayLiteral("OperationInfo"), QString(), QString(), &removeDescription);

		imtbase::COperationDescription addDescription;
		addDescription.SetOperationTypeId(addOperationTypeId);
		addDescription.SetKey(key);
		addDescription.SetKeyName(keyName);
		addDescription.SetNewValue(path);
		operationContext.GetChangesCollection()->InsertNewObject(QByteArrayLiteral("OperationInfo"), QString(), QString(), &addDescription);

		id = path;
		retVal = true;
	}

	if (retVal){
		QByteArrayList uniqueIds;
		for (const QByteArray& id : std::as_const(ids)){
			if (!uniqueIds.contains(id)){
				uniqueIds << id;
			}
		}
		ids = uniqueIds;
	}

	return retVal;
}


template<class ObjectInterface, class ConvertFunction>
bool CPermissionPathMigrationComp::MigrateCollection(imtbase::IObjectCollection& collection, const QString& objectKind, ConvertFunction convertFunction) const
{
	bool retVal = true;

	typedef QPair<QByteArray, imtbase::IObjectCollection::DataPtr> ChangedObject;
	QList<ChangedObject> changedObjects;
	QList<std::shared_ptr<imtbase::COperationContext>> operationContexts;

	{
		istd::TUniqueInterfacePtr<imtbase::IObjectCollectionIterator> iteratorPtr = collection.CreateObjectCollectionIterator(QByteArray(), 0, -1, nullptr);
		if (!iteratorPtr.IsValid()){
			SendWarningMessage(0, QStringLiteral("Permission paths are not migrated: the %1 collection cannot be iterated").arg(objectKind), "imtauthdb::CPermissionPathMigrationComp");

			return false;
		}

		while (iteratorPtr->Next()){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (!iteratorPtr->GetObjectData(dataPtr)){
				continue;
			}

			ObjectInterface* objectPtr = dataPtr.GetPtr<ObjectInterface>();
			if (objectPtr == nullptr){
				continue;
			}

			std::shared_ptr<imtbase::COperationContext> operationContextPtr = std::make_shared<imtbase::COperationContext>();
			operationContextPtr->SetOperationOwnerId(imtbase::IOperationContext::IdentifableObjectInfo(QByteArray(), QStringLiteral("System")));
			if (convertFunction(*objectPtr, *operationContextPtr)){
				changedObjects << ChangedObject(iteratorPtr->GetObjectId(), dataPtr);
				operationContexts << operationContextPtr;
			}
		}
	}

	for (int i = 0; i < changedObjects.count(); ++i){
		if (!collection.SetObjectData(changedObjects[i].first, *changedObjects[i].second, istd::IChangeable::CM_WITHOUT_REFS, operationContexts[i].get())){
			SendWarningMessage(
						0,
						QStringLiteral("Permission paths are not migrated in the %1 '%2': it could not be stored").arg(objectKind, QString::fromUtf8(changedObjects[i].first)),
						"imtauthdb::CPermissionPathMigrationComp");

			retVal = false;
		}
	}

	if (!changedObjects.isEmpty()){
		SendInfoMessage(0, QStringLiteral("Permissions of %1 %2(s) are stored as feature paths now").arg(changedObjects.count()).arg(objectKind), "imtauthdb::CPermissionPathMigrationComp");
	}

	return retVal;
}


// reimplemented (imtdb::IMigrationController)

bool CPermissionPathMigrationComp::DoMigration(int& resultRevision, const istd::CIntRange& subRange) const
{
	resultRevision = -1;

	if (!subRange.IsValid() || !m_range.Contains(subRange)){
		return false;
	}

	if (!Migrate()){
		return false;
	}

	resultRevision = subRange.GetMaxValue();

	return true;
}


} // namespace imtauthdb



// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CPermissionPathMigrationComp.h>


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QSet>
#include <QtCore/QTimer>

// ImtCore includes
#include <imtauth/IRole.h>
#include <imtauth/IUserBaseInfo.h>
#include <imtbase/COperationDescription.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtlic/IFeatureInfo.h>


namespace imtauth
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

void CPermissionPathMigrationComp::Migrate()
{
	if (!m_productInfoCompPtr.IsValid()){
		return;
	}

	QByteArrayList ambiguousIds;
	const PathMap pathMap = CreatePathMap(ambiguousIds);
	if (!ambiguousIds.isEmpty()){
		SendWarningMessage(
					0,
					QStringLiteral("Permissions stored by these ids are left as they are, the ids are not unique in the feature tree: %1")
								.arg(QString::fromUtf8(ambiguousIds.join(", "))),
					"imtauth::CPermissionPathMigrationComp");
	}

	if (pathMap.isEmpty()){
		return;
	}

	const QByteArray productId = m_productInfoCompPtr->GetProductId();

	if (m_roleCollectionCompPtr.IsValid()){
		MigrateCollection<IRole>(*m_roleCollectionCompPtr, QStringLiteral("role"), [this, &pathMap, &productId](IRole& role, imtbase::COperationContext& operationContext){
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

	auto convertUserBase = [this, &pathMap, &productId](IUserBaseInfo& userBaseInfo, imtbase::COperationContext& operationContext){
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
		MigrateCollection<IUserBaseInfo>(*m_userCollectionCompPtr, QStringLiteral("user"), convertUserBase);
	}

	if (m_userGroupCollectionCompPtr.IsValid()){
		MigrateCollection<IUserBaseInfo>(*m_userGroupCollectionCompPtr, QStringLiteral("group"), convertUserBase);
	}
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
void CPermissionPathMigrationComp::MigrateCollection(imtbase::IObjectCollection& collection, const QString& objectKind, ConvertFunction convertFunction)
{
	typedef QPair<QByteArray, imtbase::IObjectCollection::DataPtr> ChangedObject;
	QList<ChangedObject> changedObjects;
	QList<std::shared_ptr<imtbase::COperationContext>> operationContexts;

	{
		istd::TUniqueInterfacePtr<imtbase::IObjectCollectionIterator> iteratorPtr = collection.CreateObjectCollectionIterator(QByteArray(), 0, -1, nullptr);
		if (!iteratorPtr.IsValid()){
			SendWarningMessage(0, QStringLiteral("Permission paths are not migrated: the %1 collection cannot be iterated").arg(objectKind), "imtauth::CPermissionPathMigrationComp");

			return;
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
						"imtauth::CPermissionPathMigrationComp");
		}
	}

	if (!changedObjects.isEmpty()){
		SendInfoMessage(0, QStringLiteral("Permissions of %1 %2(s) are stored as feature paths now").arg(changedObjects.count()).arg(objectKind), "imtauth::CPermissionPathMigrationComp");
	}
}


// reimplemented (icomp::CComponentBase)

void CPermissionPathMigrationComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// The collections are used only once the application runs.
	QTimer::singleShot(0, this, [this](){ Migrate(); });
}


} // namespace imtauth



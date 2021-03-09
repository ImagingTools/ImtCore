#include <imtlicgui/CFeatureDependencyEditorBase.h>


// Qt includes
#include <QtWidgets/QMessageBox>

// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/IFeatureDependenciesManager.h>
#include <imtlic/IFeatureInfo.h>


namespace imtlicgui
{


// public methods

CFeatureDependencyEditorBase::CFeatureDependencyEditorBase()
	:m_treeWidgetPtr(nullptr),
	m_isGuiModelInitialized(false),
	m_isCollectionRepresentationInitialized(false)
{
	m_packageCollectionObserver.SetParent(this);
}


// protected methods

void CFeatureDependencyEditorBase::OnFeaturePackageCollectionUpdate()
{
	imtbase::IObjectCollection* collectionPtr = m_packageCollectionObserver.GetObservedObject();

	BuildDependencyMap(*collectionPtr);

	if (m_treeWidgetPtr == nullptr){
		return;
	}

	imtbase::ICollectionInfo::Ids packageIds = collectionPtr->GetElementIds();

	m_packageFeatureMap.clear();

	for (const QByteArray& packageId : packageIds){
		imtbase::IObjectCollection::DataPtr packageDataPtr;
		if (collectionPtr->GetObjectData(packageId, packageDataPtr)){
			imtlic::IFeatureInfoProvider* packagePtr = dynamic_cast<imtlic::IFeatureInfoProvider*>(packageDataPtr.GetPtr());
			if (packagePtr != nullptr){
				FeatureDescriptionList featureDescriptionList;

				imtbase::ICollectionInfo::Ids featureIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureId : featureIds){
					const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureId);
					if (featureInfoPtr != nullptr){
						FeatureDescription featureDescription;

						featureDescription.id = featureInfoPtr->GetFeatureId();
						featureDescription.name = featureInfoPtr->GetFeatureName();
						featureDescription.description = packagePtr->GetFeatureList().GetElementInfo(featureId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

						featureDescriptionList.append(featureDescription);
					}
				}

				m_packageFeatureMap[packageId] = featureDescriptionList;
				m_packageNameMap[packageId] = collectionPtr->GetElementInfo(packageId, imtbase::ICollectionInfo::EIT_NAME).toString();
			}
		}
	}

	m_isCollectionRepresentationInitialized = true;

	if (m_isGuiModelInitialized && m_isCollectionRepresentationInitialized){
		EnumerateMissingDependencies();
		UpdateFeatureTree();
		UpdateFeatureTreeCheckStates();
		UpdateFeaturePackageModel();
	}
}


void CFeatureDependencyEditorBase::EnumerateMissingDependencies()
{
	QByteArrayList allFeatureIds;

	QByteArrayList packageIds = m_packageFeatureMap.keys();
	for (const QByteArray& packageId : packageIds){
		FeatureDescriptionList featureDescriptionList = m_packageFeatureMap[packageId];
		for (const FeatureDescription& featureDescription : featureDescriptionList){
			allFeatureIds.append(featureDescription.id);
		}
	}

	for (int i = 0; i < m_features.count(); i++){
		if (!allFeatureIds.contains(m_features[i].id)){
			allFeatureIds.append(m_features[i].id);
		}
	}

	m_missingDependencies.clear();

	if (m_dependencies.contains(m_selectedFeatureId)){
		for (const QByteArray& featureId : m_dependencies[m_selectedFeatureId]){
			if (!allFeatureIds.contains(featureId)){
				m_missingDependencies.push_back(featureId);
			}
		}
	}
}


void CFeatureDependencyEditorBase::UpdateFeatureTree()
{
	if (m_treeWidgetPtr == nullptr){
		return;
	}

	QSignalBlocker blocker(m_treeWidgetPtr);

	m_treeWidgetPtr->clear();

	if (m_selectedFeatureId == "NO_SELECTION"){
		return;
	}

	QByteArrayList packageIds = m_packageFeatureMap.keys();
	for (const QByteArray& packageId : packageIds){
		QTreeWidgetItem* packageItemPtr = new QTreeWidgetItem({m_packageNameMap[packageId]});
		m_treeWidgetPtr->addTopLevelItem(packageItemPtr);
		packageItemPtr->setData(0, DR_ITEM_ID, packageId);
		packageItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);

		FeatureDescriptionList featureDescriptionList = m_packageFeatureMap[packageId];
		for (const FeatureDescription& featureDescription : featureDescriptionList){
			if (featureDescription.id == m_selectedFeatureId){
				continue;
			}

			QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem({featureDescription.name});
			packageItemPtr->addChild(featureItemPtr);
			featureItemPtr->setData(0, DR_ITEM_ID, featureDescription.id);
			featureItemPtr->setData(0, DR_ITEM_TYPE, IT_FEATURE);
		}

		if (packageItemPtr->childCount() == 0){
			m_treeWidgetPtr->takeTopLevelItem(m_treeWidgetPtr->indexOfTopLevelItem(packageItemPtr));
			delete packageItemPtr;
		}
	}

	QTreeWidgetItem* unsavedFeaturesGroupItemPtr = nullptr;
	for (int i = 0; i < m_features.count(); i++){
		if (GetItem(m_features[i].id) == nullptr){
			if (m_features[i].id == m_selectedFeatureId){
				continue;
			}

			if (unsavedFeaturesGroupItemPtr == nullptr){
				unsavedFeaturesGroupItemPtr = new QTreeWidgetItem({QObject::tr("Unsaved Features")});
				m_treeWidgetPtr->addTopLevelItem(unsavedFeaturesGroupItemPtr);
				unsavedFeaturesGroupItemPtr->setData(0, DR_ITEM_ID, "UNSAVED_FEATURES");
				unsavedFeaturesGroupItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);
			}

			QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem({m_features[i].name});
			unsavedFeaturesGroupItemPtr->addChild(featureItemPtr);
			featureItemPtr->setData(0, DR_ITEM_ID, m_features[i].id);
			featureItemPtr->setData(0, DR_ITEM_TYPE, IT_FEATURE);
		}
	}

	if (!m_missingDependencies.isEmpty()){
		QTreeWidgetItem* packageItemPtr = new QTreeWidgetItem({QObject::tr("Missing features")});
		m_treeWidgetPtr->addTopLevelItem(packageItemPtr);
		packageItemPtr->setData(0, DR_ITEM_ID, "MISSING_FEATURES");
		packageItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);
		packageItemPtr->setForeground(0, QBrush(Qt::red));

		for (const QByteArray& featureId : m_missingDependencies){
			QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem({QObject::tr("ID: %1").arg(QString(featureId))});
			packageItemPtr->addChild(featureItemPtr);
			featureItemPtr->setData(0, DR_ITEM_ID, featureId);
			featureItemPtr->setData(0, DR_ITEM_TYPE, IT_FEATURE);
			featureItemPtr->setForeground(0, QBrush(Qt::red));
		}
	}

	m_treeWidgetPtr->expandAll();
}


void CFeatureDependencyEditorBase::UpdateFeatureTreeCheckStates()
{
	if (m_treeWidgetPtr == nullptr){
		return;
	}

	QSignalBlocker blocker(m_treeWidgetPtr);

	for (int packageIndex = 0; packageIndex < m_treeWidgetPtr->topLevelItemCount(); packageIndex++){
		QTreeWidgetItem* packageItemPtr = m_treeWidgetPtr->topLevelItem(packageIndex);
		for (int featureIndex = 0; featureIndex < packageItemPtr->childCount(); featureIndex++){
			QTreeWidgetItem* featureItemPtr = packageItemPtr->child(featureIndex);

			QByteArray featureId = featureItemPtr->data(0, DR_ITEM_ID).toByteArray();

			bool isFeaturePresent = m_dependencies[m_selectedFeatureId].contains(featureId);
			featureItemPtr->setCheckState(0, isFeaturePresent ? Qt::Checked : Qt::Unchecked);
		}
	}

	for (int packageIndex = 0; packageIndex < m_treeWidgetPtr->topLevelItemCount(); packageIndex++){
		QTreeWidgetItem* packageItemPtr = m_treeWidgetPtr->topLevelItem(packageIndex);

		bool isAllFeaturesChecked = true;
		bool isAllFeaturesUnchecked = true;

		for (int featureIndex = 0; featureIndex < packageItemPtr->childCount(); featureIndex++){
			QTreeWidgetItem* featureItemPtr = packageItemPtr->child(featureIndex);
			if (featureItemPtr != nullptr){
				if (featureItemPtr->checkState(0) == Qt::Checked){
					isAllFeaturesUnchecked = false;
				}
				else if (featureItemPtr->checkState(0) == Qt::Unchecked){
					isAllFeaturesChecked = false;
				}
			}
		}

		if (packageItemPtr != nullptr){
			if (isAllFeaturesChecked){
				packageItemPtr->setCheckState(0, Qt::Checked);
			}
			else if (isAllFeaturesUnchecked){
				packageItemPtr->setCheckState(0, Qt::Unchecked);
			}
			else{
				packageItemPtr->setCheckState(0, Qt::PartiallyChecked);
			}
		}
	}

	if (!m_missingDependencies.isEmpty()){
		QTreeWidgetItem* packageItemPtr = GetItem("MISSING_FEATURES");
		if (packageItemPtr != nullptr){
			packageItemPtr->setCheckState(0, Qt::Checked);

			for (const QByteArray& featureId : m_missingDependencies){
				QTreeWidgetItem* featureItemPtr = GetItem(featureId);
				if (featureItemPtr != nullptr){
					featureItemPtr->setCheckState(0, Qt::Checked);
				}
			}
		}
	}
}


QTreeWidgetItem* CFeatureDependencyEditorBase::GetItem(const QByteArray& itemId)
{
	if (m_treeWidgetPtr == nullptr){
		return nullptr;
	}

	int packageCount = m_treeWidgetPtr->topLevelItemCount();
	for (int packageIndex = 0; packageIndex < packageCount; packageIndex++){
		QTreeWidgetItem* packageItemPtr = m_treeWidgetPtr->topLevelItem(packageIndex);
		if (packageItemPtr->data(0, DR_ITEM_ID).toByteArray() == itemId){
			return packageItemPtr;
		}

		int featureCount = packageItemPtr->childCount();
		for (int featureIndex = 0; featureIndex < featureCount; featureIndex++){
			QTreeWidgetItem* featurteItemPtr = packageItemPtr->child(featureIndex);
			if (featurteItemPtr->data(0, DR_ITEM_ID).toByteArray() == itemId){
				return featurteItemPtr;
			}
		}
	}

	return nullptr;
}


void CFeatureDependencyEditorBase::BuildDependencyMap(const imtbase::IObjectCollection& packageCollection)
{
	m_packageDependenciyMap.clear();

	imtbase::ICollectionInfo::Ids packageIds = packageCollection.GetElementIds();
	for (const QByteArray& packageId : packageIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		packageCollection.GetObjectData(packageId, dataPtr);

		const imtlic::IFeatureInfoProvider* packagePtr = dynamic_cast<const imtlic::IFeatureInfoProvider*>(dataPtr.GetPtr());
		if (packagePtr != nullptr){
			const imtlic::IFeatureDependenciesProvider* dependenciesProvider = packagePtr->GetDependenciesInfoProvider();
			if (dependenciesProvider != nullptr){
				imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureCollectionId : featureCollectionIds){
					const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);
					if (featureInfoPtr != nullptr){
						QByteArray featureId = featureInfoPtr->GetFeatureId();

						m_packageDependenciyMap[featureId] = dependenciesProvider->GetFeatureDependencies(featureId);
					}
				}
			}
		}
	}
}


bool CFeatureDependencyEditorBase::HasDependency(const DependencyMap& dependencyMap, const QByteArray& fromFeatureId, const QByteArray& toFeatureId)
{
	if (dependencyMap.contains(fromFeatureId)){
		if (dependencyMap[fromFeatureId].contains(toFeatureId)){
			return true;
		}

		for (const QByteArray& featureId : dependencyMap[fromFeatureId]){
			if (HasDependency(dependencyMap, featureId, toFeatureId)){
				return true;
			}
		}
	}

	return false;
}


void CFeatureDependencyEditorBase::on_FeatureTree_itemChanged(QTreeWidgetItem *item, int column)
{
	imtbase::IObjectCollection* collectionPtr = m_packageCollectionObserver.GetObjectPtr();

	for (const QByteArray& id : m_dependencies.keys()){
		m_packageDependenciyMap[id] = m_dependencies[id];
	}

	if (item->data(0, DR_ITEM_TYPE) == IT_PACKAGE){
		Qt::CheckState state = item->checkState(0);

		for (int i = 0; i < item->childCount(); i++){
			QTreeWidgetItem* featureItemPtr = item->child(i);
			QByteArray featureId = featureItemPtr->data(0, DR_ITEM_ID).toByteArray();

			if (state == Qt::Checked){
				if (!m_dependencies[m_selectedFeatureId].contains(featureId)){
					if (!HasDependency(m_packageDependenciyMap, featureId, m_selectedFeatureId)){
						m_dependencies[m_selectedFeatureId].append(featureId);
					}
					else{
						QMessageBox::warning(
									m_treeWidgetPtr,
									QObject::tr("Warning"),
									QObject::tr("Feature with ID '%1' depends on feature with ID '%2'")
												.arg(QString(featureId))
												.arg(QString(m_selectedFeatureId)));

						break;
					}
				}
			}
			else{
				if (m_dependencies[m_selectedFeatureId].contains(featureId)){
					m_dependencies[m_selectedFeatureId].removeOne(featureId);
				}
			}
		}
	}
	else{
		QByteArray featureId = item->data(0, DR_ITEM_ID).toByteArray();

		if (item->checkState(0) == Qt::Checked){
			if (!m_dependencies[m_selectedFeatureId].contains(featureId)){
				if (!HasDependency(m_packageDependenciyMap, featureId, m_selectedFeatureId)){
					m_dependencies[m_selectedFeatureId].append(featureId);
				}
				else{
					QMessageBox::warning(
								m_treeWidgetPtr,
								QObject::tr("Warning"),
								QObject::tr("Feature with ID '%1' depends on feature with ID '%2'")
											.arg(QString(featureId))
											.arg(QString(m_selectedFeatureId)));
				}
			}
		}
		else{
			if (m_dependencies[m_selectedFeatureId].contains(featureId)){
				m_dependencies[m_selectedFeatureId].removeOne(featureId);
			}
		}
	}

	FeatureTreeItemChanged();
}


// public methods of the embedded class FeaturePackageCollectionObserver

CFeatureDependencyEditorBase::FeaturePackageCollectionObserver::FeaturePackageCollectionObserver()
	:m_parentPtr(nullptr)
{
}


void CFeatureDependencyEditorBase::FeaturePackageCollectionObserver::SetParent(CFeatureDependencyEditorBase* parentPtr)
{
	m_parentPtr = parentPtr;
}


// reimplemented (imod::CSingleModelObserverBase)

void CFeatureDependencyEditorBase::FeaturePackageCollectionObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	m_parentPtr->OnFeaturePackageCollectionUpdate();
}


} // namespace imtlicgui



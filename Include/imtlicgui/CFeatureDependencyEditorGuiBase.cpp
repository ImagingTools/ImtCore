#include <imtlicgui/CFeatureDependencyEditorGuiBase.h>


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

CFeatureDependencyEditorGuiBase::CFeatureDependencyEditorGuiBase()
	:m_treeWidgetPtr(nullptr),
	m_isGuiModelInitialized(false),
	m_isCollectionRepresentationInitialized(false)
{
	m_packageCollectionObserver.SetParent(this);
}


// protected methods

void CFeatureDependencyEditorGuiBase::OnFeaturePackageCollectionUpdate()
{
	if (m_treeWidgetPtr == nullptr){
		return;
	}

	imtbase::IObjectCollection* collectionPtr = m_packageCollectionObserver.GetObservedObject();
	imtbase::ICollectionInfo::Ids packageIds = collectionPtr->GetElementIds();

	m_treeWidgetPtr->clear();
	m_packageFeatures.clear();

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

				m_packageFeatures[packageId] = featureDescriptionList;
				m_packageNames[packageId] = collectionPtr->GetElementInfo(packageId, imtbase::ICollectionInfo::EIT_NAME).toString();
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


void CFeatureDependencyEditorGuiBase::EnumerateMissingDependencies()
{
	QByteArrayList allFeatureIds;

	QByteArrayList packageIds = m_packageFeatures.keys();
	for (const QByteArray& packageId : packageIds){
		FeatureDescriptionList featureDescriptionList = m_packageFeatures[packageId];
		for (const FeatureDescription& featureDescription : featureDescriptionList){
			allFeatureIds.append(featureDescription.id);
		}
	}

	m_missingDependencies.clear();

	for (const QByteArray& featureId : m_dependencies){
		if (!allFeatureIds.contains(featureId)){
			m_missingDependencies.push_back(featureId);
		}
	}
}


void CFeatureDependencyEditorGuiBase::UpdateFeatureTree()
{
	if (m_treeWidgetPtr == nullptr){
		return;
	}

	QSignalBlocker blocker(m_treeWidgetPtr);

	m_treeWidgetPtr->clear();

	if (m_featureId.isEmpty()){
		return;
	}

	QByteArrayList packageIds = m_packageFeatures.keys();
	for (const QByteArray& packageId : packageIds){
		QTreeWidgetItem* packageItemPtr = new QTreeWidgetItem({m_packageNames[packageId]});
		m_treeWidgetPtr->addTopLevelItem(packageItemPtr);
		packageItemPtr->setData(0, DR_ITEM_ID, packageId);
		packageItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);

		FeatureDescriptionList featureDescriptionList = m_packageFeatures[packageId];
		for (const FeatureDescription& featureDescription : featureDescriptionList){
			if (featureDescription.id == m_featureId){
				continue;
			}

			QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem({
				featureDescription.name,
				featureDescription.id,
				featureDescription.description});
			packageItemPtr->addChild(featureItemPtr);
			featureItemPtr->setData(0, DR_ITEM_ID, featureDescription.id);
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


void CFeatureDependencyEditorGuiBase::UpdateFeatureTreeCheckStates()
{
	if (m_treeWidgetPtr == nullptr){
		return;
	}

	QSignalBlocker blocker(m_treeWidgetPtr);

	QByteArrayList packageIds = m_packageFeatures.keys();
	for (const QByteArray& packageId : packageIds){
		FeatureDescriptionList featureDescriptionList = m_packageFeatures[packageId];
		for (const FeatureDescription& featureDescription : featureDescriptionList){
			QTreeWidgetItem* featureItemPtr = GetItem(featureDescription.id);
			if (featureItemPtr != nullptr){
				bool isFeaturePresent = m_dependencies.contains(featureDescription.id);
				featureItemPtr->setCheckState(0, isFeaturePresent ? Qt::Checked : Qt::Unchecked);
			}
		}
	}

	for (const QByteArray& packageId : packageIds){
		bool isAllFeaturesChecked = true;
		bool isAllFeaturesUnchecked = true;

		FeatureDescriptionList featureDescriptionList = m_packageFeatures[packageId];
		for (const FeatureDescription& featureDescription : featureDescriptionList){
			QTreeWidgetItem* featureItemPtr = GetItem(featureDescription.id);
			if (featureItemPtr != nullptr){
				if (featureItemPtr->checkState(0) == Qt::Checked){
					isAllFeaturesUnchecked = false;
				}
				else if (featureItemPtr->checkState(0) == Qt::Unchecked){
					isAllFeaturesChecked = false;
				}
			}
		}

		QTreeWidgetItem* packageItemPtr = GetItem(packageId);
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


QTreeWidgetItem* CFeatureDependencyEditorGuiBase::GetItem(const QByteArray& itemId)
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


CFeatureDependencyEditorGuiBase::DependencyMap CFeatureDependencyEditorGuiBase::BuildDependencyMap(const imtbase::IObjectCollection& packageCollection)
{
	DependencyMap dependencyMap;

	imtbase::ICollectionInfo::Ids packageIds = packageCollection.GetElementIds();
	for (const QByteArray& packageId : packageIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		packageCollection.GetObjectData(packageId, dataPtr);

		const imtlic::IFeatureInfoProvider* packagePtr = dynamic_cast<const imtlic::IFeatureInfoProvider*>(dataPtr.GetPtr());
		if (packagePtr != nullptr){
			const imtlic::IFeatureDependenciesProvider* dependenciesProvider = packagePtr->GetDependenciesInfoProvider();
			if (dependenciesProvider != nullptr){
				imtbase::ICollectionInfo::Ids featureIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureId : featureIds){
					const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureId);
					if (featureInfoPtr != nullptr){
						dependencyMap[featureInfoPtr->GetFeatureId()] = dependenciesProvider->GetFeatureDependencies(featureInfoPtr->GetFeatureId());
					}
				}
			}
		}
	}

	return dependencyMap;
}


bool CFeatureDependencyEditorGuiBase::HasDependency(const DependencyMap& dependencyMap, const QByteArray& fromFeatureId, const QByteArray& toFeatureId)
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


void CFeatureDependencyEditorGuiBase::on_FeatureTree_itemChanged(QTreeWidgetItem *item, int column)
{
	DependencyMap dependencyMap;
	imtbase::IObjectCollection* collectionPtr = m_packageCollectionObserver.GetObjectPtr();
	if (collectionPtr != nullptr){
		dependencyMap = BuildDependencyMap(*collectionPtr);
	}

	if (item->data(0, DR_ITEM_TYPE) == IT_PACKAGE){
		Qt::CheckState state = item->checkState(0);

		for (int i = 0; i < item->childCount(); i++){
			QTreeWidgetItem* featureItemPtr = item->child(i);
			QByteArray featureId = featureItemPtr->data(0, DR_ITEM_ID).toByteArray();

			if (state == Qt::Checked){
				if (!m_dependencies.contains(featureId)){
					if (!HasDependency(dependencyMap, featureId, m_featureId)){
						m_dependencies.append(featureId);
					}
					else{
						QMessageBox::warning(
									m_treeWidgetPtr,
									QObject::tr("Warning"),
									QObject::tr("Feature with ID '%1' depends on feature with ID '%2'")
												.arg(QString(featureId))
												.arg(QString(m_featureId)));

						break;
					}
				}
			}
			else{
				if (m_dependencies.contains(featureId)){
					m_dependencies.removeOne(featureId);
				}
			}
		}
	}
	else{
		QByteArray featureId = item->data(0, DR_ITEM_ID).toByteArray();

		if (item->checkState(0) == Qt::Checked){
			if (!m_dependencies.contains(featureId)){
				if (!HasDependency(dependencyMap, featureId, m_featureId)){
					m_dependencies.append(featureId);
				}
				else{
					QMessageBox::warning(
								m_treeWidgetPtr,
								QObject::tr("Warning"),
								QObject::tr("Feature with ID '%1' depends on feature with ID '%2'")
											.arg(QString(featureId))
											.arg(QString(m_featureId)));
				}
			}
		}
		else{
			if (m_dependencies.contains(featureId)){
				m_dependencies.removeOne(featureId);
			}
		}
	}

	FeatureTreeItemChanged();
}


// public methods of the embedded class FeaturePackageCollectionObserver

CFeatureDependencyEditorGuiBase::FeaturePackageCollectionObserver::FeaturePackageCollectionObserver()
	:m_parentPtr(nullptr)
{
}


void CFeatureDependencyEditorGuiBase::FeaturePackageCollectionObserver::SetParent(CFeatureDependencyEditorGuiBase* parentPtr)
{
	m_parentPtr = parentPtr;
}


// reimplemented (imod::CSingleModelObserverBase)

void CFeatureDependencyEditorGuiBase::FeaturePackageCollectionObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	m_parentPtr->OnFeaturePackageCollectionUpdate();
}


} // namespace imtlicgui



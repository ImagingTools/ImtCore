#include <imtlicgui/CFeaturePackageGuiComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/IFeatureDependenciesProvider.h>
#include <imtlic/IFeatureInfo.h>

namespace imtlicgui
{


// public methods

CFeaturePackageGuiComp::CFeaturePackageGuiComp()
	:m_collectionObserver(*this),
	m_showCollectionEditorCommand("Edit Feartures", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_EXCLUSIVE, 2020),
	m_showDependenciesEditorCommand("Edit Dependencies", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_EXCLUSIVE, 2020)
{
	m_rootCommands.InsertChild(&m_showCollectionEditorCommand);
	m_rootCommands.InsertChild(&m_showDependenciesEditorCommand);

	connect(&m_showCollectionEditorCommand, &QAction::triggered, this, &CFeaturePackageGuiComp::OnShowCollectionEditor);
	connect(&m_showDependenciesEditorCommand, &QAction::triggered, this, &CFeaturePackageGuiComp::OnShowFeatureDependencyEditor);
}


// protected methods

void CFeaturePackageGuiComp::OnFeaturePackageCollectionUpdate()
{
	imtbase::IObjectCollection* collectionPtr = m_collectionObserver.GetObservedObject();
	imtbase::ICollectionInfo::Ids packageIds = collectionPtr->GetElementIds();

	FeatureTree->clear();
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
		DoUpdateModel();
	}
}


void CFeaturePackageGuiComp::EnumerateMissingDependencies()
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


void CFeaturePackageGuiComp::UpdateFeatureList()
{
	FeatureList->clear();

	imtlic::IFeatureInfoProvider* featureInfoProviderPtr = GetObservedObject();
	if (featureInfoProviderPtr != nullptr){
		const imtbase::ICollectionInfo& collectionInfo = featureInfoProviderPtr->GetFeatureList();
		imtbase::ICollectionInfo::Ids ids = collectionInfo.GetElementIds();
		for (QByteArray& id : ids){
			const imtlic::IFeatureInfo* featureInfoPtr = featureInfoProviderPtr->GetFeatureInfo(id);
			if (featureInfoPtr != nullptr){
				QListWidgetItem* itemPtr = new QListWidgetItem(featureInfoPtr->GetFeatureName());
				FeatureList->addItem(itemPtr);
				itemPtr->setData(DR_ITEM_ID, featureInfoPtr->GetFeatureId());
			}
		}
	}
}


void CFeaturePackageGuiComp::UpdateFeatureTree()
{
	QSignalBlocker blocker(FeatureTree);

	FeatureTree->clear();

	QListWidgetItem* selectedItemPtr = FeatureList->currentItem();
	if (selectedItemPtr != nullptr){
		QByteArrayList packageIds = m_packageFeatures.keys();
		for (const QByteArray& packageId : packageIds){
			QTreeWidgetItem* packageItemPtr = new QTreeWidgetItem({m_packageNames[packageId]});
			FeatureTree->addTopLevelItem(packageItemPtr);
			packageItemPtr->setData(0, DR_ITEM_ID, packageId);
			packageItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);

			FeatureDescriptionList featureDescriptionList = m_packageFeatures[packageId];
			for (const FeatureDescription& featureDescription : featureDescriptionList){
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
			QTreeWidgetItem* packageItemPtr = new QTreeWidgetItem({tr("Missing features")});
			FeatureTree->addTopLevelItem(packageItemPtr);
			packageItemPtr->setData(0, DR_ITEM_ID, "MISSING_FEATURES");
			packageItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);
			packageItemPtr->setForeground(0, QBrush(Qt::red));

			for (const QByteArray& featureId : m_missingDependencies){
				QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem({tr("ID: %1").arg(QString(featureId))});
				packageItemPtr->addChild(featureItemPtr);
				featureItemPtr->setData(0, DR_ITEM_ID, featureId);
				featureItemPtr->setData(0, DR_ITEM_TYPE, IT_FEATURE);
				featureItemPtr->setForeground(0, QBrush(Qt::red));
			}
		}

		FeatureTree->expandAll();
	}
}


void CFeaturePackageGuiComp::UpdateFeatureTreeCheckStates()
{
	QSignalBlocker blocker(FeatureTree);

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


QTreeWidgetItem* CFeaturePackageGuiComp::GetItem(const QByteArray& itemId)
{
	int packageCount = FeatureTree->topLevelItemCount();
	for (int packageIndex = 0; packageIndex < packageCount; packageIndex++){
		QTreeWidgetItem* packageItemPtr = FeatureTree->topLevelItem(packageIndex);
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


// reimplemented(ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CFeaturePackageGuiComp::GetCommands() const
{
	return &m_rootCommands;
}


// reimplemented (iqtgui::CGuiComponentBase)

void CFeaturePackageGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	connect(FeatureList, &QListWidget::itemSelectionChanged, this, &CFeaturePackageGuiComp::OnFeatureListSelectionChanged);
	connect(this, &CFeaturePackageGuiComp::EmitFeatureTreeItemChanged, this, &CFeaturePackageGuiComp::OnFeatureTreeItemChanged, Qt::QueuedConnection);
}


void CFeaturePackageGuiComp::OnGuiDestroyed()
{
	disconnect(this, &CFeaturePackageGuiComp::EmitFeatureTreeItemChanged, this, &CFeaturePackageGuiComp::OnFeatureTreeItemChanged);
	disconnect(FeatureList, &QListWidget::itemChanged, this, &CFeaturePackageGuiComp::OnFeatureListSelectionChanged);

	BaseClass::OnGuiDestroyed();
}


void CFeaturePackageGuiComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	m_showCollectionEditorCommand.SetVisuals(tr("Edit Features"), tr("Features"), tr("Show Feature List"), QIcon(":/ColorIcons/FeatureList"));
	m_showDependenciesEditorCommand.SetVisuals(tr("Edit Dependencies"), tr("Dependencies"), tr("Show Feature Dependencies"), QIcon(":/ColorIcons/Dependencies"));

	m_showCollectionEditorCommand.setChecked(true);
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CFeaturePackageGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_isGuiModelInitialized = true;

	if (m_isGuiModelInitialized && m_isCollectionRepresentationInitialized){
		EnumerateMissingDependencies();
		UpdateFeatureList();
		UpdateFeatureTree();
		UpdateFeatureTreeCheckStates();
		DoUpdateModel();
	}
}


void CFeaturePackageGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		imod::IModel* modelPtr = GetObservedModel();
		Q_ASSERT(modelPtr != nullptr);

		if (modelPtr->AttachObserver(m_objectCollectionObserverCompPtr.GetPtr())){
			m_objectCollectionViewCompPtr->CreateGui(FeatureCollectionView);
		}

		imod::IModel* featurePackageCollectionModelPtr = const_cast<imod::IModel*>(
					dynamic_cast<const imod::IModel*>(GetObservedObject()->GetFeaturePackages()));
		
		if (featurePackageCollectionModelPtr != nullptr){
			featurePackageCollectionModelPtr->AttachObserver(&m_collectionObserver);
		}
	}
}


void CFeaturePackageGuiComp::OnGuiModelDetached()
{
	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		imod::IModel* featurePackageCollectionModelPtr = const_cast<imod::IModel*>(
					dynamic_cast<const imod::IModel*>(GetObservedObject()->GetFeaturePackages()));

		if (featurePackageCollectionModelPtr != nullptr){
			if (featurePackageCollectionModelPtr->IsAttached(&m_collectionObserver)){
				featurePackageCollectionModelPtr->DetachObserver(&m_collectionObserver);
			}
		}

		imod::IModel* modelPtr = GetObservedModel();
		Q_ASSERT(modelPtr != nullptr);

		if (modelPtr->IsAttached(m_objectCollectionObserverCompPtr.GetPtr())){
			modelPtr->DetachObserver(m_objectCollectionObserverCompPtr.GetPtr());
		}

		if (m_objectCollectionViewCompPtr->IsGuiCreated()){
			m_objectCollectionViewCompPtr->DestroyGui();
		}
	}

	BaseClass::OnGuiModelDetached();
}


void CFeaturePackageGuiComp::UpdateModel() const
{
	//QListWidgetItem* itemPtr = FeatureList->currentItem();
	//if (itemPtr != nullptr){
	//	QByteArray featureId = itemPtr->data(DR_ITEM_ID).toByteArray();

	//	imtlic::IFeatureInfoProvider* featureInfoProviderPtr = GetObservedObject();
	//	if (featureInfoProviderPtr != nullptr){
	//		const imtlic::IFeatureDependenciesProvider* dependenciesProviderPtr = featureInfoProviderPtr->GetDependenciesInfoProvider();
	//		if (dependenciesProviderPtr != nullptr){
	//			QByteArrayList m_dependencies = dependenciesProviderPtr->SetFeatureDependencies(featureId);
	//		}
	//	}
	//}
}


// protected slots

void CFeaturePackageGuiComp::on_FeatureTree_itemChanged(QTreeWidgetItem *item, int column)
{
	if (item->data(0, DR_ITEM_TYPE) == IT_PACKAGE){
		Qt::CheckState state = item->checkState(0);

		for (int i = 0; i < item->childCount(); i++){
			QTreeWidgetItem* featureItemPtr = item->child(i);
			QByteArray id = featureItemPtr->data(0, DR_ITEM_ID).toByteArray();

			if (state == Qt::Checked){
				if (!m_dependencies.contains(id)){
					m_dependencies.append(id);
				}
			}
			else{
				if (m_dependencies.contains(id)){
					m_dependencies.removeOne(id);
				}
			}
		}
	}
	else{
		QByteArray id = item->data(0, DR_ITEM_ID).toByteArray();

		if (item->checkState(0) == Qt::Checked){
			if (!m_dependencies.contains(id)){
				m_dependencies.append(id);
			}
		}
		else{
			if (m_dependencies.contains(id)){
				m_dependencies.removeOne(id);
			}
		}
	}

	Q_EMIT EmitFeatureTreeItemChanged();
}


void CFeaturePackageGuiComp::OnShowCollectionEditor()
{
	Pages->setCurrentIndex(0);
}


void CFeaturePackageGuiComp::OnShowFeatureDependencyEditor()
{
	Pages->setCurrentIndex(1);
}


void CFeaturePackageGuiComp::OnFeatureListSelectionChanged()
{
	m_dependencies.clear();

	QListWidgetItem* itemPtr = FeatureList->currentItem();
	if (itemPtr != nullptr){
		QByteArray featureId = itemPtr->data(DR_ITEM_ID).toByteArray();

		imtlic::IFeatureInfoProvider* featureInfoProviderPtr = GetObservedObject();
		if (featureInfoProviderPtr != nullptr){
			const imtlic::IFeatureDependenciesProvider* dependenciesProviderPtr = featureInfoProviderPtr->GetDependenciesInfoProvider();
			if (dependenciesProviderPtr != nullptr){
				QByteArrayList m_dependencies = dependenciesProviderPtr->GetFeatureDependencies(featureId);
			}
		}
	}

	if (m_isGuiModelInitialized && m_isCollectionRepresentationInitialized){
		EnumerateMissingDependencies();
		UpdateFeatureTree();
		UpdateFeatureTreeCheckStates();
	}
}


void CFeaturePackageGuiComp::OnFeatureTreeItemChanged()
{
	if (m_isGuiModelInitialized && m_isCollectionRepresentationInitialized){
		EnumerateMissingDependencies();
		UpdateFeatureTree();
		UpdateFeatureTreeCheckStates();
		DoUpdateModel();
	}
}


// public methods of the embedded class FeaturePackageCollectionObserver

CFeaturePackageGuiComp::FeaturePackageCollectionObserver::FeaturePackageCollectionObserver(CFeaturePackageGuiComp& parent)
	:m_parent(parent)
{
}


// reimplemented (imod::CSingleModelObserverBase)

void CFeaturePackageGuiComp::FeaturePackageCollectionObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	m_parent.OnFeaturePackageCollectionUpdate();
}


} // namespace imtlicgui



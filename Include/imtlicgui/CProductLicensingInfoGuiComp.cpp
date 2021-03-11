#include <imtlicgui/CProductLicensingInfoGuiComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/IFeatureDependenciesProvider.h>
#include <imtlic/IFeatureInfoProvider.h>

#include <imtlic/IFeatureInfo.h>
#include <imtlic/ILicenseInfo.h>

namespace imtlicgui
{


// public methods

CProductLicensingInfoGuiComp::CProductLicensingInfoGuiComp()
	:m_featurePackageCollectionObserver(*this),
	m_licenseSelectionObserver(*this),
	m_isGuiModelInitialized(false),
	m_isCollectionRepresentationInitialized(false)
{
	m_selectedLicenseId = "NO_SELECTION";
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CProductLicensingInfoGuiComp::GetCommands() const
{
	ibase::ICommandsProvider* commandsProviderPtr = dynamic_cast<ibase::ICommandsProvider*>(m_objectCollectionViewCompPtr.GetPtr());
	if (commandsProviderPtr != nullptr){
		commandsProviderPtr->GetCommands();
	}
	
	return nullptr;
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CProductLicensingInfoGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::IProductLicensingInfo* productLicensingInfoPtr = GetObservedObject();
	Q_ASSERT(productLicensingInfoPtr != nullptr);

	m_featureIds.clear();
	const imtlic::ILicenseInfo* licenseInfoPtr = productLicensingInfoPtr->GetLicenseInfo(m_selectedLicenseId);
	if (licenseInfoPtr != nullptr){
		m_featureIds = licenseInfoPtr->GetFeatures();
	}	

	EnumerateMissingFeatures();
	UpdateFeatureTree();
	UpdateFeatureTreeCheckStates();
	DoUpdateModel();
}


void CProductLicensingInfoGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		imod::IModel* licensingInfoModelPtr = GetObservedModel();
		Q_ASSERT(licensingInfoModelPtr != nullptr);

		if (licensingInfoModelPtr->AttachObserver(m_objectCollectionObserverCompPtr.GetPtr())){
			m_objectCollectionViewCompPtr->CreateGui(Licenses);
		}

		imod::IModel* featurePackageCollectionModelPtr = const_cast<imod::IModel*>(
			dynamic_cast<const imod::IModel*>(m_featurePackageCollectionCompPtr.GetPtr()));

		if (featurePackageCollectionModelPtr != nullptr){
			featurePackageCollectionModelPtr->AttachObserver(&m_featurePackageCollectionObserver);
		}

		imod::IModel* licenseSelectionModelPtr = dynamic_cast<imod::IModel*>(m_objectCollectionViewCompPtr.GetPtr());

		if (licenseSelectionModelPtr != nullptr){
			licenseSelectionModelPtr->AttachObserver(&m_licenseSelectionObserver);
		}
	}
}


void CProductLicensingInfoGuiComp::OnGuiModelDetached()
{
	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		imod::IModel* licensingInfoModelPtr = GetObservedModel();
		Q_ASSERT(licensingInfoModelPtr != nullptr);

		m_featurePackageCollectionObserver.EnsureModelDetached();

		if (licensingInfoModelPtr->IsAttached(m_objectCollectionObserverCompPtr.GetPtr())){
			licensingInfoModelPtr->DetachObserver(m_objectCollectionObserverCompPtr.GetPtr());

			m_objectCollectionViewCompPtr->DestroyGui();
		}
	}

	BaseClass::OnGuiModelDetached();
}


void CProductLicensingInfoGuiComp::UpdateModel() const
{
	imtlic::IProductLicensingInfo* productLicensingInfoPtr = GetObservedObject();
	Q_ASSERT(productLicensingInfoPtr != nullptr);

	imtlic::ILicenseInfo* licenseInfoPtr = nullptr;
	
	imtbase::ICollectionInfo::Ids licenseCollectionIds = productLicensingInfoPtr->GetLicenseList().GetElementIds();
	for (const QByteArray& licenseCollectionId : licenseCollectionIds){
		const imtlic::ILicenseInfo* currentLicenseInfoPtr = productLicensingInfoPtr->GetLicenseInfo(licenseCollectionId);
		if (currentLicenseInfoPtr->GetLicenseId() == m_selectedLicenseId){
			licenseInfoPtr = const_cast<imtlic::ILicenseInfo*>(currentLicenseInfoPtr);

			break;
		}
	}

	if (licenseInfoPtr != nullptr){
		licenseInfoPtr->SetFeatures(m_featureIds);
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CProductLicensingInfoGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	//FeatureTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

	//if (m_packageCollectionObserver.IsModelAttached()){
	//	OnFeaturePackageCollectionUpdate();
	//}

	//connect(this, &CFeaturePackageGuiComp::EmitFeatureTreeItemChanged, this, &CFeaturePackageGuiComp::OnFeatureTreeItemChanged, Qt::QueuedConnection);
}


void CProductLicensingInfoGuiComp::OnGuiDestroyed()
{
	//disconnect(this, &CFeaturePackageGuiComp::EmitFeatureTreeItemChanged, this, &CFeaturePackageGuiComp::OnFeatureTreeItemChanged);

	BaseClass::OnGuiDestroyed();
}


void CProductLicensingInfoGuiComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();
}


// private methods

void CProductLicensingInfoGuiComp::OnFeaturePackageCollectionUpdate()
{
	imtbase::IObjectCollection* collectionPtr = m_featurePackageCollectionObserver.GetObservedObject();

	BuildDependencyMap(*collectionPtr);

	imtbase::ICollectionInfo::Ids packageIds = collectionPtr->GetElementIds();

	m_packages.clear();

	for (const QByteArray& packageId : packageIds){
		imtbase::IObjectCollection::DataPtr packageDataPtr;
		if (collectionPtr->GetObjectData(packageId, packageDataPtr)){
			imtlic::IFeatureInfoProvider* packagePtr = dynamic_cast<imtlic::IFeatureInfoProvider*>(packageDataPtr.GetPtr());
			if (packagePtr != nullptr){
				FeatureDescriptionList featureDescriptionList;

				imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureCollectionId : featureCollectionIds){
					const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);
					if (featureInfoPtr != nullptr){
						FeatureDescription featureDescription;

						featureDescription.id = featureInfoPtr->GetFeatureId();
						featureDescription.name = featureInfoPtr->GetFeatureName();
						featureDescription.description = packagePtr->GetFeatureList().GetElementInfo(featureCollectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

						featureDescriptionList.append(featureDescription);
					}
				}

				m_packages[packageId].features = featureDescriptionList;
				m_packages[packageId].name = collectionPtr->GetElementInfo(packageId, imtbase::ICollectionInfo::EIT_NAME).toString();
			}
		}
	}
	
	EnumerateMissingFeatures();
	UpdateFeatureTree();
	UpdateFeatureTreeCheckStates();
	DoUpdateModel();
}


void CProductLicensingInfoGuiComp::OnLicenseSelectionChanged()
{
	m_selectedLicenseId = "NO_SELECTION";
	m_featureIds.clear();

	imtbase::IMultiSelection::Ids licenseIds = m_licenseSelectionObserver.GetObservedObject()->GetSelectedIds();

	if (licenseIds.count() == 1){
		imtlic::IProductLicensingInfo* productLicensingInfo = GetObservedObject();
		if (productLicensingInfo != nullptr){
			const imtlic::ILicenseInfo* licenseInfo = productLicensingInfo->GetLicenseInfo(licenseIds.first());
			if (licenseInfo != nullptr){
				m_selectedLicenseId = licenseInfo->GetLicenseId();
				m_featureIds = licenseInfo->GetFeatures();
			}
		}
	}

	EnumerateMissingFeatures();
	UpdateFeatureTree();
	UpdateFeatureTreeCheckStates();
}


void CProductLicensingInfoGuiComp::EnumerateMissingFeatures()
{
	QByteArrayList allFeatureIds;
	
	QByteArrayList packageIds = m_packages.keys();
	for (const QByteArray& packageId : packageIds){
		FeatureDescriptionList featureDescriptionList = m_packages[packageId].features;
		for (const FeatureDescription&  featureDescription : featureDescriptionList){
			allFeatureIds.append(featureDescription.id);
		}
	}
	
	m_missingFeatureIds.clear();
	
	for (const QByteArray& featureId : m_featureIds){
		if (!allFeatureIds.contains(featureId)){
			m_missingFeatureIds.push_back(featureId);
		}
	}
}


void CProductLicensingInfoGuiComp::UpdateFeatureTree()
{
	QSignalBlocker blocker(Features);

	Features->clear();

	if (m_selectedLicenseId == "NO_SELECTION"){
		return;
	}

	QByteArrayList packageIds = m_packages.keys();
	for (const QByteArray& packageId : packageIds){
		QTreeWidgetItem* packageItemPtr = new QTreeWidgetItem({m_packages[packageId].name});
		Features->addTopLevelItem(packageItemPtr);
		packageItemPtr->setData(0, DR_ITEM_ID, packageId);
		packageItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);

		FeatureDescriptionList featureDescriptionList = m_packages[packageId].features;
		for (const FeatureDescription& featureDescription : featureDescriptionList){
			if (featureDescription.id == m_selectedLicenseId){
				continue;
			}

			QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem({featureDescription.name});
			packageItemPtr->addChild(featureItemPtr);
			featureItemPtr->setData(0, DR_ITEM_ID, featureDescription.id);
			featureItemPtr->setData(0, DR_ITEM_TYPE, IT_FEATURE);
		}

		if (packageItemPtr->childCount() == 0){
			Features->takeTopLevelItem(Features->indexOfTopLevelItem(packageItemPtr));
			delete packageItemPtr;
		}
	}

	if (!m_missingFeatureIds.isEmpty()){
		QTreeWidgetItem* packageItemPtr = new QTreeWidgetItem({QObject::tr("Missing features")});
		Features->addTopLevelItem(packageItemPtr);
		packageItemPtr->setData(0, DR_ITEM_ID, "MISSING_FEATURES");
		packageItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);
		packageItemPtr->setForeground(0, QBrush(Qt::red));

		for (const QByteArray& featureId : m_missingFeatureIds){
			QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem({QObject::tr("ID: %1").arg(QString(featureId))});
			packageItemPtr->addChild(featureItemPtr);
			featureItemPtr->setData(0, DR_ITEM_ID, featureId);
			featureItemPtr->setData(0, DR_ITEM_TYPE, IT_FEATURE);
			featureItemPtr->setForeground(0, QBrush(Qt::red));
		}
	}

	Features->expandAll();
}


void CProductLicensingInfoGuiComp::UpdateFeatureTreeCheckStates()
{
	QSignalBlocker blocker(Features);
	
	if (m_selectedLicenseId == "NO_SELECTION"){
		return;
	}

	QByteArrayList packageIds = m_packages.keys();
	for (const QByteArray& packageId : packageIds){
		FeatureDescriptionList featureDescriptionList = m_packages[packageId].features;
		for (const FeatureDescription& featureDescription : featureDescriptionList){
			QTreeWidgetItem* featureItemPtr = GetItem(featureDescription.id);
			bool isFeaturePresent = m_featureIds.contains(featureDescription.id);
			featureItemPtr->setCheckState(0, isFeaturePresent ? Qt::Checked : Qt::Unchecked);
		}
	}
	
	for (const QByteArray& packageId : packageIds){
		bool isAllFeaturesChecked = true;
		bool isAllFeaturesUnchecked = true;
	
		FeatureDescriptionList featureDescriptionList = m_packages[packageId].features;
		for (const FeatureDescription& featureDescription : featureDescriptionList){
			QTreeWidgetItem* featureItemPtr = GetItem(featureDescription.id);
			if (featureItemPtr->checkState(0) == Qt::Checked){
				isAllFeaturesUnchecked = false;
			}
			else if (featureItemPtr->checkState(0) == Qt::Unchecked){
				isAllFeaturesChecked = false;
			}
		}
	
		QTreeWidgetItem* packageItemPtr = GetItem(packageId);
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
	
	if (!m_missingFeatureIds.isEmpty()){
		GetItem("MISSING_FEATURES")->setCheckState(0, Qt::Checked);
		for (const QByteArray& featureId : m_missingFeatureIds){
			GetItem(featureId)->setCheckState(0, Qt::Checked);
		}
	}
}


QTreeWidgetItem* CProductLicensingInfoGuiComp::GetItem(const QByteArray& itemId)
{
	int packageCount = Features->topLevelItemCount();
	for (int packageIndex = 0; packageIndex < packageCount; packageIndex++){
		QTreeWidgetItem* packageItemPtr = Features->topLevelItem(packageIndex);
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


void CProductLicensingInfoGuiComp::BuildDependencyMap(const imtbase::IObjectCollection& packageCollection)
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


bool CProductLicensingInfoGuiComp::HasDependency(const DependencyMap& dependencyMap, const QByteArray& fromFeatureId, const QByteArray& toFeatureId)
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


void CProductLicensingInfoGuiComp::ActivateDependencies(const QByteArrayList& featureIds)
{

}


// private slots

void CProductLicensingInfoGuiComp::on_Features_itemChanged(QTreeWidgetItem *item, int column)
{
	if (item->data(0, DR_ITEM_TYPE) == IT_PACKAGE){
		Qt::CheckState state = item->checkState(0);

		for (int i = 0; i < item->childCount(); i++){
			QTreeWidgetItem* featureItemPtr = item->child(i);
			QByteArray featureId = featureItemPtr->data(0, DR_ITEM_ID).toByteArray();

			if (state == Qt::Checked){
				if (!m_featureIds.contains(featureId)){
					m_featureIds.append(featureId);
				}
			}
			else{
				if (m_featureIds.contains(featureId)){
					m_featureIds.removeOne(featureId);
				}
			}
		}
	}
	else{
		QByteArray featureId = item->data(0, DR_ITEM_ID).toByteArray();

		if (item->checkState(0) == Qt::Checked){
			if (!m_featureIds.contains(featureId)){
				m_featureIds.append(featureId);
			}
		}
		else{
			if (m_featureIds.contains(featureId)){
				m_featureIds.removeOne(featureId);
			}
		}
	}

	UpdateFeatureTreeCheckStates();
	DoUpdateModel();
}


// public methods of the embedded class FeaturePackageCollectionObserver

CProductLicensingInfoGuiComp::FeaturePackageCollectionObserver::FeaturePackageCollectionObserver(CProductLicensingInfoGuiComp& parent)
	:m_parent(parent)
{
}


// reimplemented (imod::CSingleModelObserverBase)

void CProductLicensingInfoGuiComp::FeaturePackageCollectionObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	m_parent.OnFeaturePackageCollectionUpdate();
}


// public methods of the embedded class FeatureSelectionObserver

CProductLicensingInfoGuiComp::LicenseSelectionObserver::LicenseSelectionObserver(CProductLicensingInfoGuiComp& parent)
	:m_parent(parent)
{
}


// reimplemented (imod::CSingleModelObserverBase)

void CProductLicensingInfoGuiComp::LicenseSelectionObserver::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.OnLicenseSelectionChanged();
}


} // namespace imtlicgui



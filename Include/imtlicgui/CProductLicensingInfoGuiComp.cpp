#include <imtlicgui/CProductLicensingInfoGuiComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/IFeatureDependenciesProvider.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IFeatureInfoProvider.h>
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

	m_selectedFeatures.clear();
	const imtlic::ILicenseInfo* licenseInfoPtr = productLicensingInfoPtr->GetLicenseInfo(m_selectedLicenseId);
	if (licenseInfoPtr != nullptr){
		m_selectedFeatures = licenseInfoPtr->GetFeatures();
	}	

	ProcessChanges();
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
		licenseInfoPtr->SetFeatures(m_selectedFeatures);
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CProductLicensingInfoGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	m_featureTreeWidget = Features;

	Features->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

	connect(this, &CProductLicensingInfoGuiComp::EmitFeatureTreeItemChanged, this, &CProductLicensingInfoGuiComp::OnFeatureTreeItemChanged, Qt::QueuedConnection);
}


void CProductLicensingInfoGuiComp::OnGuiDestroyed()
{
	disconnect(this, &CProductLicensingInfoGuiComp::EmitFeatureTreeItemChanged, this, &CProductLicensingInfoGuiComp::OnFeatureTreeItemChanged);

	BaseClass::OnGuiDestroyed();
}


// private methods

void CProductLicensingInfoGuiComp::OnFeaturePackageCollectionUpdate()
{
	imtbase::IObjectCollection* collectionPtr = m_featurePackageCollectionObserver.GetObservedObject();
	imtbase::ICollectionInfo::Ids packageIds = collectionPtr->GetElementIds();

	BuildDependencyMap(*collectionPtr);

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

						featureDescriptionList.append(featureDescription);
					}
				}

				m_packages[packageId].features = featureDescriptionList;
				m_packages[packageId].name = collectionPtr->GetElementInfo(packageId, imtbase::ICollectionInfo::EIT_NAME).toString();
			}
		}
	}
	
	ProcessChanges();
}


void CProductLicensingInfoGuiComp::OnLicenseSelectionChanged()
{
	m_selectedLicenseId.clear();
	m_selectedFeatures.clear();
	m_isFeatureTreeUpdateEnabled = false;

	imtbase::IMultiSelection::Ids licenseIds = m_licenseSelectionObserver.GetObservedObject()->GetSelectedIds();

	if (licenseIds.count() == 1){
		imtlic::IProductLicensingInfo* productLicensingInfo = GetObservedObject();
		if (productLicensingInfo != nullptr){
			const imtlic::ILicenseInfo* licenseInfo = productLicensingInfo->GetLicenseInfo(licenseIds.first());
			if (licenseInfo != nullptr){
				m_selectedLicenseId = licenseInfo->GetLicenseId();
				m_selectedFeatures = licenseInfo->GetFeatures();
				m_isFeatureTreeUpdateEnabled = true;
			}
		}
	}

	ProcessChanges();
}


void CProductLicensingInfoGuiComp::EnumerateDependencies(const QByteArrayList& featureIds)
{
	QByteArrayList nextIdsForEnumeration;

	for (const QByteArray& featureId : featureIds){
		if (m_featureDependencyMap.contains(featureId)){
			QByteArrayList dependencies = m_featureDependencyMap[featureId];

			for (const QByteArray& dependency : dependencies){
				if (!m_selectedFeatures.contains(dependency)){
					m_selectedFeatures.append(dependency);
					nextIdsForEnumeration.append(dependency);
				}
			}
		}
	}

	if (!nextIdsForEnumeration.isEmpty()){
		EnumerateDependencies(nextIdsForEnumeration);
	}
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
	
	m_missingFeatures.clear();
	
	for (const QByteArray& featureId : m_selectedFeatures){
		if (!allFeatureIds.contains(featureId)){
			m_missingFeatures.push_back(featureId);
		}
	}
}


void CProductLicensingInfoGuiComp::UpdateFeatureTreeItemEnableStates()
{
	QSignalBlocker blocker(Features);

	for (const QByteArray& fromId : m_selectedFeatures){
		for (const QByteArray& toId : m_selectedFeatures){
			if (HasDependency(m_featureDependencyMap, toId, fromId)){
				QTreeWidgetItem* featureItemPtr = GetItem(fromId);
				if (featureItemPtr != nullptr){
					featureItemPtr->setFlags(featureItemPtr->flags() & (~Qt::ItemIsEnabled));
				}
			}
		}
	}
}


void CProductLicensingInfoGuiComp::ProcessChanges()
{
	EnumerateDependencies(m_selectedFeatures);
	EnumerateMissingFeatures();
	UpdateFeatureTree();
	UpdateFeatureTreeCheckStates();
	UpdateFeatureTreeItemEnableStates();
	DoUpdateModel();
}


void CProductLicensingInfoGuiComp::BuildDependencyMap(const imtbase::IObjectCollection& packageCollection)
{
	m_featureDependencyMap.clear();

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

						m_featureDependencyMap[featureId] = dependenciesProvider->GetFeatureDependencies(featureId);
					}
				}
			}
		}
	}
}


bool CProductLicensingInfoGuiComp::HasDependency(const FeatureDependencyMap& dependencyMap, const QByteArray& fromFeatureId, const QByteArray& toFeatureId)
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


// private slots

void CProductLicensingInfoGuiComp::OnFeatureTreeItemChanged()
{
	ProcessChanges();
}


void CProductLicensingInfoGuiComp::on_Features_itemChanged(QTreeWidgetItem *item, int column)
{
	if (item->data(0, DR_ITEM_TYPE) == IT_PACKAGE){
		Qt::CheckState state = item->checkState(0);

		for (int i = 0; i < item->childCount(); i++){
			QTreeWidgetItem* featureItemPtr = item->child(i);
			QByteArray featureId = featureItemPtr->data(0, DR_ITEM_ID).toByteArray();

			if (state == Qt::Checked){
				if (!m_selectedFeatures.contains(featureId)){
					m_selectedFeatures.append(featureId);
				}
			}
			else{
				if (m_selectedFeatures.contains(featureId)){
					m_selectedFeatures.removeOne(featureId);
				}
			}
		}
	}
	else{
		QByteArray featureId = item->data(0, DR_ITEM_ID).toByteArray();

		if (item->checkState(0) == Qt::Checked){
			if (!m_selectedFeatures.contains(featureId)){
				m_selectedFeatures.append(featureId);
			}
		}
		else{
			if (m_selectedFeatures.contains(featureId)){
				m_selectedFeatures.removeOne(featureId);
			}
		}
	}

	Q_EMIT EmitFeatureTreeItemChanged();
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



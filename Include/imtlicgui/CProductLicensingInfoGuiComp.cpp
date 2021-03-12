#include <imtlicgui/CProductLicensingInfoGuiComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/IFeatureDependenciesProvider.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtlic/ILicenseInfo.h>
#include <imtlic/CFeaturePackage.h>


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


// reimplemented (imtlicgui::IFeatureItemStateHandler)

void CProductLicensingInfoGuiComp::OnItemStateChanged(const QByteArray& itemId, bool isChecked)
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

	const imtlic::ILicenseInfo* licenseInfoPtr = productLicensingInfoPtr->GetLicenseInfo(m_selectedLicenseId);
	if (licenseInfoPtr != nullptr){
		m_selectedFeatures = licenseInfoPtr->GetFeatures();
	}

	UpdateFeaturePackageCollectionProxy();
}


void CProductLicensingInfoGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		imod::IModel* licensingInfoModelPtr = GetObservedModel();
		Q_ASSERT(licensingInfoModelPtr != nullptr);

		imod::IModel* featurePackageCollectionModelPtr = const_cast<imod::IModel*>(
					dynamic_cast<const imod::IModel*>(m_featurePackageCollectionCompPtr.GetPtr()));
		if (featurePackageCollectionModelPtr != nullptr){
			m_featurePackageCollectionObserver.RegisterObject(m_featurePackageCollectionCompPtr.GetPtr(), &CProductLicensingInfoGuiComp::OnFeaturePackageCollectionUpdate);
		}

		if (licensingInfoModelPtr->AttachObserver(m_objectCollectionObserverCompPtr.GetPtr())){
			m_objectCollectionViewCompPtr->CreateGui(Licenses);
		}

		imod::IModel* licenseSelectionModelPtr = dynamic_cast<imod::IModel*>(m_objectCollectionViewCompPtr.GetPtr());
		if (licenseSelectionModelPtr != nullptr){
			licenseSelectionModelPtr->AttachObserver(&m_licenseSelectionObserver);
		}
	}

	if (m_featureTreeCompPtr.IsValid() && m_featureTreeObserverCompPtr.IsValid()){
		m_featurePackageCollectionProxy.AttachObserver(m_featureTreeObserverCompPtr.GetPtr());
	}
}


void CProductLicensingInfoGuiComp::OnGuiModelDetached()
{
	m_featurePackageCollectionProxy.DetachAllObservers();

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		imod::IModel* licensingInfoModelPtr = GetObservedModel();
		Q_ASSERT(licensingInfoModelPtr != nullptr);

		m_featurePackageCollectionObserver.UnregisterAllObjects();

		if (licensingInfoModelPtr->IsAttached(m_objectCollectionObserverCompPtr.GetPtr())){
			licensingInfoModelPtr->DetachObserver(m_objectCollectionObserverCompPtr.GetPtr());
		}

		imod::IModel* licenseSelectionModelPtr = dynamic_cast<imod::IModel*>(m_objectCollectionViewCompPtr.GetPtr());
		if (licenseSelectionModelPtr != nullptr && licenseSelectionModelPtr->IsAttached(&m_licenseSelectionObserver)){
			licenseSelectionModelPtr->DetachObserver(&m_licenseSelectionObserver);
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

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		m_objectCollectionViewCompPtr->CreateGui(Licenses);
	}

	if (m_featureTreeCompPtr.IsValid() && m_featureTreeObserverCompPtr.IsValid()){
		m_featureTreeCompPtr->CreateGui(Features);
	}
}


void CProductLicensingInfoGuiComp::OnGuiDestroyed()
{
	if (m_featureTreeCompPtr.IsValid() && m_featureTreeCompPtr->IsGuiCreated()){
		m_featureTreeCompPtr->DestroyGui();
	}

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionViewCompPtr->IsGuiCreated()){
		m_objectCollectionViewCompPtr->DestroyGui();
	}

	BaseClass::OnGuiDestroyed();
}


// private methods

void CProductLicensingInfoGuiComp::OnFeaturePackageCollectionUpdate(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::IObjectCollection* collectionPtr)
{
	imtbase::ICollectionInfo::Ids packageCollectionIds = collectionPtr->GetElementIds();

	BuildDependencyMap(*collectionPtr);

	m_featurePackageCollection.ResetData();

	for (const QByteArray& packageCollectionId : packageCollectionIds){
		imtbase::IObjectCollection::DataPtr packageDataPtr;
		if (collectionPtr->GetObjectData(packageCollectionId, packageDataPtr)){
			imtlic::IFeaturePackage* packagePtr = dynamic_cast<imtlic::IFeaturePackage*>(packageDataPtr.GetPtr());
			if (packagePtr != nullptr){
				// TODO: GetPackageId->GetPackageId();

				m_featurePackageCollection.InsertNewObject(
							"FeaturePackage",
							collectionPtr->GetElementInfo(packageCollectionId, imtbase::ICollectionInfo::EIT_NAME).toString(),
							collectionPtr->GetElementInfo(packageCollectionId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString(),
							packagePtr,
							packageCollectionId);
			}
		}
	}

	UpdateFeaturePackageCollectionProxy();
}


void CProductLicensingInfoGuiComp::OnLicenseSelectionChanged()
{
	m_selectedLicenseId.clear();
	m_selectedFeatures.clear();

	imtbase::IMultiSelection::Ids licenseIds = m_licenseSelectionObserver.GetObservedObject()->GetSelectedIds();

	if (licenseIds.count() == 1){
		imtlic::IProductLicensingInfo* productLicensingInfo = GetObservedObject();
		if (productLicensingInfo != nullptr){
			const imtlic::ILicenseInfo* licenseInfo = productLicensingInfo->GetLicenseInfo(licenseIds.first());
			if (licenseInfo != nullptr){
				m_selectedLicenseId = licenseInfo->GetLicenseId();
				m_selectedFeatures = licenseInfo->GetFeatures();
			}
		}
	}

	UpdateFeaturePackageCollectionProxy();
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

	imtbase::ICollectionInfo::Ids packageCollectionIds = m_featurePackageCollection.GetElementIds();

	for (const QByteArray& packageCollectionId : packageCollectionIds){
		const istd::IChangeable* constObjectPtr = m_featurePackageCollection.GetObjectPtr(packageCollectionId);
		istd::IChangeable* objectPtr = const_cast<istd::IChangeable*>(constObjectPtr);
		imtlic::IFeaturePackage* packagePtr = dynamic_cast<imtlic::IFeaturePackage*>(objectPtr);

		if (packagePtr != nullptr){
			imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();

			for (const QByteArray& featureCollectionId : featureCollectionIds){
				const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);
				if (featureInfoPtr != nullptr){
					allFeatureIds.append(featureInfoPtr->GetFeatureId());
				}
			}
		}
	}

	m_missingFeatures.clear();

	for (const QByteArray& featureId : m_selectedFeatures){
		if (!allFeatureIds.contains(featureId)){
			m_missingFeatures.push_back(featureId);
		}
	}
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


void CProductLicensingInfoGuiComp::UpdateFeaturePackageCollectionProxy()
{
	m_featurePackageCollectionProxy.CopyFrom(m_featurePackageCollection);

	EnumerateMissingFeatures();
	EnumerateDependencies(m_selectedFeatures);

	QByteArrayList selectedFeatures = m_selectedFeatures;

	if (!m_missingFeatures.isEmpty()){
		imtlic::CFeaturePackage missingPackage;
		missingPackage.SetPackageId("MISSING_FEATURES");
		
		for (const QByteArray& featureId : m_missingFeatures){
			imtlic::CFeatureInfo featureInfo;
			featureInfo.SetFeatureId(featureId);
			featureInfo.SetFeatureName(tr("ID: %1").arg(QString(featureId)));
			missingPackage.InsertNewObject(
						"FeatureInfo",
						tr("ID: %1").arg(QString(featureId)),
						"",
						&featureInfo);

			selectedFeatures.append(featureId);
		}

		m_featurePackageCollectionProxy.InsertNewObject(
					"FeaturePackage",
					tr("Missing features"),
					"",
					&missingPackage);
	}

	m_selectedFeaturesModel.SetSelectedIds(selectedFeatures.toVector());

	// Disable dependent features
	QByteArrayList disabledFeatures;
	for (const QByteArray& fromId : m_selectedFeatures){
		for (const QByteArray& toId : m_selectedFeatures){
			if (HasDependency(m_featureDependencyMap, fromId, toId)){
				disabledFeatures.append(toId);
			}
		}
	}

	m_disabledFeaturesModel.SetSelectedIds(disabledFeatures.toVector());
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



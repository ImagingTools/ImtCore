#include <imtlicgui/CProductLicensingInfoGuiComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/IFeatureDependenciesProvider.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtlic/ILicenseInfo.h>
#include <imtlic/CFeaturePackage.h>
#include <imtlic/CFeaturePackageCollection.h>


namespace imtlicgui
{


// public methods

CProductLicensingInfoGuiComp::CProductLicensingInfoGuiComp()
	:m_licenseSelectionObserver(*this)
{
}


// reimplemented (imtlicgui::IFeatureItemStateHandler)

void CProductLicensingInfoGuiComp::OnItemStateChanged(const QByteArray& itemId, bool isChecked)
{
	if (isChecked && !m_selectedFeatureIds.contains(itemId)){
		m_selectedFeatureIds.append(itemId);

		DoUpdateItemTree();
		DoUpdateModel();
	}

	if (!isChecked && m_selectedFeatureIds.contains(itemId)){
		m_selectedFeatureIds.removeOne(itemId);

		DoUpdateItemTree();
		DoUpdateModel();
	}
}


// protected methods

// reimplemented (imtlicgui::TFeatureTreeModelCompWrap)

void CProductLicensingInfoGuiComp::UpdateItemTree(IItemTree* itemTreePtr)
{
	//QByteArrayList selectedFeatures;
	//QByteArrayList disabledFeatures;

	//{
	//	istd::CChangeGroup changeGroup(featureTreeModelPtr);

	//	featureTreeModelPtr->ResetData();

	//	if (!m_selectedLicenseId.isEmpty()){
	//		featureTreeModelPtr->CopyFrom(*GetObjectCollection());

	//		EnumerateDependencies(m_selectedFeatureIds);
	//		selectedFeatures = m_selectedFeatureIds;

	//		EnumerateMissingFeatures();
	//		if (!m_missingFeatureIds.isEmpty()){
	//			imtlic::CFeaturePackage missingPackage;
	//			missingPackage.SetPackageId("MISSING_FEATURES");

	//			for (const QByteArray& featureId : m_missingFeatureIds){
	//				imtlic::CFeatureInfo featureInfo;
	//				featureInfo.SetFeatureId(featureId);
	//				featureInfo.SetFeatureName(tr("ID: %1").arg(QString(featureId)));
	//				missingPackage.InsertNewObject(
	//					"FeatureInfo",
	//					tr("ID: %1").arg(QString(featureId)),
	//					"",
	//					&featureInfo);

	//				selectedFeatures.append(featureId);
	//			}

	//			featureTreeModelPtr->InsertNewObject(
	//				"FeaturePackage",
	//				tr("Missing features"),
	//				"",
	//				&missingPackage);
	//		}

	//		const QMap<QByteArray, QByteArrayList> dependencyMap =
	//					imtlic::CFeaturePackageCollectionUtility::GetDependencies(*GetObjectCollection());

	//		// Disable dependent features
	//		for (const QByteArray& fromId : m_selectedFeatureIds){
	//			for (const QByteArray& toId : m_selectedFeatureIds){
	//				if (imtlic::CFeaturePackageCollectionUtility::HasDependency(dependencyMap, fromId, toId)){
	//					if (!disabledFeatures.contains(toId)){
	//						if (!m_missingFeatureIds.contains(toId)){
	//							disabledFeatures.append(toId);
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	//selectedFeaturesModelPtr->SetSelectedIds(selectedFeatures.toVector());
	//disabledFeaturesModelPtr->SetSelectedIds(disabledFeatures.toVector());
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CProductLicensingInfoGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::IProductLicensingInfo* productLicensingInfoPtr = GetObservedObject();
	Q_ASSERT(productLicensingInfoPtr != nullptr);

	const imtlic::ILicenseInfo* licenseInfoPtr = productLicensingInfoPtr->GetLicenseInfo(m_selectedLicenseId);
	if (licenseInfoPtr != nullptr){
		m_selectedFeatureIds = licenseInfoPtr->GetFeatures();
	}

	DoUpdateItemTree();
}


void CProductLicensingInfoGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		GetObservedModel()->AttachObserver(m_objectCollectionObserverCompPtr.GetPtr());

		m_licenseSelectionObserver.RegisterObject(
					dynamic_cast<imtbase::IMultiSelection*>(m_objectCollectionViewCompPtr.GetPtr()),
					&CProductLicensingInfoGuiComp::OnLicenseSelectionChanged);
	}
}


void CProductLicensingInfoGuiComp::OnGuiModelDetached()
{
	m_licenseSelectionObserver.UnregisterAllObjects();

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		GetObservedModel()->DetachObserver(m_objectCollectionObserverCompPtr.GetPtr());
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
		licenseInfoPtr->SetFeatures(m_selectedFeatureIds);
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

void CProductLicensingInfoGuiComp::OnLicenseSelectionChanged(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::IMultiSelection* selectionPtr)
{
	m_selectedLicenseId.clear();
	m_selectedFeatureIds.clear();

	imtbase::IMultiSelection::Ids licenseIds = selectionPtr->GetSelectedIds();

	if (licenseIds.count() == 1){
		imtlic::IProductLicensingInfo* productLicensingInfo = GetObservedObject();
		if (productLicensingInfo != nullptr){
			const imtlic::ILicenseInfo* licenseInfo = productLicensingInfo->GetLicenseInfo(licenseIds.first());
			if (licenseInfo != nullptr){
				m_selectedLicenseId = licenseInfo->GetLicenseId();
				m_selectedFeatureIds = licenseInfo->GetFeatures();
			}
		}
	}

	DoUpdateItemTree();
}


void CProductLicensingInfoGuiComp::EnumerateDependencies(const QByteArrayList& featureIds)
{
	QByteArrayList nextIdsForEnumeration;

	const QMap<QByteArray, QByteArrayList> dependencyMap =
				imtlic::CFeaturePackageCollectionUtility::GetDependencies(*GetObjectCollection());

	//QByteArrayList allFeatureIds = imtlic::CFeaturePackageCollectionUtility::GetAllFeatureIds(*GetObjectCollection());

	for (const QByteArray& featureId : featureIds){
		if (dependencyMap.contains(featureId)){
			QByteArrayList dependencies = dependencyMap.value(featureId);

			for (const QByteArray& dependency : dependencies){
				if (!m_selectedFeatureIds.contains(dependency)){
					m_selectedFeatureIds.append(dependency);
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
	m_missingFeatureIds.clear();

	QByteArrayList allFeatureIds;

	const imtbase::IObjectCollection* collectionPtr = GetObjectCollection();
	Q_ASSERT(collectionPtr != nullptr);

	imtbase::ICollectionInfo::Ids packageCollectionIds = collectionPtr->GetElementIds();

	for (const QByteArray& packageCollectionId : packageCollectionIds){
		const istd::IChangeable* constObjectPtr = collectionPtr->GetObjectPtr(packageCollectionId);
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

	for (const QByteArray& featureId : m_selectedFeatureIds){
		if (!allFeatureIds.contains(featureId)){
			m_missingFeatureIds.push_back(featureId);
		}
	}
}


} // namespace imtlicgui



#include <imtlicgui/CFeaturePackageGuiComp.h>


// Qt includes
#include <QtWidgets/QMessageBox>

// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/IFeatureDependenciesManager.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/CFeaturePackage.h>


namespace imtlicgui
{


// public methods

CFeaturePackageGuiComp::CFeaturePackageGuiComp()
	:m_featurePackageProxy(*this),
	m_featureSelectionObserver(*this)
{
}


// reimplemented (imtlicgui::IFeatureItemStateHandler)

void CFeaturePackageGuiComp::OnItemStateChanged(const QByteArray& itemId, bool isChecked)
{
	imtlic::CFeaturePackageCollectionUtility::FeatureDependencyMap collectionDependencyMap =
				imtlic::CFeaturePackageCollectionUtility::GetDependencies(*GetObjectCollection());

	for (const QByteArray& id : m_dependencyMap.keys()){
		collectionDependencyMap[id] = m_dependencyMap[id];
	}

	if (isChecked){
		if (!m_dependencyMap[m_selectedFeatureId].contains(itemId)){
			if (!imtlic::CFeaturePackageCollectionUtility::HasDependency(collectionDependencyMap, itemId, m_selectedFeatureId)){
				m_dependencyMap[m_selectedFeatureId].append(itemId);
			}
			else{
				QMessageBox::warning(
					GetQtWidget(),
					QObject::tr("Warning"),
					QObject::tr("Feature with ID '%1' depends on feature with ID '%2'")
					.arg(QString(itemId))
					.arg(QString(m_selectedFeatureId)));
			}
		}
	}
	else{
		if (m_dependencyMap[m_selectedFeatureId].contains(itemId)){
			m_dependencyMap[m_selectedFeatureId].removeOne(itemId);
		}
	}

	DoUpdateFeatureTreeModels();
	DoUpdateModel();
}


// protected methods

// reimplemented (imtlicgui::TFeatureTreeModelCompWrap)

void CFeaturePackageGuiComp::UpdateFeatureTreeModels(
			imtbase::IObjectCollection* featureTreeModelPtr,
			imtbase::IMultiSelection* selectedFeaturesModelPtr,
			imtbase::IMultiSelection* disabledFeaturesModelPtr)
{
	QByteArrayList selectedFeatures;
	QSet<QByteArray> disabledFeatures;

	{
		istd::CChangeGroup changeGroup(featureTreeModelPtr);

		featureTreeModelPtr->ResetData();
		
		if (!m_selectedFeatureId.isEmpty()){
			featureTreeModelPtr->CopyFrom(*GetObjectCollection());

			QByteArrayList allFeatureIds= imtlic::CFeaturePackageCollectionUtility::GetAllFeatureIds(*featureTreeModelPtr);

			// Unsaved features
			imtlic::CFeaturePackage unsavedFeaturesPackage;

			imtlic::IFeatureInfoProvider* packagePtr = dynamic_cast<imtlic::IFeatureInfoProvider*>(GetObservedObject());
			if (packagePtr != nullptr){
				imtbase::ICollectionInfo::Ids featureIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureId : featureIds){
					const imtlic::IFeatureInfo* featurePtr = dynamic_cast<const imtlic::IFeatureInfo*>(packagePtr->GetFeatureInfo(featureId));
					if (featurePtr != nullptr){
						if (!allFeatureIds.contains(featurePtr->GetFeatureId())){
							imtlic::CFeatureInfo featureInfo;
							featureInfo.SetFeatureId(featurePtr->GetFeatureId());
							featureInfo.SetFeatureName(featurePtr->GetFeatureName());
							unsavedFeaturesPackage.InsertNewObject(
										"FeatureInfo",
										featurePtr->GetFeatureName(),
										"",
										&featureInfo);
						}
					}
				}
			}

			if (!unsavedFeaturesPackage.GetFeatureList().GetElementIds().isEmpty()){
				featureTreeModelPtr->InsertNewObject(
					"FeaturePackage",
					tr("Unsaved Features"),
					"",
					&unsavedFeaturesPackage);
			}

			// Remove m_selectedFeatureId feature
			imtbase::ICollectionInfo::Ids packageIds = featureTreeModelPtr->GetElementIds();
			for (const QByteArray& packageId : packageIds){
				imtbase::IObjectCollection* packagePtr = dynamic_cast<imtbase::IObjectCollection*>(
							const_cast<istd::IChangeable*>(featureTreeModelPtr->GetObjectPtr(packageId)));

				if (packagePtr != nullptr){
					imtbase::ICollectionInfo::Ids featureIds = packagePtr->GetElementIds();
					for (const QByteArray& featureId : featureIds){
						const imtlic::IFeatureInfo* featurePtr = dynamic_cast<const imtlic::IFeatureInfo*>(packagePtr->GetObjectPtr(featureId));
						if (featurePtr != nullptr){
							if (featurePtr->GetFeatureId() == m_selectedFeatureId){
								packagePtr->RemoveObject(featureId);
								continue;
							}
						}
					}
				}
			}

			// Disabled features
			//allFeatureIds = imtlic::CFeaturePackageCollectionUtility::GetAllFeatureIds(*featureTreeModelPtr);
			//const QMap<QByteArray, QByteArrayList> dependencyMap =
			//			imtlic::CFeaturePackageCollectionUtility::GetDependencies(*GetObjectCollection());

			//for (const QByteArray& fromId : allFeatureIds){
			//	for (const QByteArray& toId : allFeatureIds){
			//		if (imtlic::CFeaturePackageCollectionUtility::HasDependency(dependencyMap, fromId, toId)){
			//			if (!disabledFeatures.contains(toId)){
			//				disabledFeatures.insert(toId);
			//			}
			//		}

			//		if (imtlic::CFeaturePackageCollectionUtility::HasDependency(dependencyMap, m_selectedFeatureId, toId)){
			//			disabledFeatures.insert(toId);
			//		}
			//	}
			//}

			// Selected features
			selectedFeatures = m_dependencyMap[m_selectedFeatureId];

			// Missing features group
			allFeatureIds = imtlic::CFeaturePackageCollectionUtility::GetAllFeatureIds(*featureTreeModelPtr);

			imtlic::CFeaturePackage missingFeaturesPackage;

			if (packagePtr != nullptr){
				imtbase::ICollectionInfo::Ids featureIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureId : featureIds){
					const imtlic::IFeatureInfo* featurePtr = dynamic_cast<const imtlic::IFeatureInfo*>(packagePtr->GetFeatureInfo(featureId));
					if (featurePtr != nullptr){
						if (!allFeatureIds.contains(featurePtr->GetFeatureId()) && featurePtr->GetFeatureId() != m_selectedFeatureId){
							imtlic::CFeatureInfo featureInfo;
							featureInfo.SetFeatureId(featurePtr->GetFeatureId());
							featureInfo.SetFeatureName(tr("ID: %1").arg(QString(featurePtr->GetFeatureId())));
							missingFeaturesPackage.InsertNewObject(
										"FeatureInfo",
										tr("ID: %1").arg(QString(featurePtr->GetFeatureId())),
										"",
										&featureInfo);

							//disabledFeatures.append(featurePtr->GetFeatureId());
						}
					}
				}
			}

			if (!missingFeaturesPackage.GetFeatureList().GetElementIds().isEmpty()){
				missingFeaturesPackage.SetPackageId("MISSING_FEATURES");

				featureTreeModelPtr->InsertNewObject(
							"FeaturePackage",
							tr("Missing Features"),
							"",
							&missingFeaturesPackage);
			}
		}
	}

	selectedFeaturesModelPtr->SetSelectedIds(selectedFeatures.toVector());
	disabledFeaturesModelPtr->SetSelectedIds(disabledFeatures.toList().toVector());
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CFeaturePackageGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::IFeaturePackage* packagePtr = GetObservedObject();
	const imtlic::IFeatureDependenciesProvider* dependenciesProviderPtr = packagePtr->GetDependenciesInfoProvider();

	imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();
	for (const QByteArray& featureCollectionId : featureCollectionIds){
		const imtlic::IFeatureInfo* featurePtr = packagePtr->GetFeatureInfo(featureCollectionId);

		if (featurePtr != nullptr && dependenciesProviderPtr != nullptr){
			QByteArray featureId = featurePtr->GetFeatureId();
			m_dependencyMap[featureId] = dependenciesProviderPtr->GetFeatureDependencies(featureId);
		}
	}

	DoUpdateFeatureTreeModels();
}


void CFeaturePackageGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		GetObservedModel()->AttachObserver(m_objectCollectionObserverCompPtr.GetPtr());

		m_featureSelectionObserver.RegisterObject(
					dynamic_cast<imtbase::IMultiSelection*>(m_objectCollectionViewCompPtr.GetPtr()),
					&CFeaturePackageGuiComp::OnFeatureSelectionChanged);
	}
}


void CFeaturePackageGuiComp::OnGuiModelDetached()
{
	m_featureSelectionObserver.UnregisterAllObjects();

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		GetObservedModel()->DetachObserver(m_objectCollectionObserverCompPtr.GetPtr());
	}

	BaseClass::OnGuiModelDetached();
}


void CFeaturePackageGuiComp::UpdateModel() const
{
	if (!m_selectedFeatureId.isEmpty()){
		imtlic::IFeaturePackage* packagePtr = GetObservedObject();

		if (packagePtr != nullptr){
			imtlic::IFeatureDependenciesManager* dependenciesManagerPtr = dynamic_cast<imtlic::IFeatureDependenciesManager*>(
						const_cast<imtlic::IFeatureDependenciesProvider*>(packagePtr->GetDependenciesInfoProvider()));

			if (dependenciesManagerPtr != nullptr){
				dependenciesManagerPtr->SetFeatureDependencies(m_selectedFeatureId, m_dependencyMap[m_selectedFeatureId]);
			}
		}
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CFeaturePackageGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		m_objectCollectionViewCompPtr->CreateGui(Features);
	}

	if (m_featureTreeCompPtr.IsValid() && m_featureTreeObserverCompPtr.IsValid()){
		m_featureTreeCompPtr->CreateGui(Dependencies);
	}
}


void CFeaturePackageGuiComp::OnGuiDestroyed()
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

void CFeaturePackageGuiComp::OnFeatureSelectionChanged(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::IMultiSelection* selectionPtr)
{
	m_selectedFeatureId.clear();

	imtbase::IMultiSelection::Ids featureCollectionIds = selectionPtr->GetSelectedIds();

	if (featureCollectionIds.count() == 1){
		imtlic::IFeaturePackage* packagePtr = GetObservedObject();
		if (packagePtr != nullptr){
			const imtlic::IFeatureInfo* featurePtr = packagePtr->GetFeatureInfo(featureCollectionIds.first());
			if (featurePtr != nullptr){
				m_selectedFeatureId = featurePtr->GetFeatureId();
			}
		}
	}

	DoUpdateFeatureTreeModels();
}


// public methods of the embedded class FeaturePackageProxy

CFeaturePackageGuiComp::FeaturePackageProxy::FeaturePackageProxy(CFeaturePackageGuiComp& parent)
	:m_parent(parent)
{
}


// reimplemented (IFeaturePackage)

QByteArray CFeaturePackageGuiComp::FeaturePackageProxy::GetPackageId() const
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetPackageId();
	}

	return QByteArray();
}


void CFeaturePackageGuiComp::FeaturePackageProxy::SetPackageId(const QByteArray& packageId)
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		featurePackagePtr->SetPackageId(packageId);
	}
}


// reimplemented (imtlic::IFeatureInfoProvider)

const imtbase::IObjectCollection* CFeaturePackageGuiComp::FeaturePackageProxy::GetFeaturePackages() const
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetFeaturePackages();
	}

	return nullptr;
}


const imtbase::ICollectionInfo& CFeaturePackageGuiComp::FeaturePackageProxy::GetFeatureList() const
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetFeatureList();
	}

	return m_collectionInfo;
}


const imtlic::IFeatureInfo* CFeaturePackageGuiComp::FeaturePackageProxy::GetFeatureInfo(const QByteArray& featureId) const
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetFeatureInfo(featureId);
	}

	return nullptr;
}


const imtlic::IFeatureDependenciesProvider* CFeaturePackageGuiComp::FeaturePackageProxy::GetDependenciesInfoProvider() const
{
	imtlic::IFeatureInfoProvider* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetDependenciesInfoProvider();
	}

	return nullptr;
}


const imtbase::ICollectionInfo* CFeaturePackageGuiComp::FeaturePackageProxy::GetParentFeatureInfoProviderList() const
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetParentFeatureInfoProviderList();
	}

	return nullptr;
}


const imtlic::IFeatureInfoProvider* CFeaturePackageGuiComp::FeaturePackageProxy::GetParentFeatureInfoProvider(const QByteArray& parentId) const
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetParentFeatureInfoProvider(parentId);
	}

	return nullptr;
}

// reimplemented (iser::ISerializable)

bool CFeaturePackageGuiComp::FeaturePackageProxy::Serialize(iser::IArchive& archive)
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->Serialize(archive);
	}

	return false;
}


quint32 CFeaturePackageGuiComp::FeaturePackageProxy::GetMinimalVersion(int versionId) const
{
	imtlic::IFeaturePackage* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetMinimalVersion(versionId);
	}

	return iser::ISerializable::GetMinimalVersion(versionId);
}


} // namespace imtlicgui



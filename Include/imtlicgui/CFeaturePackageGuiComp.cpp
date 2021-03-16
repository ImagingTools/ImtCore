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
	//if (isChecked && !m_dependencyMap[m_selectedFeatureId].contains(itemId)){
	//	m_dependencyMap[m_selectedFeatureId].append(itemId);

	//	DoUpdateFeatureTreeModels();
	//	DoUpdateModel();
	//}

	//if (!isChecked && m_dependencyMap[m_selectedFeatureId].contains(itemId)){
	//	m_dependencyMap[m_selectedFeatureId].removeOne(itemId);

	//	DoUpdateFeatureTreeModels();
	//	DoUpdateModel();
	//}
}


// protected methods

// reimplemented (imtlicgui::TFeatureTreeModelCompWrap)

void CFeaturePackageGuiComp::UpdateFeatureTreeModels(
			imtbase::IObjectCollection* featureTreeModelPtr,
			imtbase::IMultiSelection* selectedFeaturesModelPtr,
			imtbase::IMultiSelection* disabledFeaturesModelPtr)
{
	QByteArrayList selectedFeatures;
	QByteArrayList disabledFeatures;

	{
		istd::CChangeGroup changeGroup(featureTreeModelPtr);

		featureTreeModelPtr->ResetData();

		if (!m_selectedFeatureId.isEmpty()){
			featureTreeModelPtr->CopyFrom(*GetObjectCollection());

			selectedFeatures = m_dependencyMap[m_selectedFeatureId];

			//EnumerateMissingFeatures();
			if (!m_missingDependencyIds.isEmpty()){
				imtlic::CFeaturePackage missingPackage;
				missingPackage.SetPackageId("MISSING_FEATURES");

				for (const QByteArray& featureId : m_missingDependencyIds){
					if (featureId == m_selectedFeatureId){
						continue;
					}

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

				featureTreeModelPtr->InsertNewObject(
					"FeaturePackage",
					tr("Missing features"),
					"",
					&missingPackage);
			}

			// Disable dependent features
			for (const QByteArray& fromId : selectedFeatures){
				for (const QByteArray& toId : selectedFeatures){
					if (HasDependency(fromId, toId)){
						if (!disabledFeatures.contains(toId)){
							if (!selectedFeatures.contains(toId)){
								disabledFeatures.append(toId);
							}
						}
					}
				}
			}
		}
	}

	selectedFeaturesModelPtr->SetSelectedIds(selectedFeatures.toVector());
	disabledFeaturesModelPtr->SetSelectedIds(disabledFeatures.toVector());
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CFeaturePackageGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	//m_features.clear();
	//m_dependencies.clear();

	imtlic::IFeatureInfoProvider* featureInfoProviderPtr = GetObservedObject();
	if (featureInfoProviderPtr != nullptr){
		imtbase::ICollectionInfo::Ids ids = featureInfoProviderPtr->GetFeatureList().GetElementIds();

		for (const QByteArray& id : ids){
			const imtlic::IFeatureDependenciesProvider* dependenciesProviderPtr = featureInfoProviderPtr->GetDependenciesInfoProvider();
			const imtlic::IFeatureInfo* featureInfoPtr = featureInfoProviderPtr->GetFeatureInfo(id);

			if (featureInfoPtr != nullptr && dependenciesProviderPtr != nullptr){
				QByteArray featureId = featureInfoPtr->GetFeatureId();
				//m_dependencies[featureId] = dependenciesProviderPtr->GetFeatureDependencies(featureId);

				//FeatureDescription desc;
				//desc.name = featureInfoPtr->GetFeatureName();
				//desc.id = featureId;
				//m_features.append(desc);
			}
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
		imtlic::IFeatureInfoProvider* featureInfoProviderPtr = GetObservedObject();
		if (featureInfoProviderPtr != nullptr){
			imtlic::IFeatureDependenciesManager* dependenciesManagerPtr = dynamic_cast<imtlic::IFeatureDependenciesManager*>(
						const_cast<imtlic::IFeatureDependenciesProvider*>(featureInfoProviderPtr->GetDependenciesInfoProvider()));

			if (dependenciesManagerPtr != nullptr){
				//dependenciesManagerPtr->SetFeatureDependencies(m_selectedFeatureId, m_dependencies[m_selectedFeatureId]);
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

	imtbase::IMultiSelection::Ids featureIds = selectionPtr->GetSelectedIds();

	if (featureIds.count() == 1){
		imtlic::IFeatureInfoProvider* packagePtr = GetObservedObject();
		if (packagePtr != nullptr){
			const imtlic::IFeatureInfo* featurePtr = packagePtr->GetFeatureInfo(featureIds.first());
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



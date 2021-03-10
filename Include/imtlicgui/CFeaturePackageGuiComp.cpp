#include <imtlicgui/CFeaturePackageGuiComp.h>


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

CFeaturePackageGuiComp::CFeaturePackageGuiComp()
	:m_featurePackageProxy(*this),
	m_featureSelectionObserver(*this)
{
	m_selectedFeatureId = "NO_SELECTION";
}


// protected methods

void CFeaturePackageGuiComp::OnFeatureSelectionChanged()
{
	m_selectedFeatureId = "NO_SELECTION";

	imtbase::IMultiSelection::Ids featureIds = m_featureSelectionObserver.GetObservedObject()->GetSelectedIds();
	
	if (featureIds.count() == 1){
		imtlic::IFeatureInfoProvider* packagePtr = GetObservedObject();
		if (packagePtr != nullptr){
			const imtlic::IFeatureInfo* featurePtr = packagePtr->GetFeatureInfo(featureIds.first());
			if (featurePtr != nullptr){
				m_selectedFeatureId = featurePtr->GetFeatureId();
			}
		}
	}

	if (m_isGuiModelInitialized && m_isCollectionRepresentationInitialized){
		EnumerateMissingDependencies();
		UpdateFeatureTree();
		UpdateFeatureTreeCheckStates();
	}
}


// reimplemente (imtlicgui::CFeatureDependencyEditorBase)

void CFeaturePackageGuiComp::UpdateFeaturePackageModel()
{
	DoUpdateModel();
}


void CFeaturePackageGuiComp::FeatureTreeItemChanged()
{
	Q_EMIT EmitFeatureTreeItemChanged();
}


// reimplemented(ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CFeaturePackageGuiComp::GetCommands() const
{
	return nullptr;
}


// reimplemented (iqtgui::CGuiComponentBase)

void CFeaturePackageGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	FeatureTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

	m_treeWidgetPtr = FeatureTree;
	if (m_packageCollectionObserver.IsModelAttached()){
		OnFeaturePackageCollectionUpdate();
	}

	connect(this, &CFeaturePackageGuiComp::EmitFeatureTreeItemChanged, this, &CFeaturePackageGuiComp::OnFeatureTreeItemChanged, Qt::QueuedConnection);
}


void CFeaturePackageGuiComp::OnGuiDestroyed()
{
	disconnect(this, &CFeaturePackageGuiComp::EmitFeatureTreeItemChanged, this, &CFeaturePackageGuiComp::OnFeatureTreeItemChanged);

	BaseClass::OnGuiDestroyed();
}


void CFeaturePackageGuiComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CFeaturePackageGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_isGuiModelInitialized = true;

	m_features.clear();
	m_dependencies.clear();

	imtlic::IFeatureInfoProvider* featureInfoProviderPtr = GetObservedObject();
	if (featureInfoProviderPtr != nullptr){
		imtbase::ICollectionInfo::Ids ids = featureInfoProviderPtr->GetFeatureList().GetElementIds();

		for (const QByteArray& id : ids){
			const imtlic::IFeatureDependenciesProvider* dependenciesProviderPtr = featureInfoProviderPtr->GetDependenciesInfoProvider();
			const imtlic::IFeatureInfo* featureInfoPtr = featureInfoProviderPtr->GetFeatureInfo(id);

			if (featureInfoPtr != nullptr && dependenciesProviderPtr != nullptr){
				QByteArray featureId = featureInfoPtr->GetFeatureId();
				m_dependencies[featureId] = dependenciesProviderPtr->GetFeatureDependencies(featureId);

				FeatureDescription desc;
				desc.name = featureInfoPtr->GetFeatureName();
				desc.id = featureId;
				m_features.append(desc);
			}
		}
	}

	if (m_isGuiModelInitialized && m_isCollectionRepresentationInitialized){
		EnumerateMissingDependencies();
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
			m_objectCollectionViewCompPtr->CreateGui(FeatureList);
		}

		imod::IModel* featurePackageCollectionModelPtr = const_cast<imod::IModel*>(
					dynamic_cast<const imod::IModel*>(GetObservedObject()->GetFeaturePackages()));
		
		if (featurePackageCollectionModelPtr != nullptr){
			featurePackageCollectionModelPtr->AttachObserver(&m_packageCollectionObserver);
		}

		imod::IModel* featureSelectionModelPtr = dynamic_cast<imod::IModel*>(m_objectCollectionViewCompPtr.GetPtr());

		if (featureSelectionModelPtr != nullptr){
			featureSelectionModelPtr->AttachObserver(&m_featureSelectionObserver);
		}
	}
}


void CFeaturePackageGuiComp::OnGuiModelDetached()
{
	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		if (m_featureSelectionObserver.IsModelAttached()){
			m_featureSelectionObserver.EnsureModelDetached();
		}

		imod::IModel* featurePackageCollectionModelPtr = const_cast<imod::IModel*>(
					dynamic_cast<const imod::IModel*>(GetObservedObject()->GetFeaturePackages()));

		if (featurePackageCollectionModelPtr != nullptr){
			if (featurePackageCollectionModelPtr->IsAttached(&m_packageCollectionObserver)){
				featurePackageCollectionModelPtr->DetachObserver(&m_packageCollectionObserver);
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
	if (!m_selectedFeatureId.isEmpty()){
		imtlic::IFeatureInfoProvider* featureInfoProviderPtr = GetObservedObject();
		if (featureInfoProviderPtr != nullptr){
			imtlic::IFeatureDependenciesManager* dependenciesManagerPtr = dynamic_cast<imtlic::IFeatureDependenciesManager*>(
						const_cast<imtlic::IFeatureDependenciesProvider*>(featureInfoProviderPtr->GetDependenciesInfoProvider()));

			if (dependenciesManagerPtr != nullptr){
				dependenciesManagerPtr->SetFeatureDependencies(m_selectedFeatureId, m_dependencies[m_selectedFeatureId]);
			}
		}
	}
}


// protected slots

void CFeaturePackageGuiComp::on_FeatureTree_itemChanged(QTreeWidgetItem *item, int column)
{
	BaseClass2::on_FeatureTree_itemChanged(item, column);
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


// public methods of the embedded class FeatureSelectionObserver

CFeaturePackageGuiComp::FeatureSelectionObserver::FeatureSelectionObserver(CFeaturePackageGuiComp& parent)
	:m_parent(parent)
{
}


// reimplemented (imod::CSingleModelObserverBase)

void CFeaturePackageGuiComp::FeatureSelectionObserver::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.OnFeatureSelectionChanged();
}


} // namespace imtlicgui



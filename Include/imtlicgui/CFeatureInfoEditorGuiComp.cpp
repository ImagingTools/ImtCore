#include <imtlicgui/CFeatureInfoEditorGuiComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtlic/IFeatureDependenciesManager.h>

namespace imtlicgui
{


// public methods

CFeatureInfoEditorGuiComp::CFeatureInfoEditorGuiComp()
{
}


// protected methods

void CFeatureInfoEditorGuiComp::UpdateFeaturePackageModel()
{
	DoUpdateModel();
}


void CFeatureInfoEditorGuiComp::FeatureTreeItemChanged()
{
	Q_EMIT EmitFeatureTreeItemChanged();
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CFeatureInfoEditorGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::IFeatureInfo* featureInfoPtr = GetObservedObject();
	Q_ASSERT(featureInfoPtr != nullptr);

	IdEdit->setText(featureInfoPtr->GetFeatureId());
	NameEdit->setText(featureInfoPtr->GetFeatureName());

	m_featureId = featureInfoPtr->GetFeatureId();
	m_dependencies.clear();

	const imtlic::IFeatureInfoProvider* featureInfoProviderPtr = GetObservedObject()->GetFeaturePackage();
	if (featureInfoProviderPtr != nullptr){
		const imtlic::IFeatureDependenciesProvider* dependenciesProviderPtr = featureInfoProviderPtr->GetDependenciesInfoProvider();
		if (dependenciesProviderPtr != nullptr){
			m_dependencies = dependenciesProviderPtr->GetFeatureDependencies(m_featureId);
		}
	}

	m_isGuiModelInitialized = true;

	if (m_isGuiModelInitialized && m_isCollectionRepresentationInitialized){
		EnumerateMissingDependencies();
		UpdateFeatureTree();
		UpdateFeatureTreeCheckStates();
		DoUpdateModel();
	}
}


void CFeatureInfoEditorGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	m_isGuiModelInitialized = false;
	m_isCollectionRepresentationInitialized = false;

	m_treeWidgetPtr = FeatureTree;

	const imtlic::IFeatureInfoProvider* featurePackagePtr = dynamic_cast<const imtlic::IFeatureInfoProvider*>(
				GetObservedObject()->GetFeaturePackage());

	if (featurePackagePtr != nullptr){
		imod::IModel* featurePackageCollectionModelPtr = dynamic_cast<imod::IModel*>(
					const_cast<imtbase::IObjectCollection*>(featurePackagePtr->GetFeaturePackages()));

		if (featurePackageCollectionModelPtr != nullptr){
			featurePackageCollectionModelPtr->AttachObserver(&m_packageCollectionObserver);
		}
	}

	connect(this, &CFeatureInfoEditorGuiComp::EmitFeatureTreeItemChanged, this, &CFeatureInfoEditorGuiComp::OnFeatureTreeItemChanged, Qt::QueuedConnection);
}


void CFeatureInfoEditorGuiComp::OnGuiModelDetached()
{
	disconnect(this, &CFeatureInfoEditorGuiComp::EmitFeatureTreeItemChanged, this, &CFeatureInfoEditorGuiComp::OnFeatureTreeItemChanged);

	const imtlic::IFeatureInfoProvider* featurePackagePtr = dynamic_cast<const imtlic::IFeatureInfoProvider*>(
			GetObservedObject()->GetFeaturePackage());

	if (featurePackagePtr != nullptr){
		imod::IModel* featurePackageCollectionModelPtr = dynamic_cast<imod::IModel*>(
					const_cast<imtbase::IObjectCollection*>(featurePackagePtr->GetFeaturePackages()));

		if (featurePackageCollectionModelPtr != nullptr){
			if (featurePackageCollectionModelPtr->IsAttached(&m_packageCollectionObserver)){
				featurePackageCollectionModelPtr->DetachObserver(&m_packageCollectionObserver);
			}
		}
	}

	BaseClass::OnGuiModelDetached();
}


void CFeatureInfoEditorGuiComp::UpdateModel() const
{
	imtlic::IFeatureInfo* featureInfoPtr = GetObservedObject();
	Q_ASSERT(featureInfoPtr != nullptr);

	imtlic::CFeatureInfo featureInfo;

	featureInfo.SetFeatureId(IdEdit->text().toUtf8());
	featureInfo.SetFeatureName(NameEdit->text());

	featureInfoPtr->CopyFrom(featureInfo);

	const imtlic::IFeatureInfoProvider* featureInfoProviderPtr = GetObservedObject()->GetFeaturePackage();
	if (featureInfoProviderPtr != nullptr){
		imtlic::IFeatureDependenciesManager* dependenciesManagerPtr = dynamic_cast<imtlic::IFeatureDependenciesManager*>(
					const_cast<imtlic::IFeatureDependenciesProvider*>(
								featureInfoProviderPtr->GetDependenciesInfoProvider()));

		if (dependenciesManagerPtr != nullptr){
			dependenciesManagerPtr->SetFeatureDependencies(m_featureId, m_dependencies);
		}
	}
}


// private slots

void CFeatureInfoEditorGuiComp::on_FeatureTree_itemChanged(QTreeWidgetItem *item, int column)
{
	BaseClass2::on_FeatureTree_itemChanged(item, column);
}


void CFeatureInfoEditorGuiComp::on_NameEdit_editingFinished()
{
	DoUpdateModel();
}


void CFeatureInfoEditorGuiComp::on_IdEdit_editingFinished()
{
	DoUpdateModel();
}


void CFeatureInfoEditorGuiComp::OnFeatureTreeItemChanged()
{
	if (m_isGuiModelInitialized && m_isCollectionRepresentationInitialized){
		EnumerateMissingDependencies();
		UpdateFeatureTree();
		UpdateFeatureTreeCheckStates();
		DoUpdateModel();
	}
}


} // namespace imtlicgui



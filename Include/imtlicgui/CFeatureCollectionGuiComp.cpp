#include <imtlicgui/CFeatureCollectionGuiComp.h>


// ACF includes
#include <istd/CChangeGroup.h>


namespace imtlicgui
{


// public methods

CFeatureCollectionGuiComp::CFeatureCollectionGuiComp()
	:m_showCollectionEditorCommand("Show Collection Editor", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_EXCLUSIVE, 2020),
	m_showDependenciesEditorCommand("Show Feature Dependency Editor", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_EXCLUSIVE, 2020)
{
	m_rootCommands.InsertChild(&m_showCollectionEditorCommand);
	m_rootCommands.InsertChild(&m_showDependenciesEditorCommand);

	connect(&m_showCollectionEditorCommand, &QAction::triggered, this, &CFeatureCollectionGuiComp::OnShowCollectionEditor);
	connect(&m_showDependenciesEditorCommand, &QAction::triggered, this, &CFeatureCollectionGuiComp::OnShowFeatureDependencyEditor);
}


// protected methods

// reimplemented(ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CFeatureCollectionGuiComp::GetCommands() const
{
	return &m_rootCommands;
}


// reimplemented (iqtgui::CGuiComponentBase)

void CFeatureCollectionGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();
}


void CFeatureCollectionGuiComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


void CFeatureCollectionGuiComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	m_showCollectionEditorCommand.SetVisuals(tr("Show Collection Editor"), tr("Collection Editor"), tr("Show Collection Editor"), QIcon(":/Icons/Lamp"));
	m_showDependenciesEditorCommand.SetVisuals(tr("Show Feature Dependency Editor"), tr("Dependency Editor"), tr("Show Feature Dependency Editor"), QIcon(":/Icons/Lamp"));

	m_showCollectionEditorCommand.setChecked(true);
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CFeatureCollectionGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	//imtlic::IProductLicensingInfo* licenseInfoPtr = GetObservedObject();
	//Q_ASSERT(licenseInfoPtr != nullptr);
}


void CFeatureCollectionGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
		imod::IModel* modelPtr = GetObservedModel();
		Q_ASSERT(modelPtr != nullptr);

		if (modelPtr->AttachObserver(m_objectCollectionObserverCompPtr.GetPtr())){
			m_objectCollectionViewCompPtr->CreateGui(objectCollectionView);
		}
	}
}


void CFeatureCollectionGuiComp::OnGuiModelDetached()
{
	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
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


void CFeatureCollectionGuiComp::UpdateModel() const
{
	//imtlic::IProductLicensingInfo* licenseInfoPtr = GetObservedObject();
	//Q_ASSERT(licenseInfoPtr != nullptr);
}


// protected slots

void CFeatureCollectionGuiComp::OnShowCollectionEditor()
{
	pages->setCurrentIndex(0);
}


void CFeatureCollectionGuiComp::OnShowFeatureDependencyEditor()
{
	pages->setCurrentIndex(1);
}


} // namespace imtlicgui



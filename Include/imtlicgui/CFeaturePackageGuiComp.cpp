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
	m_showCollectionEditorCommand("Edit Feartures", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_EXCLUSIVE, 2020),
	m_showDependenciesEditorCommand("Edit Dependencies", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_EXCLUSIVE, 2020)
{
	m_rootCommands.InsertChild(&m_showCollectionEditorCommand);
	m_rootCommands.InsertChild(&m_showDependenciesEditorCommand);

	connect(&m_showCollectionEditorCommand, &QAction::triggered, this, &CFeaturePackageGuiComp::OnShowCollectionEditor);
	connect(&m_showDependenciesEditorCommand, &QAction::triggered, this, &CFeaturePackageGuiComp::OnShowFeatureDependencyEditor);
}


// protected methods

void CFeaturePackageGuiComp::UpdateFeatureList()
{
	FeatureList->clear();

	imtlic::IFeatureInfoProvider* featureInfoProviderPtr = GetObservedObject();
	if (featureInfoProviderPtr != nullptr){
		const imtbase::ICollectionInfo& collectionInfo = featureInfoProviderPtr->GetFeatureList();
		imtbase::ICollectionInfo::Ids ids = collectionInfo.GetElementIds();
		for (const QByteArray& id : ids){
			const imtlic::IFeatureInfo* featureInfoPtr = featureInfoProviderPtr->GetFeatureInfo(id);
			if (featureInfoPtr != nullptr){
				QTreeWidgetItem* itemPtr = new QTreeWidgetItem({
							featureInfoPtr->GetFeatureName(),
							featureInfoPtr->GetFeatureId(),
							collectionInfo.GetElementInfo(id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString()});
				FeatureList->addTopLevelItem(itemPtr);
				itemPtr->setData(0, DR_ITEM_ID, featureInfoPtr->GetFeatureId());
			}
		}
	}
}


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
	return &m_rootCommands;
}


// reimplemented (iqtgui::CGuiComponentBase)

void CFeaturePackageGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	FeatureTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	FeatureList->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	FeatureList->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	FeatureList->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

	m_treeWidgetPtr = FeatureTree;
	if (m_packageCollectionObserver.IsModelAttached()){
		OnFeaturePackageCollectionUpdate();
	}

	connect(FeatureList, &QTreeWidget::itemSelectionChanged, this, &CFeaturePackageGuiComp::OnFeatureListSelectionChanged);
	connect(this, &CFeaturePackageGuiComp::EmitFeatureTreeItemChanged, this, &CFeaturePackageGuiComp::OnFeatureTreeItemChanged, Qt::QueuedConnection);
}


void CFeaturePackageGuiComp::OnGuiDestroyed()
{
	disconnect(this, &CFeaturePackageGuiComp::EmitFeatureTreeItemChanged, this, &CFeaturePackageGuiComp::OnFeatureTreeItemChanged);
	disconnect(FeatureList, &QTreeWidget::itemSelectionChanged, this, &CFeaturePackageGuiComp::OnFeatureListSelectionChanged);

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

	if (!m_featureId.isEmpty()){
		int count = FeatureList->topLevelItemCount();
		for (int i = 0; i < count; i++){
			QByteArray id = FeatureList->topLevelItem(i)->data(0, DR_ITEM_ID).toByteArray();
			if (id == m_featureId){
				FeatureList->setCurrentItem(FeatureList->topLevelItem(i));
				return;
			}
		}
	}

	m_featureId.clear();
	FeatureList->setCurrentItem(nullptr);
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
			featurePackageCollectionModelPtr->AttachObserver(&m_packageCollectionObserver);
		}
	}
}


void CFeaturePackageGuiComp::OnGuiModelDetached()
{
	if (m_objectCollectionViewCompPtr.IsValid() && m_objectCollectionObserverCompPtr.IsValid()){
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
	QTreeWidgetItem* itemPtr = FeatureList->currentItem();
	if (itemPtr != nullptr){
		QByteArray featureId = itemPtr->data(0, DR_ITEM_ID).toByteArray();

		imtlic::IFeatureInfoProvider* featureInfoProviderPtr = GetObservedObject();
		if (featureInfoProviderPtr != nullptr){
			imtlic::IFeatureDependenciesManager* dependenciesManagerPtr = dynamic_cast<imtlic::IFeatureDependenciesManager*>(
						const_cast<imtlic::IFeatureDependenciesProvider*>(featureInfoProviderPtr->GetDependenciesInfoProvider()));

			if (dependenciesManagerPtr != nullptr){
				dependenciesManagerPtr->SetFeatureDependencies(featureId, m_dependencies);
			}
		}
	}
}


// protected slots

void CFeaturePackageGuiComp::on_FeatureTree_itemChanged(QTreeWidgetItem *item, int column)
{
	BaseClass2::on_FeatureTree_itemChanged(item, column);
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
	m_featureId.clear();

	QTreeWidgetItem* itemPtr = FeatureList->currentItem();
	if (itemPtr != nullptr){
		QByteArray featureId = itemPtr->data(0, DR_ITEM_ID).toByteArray();

		m_featureId = featureId;

		imtlic::IFeatureInfoProvider* featureInfoProviderPtr = GetObservedObject();
		if (featureInfoProviderPtr != nullptr){
			const imtlic::IFeatureDependenciesProvider* dependenciesProviderPtr = featureInfoProviderPtr->GetDependenciesInfoProvider();
			if (dependenciesProviderPtr != nullptr){
				m_dependencies = dependenciesProviderPtr->GetFeatureDependencies(featureId);
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


// public methods of the embedded class DocumentList

CFeaturePackageGuiComp::FeaturePackageProxy::FeaturePackageProxy(CFeaturePackageGuiComp& parent)
	:m_parent(parent)
{
}


// reimplemented (imtlic::IFeatureInfoProvider)

const imtbase::IObjectCollection* CFeaturePackageGuiComp::FeaturePackageProxy::GetFeaturePackages() const
{
	imtlic::IFeatureInfoProvider* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetFeaturePackages();
	}

	return nullptr;
}


const imtbase::ICollectionInfo& CFeaturePackageGuiComp::FeaturePackageProxy::GetFeatureList() const
{
	imtlic::IFeatureInfoProvider* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetFeatureList();
	}

	return m_collectionInfo;
}


const imtlic::IFeatureInfo* CFeaturePackageGuiComp::FeaturePackageProxy::GetFeatureInfo(const QByteArray& featureId) const
{
	imtlic::IFeatureInfoProvider* featurePackagePtr = m_parent.GetObservedObject();
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
	imtlic::IFeatureInfoProvider* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetParentFeatureInfoProviderList();
	}

	return nullptr;
}


const imtlic::IFeatureInfoProvider* CFeaturePackageGuiComp::FeaturePackageProxy::GetParentFeatureInfoProvider(const QByteArray& parentId) const
{
	imtlic::IFeatureInfoProvider* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetParentFeatureInfoProvider(parentId);
	}

	return nullptr;
}

// reimplemented (iser::ISerializable)

bool CFeaturePackageGuiComp::FeaturePackageProxy::Serialize(iser::IArchive& archive)
{
	imtlic::IFeatureInfoProvider* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->Serialize(archive);
	}

	return false;
}


quint32 CFeaturePackageGuiComp::FeaturePackageProxy::GetMinimalVersion(int versionId) const
{
	imtlic::IFeatureInfoProvider* featurePackagePtr = m_parent.GetObservedObject();
	if (featurePackagePtr != nullptr){
		return featurePackagePtr->GetMinimalVersion(versionId);
	}

	return iser::ISerializable::GetMinimalVersion(versionId);
}


} // namespace imtlicgui



#include <imtgui/CObjectCollectionViewComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QDir>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QMessageBox>

// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iqtgui/CCommandTools.h>
#include <iqtgui/CHierarchicalCommand.h>


namespace imtgui
{


// protected methods

CObjectCollectionViewComp::CObjectCollectionViewComp()
{
	m_commands.SetParent(this);
}


// protected methods

ICollectionViewDelegate & CObjectCollectionViewComp::GetViewDelegateRef(const QByteArray& typeId)
{
	if (m_viewDelegateMap.contains(typeId)){
		return *m_viewDelegateMap[typeId];
	}

	return m_defaultViewDelegate;
}


const ICollectionViewDelegate& CObjectCollectionViewComp::GetViewDelegate(const QByteArray& typeId) const
{
	return (const_cast<CObjectCollectionViewComp*>(this))->GetViewDelegateRef(typeId);
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CObjectCollectionViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	const imtbase::IObjectCollection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	m_typeItems.clear();
	m_itemModel.clear();
	m_itemModel.setColumnCount(1);
	m_itemModel.setHorizontalHeaderLabels(QStringList() << tr("Name"));

	const iprm::IOptionsList* objectTypeInfoPtr = objectPtr->GetObjectTypesInfo();
	if (objectTypeInfoPtr != nullptr){
		int typesCount = objectTypeInfoPtr->GetOptionsCount();
		imtbase::IObjectCollectionInfo::Ids collectionItemIds = objectPtr->GetElementIds();

		for (int typeIndex = 0; typeIndex < typesCount; ++typeIndex){
			QByteArray typeId = objectTypeInfoPtr->GetOptionId(typeIndex);
			QString typeName = objectTypeInfoPtr->GetOptionName(typeIndex);

			QStandardItem* typeItemPtr = new QStandardItem;
			typeItemPtr->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			typeItemPtr->setData(typeName, Qt::DisplayRole);
			typeItemPtr->setData(typeName, Qt::EditRole);
			typeItemPtr->setData(typeId, DR_TYPE_ID);
			m_itemModel.appendRow(typeItemPtr);

			m_typeItems.push_back(typeItemPtr);

			ItemTree->setExpanded(typeItemPtr->index(), true);
		}
		
		for (const QByteArray& itemId : collectionItemIds){
			QByteArray itemTypeId = objectPtr->GetObjectTypeId(itemId);

			QStandardItem* parentItemPtr = nullptr;
			for (int i = 0; i < m_typeItems.count(); ++i){
				QByteArray typeId = m_typeItems[i]->data(Qt::UserRole).toByteArray();
				if (typeId == itemTypeId){
					parentItemPtr = m_typeItems[i];
					break;
				}
			}

			QStandardItem* objectItemPtr = new QStandardItem;
			QString objectName = objectPtr->GetElementInfo(itemId, imtbase::IObjectCollectionInfo::EIT_NAME).toString();
			objectItemPtr->setData(objectName, Qt::DisplayRole);
			objectItemPtr->setData(objectName, Qt::EditRole);
			objectItemPtr->setData(itemId, DR_OBJECT_ID);
			objectItemPtr->setData(objectPtr->GetElementInfo(itemId, imtbase::IObjectCollectionInfo::EIT_TYPE_ID), DR_TYPE_ID);

			Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

			if (objectPtr->GetSupportedOperations() & imtbase::IObjectCollection::OF_SUPPORT_RENAME){
				flags |= Qt::ItemIsEditable;
			}

			objectItemPtr->setFlags(flags);

			if (parentItemPtr != nullptr){
				parentItemPtr->appendRow(objectItemPtr);
			}
			else{
				m_itemModel.appendRow(objectItemPtr);
			}
		}
	}

	UpdateCommands();
}


void CObjectCollectionViewComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	imtbase::IObjectCollection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	m_defaultViewDelegate.InitializeDelegate(objectPtr, this);

	for (ViewDelegateMap::Iterator iter = m_viewDelegateMap.begin(); iter != m_viewDelegateMap.end(); ++iter){
		iter.value()->InitializeDelegate(objectPtr, this);
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CObjectCollectionViewComp::OnGuiCreated()
{
	ItemTree->setModel(&m_itemModel);

	if (*m_showCommandsToolBarAttrPtr){
		QToolBar* toolBarPtr = new QToolBar(TopFrame);
		TopFrame->layout()->addWidget(toolBarPtr);

		iqtgui::CHierarchicalCommand* rootCommandPtr = dynamic_cast<iqtgui::CHierarchicalCommand*>(const_cast<ibase::IHierarchicalCommand*>(GetViewDelegate(m_currentTypeId).GetCommands()));
		if (rootCommandPtr != nullptr){
			iqtgui::CCommandTools::SetupToolbar(*rootCommandPtr, *toolBarPtr);
			toolBarPtr->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			toolBarPtr->setIconSize(QSize(16, 16));
		}
	}

	TopFrame->setVisible(*m_showCommandsToolBarAttrPtr);
	ItemTree->header()->setVisible(!*m_showCommandsToolBarAttrPtr);

	QItemSelectionModel* selectionModelPtr = ItemTree->selectionModel();
	if (selectionModelPtr != nullptr){
		connect(
					selectionModelPtr,
					SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
					this, SLOT(OnSelectionChanged(const QItemSelection&, const QItemSelection&)));
	}

	connect(&m_itemModel, &QStandardItemModel::itemChanged, this, &CObjectCollectionViewComp::OnItemChanged);

	BaseClass::OnGuiCreated();
}


void CObjectCollectionViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	UpdateGui(istd::IChangeable::GetAnyChange());
}


// reimplemented (icomp::CComponentBase)

void CObjectCollectionViewComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int delegatesCount = qMin(m_viewDelegatesCompPtr.GetCount(), m_objectTypeIdsAttrPtr.GetCount());
	for (int i = 0; i < delegatesCount; ++i){
		ICollectionViewDelegate* delegatePtr = m_viewDelegatesCompPtr[i];
		if (delegatePtr != nullptr){
			QByteArray typeId = m_objectTypeIdsAttrPtr[i];
			Q_ASSERT(!typeId.isEmpty());

			m_viewDelegateMap[typeId] = delegatePtr;
		}
	}
}


void CObjectCollectionViewComp::OnComponentDestroyed()
{
	m_viewDelegateMap.clear();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CObjectCollectionViewComp::UpdateCommands()
{
	QModelIndexList selectedIndexes = ItemTree->selectionModel()->selectedRows();

	imtbase::ICollectionInfo::Ids itemIds;

	QSet<QByteArray> selectedTypes;

	if (!selectedIndexes.isEmpty()){
		const imtbase::IObjectCollection* collectionPtr = GetObservedObject();
		Q_ASSERT(collectionPtr != nullptr);
		for (int i = 0; i < selectedIndexes.count(); ++i){
			QStandardItem* itemPtr = m_itemModel.itemFromIndex(selectedIndexes[0]);
			if (itemPtr != nullptr){
				QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
				if (!itemId.isEmpty()){
					itemIds.push_back(itemPtr->data(DR_OBJECT_ID).toByteArray());
				}

				QByteArray typeId = itemPtr->data(DR_TYPE_ID).toByteArray();
				if (!typeId.isEmpty()){
					selectedTypes.insert(typeId);
				}
			}
		}
	}

	int stateFlags = imtgui::ICollectionViewDelegate::VS_NONE;

	if (!itemIds.isEmpty()){
		stateFlags |= imtgui::ICollectionViewDelegate::VS_SELECTED;
		if (itemIds.count() == 1){
			stateFlags |= imtgui::ICollectionViewDelegate::VS_SINGLE_SELECTION;
		}
	}

	QByteArray selectedTypeId;
	if (selectedTypes.count() == 1){
		selectedTypeId = *selectedTypes.begin();
	}

	GetViewDelegateRef(selectedTypeId).UpdateItemSelection(stateFlags, itemIds, selectedTypeId);

	istd::IChangeable::ChangeSet changes(ibase::ICommandsProvider::CF_COMMANDS);
	istd::CChangeNotifier changeNotifier(&m_commands, &changes);

	m_currentTypeId = selectedTypeId;
}


// private slots

void CObjectCollectionViewComp::OnSelectionChanged(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/)
{
	UpdateCommands();
}


void CObjectCollectionViewComp::OnItemChanged(QStandardItem* itemPtr)
{
	imtbase::IObjectCollection* collectionPtr = GetObservedObject();
	Q_ASSERT(collectionPtr != nullptr);

	QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
	if (!itemId.isEmpty()){
		QString currentName = collectionPtr->GetElementInfo(itemId, imtbase::ICollectionInfo::EIT_NAME).toString();
		QString newName = itemPtr->text();

		collectionPtr->SetObjectName(itemId, newName);
	}
}


// public methods of the embedded class Commands

CObjectCollectionViewComp::Commands::Commands()
	:m_parentPtr(nullptr)
{
}


void CObjectCollectionViewComp::Commands::SetParent(CObjectCollectionViewComp* parentPtr)
{
	Q_ASSERT(parentPtr != nullptr);

	m_parentPtr = parentPtr;
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CObjectCollectionViewComp::Commands::GetCommands() const
{
	Q_ASSERT(m_parentPtr != nullptr);

	return m_parentPtr->GetViewDelegate(m_parentPtr->m_currentTypeId).GetCommands();
}


} // namespace imtgui



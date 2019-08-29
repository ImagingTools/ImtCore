#include <imtgui/CObjectCollectionViewComp.h>


// Qt includes
#include <QtWidgets/QToolBar>
#include <QtWidgets/QMessageBox>

// ACF includes
#include <iqtgui/CCommandTools.h>

namespace imtgui
{


// protected methods

CObjectCollectionViewComp::CObjectCollectionViewComp()
	:m_editCommands("&Edit", 100),
	m_insertCommand("Add", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 2020),
	m_removeCommand("Remove", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 2020)
{
	connect(&m_insertCommand, SIGNAL(triggered()), this, SLOT(OnInsert()));
	connect(&m_removeCommand, SIGNAL(triggered()), this, SLOT(OnRemove()));

	m_editCommands.InsertChild(&m_insertCommand);
	m_editCommands.InsertChild(&m_removeCommand);

	m_rootCommands.InsertChild(&m_editCommands);
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand * CObjectCollectionViewComp::GetCommands() const
{
	return &m_rootCommands;
}


// protected methods

// reimplemented (iqtgui::CGuiComponentBase)

void CObjectCollectionViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	ItemTree->header()->hide();

	ItemTree->setModel(&m_itemModel);

	QToolBar* toolBarPtr = new QToolBar(TopFrame);
	TopFrame->layout()->addWidget(toolBarPtr);

	iqtgui::CCommandTools::SetupToolbar(m_rootCommands, *toolBarPtr);
	toolBarPtr->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

	QItemSelectionModel* selectionModelPtr = ItemTree->selectionModel();
	if (selectionModelPtr != nullptr){
		connect(
					selectionModelPtr,
					SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
					this, SLOT(OnSelectionChanged(const QItemSelection&, const QItemSelection&)));
	}
}


void CObjectCollectionViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	m_insertCommand.SetVisuals(tr("Insert"), tr("New"), tr("Insert new resource into the collection"), QIcon(":/Icons/Add"));
	m_removeCommand.SetVisuals(tr("Remove"), tr("Remove"), tr("Remove selected resource from the collection"), QIcon(":/Icons/Delete"));

	UpdateGui(istd::IChangeable::GetAnyChange());
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CObjectCollectionViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	const imtbase::IObjectCollection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	m_typeItems.clear();
	m_itemModel.clear();
	m_itemModel.setColumnCount(1);

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

	const imtbase::IObjectCollection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	const iprm::IOptionsList* typesPtr = objectPtr->GetObjectTypesInfo();
	if (typesPtr != nullptr){
		int typesCount = typesPtr->GetOptionsCount();
		if (typesCount > 1){
			for (int i = 0; i < typesCount; ++i){
				const QString typeName = typesPtr->GetOptionName(i);

				QAction* action = m_startVariableMenus.addAction(typeName);
				action->setData(typesPtr->GetOptionId(i));
			}

			m_insertCommand.setMenu(&m_startVariableMenus);

			QObject::connect(&m_startVariableMenus, SIGNAL(triggered(QAction*)), this, SLOT(OnAddMenuOptionClicked(QAction*)));
		}
	}
}


// private methods

void CObjectCollectionViewComp::UpdateCommands()
{
	const imtbase::IObjectCollection* collectionPtr = GetObservedObject();
	Q_ASSERT(collectionPtr != nullptr);

	QModelIndexList selectedIndexes = ItemTree->selectionModel()->selectedRows();

	bool isAddEnabled = collectionPtr->GetOperationFlags() & imtbase::IObjectCollection::OF_SUPPORT_INSERT;
	bool isRemoveEnabled = false;

	if (!selectedIndexes.isEmpty()){
		QStandardItem* itemPtr = m_itemModel.itemFromIndex(selectedIndexes[0]);
		if (itemPtr != nullptr){
			QByteArray objectId = itemPtr->data(DR_OBJECT_ID).toByteArray();
			if (!objectId.isEmpty()){
				int flags = collectionPtr->GetOperationFlags(objectId);
				if ((flags & imtbase::IObjectCollection::OF_SUPPORT_DELETE) && ((flags & imtbase::IObjectCollection::OF_FIXED) == 0)){
					isRemoveEnabled = true;
				}
			}
		}
	}

	m_insertCommand.setEnabled(isAddEnabled);
	m_removeCommand.setEnabled(isRemoveEnabled);
}


// private slots

void CObjectCollectionViewComp::OnInsert()
{
	imtbase::IObjectCollection* collectionPtr = GetObservedObject();
	Q_ASSERT(collectionPtr != nullptr);

	const iprm::IOptionsList* typesPtr = collectionPtr->GetObjectTypesInfo();
	if (typesPtr == nullptr || typesPtr->GetOptionsCount() < 1){
		return;
	}

	QByteArray typeId = typesPtr->GetOptionId(0);

	QByteArray objectId = collectionPtr->InsertNewObject(typeId, tr("New"), QString());
	if (objectId.isEmpty()){
		QMessageBox::critical(GetWidget(), tr("Collection"), tr("New resource could not be created"));
	}
}


void CObjectCollectionViewComp::OnRemove()
{
	const QItemSelection currentSelection = ItemTree->selectionModel()->selection();
	const QModelIndexList indexes = currentSelection.indexes();
	if (indexes.isEmpty()){
		return;
	}

	const QModelIndex& index = indexes[0];

	ItemTree->selectionModel()->select(currentSelection, QItemSelectionModel::Deselect);

	imtbase::IObjectCollection* collectionPtr = GetObservedObject();
	Q_ASSERT(collectionPtr != nullptr);

	imtbase::ICollectionInfo::Ids elementIds = collectionPtr->GetElementIds();

	QByteArray objectId = index.data(DR_OBJECT_ID).toByteArray();

	collectionPtr->RemoveObject(objectId);
}


void CObjectCollectionViewComp::OnSelectionChanged(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/)
{
	UpdateCommands();
}


void CObjectCollectionViewComp::OnAddMenuOptionClicked(QAction * action)
{
	imtbase::IObjectCollection* collectionPtr = GetObservedObject();
	Q_ASSERT(collectionPtr != nullptr);

	QByteArray typeId = action->data().toByteArray();

	QByteArray objectId = collectionPtr->InsertNewObject(typeId, tr("New"), QString());
	if (objectId.isEmpty()){
		QMessageBox::critical(GetWidget(), tr("Collection"), tr("New resource could not be created"));
	}
}


} // namespace imtgui



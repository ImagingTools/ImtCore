#include <imtgui/CObjectCollectionViewComp.h>


// Qt includes
#include <QtWidgets/QToolBar>

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
			typeItemPtr->setFlags(Qt::ItemIsEnabled);
			typeItemPtr->setData(typeName, Qt::DisplayRole);
			typeItemPtr->setData(typeName, Qt::EditRole);
			typeItemPtr->setData(typeId, Qt::UserRole);
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
			objectItemPtr->setData(itemId, Qt::UserRole);

			if (parentItemPtr != nullptr){
				parentItemPtr->appendRow(objectItemPtr);
			}
			else{
				m_itemModel.appendRow(objectItemPtr);
			}
		}
	}
}


void CObjectCollectionViewComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	const imtbase::IObjectCollection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);
}


// private slots

void CObjectCollectionViewComp::OnInsert()
{
}


void CObjectCollectionViewComp::OnRemove()
{
}


} // namespace imtgui



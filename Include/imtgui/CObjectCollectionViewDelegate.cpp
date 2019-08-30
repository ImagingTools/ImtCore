#include <imtgui/CObjectCollectionViewDelegate.h>


// Qt includes
#include <QtWidgets/QMessageBox>

// ACF includes
#include <istd/CChangeGroup.h>


namespace imtgui
{


CObjectCollectionViewDelegate::CObjectCollectionViewDelegate()
	:m_editCommands("&Edit", 100),
	m_insertCommand("Add", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 2020),
	m_removeCommand("Remove", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, 2020),
	m_collectionPtr(nullptr)
{
	connect(&m_insertCommand, SIGNAL(triggered()), this, SLOT(OnInsert()));
	connect(&m_removeCommand, SIGNAL(triggered()), this, SLOT(OnRemove()));

	m_editCommands.InsertChild(&m_insertCommand);
	m_editCommands.InsertChild(&m_removeCommand);

	m_rootCommands.InsertChild(&m_editCommands);
}


// reimplemented (ICollectionViewDelegate)

bool CObjectCollectionViewDelegate::InitializeDelegate(imtbase::IObjectCollection* collectionPtr, iqtgui::IGuiObject* parentGuiPtr)
{
	OnLanguageChanged();

	m_collectionPtr = collectionPtr;
	m_parentGuiPtr = parentGuiPtr;

	if (m_collectionPtr != nullptr){
		const iprm::IOptionsList* typesPtr = m_collectionPtr->GetObjectTypesInfo();
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

		return true;
	}

	return false;
}


QByteArrayList CObjectCollectionViewDelegate::GetObjectTypeIds() const
{
	QByteArrayList retVal;

	return retVal;
}


void CObjectCollectionViewDelegate::UpdateCommands(int viewStateFlags, const imtbase::ICollectionInfo::Ids& itemIds)
{
	bool isAddEnabled = false;
	bool isRemoveEnabled = false;

	if (m_collectionPtr != nullptr){
		isAddEnabled = m_collectionPtr->GetOperationFlags() & imtbase::IObjectCollection::OF_SUPPORT_INSERT;
		isRemoveEnabled = false;

		if (!itemIds.isEmpty()){
			int flags = m_collectionPtr->GetOperationFlags(itemIds[0]);
			if ((flags & imtbase::IObjectCollection::OF_SUPPORT_DELETE) && ((flags & imtbase::IObjectCollection::OF_FIXED) == 0)){
				isRemoveEnabled = true;
			}
		}
	}

	m_insertCommand.setEnabled(isAddEnabled);
	m_removeCommand.setEnabled(isRemoveEnabled);

	m_selectedItemIds = itemIds;
}


QByteArray CObjectCollectionViewDelegate::CreateNewObject(const QByteArray& typeId) const
{
	if (m_collectionPtr != nullptr){
		return m_collectionPtr->InsertNewObject(typeId, tr("New"), QString(), nullptr);
	}

	return QByteArray();
}


QByteArray CObjectCollectionViewDelegate::ImportObject(const QByteArray& typeId, const QString& sourcePath) const
{
	return QByteArray();
}


bool CObjectCollectionViewDelegate::ExportObject(const QByteArray& objectId, const QString& targetPath) const
{
	return false;
}


void CObjectCollectionViewDelegate::RemoveObjects(const imtbase::ICollectionInfo::Ids& objectIds)  const
{
	if (m_collectionPtr != nullptr){
		istd::CChangeGroup changeGroup(m_collectionPtr);

		for (const QByteArray& id : objectIds){
			m_collectionPtr->RemoveObject(id);
		}
	}
}


bool CObjectCollectionViewDelegate::UpdateObject(const QByteArray& objectId, const istd::IChangeable& contents) const
{
	return false;
}


bool CObjectCollectionViewDelegate::RenameObject(const QByteArray& objectId, const QString& newName) const
{
	return false;
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CObjectCollectionViewDelegate::GetCommands() const
{
	return &m_rootCommands;
}


// protected methods

// reimplemented (ibase::TLocalizableWrap)

void CObjectCollectionViewDelegate::OnLanguageChanged()
{
	m_insertCommand.SetVisuals(tr("Insert"), tr("New"), tr("Insert new resource into the collection"), QIcon(":/Icons/Add"));
	m_removeCommand.SetVisuals(tr("Remove"), tr("Remove"), tr("Remove selected resource from the collection"), QIcon(":/Icons/Delete"));
}


// private slots

void CObjectCollectionViewDelegate::OnInsert()
{
	Q_ASSERT(m_collectionPtr != nullptr);

	const iprm::IOptionsList* typesPtr = m_collectionPtr->GetObjectTypesInfo();
	if (typesPtr == nullptr || typesPtr->GetOptionsCount() < 1){
		return;
	}

	QByteArray objectId = CreateNewObject(typesPtr->GetOptionId(0));
	if (objectId.isEmpty()){
		QMessageBox::critical((m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr, tr("Collection"), tr("New resource could not be created"));
	}
}


void CObjectCollectionViewDelegate::OnRemove()
{
	RemoveObjects(m_selectedItemIds);
}


void CObjectCollectionViewDelegate::OnAddMenuOptionClicked(QAction* action)
{
	QByteArray objectId = CreateNewObject(action->data().toByteArray());
	if (objectId.isEmpty()){
		QMessageBox::critical((m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr, tr("Collection"), tr("New resource could not be created"));
	}
}


} // namespace imtgui



#include <imtgui/CObjectCollectionViewComp.h>


// Qt includes
#include <QtWidgets/QToolBar>
#include <QtWidgets/QMessageBox>

// ACF includes
#include <iqtgui/CCommandTools.h>
#include <iqtgui/CHierarchicalCommand.h>


namespace imtgui
{


// protected methods

CObjectCollectionViewComp::CObjectCollectionViewComp()
{
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CObjectCollectionViewComp::GetCommands() const
{
	return GetViewDelegate().GetCommands();
}


// protected methods

ICollectionViewDelegate & CObjectCollectionViewComp::GetViewDelegateRef()
{
	if (m_viewDelegateCompPtr.IsValid()){
		return *m_viewDelegateCompPtr;
	}

	return m_defaultViewDelegate;
}


const ICollectionViewDelegate & CObjectCollectionViewComp::GetViewDelegate() const
{
	return (const_cast<CObjectCollectionViewComp*>(this))->GetViewDelegateRef();
}


// reimplemented (iqtgui::CGuiComponentBase)

void CObjectCollectionViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	ItemTree->header()->hide();

	ItemTree->setModel(&m_itemModel);

	QToolBar* toolBarPtr = new QToolBar(TopFrame);
	TopFrame->layout()->addWidget(toolBarPtr);

	iqtgui::CHierarchicalCommand* rootCommandPtr = dynamic_cast<iqtgui::CHierarchicalCommand*>(const_cast<ibase::IHierarchicalCommand*>(GetViewDelegate().GetCommands()));
	if (rootCommandPtr != nullptr){
		iqtgui::CCommandTools::SetupToolbar(*rootCommandPtr, *toolBarPtr);
		toolBarPtr->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		toolBarPtr->setIconSize(QSize(16, 16));
	}

	QItemSelectionModel* selectionModelPtr = ItemTree->selectionModel();
	if (selectionModelPtr != nullptr){
		connect(
					selectionModelPtr,
					SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
					this, SLOT(OnSelectionChanged(const QItemSelection&, const QItemSelection&)));
	}

	connect(&m_itemModel, &QStandardItemModel::itemChanged, this, &CObjectCollectionViewComp::OnItemChanged);
}


void CObjectCollectionViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

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

	GetViewDelegateRef().InitializeDelegate(objectPtr, this);
}


// private methods

void CObjectCollectionViewComp::UpdateCommands()
{
	QModelIndexList selectedIndexes = ItemTree->selectionModel()->selectedRows();

	imtbase::ICollectionInfo::Ids itemIds;

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

	GetViewDelegateRef().UpdateCommands(stateFlags, itemIds);
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


} // namespace imtgui



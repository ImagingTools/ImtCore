#include <imtgui/CObjectCollectionViewComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QDir>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QMessageBox>
#include <QLineEdit>
#include <QInputDialog>

// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iqtgui/CCommandTools.h>
#include <iqtgui/CHierarchicalCommand.h>


namespace imtgui
{


// protected methods

CObjectCollectionViewComp::CObjectCollectionViewComp()
	:m_blockHeaderSectionSizeStore(false)
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


// reimplemented (iqtgui::TRestorableGuiWrap)

void CObjectCollectionViewComp::OnRestoreSettings(const QSettings& settings)
{
}


void CObjectCollectionViewComp::OnSaveSettings(QSettings& settings) const
{
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CObjectCollectionViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	const imtbase::IObjectCollection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	m_blockHeaderSectionSizeStore = true;

	QByteArray lastTypeId = m_currentTypeId;

	TypeList->clear();
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

			QTreeWidgetItem* typeItemPtr = new QTreeWidgetItem;
			typeItemPtr->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			typeItemPtr->setData(0, Qt::DisplayRole, typeName);
			typeItemPtr->setData(0, Qt::EditRole, typeName);
			typeItemPtr->setData(0, DR_TYPE_ID, typeId);
			TypeList->addTopLevelItem(typeItemPtr);

			if (lastTypeId == typeId){
				TypeList->setItemSelected(typeItemPtr, true);
			}

			if (lastTypeId.isEmpty() && (typeIndex == 0)){
				TypeList->setItemSelected(typeItemPtr, true);
			}
		}
		
		for (const QByteArray& itemId : collectionItemIds){
			QByteArray itemTypeId = objectPtr->GetObjectTypeId(itemId);

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
			
			QList<QStandardItem*> columns;
			QStringList metaInfo = GetObjectMetaInfo(itemId, itemTypeId);

			columns += objectItemPtr;
			for(QString infoItem : metaInfo){
				columns += new QStandardItem(infoItem);
			}

			m_itemModel.appendRow(columns);
		}
	}

	UpdateCommands();

	on_TypeList_itemSelectionChanged();

	m_blockHeaderSectionSizeStore = false;
}


void CObjectCollectionViewComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	imtbase::IObjectCollection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	if (m_viewDelegateMap.isEmpty()){
		m_defaultViewDelegate.InitializeDelegate(objectPtr, this);
	}

	for (ViewDelegateMap::Iterator iter = m_viewDelegateMap.begin(); iter != m_viewDelegateMap.end(); ++iter){
		iter.value()->InitializeDelegate(objectPtr, this);
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CObjectCollectionViewComp::OnGuiCreated()
{
	m_proxyModelPtr = new QSortFilterProxyModel(this);
	m_proxyModelPtr->setSourceModel(&m_itemModel);
	m_proxyModelPtr->setFilterKeyColumn(0);
	m_proxyModelPtr->setFilterRole(DR_TYPE_ID);

	ItemList->setModel(m_proxyModelPtr);

	QItemSelectionModel* selectionModelPtr = ItemList->selectionModel();
	if (selectionModelPtr != nullptr){
		connect(
					selectionModelPtr,
					SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
					this, SLOT(OnSelectionChanged(const QItemSelection&, const QItemSelection&)));
	}

	connect(&m_itemModel, &QStandardItemModel::itemChanged, this, &CObjectCollectionViewComp::OnItemChanged);
	connect(ItemList, &QTreeView::doubleClicked, this, &CObjectCollectionViewComp::OnItemDoubleClick);
	connect(ItemList, &QTreeView::customContextMenuRequested, this, &CObjectCollectionViewComp::OnCustomContextMenuRequested);
	ItemList->setContextMenuPolicy(Qt::CustomContextMenu);

	ItemList->setEditTriggers(QAbstractItemView::NoEditTriggers);

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
	istd::IChangeable::ChangeSet changes(ibase::ICommandsProvider::CF_COMMANDS);
	istd::CChangeNotifier changeNotifier(&m_commands, &changes);

	QModelIndexList selectedIndexes = ItemList->selectionModel()->selectedRows();

	imtbase::ICollectionInfo::Ids itemIds;

	QSet<QByteArray> selectedTypes;

	if (!selectedIndexes.isEmpty()){
		const imtbase::IObjectCollection* collectionPtr = GetObservedObject();
		Q_ASSERT(collectionPtr != nullptr);
		for (int i = 0; i < selectedIndexes.count(); ++i){
			QStandardItem* itemPtr = m_itemModel.itemFromIndex(m_proxyModelPtr->mapToSource(selectedIndexes[i]));
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

	GetViewDelegateRef(m_currentTypeId).UpdateItemSelection(stateFlags, itemIds, m_currentTypeId);
}


QStringList CObjectCollectionViewComp::GetTypeIdMetaHeader(const QByteArray &typeId)
{
	const ICollectionViewDelegate& viewDelegate = GetViewDelegateRef(typeId);
	const imtbase::ICollectionInfo& informationTypes = viewDelegate.GetSummaryInformationTypes();
	QVector<QByteArray> informationTypesIds = informationTypes.GetElementIds();

	QStringList headers;

	for (int informationIndex = 0; informationIndex < informationTypesIds.count(); informationIndex++){
		headers += informationTypes.GetElementInfo(informationTypesIds[informationIndex], imtbase::ICollectionInfo::EIT_NAME).toString();
	}

	return headers;
}


QStringList CObjectCollectionViewComp::GetObjectMetaInfo(const QByteArray &itemId, const QByteArray &typeId)
{
	const ICollectionViewDelegate& viewDelegate = GetViewDelegateRef(typeId);
	const imtbase::ICollectionInfo& informationTypes = viewDelegate.GetSummaryInformationTypes();
	QVector<QByteArray> informationTypesIds = informationTypes.GetElementIds();
	
	QStringList informationsItems;

	for (int informationIndex = 0; informationIndex < informationTypesIds.count(); informationIndex++){
		QVariant info = viewDelegate.GetSummaryInformation(itemId, informationTypesIds[informationIndex]);

		switch (info.type()){
		case QVariant::ByteArray:
			informationsItems.append(QString(info.toByteArray()));
			break;
		case QVariant::String:
			informationsItems.append(info.toString());
			break;
		case QVariant::DateTime:
			informationsItems.append(info.toDateTime().toString("dd.MM.yyyy hh:mm:ss"));
			break;
		default:
			informationsItems.append(QString());
		}
	}

	return informationsItems;
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


void CObjectCollectionViewComp::OnItemDoubleClick(const QModelIndex &item)
{
	int sourceRow = m_proxyModelPtr->mapToSource(item).row();
	QByteArray itemId = m_itemModel.item(sourceRow, 0)->data(DR_OBJECT_ID).toByteArray();

	const ICollectionViewDelegate &delegate = GetViewDelegateRef(m_currentTypeId);

	delegate.OpenDocumentEditor(itemId);
}


void CObjectCollectionViewComp::OnCustomContextMenuRequested(const QPoint &point)
{
	QAction *actionRename;
	QAction *actionEdit;
	QAction *actionRemove;

	QMenu menu(ItemList);
	actionRename = menu.addAction(QIcon(":/Icons/Rename"), tr("Rename"));
	actionEdit = menu.addAction(QIcon(":/Icons/Edit"), tr("Edit"));
	actionRemove = menu.addAction(QIcon(":/Icons/Remove"), tr("Remove"));

	connect(actionRename, &QAction::triggered, this, &CObjectCollectionViewComp::OnContextMenuRename);
	connect(actionEdit, &QAction::triggered, this, &CObjectCollectionViewComp::OnContextMenuEdit);
	connect(actionRemove, &QAction::triggered, this, &CObjectCollectionViewComp::OnContextMenuRemove);

	menu.exec(ItemList->viewport()->mapToGlobal(point));
}


void CObjectCollectionViewComp::on_TypeList_itemSelectionChanged()
{
	if (!m_blockHeaderSectionSizeStore && m_proxyModelPtr->rowCount()){
		QList<int> headerSectionsSize;

		for (int i = 0; i < m_itemModel.columnCount(); i++){
			headerSectionsSize.append(ItemList->header()->sectionSize(i));
		}

		m_headerSectionSize[m_currentTypeId] = headerSectionsSize;
	}

	m_currentTypeId.clear();

	QList<QTreeWidgetItem*> selectedItems = TypeList->selectedItems();
	if (!selectedItems.isEmpty()){
		m_currentTypeId = selectedItems[0]->data(0, DR_TYPE_ID).toByteArray();
	}

	m_proxyModelPtr->setFilterFixedString(m_currentTypeId);

	UpdateCommands();
	
	QStringList headerLabels;
	headerLabels.append(tr("Name"));
	headerLabels.append(GetTypeIdMetaHeader(m_currentTypeId));
	m_itemModel.setHorizontalHeaderLabels(headerLabels);

	for (int i = 0; i < m_itemModel.columnCount(); i++){
		if (i < headerLabels.count()){
			ItemList->showColumn(i);
		}
		else {
			ItemList->hideColumn(i);
		}
	}

	int columnCount = m_itemModel.columnCount();
	if (m_headerSectionSize.contains(m_currentTypeId)){
		for (int i = 0; i < columnCount; i++){
			ItemList->setColumnWidth(i, m_headerSectionSize[m_currentTypeId][i]);
		}
	}
	else{
		for (int i = 0; i < columnCount; i++){
			ItemList->resizeColumnToContents(i);
		}
	}
}


void CObjectCollectionViewComp::OnContextMenuRename(bool checked)
{
	ICollectionViewDelegate & delegate = GetViewDelegateRef(m_currentTypeId);
	QModelIndexList selectedIndexes = ItemList->selectionModel()->selectedRows();

	if (selectedIndexes.count() != 1){
		return;
	}	

	if (!selectedIndexes.isEmpty()){
		for (int i = 0; i < selectedIndexes.count(); ++i){
			QStandardItem* itemPtr = m_itemModel.itemFromIndex(m_proxyModelPtr->mapToSource(selectedIndexes[i]));
			if (itemPtr != nullptr){
				QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
				QByteArray typeId = itemPtr->data(DR_TYPE_ID).toByteArray();
				if (!itemId.isEmpty()){
					bool ok;
					const QString newName = QInputDialog::getText(ItemList, tr("Enter new object name"), tr("Name"), QLineEdit::Normal, "", &ok);
					if (ok){
						delegate.RenameObject(itemId, newName);
					}
				}
			}
		}
	}
}


void CObjectCollectionViewComp::OnContextMenuEdit(bool checked)
{
	ICollectionViewDelegate & delegate = GetViewDelegateRef(m_currentTypeId);
	QModelIndexList selectedIndexes = ItemList->selectionModel()->selectedRows();

	if (!selectedIndexes.isEmpty()){
		for (int i = 0; i < selectedIndexes.count(); ++i){
			QStandardItem* itemPtr = m_itemModel.itemFromIndex(m_proxyModelPtr->mapToSource(selectedIndexes[i]));
			if (itemPtr != nullptr){
				QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
				QByteArray typeId = itemPtr->data(DR_TYPE_ID).toByteArray();
				if (!itemId.isEmpty()){
					bool result = delegate.OpenDocumentEditor(itemId);
				}
			}
		}
	}
}


void CObjectCollectionViewComp::OnContextMenuRemove(bool checked)
{
	ICollectionViewDelegate & delegate = GetViewDelegateRef(m_currentTypeId);
	QModelIndexList selectedIndexes = ItemList->selectionModel()->selectedRows();

	QVector<QByteArray> itemIds;
	if (!selectedIndexes.isEmpty()){
		for (int i = 0; i < selectedIndexes.count(); ++i){
			QStandardItem* itemPtr = m_itemModel.itemFromIndex(m_proxyModelPtr->mapToSource(selectedIndexes[i]));
			if (itemPtr != nullptr){
				QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
				QByteArray typeId = itemPtr->data(DR_TYPE_ID).toByteArray();
				if (!itemId.isEmpty()){
					itemIds.append(itemId);
				}
			}
		}
	}

	delegate.RemoveObjects(itemIds);
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


